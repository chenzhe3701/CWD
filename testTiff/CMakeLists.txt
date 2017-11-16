cmake_minimum_required(VERSION 3.1)
project(TestTiff)

add_executable (myFun test_tif.cpp)
set_property(TARGET myFun PROPERTY CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fftw)	
find_library(FFTW_LIBRARY_1 libfftw3-3 ${CMAKE_CURRENT_SOURCE_DIR}/fftw)
find_library(FFTW_LIBRARY_2 libfftw3f-3 ${CMAKE_CURRENT_SOURCE_DIR}/fftw)
find_library(FFTW_LIBRARY_3 libfftw3l-3 ${CMAKE_CURRENT_SOURCE_DIR}/fftw)
target_link_libraries(myFun ${FFTW_LIBRARY_1} ${FFTW_LIBRARY_2} ${FFTW_LIBRARY_3})