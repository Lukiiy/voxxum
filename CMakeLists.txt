cmake_minimum_required(VERSION 4.4)
project(voxxum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VOXXUM_LINKRUNTIME "add c/c++ runtimes static links" OFF)

if(VOXXUM_LINKRUNTIME)
    if(MSVC)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    elseif(UNIX AND NOT APPLE)
        add_link_options(-static-libstdc++ -static-libgcc)
    endif()
endif()

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

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_TESTS OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_SAMPLES OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG release-3.4.8
    GIT_SHALLOW TRUE
    FIND_PACKAGE_ARGS NAMES SDL3_image
)

FetchContent_MakeAvailable(SDL3 SDL3_image)

add_executable(voxxum main.cpp world.cpp player.cpp renderer.cpp)

add_custom_command(
    TARGET voxxum POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/terrain.png" "$<TARGET_FILE_DIR:voxxum>/terrain.png"
)

target_link_libraries(voxxum PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)