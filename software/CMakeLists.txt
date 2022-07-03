#
# Copyright 2014-2015 Ettus Research LLC
# Copyright 2018 Ettus Research, a National Instruments Company
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

cmake_minimum_required(VERSION 3.5.1)
project(uhd_hf CXX)
set(CMAKE_CXX_STANDARD 11)
set(OpenGL_GL_PREFERENCE "GLVND")


if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" AND ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "-lthr ${CMAKE_EXE_LINKER_FLAGS}")
    set(CMAKE_CXX_FLAGS "-stdlib=libc++ ${CMAKE_CXX_FLAGS}")
endif()

### Set up build environment ##################################################
# Choose a static or shared-library build (shared is default, and static will
# probably need some special care!)
# Set this to ON in order to link a static build of UHD:
option(UHD_USE_STATIC_LIBS OFF)

# To add UHD as a dependency to this project, add a line such as this:

find_package(UHD 4.2 EXACT REQUIRED PATHS /usr/local)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(portaudio)

# This example also requires Boost.
# Set components here, then include UHDBoost to do the actual finding
set(UHD_BOOST_REQUIRED_COMPONENTS
    program_options
    system
    thread
)
set(BOOST_MIN_VERSION 1.65)
include(UHDBoost)

# need these include and link directories for the build
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${UHD_INCLUDE_DIRS}
    libdsp
    imgui
    imgui/backends
    src
)
link_directories(${Boost_LIBRARY_DIRS})

### Make the executable #######################################################
add_library(hf src/demod_am.cxx)
add_executable(uhd_hf src/uhd_rx.cxx src/main.cxx imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp src/pa_sink.cxx src/waterfall.cxx)

add_subdirectory(libdsp)

if(NOT CMAKE_BUILD_TYPE)
       set(CMAKE_BUILD_TYPE "Release")
endif()
target_link_libraries(hf Libdsp)
target_link_libraries(uhd_hf ${UHD_LIBRARIES} ${Boost_LIBRARIES} glfw OpenGL::GL portaudio hf)

enable_testing()
add_executable(test_demod_am test/test_demod_am.cxx $<TARGET_OBJECTS:hf>)
target_link_libraries(test_demod_am Libdsp)

add_test(NAME test_demod_am COMMAND test_demod_am ${CMAKE_CURRENT_SOURCE_DIR}/test/FM_21.1M_sr500k_cf21.25M.data)
