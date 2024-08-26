add_executable(ByteCodeTest test_common_bytecode.cpp)
target_include_directories(ByteCodeTest PRIVATE ${CMAKE_SOURCE_DIR}/src/common/include)
target_link_libraries(ByteCodeTest PRIVATE common GTest::gtest_main)

add_executable(UtilTest test_common_util.cpp)
target_include_directories(UtilTest PRIVATE ${CMAKE_SOURCE_DIR}/src/common/include)
target_link_libraries(UtilTest PRIVATE common GTest::gtest_main)

gtest_discover_tests(ByteCodeTest)
gtest_discover_tests(UtilTest)