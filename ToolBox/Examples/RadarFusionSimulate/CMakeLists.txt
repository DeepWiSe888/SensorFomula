
cmake_minimum_required (VERSION 3.0)
SET(CMAKE_CXX_STANDARD 11)

project("radar_example")
include_directories(../../../BaseLibs/UniversalData ../../../BaseLibs/BLAS ../../../BaseLibs/Utils ../../SensorInterfaces /usr/local/include)

SET(baselib_cpp
        ../../SensorInterfaces/Radar/x4.cpp
        ../../SensorInterfaces/OldFileAccess/old_file.cpp
        ../../../BaseLibs/UniversalData/UniversalData.cpp
        ../../../BaseLibs/UniversalData/BaseDataDefine.c
        ../../../BaseLibs/Utils/UniversalTcp.cpp
        ../../../BaseLibs/Utils/UniversalSocket.cpp
        ../../../BaseLibs/Utils/UniversalThread.cpp
        ../../../BaseLibs/BLAS/fourier.cpp
        ../../../BaseLibs/BLAS/filter.cpp
)


add_executable(
        radar_server
        server.cpp
        ${baselib_cpp}
)

add_executable(
        fmcw_cli
        fmcw.cpp
)
add_executable(
        x4_cli
        x4pi.cpp
        ${baselib_cpp}
)
add_executable(
        octopus_cli
        octopus.cpp
)

#use find_library for linux/osx/raspi cross-platform
#maybe you need to change the fftw3 lib directory
find_library (libfftvar fftw3 /usr/local/lib)

target_link_libraries(
        x4_cli
        ${libfftvar}
)

target_link_libraries(
        radar_server
        ${libfftvar}
)