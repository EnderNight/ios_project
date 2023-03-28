cmake_minimum_required(VERSION 3.22)
project(ios_project VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_executable(ios_project
        test_shell.c
        inventory.c where_am_i.c parse.c parse.c)
