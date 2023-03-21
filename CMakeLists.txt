cmake_minimum_required(VERSION 3.0)

project(json_sum_up LANGUAGES C)

add_compile_options(-std=c99 -Wall)

add_executable(json_sum_up main.c)

target_link_libraries(json_sum_up json-c)