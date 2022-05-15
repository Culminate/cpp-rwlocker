cmake_minimum_required (VERSION 3.8)

project ("test_rwlocker")

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/rwlocker)

add_executable(${PROJECT_NAME} test_rwlocker.cpp)
target_link_libraries(${PROJECT_NAME} librwlocker)