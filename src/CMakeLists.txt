add_subdirectory(${CMAKE_SOURCE_DIR}/src/common)

add_library(debugger debugger.cpp)
target_include_directories(debugger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(debugger PUBLIC common)


add_library(vm vm.cpp)
target_include_directories(vm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(vm PUBLIC common)