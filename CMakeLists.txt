cmake_minimum_required(VERSION 3.7)
project(bag)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c utils.c bag.c)
add_executable(bag ${SOURCE_FILES})