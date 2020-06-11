cmake_minimum_required(VERSION 3.16)
project(c_sudoku_verifier C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(c_sudoku_verifier
    src/main.c
        )

target_link_libraries(${PROJECT_NAME} pthread)
