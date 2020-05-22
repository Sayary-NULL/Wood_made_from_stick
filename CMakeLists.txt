cmake_minimum_required(VERSION 3.15)
project(Wood_made_from_stick)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)
find_package(Qt5Core REQUIRED)

add_executable(Wood_made_from_stick main.cpp)

target_link_libraries(Wood_made_from_stick sfml-graphics sfml-audio)
target_link_libraries(Wood_made_from_stick Qt5::Core)