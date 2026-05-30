cmake_minimum_required(VERSION 3.10)
project(FixLink-CPP VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(FixLink-CPP src/main.cpp)

if(MSVC)
    target_compile_options(FixLink-CPP PRIVATE /W4 /O2 /permissive-)
else()
    target_compile_options(FixLink-CPP PRIVATE -Wall -Wextra -O3 -march=native)
endif()
