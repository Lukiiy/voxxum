cmake_minimum_required(VERSION 4.4)
project(voxxum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# bundle sdl3
set(SDL_SHARED OFF CACHE BOOL "Bundle shared SDL3" FORCE)
set(SDL_STATIC ON CACHE BOOL "Bundle SDL3" FORCE)

# disable unused sdl stuff
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_DISABLE_INSTALL ON CACHE BOOL "" FORCE)

include(FetchContent)
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS NAMES SDL3
)
FetchContent_MakeAvailable(SDL3)

add_executable(voxxum main.cpp world.cpp player.cpp renderer.cpp)
target_link_libraries(voxxum PRIVATE SDL3::SDL3)