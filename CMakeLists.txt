cmake_minimum_required(VERSION 4.4)
project(voxxum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(voxxum main.cpp world.cpp player.cpp renderer.cpp)
target_link_libraries(voxxum PRIVATE SDL3::SDL3)