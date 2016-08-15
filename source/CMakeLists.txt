project(llvm_test)
cmake_minimum_required(VERSION 3.4.3)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories(.)

set(SRC_LIST
    main.cpp
    jit/mapper.cpp
    jit/range_mapper.cpp)

set(INC_LIST
    gsl/gsl.h
    gsl/gsl_assert.h
    gsl/gsl_byte.h
    gsl/gsl_util.h
    gsl/multi_span.h
    gsl/span.h
    gsl/string_span.h

    util/llvm_warn_push.hpp
    util/llvm_warn_pop.hpp
    util/types.hpp

    jit/node.hpp
    jit/mapper.hpp
    jit/range_mapper.hpp)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${INC_LIST})

# nativecodegen
llvm_map_components_to_libnames(llvm_libs support core irreader executionengine target x86 x86codegen x86info x86utils)

target_link_libraries(${PROJECT_NAME} ${llvm_libs})
