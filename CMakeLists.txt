cmake_minimum_required(VERSION 3.27)
project(CppLang LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

enable_testing()

add_executable(${PROJECT_NAME} src/main.cpp src/bytecode/bytecode.cpp src/vm/vm.cpp src/debugger/debugger.cpp)

# This definition is for clang <= 18.0 which currently has a bug of incomplete c++23 feature implementation
# For those using clangd as lsp or clang with version <= 18.0 (Arch currently has maximum 18.0 at the time of building this project)
target_compile_definitions(${PROJECT_NAME} PUBLIC __cpp_concepts=202002L)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_23)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wno-sign-compare)