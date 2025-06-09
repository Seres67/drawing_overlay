cmake_minimum_required(VERSION 4.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(drawing_overlay)

include_directories(include)

file(GLOB_RECURSE src src/**.c)

add_executable(drawing_overlay ${src})
target_link_libraries(drawing_overlay SDL3 SDL3_ttf SDL3_image)
