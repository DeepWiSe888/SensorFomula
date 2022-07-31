
cmake_minimum_required (VERSION 3.0)
SET(CMAKE_CXX_STANDARD 11)

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
        ../../../BaseLibs/Utils/UniversalTcp.cpp
        ../../../BaseLibs/Utils/UniversalSocket.cpp
        ../../../BaseLibs/Utils/UniversalThread.cpp
)
add_executable(
        octopus_cli
        octopus.cpp
)