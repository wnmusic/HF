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
find_package(UHD 3.15.0 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(SDL2)

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
    ${SDL2_INCLUDE_DIRECTORY}
    ${UHD_INCLUDE_DIRS}
    libdsp
    imgui
    imgui/backends
    src
)
link_directories(${Boost_LIBRARY_DIRS})

### Make the executable #######################################################
add_executable(uhd_hf src/uhd_rx.cxx src/main.cxx imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp src/demod_am.cxx src/sdl_audio_impl.cxx src/waterfall.cxx)
add_subdirectory(libdsp)

if(NOT CMAKE_BUILD_TYPE)
       set(CMAKE_BUILD_TYPE "Release")
endif()


# Shared library case: All we need to do is link against the library, and
# anything else we need (in this case, some Boost libraries):
if(NOT UHD_USE_STATIC_LIBS)
    message(STATUS "Linking against shared UHD library.")
    target_link_libraries(uhd_hf ${UHD_LIBRARIES} ${Boost_LIBRARIES} Libdsp glfw OpenGL::GL ${SDL2_LIBRARIES} )
# Shared library case: All we need to do is link against the library, and
# anything else we need (in this case, some Boost libraries):
else(NOT UHD_USE_STATIC_LIBS)
    message(STATUS "Linking against static UHD library.")
    target_link_libraries(uhd_hf
        # We could use ${UHD_LIBRARIES}, but linking requires some extra flags,
        # so we use this convenience variable provided to us
        ${UHD_STATIC_LIB_LINK_FLAG}
        # Also, when linking statically, we need to pull in all the deps for
        # UHD as well, because the dependencies don't get resolved automatically
        ${UHD_STATIC_LIB_DEPS}
        Libdsp
        glfw
        OpenGL::GL
        ${SDL2_LIBRARIES} 
    )
endif(NOT UHD_USE_STATIC_LIBS)

