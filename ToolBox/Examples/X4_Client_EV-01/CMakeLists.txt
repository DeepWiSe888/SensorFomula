cmake_minimum_required (VERSION 2.8)

project("x4vs")
SET(CMAKE_CXX_STANDARD 11)

set(SENSOR_FORMULA_PATH "../../../../SensorFormula")

include_directories(./include ./src/hal ./src/Radar ./src/XDriver ./src/alg
        ${SENSOR_FORMULA_PATH}/BaseLibs/UniversalData/
        ${SENSOR_FORMULA_PATH}/BaseLibs/Utils/)

SET(sf_cpp
        ${SENSOR_FORMULA_PATH}/BaseLibs/UniversalData/UniversalData.cpp
        ${SENSOR_FORMULA_PATH}/BaseLibs/UniversalData/BaseDataDefine.c
        ${SENSOR_FORMULA_PATH}/BaseLibs/Utils/UniversalTcp.cpp
        ${SENSOR_FORMULA_PATH}/BaseLibs/Utils/UniversalSocket.cpp
        ${SENSOR_FORMULA_PATH}/BaseLibs/Utils/UniversalThread.cpp
        ${SENSOR_FORMULA_PATH}/BaseLibs/BLAS/fourier.cpp
        ${SENSOR_FORMULA_PATH}/BaseLibs/BLAS/filter.cpp
        ${SENSOR_FORMULA_PATH}/BaseLibs/BLAS/blas_clang.c
        )

add_executable(
    x4vs

    ./src/main.c
    ./src/hal/radar_hal.c
    ./src/Radar/taskRadar.c
    ./src/Radar/sf_client.cpp
    ./src/XDriver/x4driver.c
    ./src/alg/alg.c
    ./src/alg/vs.c
    ${sf_cpp}
)

target_link_libraries(
    x4vs
    pthread
    fftw3
    m
)