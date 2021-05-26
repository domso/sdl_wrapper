cmake_minimum_required(VERSION 3.5)

project(sdl_wrapper)
include(FindPkgConfig)

find_package(SDL2 REQUIRED)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)


include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${SDL2_INCLUDE_DIR} ${SDL2IMAGE_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FALGS} -std=c++17 -pthread -Wall -Wextra -pedantic")

file(GLOB src_sdl 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/*.cpp"
)

add_library(sdl_wrapper STATIC ${src_sdl})
target_link_libraries(sdl_wrapper ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES})

add_executable(demo demo/main.cpp)
target_link_libraries(demo sdl_wrapper)
