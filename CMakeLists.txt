project ("rwlocker_example")
cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/rwlocker)

add_executable(${PROJECT_NAME} rwlocker_example.cpp)
target_link_libraries(${PROJECT_NAME} librwlocker)