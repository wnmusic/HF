cmake_minimum_required(VERSION 3.0)   
project(Libdsp)

set ( CMAKE_MODULE_PATH
  ${PROJECT_SOURCE_DIR}/cmake
)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if (WIN32)
set(FFTW_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../contrib/fftw-3.3.5-dll64)
set(FFTWF_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../contrib/fftw-3.3.5-dll64/libfftw3f-3.lib)
else()
  find_package(FFTW REQUIRED)
  find_package(Volk REQUIRED PATH /usr/lib/x86_64-linux-gnu/cmake/volk)
endif()

SET(CMAKE_CXX_FLAGS  "-fPIC")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mavx")
add_library(Libdsp blkconv.cxx resample.cxx decimate.cxx polyphase.cxx fxlat_filter.cxx upsamp_fxlat_filter.cxx hilbert_xfrm.cxx)

target_include_directories(Libdsp  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Libdsp  PUBLIC ${FFTW_INCLUDE_DIR})

target_link_libraries(Libdsp PUBLIC ${FFTWF_LIB} ${VOLK_LIBRARIES})

add_subdirectory(test)
