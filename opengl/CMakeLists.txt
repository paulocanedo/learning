cmake_minimum_required(VERSION 2.8.4)
project(opengl)

##############################
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
include_directories(include)

find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIR})
##############################

set(SOURCE_FILES src/util/FileLoader.cpp src/gl3w.c src/main.cpp)
add_executable(opengl ${SOURCE_FILES})
target_link_libraries(opengl ${GLFW_STATIC_LIBRARIES} ${PNG_LIBRARY})