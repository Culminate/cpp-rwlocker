project ("librwlocker")
cmake_minimum_required (VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)

set(CONAN_INSTALL_ARGS "" CACHE STRING "флаги для команды conan install")

option(EXAMPLE "build example" OFF)
option(EXAMPLE_RWLOCKER_DISABLE "disable rwlocker for example" OFF)
set(EXAMPLE_SEED 289352958 CACHE STRING "Число, с которого начинается генерация чисел")
set(EXAMPLE_THREAD_COUNT 100 CACHE STRING "Количество потоков для запуска")
set(EXAMPLE_LIST_SIZE 1000 CACHE STRING "Размер списка для заполнения")
set(EXAMPLE_STRING_MULT 100 CACHE STRING "Множитель для строки. Количество конкатенаций в одну строку")

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include/)

if (${CMAKE_TESTING_ENABLED})
    find_program(CONAN "conan" REQUIRED DOC "менеджер зависимостей для с++")
    execute_process(COMMAND conan install ${PROJECT_SOURCE_DIR} ${CONAN_INSTALL_ARGS}
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
    target_compile_definitions(rwlocker_example PRIVATE
        EXAMPLE_SEED=${EXAMPLE_SEED}
        EXAMPLE_THREAD_COUNT=${EXAMPLE_THREAD_COUNT}
        EXAMPLE_LIST_SIZE=${EXAMPLE_LIST_SIZE}
        EXAMPLE_STRING_MULT=${EXAMPLE_STRING_MULT})
    if(${EXAMPLE_RWLOCKER_DISABLE})
        target_compile_definitions(rwlocker_example PRIVATE EXAMPLE_RWLOCKER_DISABLE)
    endif()
endif()