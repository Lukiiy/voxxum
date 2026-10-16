cmake_minimum_required(VERSION 4.4)
project(voxxum CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VOXXUM_LINKRUNTIME "add c/c++ runtimes static links" OFF)
option(VOXXUM_USE_OPENGL "use OpenGL backend" OFF)

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
FetchContent_Declare(
    zstd
    GIT_REPOSITORY https://github.com/facebook/zstd.git
    GIT_TAG v1.5.7
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR build/cmake
    FIND_PACKAGE_ARGS NAMES zstd
)
FetchContent_Declare(
    FastNoiseLite
    GIT_REPOSITORY https://github.com/Auburn/FastNoiseLite.git
    GIT_TAG v1.1.1
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    sokol
    GIT_REPOSITORY https://github.com/floooh/sokol.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_Declare(
    sokol_gp
    GIT_REPOSITORY https://github.com/edubart/sokol_gp.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(SDL3 FastNoiseLite sokol sokol_gp)
find_library(ZSTD_LIB NAMES zstd libzstd REQUIRED)

add_executable(voxxum WIN32 main.cpp world.cpp player.cpp renderer.cpp)

target_include_directories(voxxum PRIVATE
    ${fastnoiselite_SOURCE_DIR}/Cpp
    ${sokol_SOURCE_DIR}
    ${sokol_gp_SOURCE_DIR}
)

target_link_libraries(voxxum PRIVATE SDL3::SDL3 ${ZSTD_LIB})

if(VOXXUM_USE_OPENGL)
    target_compile_definitions(voxxum PRIVATE SOKOL_GLCORE)
else()
    target_compile_definitions(voxxum PRIVATE SOKOL_VULKAN)
endif()