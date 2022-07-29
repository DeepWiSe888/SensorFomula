
cmake_minimum_required (VERSION 2.8)

project("radar_example")

add_executable(
        radar_server
        server.cpp
)

add_executable(
        fmcw_cli
        fmcw.cpp
)
add_executable(
        x4_cli
        x4.cpp
)
add_executable(
        octopus_cli
        octopus.cpp
)