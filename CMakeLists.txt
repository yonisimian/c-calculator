cmake_minimum_required(VERSION 3.0.0)

project(shits VERSION 0.1.0 LANGUAGES C)

set(MTM_FLAGS_DEBUG "-std=c99 --pedantic-error -Wall -Werror")
set(MTM_FLAGS_RELEASE "${MTM_FLAGS_DEBUG} -DNDEBUG")

set(CMAKE_C_FLAGS ${MTM_FLAGS_DEBUG})

add_executable(my_executable culculator.c)