cmake_minimum_required(VERSION 3.19)
project(Tunnel)

set(CMAKE_CXX_STANDARD 14)
add_subdirectory(external)

add_executable(Tunnel window.cpp)

target_link_libraries(Tunnel GL glut glfw3 glad dl X11 pthread)