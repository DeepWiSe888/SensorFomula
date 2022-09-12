
cmake_minimum_required (VERSION 3.0)
SET(CMAKE_CXX_STANDARD 11)

#set(CMAKE_C_FLAGS ”${CMAKE_C_FLAGS} -Wall -Wextra”)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter")

project("sf01_baselib")
include_directories(../UniversalData ../BaseLibs/BLAS ../BaseLibs/Utils ../SensorInterfaces /usr/local/include)

SET(baselib_cpp
        ../../../BaseLibs/UniversalData/UniversalData.cpp
        ../../../BaseLibs/UniversalData/BaseDataDefine.cpp
        ../../../BaseLibs/Utils/UniversalTcp.cpp
        ../../../BaseLibs/Utils/UniversalSocket.cpp
        ../../../BaseLibs/Utils/UniversalThread.cpp
        ../../../BaseLibs/BLAS/fourier.cpp
        ../../../BaseLibs/BLAS/filter.cpp
        ../../../BaseLibs/BLAS/blas_clang.c
        )

SET(baselib_clang
        #../../../BaseLibs/UniversalData/BaseDataDefine.c
        ../../../BaseLibs/BLAS/blas_clang.c
        )

add_library(sfbase ${baselib_cpp})

add_library(sfbasec ${baselib_clang})

find_library (libfftvar fftw3 /usr/local/lib)

target_link_libraries(
        sfbase
        ${libfftvar}
        pthread
)
