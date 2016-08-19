enable_testing()
find_package(GTest)

if(NOT GTEST_FOUND)
    message("gtest not available")
    return()
endif()

include_directories(${GTEST_INCLUDE_DIRS})
include_directories(${LIB_INCLUDES})

set(SRC_LIST
    main.cpp

    test_stream.cpp
    test_utils.cpp
    test_graph_compiler.cpp)

set(INC_LIST)

set(TESTS_EXE_NAME "${PROJECT_NAME}-tests")

add_executable(${TESTS_EXE_NAME} ${SRC_LIST} ${INC_LIST})

target_link_libraries(${TESTS_EXE_NAME} ${GTEST_BOTH_LIBRARIES})
target_link_libraries(${TESTS_EXE_NAME} ${LIB_NAME})

add_test(AllTest ${TESTS_EXE_NAME})
