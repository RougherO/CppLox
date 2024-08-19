add_executable(ByteCodeTest test_common_bytecode.cpp)
target_include_directories(ByteCodeTest PRIVATE ${CMAKE_SOURCE_DIR}/src/common/include)
target_link_libraries(ByteCodeTest PRIVATE common GTest::gtest_main)

gtest_discover_tests(ByteCodeTest)