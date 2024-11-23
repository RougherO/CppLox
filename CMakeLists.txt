cmake_minimum_required(VERSION 3.27)
project(CppLox LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if ($<CONFIG:Debug> OR $<CONFIG:RelWithDebInfo>)
add_compile_options(
    -Wall -Wpedantic -Wextra -Wshadow -Wno-sign-compare
    -fsanitize=signed-integer-overflow -fsanitize=pointer-overflow -fsanitize=shift
    -fsanitize=address -fsanitize=bounds
    -fsanitize=integer-divide-by-zero -fsanitize=float-divide-by-zero
    -fsanitize=return -fsanitize=null
    -fsanitize=undefined
)
add_link_options(
    $<IF:$<CONFIG:Debug>,-fsanitize=address;
    -fsanitize=undefined;,>
)
endif()

add_subdirectory(src)

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTIONS})
target_link_options(${PROJECT_NAME} PRIVATE ${LINK_OPTIONS})
target_link_libraries(${PROJECT_NAME} lexer parser compiler logger vm)

option(CPPLOX_ENABLE_TESTING "Set this value to enable and allow tests to run" ON)

if (CPPLOX_ENABLE_TESTING)
    enable_testing()
    option(USE_SYSTEM_GTEST "Use pre-installed gtest on the system if available" OFF)
    
    if (USE_SYSTEM_GTEST)
        find_package(GTest REQUIRED)
    endif()
    
    if (NOT GTest_FOUND)
        set(BUILD_SHARED_LIBS ON)
        add_subdirectory(${CMAKE_SOURCE_DIR}/submodules/gtest)
        set(BUILD_SHARED_LIBS OFF)
    endif()

    include(GoogleTest)

    add_subdirectory(tests)
endif()

# This definition is for clang <= 18.0 which currently has a bug of incomplete c++23 feature implementation
# For those using clangd as lsp or clang with version <= 18.0 (Arch currently has maximum 18.0 at the time of building this project)
# target_compile_definitions(${PROJECT_NAME} PUBLIC __cpp_concepts=202002L)
# target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)