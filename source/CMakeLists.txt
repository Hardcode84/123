project(llvm_test)
cmake_minimum_required(VERSION 3.4.3)

set(LIB_NAME "${PROJECT_NAME}-lib")

add_subdirectory(./lib)
add_subdirectory(./tests)
