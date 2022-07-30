
cmake_minimum_required (VERSION 2.8)

project("radar_example")
include_directories(../../../BaseLibs/UniversalData ../../../BaseLibs/Utils ../../SensorInterfaces)

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
        x4pi.cpp
        ../../SensorInterfaces/Radar/x4.cpp
        ../../../BaseLibs/UniversalData/UniversalData.cpp
        ../../../BaseLibs/UniversalData/BaseDataDefine.c
)
add_executable(
        octopus_cli
        octopus.cpp
)