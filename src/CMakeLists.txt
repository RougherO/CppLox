add_library(lexer SHARED lexer.cpp)
target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(parser SHARED parser.cpp)
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(compiler SHARED compiler.cpp)
target_include_directories(compiler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(logger SHARED logger.cpp)
target_include_directories(logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(vm SHARED vm.cpp)
target_include_directories(vm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
