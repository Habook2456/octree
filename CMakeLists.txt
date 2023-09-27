cmake_minimum_required(VERSION 3.0)

project(MiProyecto)

find_package(VTK 9.1 REQUIRED)

add_executable(mi_programa main.cpp)

target_link_libraries(mi_programa ${VTK_LIBRARIES})
