project ("librwlocker")
cmake_minimum_required (VERSION 3.22)

option(EXAMPLE "build example" OFF)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include/)

if (${CMAKE_TESTING_ENABLED})
    find_program(CONAN "conan" REQUIRED DOC "depency manager")
    execute_process(COMMAND conan install ${PROJECT_SOURCE_DIR}
                    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
                    OUTPUT_FILE conaninstall.txt)

    include(${PROJECT_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()

    add_executable(test_rwlocker src/test_rwlocker.cpp)
    target_link_libraries(test_rwlocker librwlocker pthread)
    add_test(NAME test_rwlocker COMMAND test_rwlocker --force-colors=true)
endif()

if (${EXAMPLE})
    add_executable(rwlocker_example src/rwlocker_example.cpp)
    target_link_libraries(rwlocker_example librwlocker pthread)
endif()