cmake_minimum_required(VERSION 3.10)
project(schoolcoin)

set(CMAKE_CXX_STANDARD 11)

add_executable(schoolcoin main.cpp sha256.cpp sha256.h)