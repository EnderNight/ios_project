cmake_minimum_required(VERSION 3.24)
project(ios_project C)

set(CMAKE_C_STANDARD 11)

add_executable(ios_project
        test_shell.c
        use.c)
