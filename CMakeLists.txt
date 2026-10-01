cmake_minimum_required(VERSION 4.4)
project(voxxum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG main
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(SDL3)

add_executable(voxxum main.cpp world.cpp player.cpp renderer.cpp)
target_link_libraries(voxxum PRIVATE SDL3::SDL3)