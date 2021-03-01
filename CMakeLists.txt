cmake_minimum_required(VERSION 3.17)
project(BASIC_Parser_C__)

set(CMAKE_CXX_STANDARD 17)

add_executable(BASIC_Parser_C__ main.cpp Lexer.cpp Lexer.h)