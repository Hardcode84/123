#include <iostream>
#include <memory>
#include <array>
#include <cassert>

#include "util/llvm_warn_push.hpp"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/LegacyPassManager.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/RuntimeDyld.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/ExecutionEngine/Orc/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/LambdaResolver.h"
#include "llvm/ExecutionEngine/Orc/ObjectLinkingLayer.h"

#include "llvm/Support/DynamicLibrary.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar/GVN.h"

#include "util/llvm_warn_pop.hpp"

class MyJIT
{
private:
    std::unique_ptr<llvm::TargetMachine> TM;
    const llvm::DataLayout DL;
    llvm::orc::ObjectLinkingLayer<> ObjectLayer;
    llvm::orc::IRCompileLayer<decltype(ObjectLayer)> CompileLayer;

public:
    typedef decltype(CompileLayer)::ModuleSetHandleT ModuleHandle;

    MyJIT():
        TM(llvm::EngineBuilder().selectTarget(llvm::Triple(),{},{},llvm::SmallVector<std::string,0>())),
        DL(TM->createDataLayout()),
        CompileLayer(ObjectLayer, llvm::orc::SimpleCompiler(*TM))
    {
        llvm::sys::DynamicLibrary::LoadLibraryPermanently(nullptr);
    }

    llvm::TargetMachine& getTargetMachine() { return *TM; }

    ModuleHandle addModule(std::unique_ptr<llvm::Module>& M)
    {
        // Build our symbol resolver:
        // Lambda 1: Look back into the JIT itself to find symbols that are part of
        //           the same "logical dylib".
        // Lambda 2: Search for external symbols in the host process.
        auto Resolver = llvm::orc::createLambdaResolver(
                            [&](const std::string& Name)
        {
            if (auto Sym = CompileLayer.findSymbol(Name, false))
            {
                return llvm::RuntimeDyld::SymbolInfo(Sym.getAddress(), Sym.getFlags());
            }
            return llvm::RuntimeDyld::SymbolInfo(nullptr);
        },
        [](const std::string& Name)
        {
            if (auto SymAddr = llvm::RTDyldMemoryManager::getSymbolAddressInProcess(Name))
            {
                return llvm::RuntimeDyld::SymbolInfo(SymAddr, llvm::JITSymbolFlags::Exported);
            }
            return llvm::RuntimeDyld::SymbolInfo(nullptr);
        });

        // Build a singlton module set to hold our module.
        std::vector<std::unique_ptr<llvm::Module>> Ms;
        Ms.push_back(std::move(M));

        // Add the set to the JIT with the resolver we created above and a newly
        // created SectionMemoryManager.
        return CompileLayer.addModuleSet(std::move(Ms),
                                         std::make_unique<llvm::SectionMemoryManager>(),
                                         std::move(Resolver));
    }

    llvm::orc::JITSymbol findSymbol(const std::string& Name) {
        std::string MangledName;
        llvm::raw_string_ostream MangledNameStream(MangledName);
        llvm::Mangler::getNameWithPrefix(MangledNameStream, Name, DL);
        return CompileLayer.findSymbol(MangledNameStream.str(), false);
    }

    void removeModule(ModuleHandle H) {
        CompileLayer.removeModuleSet(H);
    }

};

int main(int argc, char *argv[])
{
    std::cout << "Hello World!" << std::endl;
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder(context);
    std::unique_ptr<llvm::Module> module = std::make_unique<llvm::Module>("my module", context);
    MyJIT jit;
    module->setDataLayout(jit.getTargetMachine().createDataLayout());

    // Create a new pass manager attached to it.
    auto fpm = std::make_unique<llvm::legacy::FunctionPassManager>(module.get());

    // Do simple "peephole" optimizations and bit-twiddling optzns.
    fpm->add(llvm::createInstructionCombiningPass());
    // Reassociate expressions.
    fpm->add(llvm::createReassociatePass());
    // Eliminate Common SubExpressions.
    fpm->add(llvm::createGVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).

    llvm::Type* t = llvm::Type::getDoubleTy(context);
    llvm::FunctionType* ft = llvm::FunctionType::get(t, llvm::ArrayRef<llvm::Type*>({t, t}), false);
    llvm::Function* fun = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "myFun", module.get());
    std::array<llvm::Value*,2> args = {&*(fun->args().begin()), &*(++fun->args().begin())};
    args[0]->setName("a");
    args[1]->setName("b");

    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context, "", fun);
    builder.SetInsertPoint(bb);
    //llvm::Value* val = llvm::ConstantFP::get(context, llvm::APFloat(123.0));

    llvm::Value* val = builder.CreateFAdd(args[0], args[1], "addtmp");
    builder.CreateRet(val);

    module->dump();

    std::string str;
    if(llvm::verifyFunction(*fun, &llvm::raw_string_ostream(str)))
    {
        std::cout << "error " << str << std::endl;
    }
    else
    {
        std::cout << "ok" << std::endl;
    }

    fpm->run(*fun);

    jit.addModule(std::move(module));
    auto exprSymbol = jit.findSymbol("myFun");
    assert(exprSymbol && "Function not found");

    double (*FP)(double,double) = (double (*)(double,double))(intptr_t)exprSymbol.getAddress();

    std::cout << FP(999,999) << std::endl;

    return 0;
}
