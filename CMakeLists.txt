cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
include(cmake/Utils.cmake)
project(parfis LANGUAGES CXX C)

set_version()

option(BUILD_DEBUG "Build debug version" OFF)
option(BUILD_STATIC_LIB "Build static library" OFF)
option(BUILD_PARFISAPP "Build executable application" ON)
option(PARFIS_STATE_TYPE_DOUBLE "Use double for state data type" OFF)
option(BUILD_DOXYGEN "Build documentation C++ code using Doxygen." OFF)
option(BUILD_SPHINX "Build documentation using Sphinx." OFF)
option(BUILD_GOOGLE_TEST "Build google test" OFF)
option(BUILD_GTESTALL "Build gtestAll" ON)

if(BUILD_DEBUG)
    message("Build Debug version")
    set(CMAKE_BUILD_TYPE Debug) 
    set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
else(BUILD_DEBUG)
    message("Build Release version")
    set(CMAKE_BUILD_TYPE Release)
endif(BUILD_DEBUG)

# Core parfis
file(GLOB_RECURSE PARFIS_CORE_SOURCES ${parfis_SOURCE_DIR}/src/parfis/*.cpp ${parfis_SOURCE_DIR}/src/parfis/*.h)
if (BUILD_STATIC_LIB)
    message("Build static lib")
    add_library(parfis STATIC)
else (BUILD_STATIC_LIB)
    message("Build shared lib")
    add_library(parfis SHARED)
    add_compile_definitions(PARFIS_SHARED_LIB)
    set_target_properties(parfis PROPERTIES SUFFIX "d.so")
endif (BUILD_STATIC_LIB)
target_sources(parfis PRIVATE ${PARFIS_CORE_SOURCES})
set_target_properties(parfis PROPERTIES 
    PUBLIC_HEADER "parfis.h")
target_include_directories(parfis PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/parfis>
  $<INSTALL_INTERFACE:lib/parfis/include>  # <prefix>/include
)
set_output_directory(parfis ${parfis_SOURCE_DIR}/build/lib/parfis)

# parfisApp
if(BUILD_PARFISAPP)
    file(GLOB_RECURSE PARFIS_APP_SOURCES ${parfis_SOURCE_DIR}/src/parfisApp/*.cpp ${parfis_SOURCE_DIR}/src/parfisApp/*.h)
    add_executable(parfisApp)
    target_sources(parfisApp PRIVATE ${PARFIS_APP_SOURCES})
    target_include_directories(parfisApp PUBLIC
        $<BUILD_INTERFACE:${parfis_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:lib/parfis/include>  # <prefix>/include
    )
    set_output_directory(parfisApp ${parfis_SOURCE_DIR}/build/bin/parfisApp)
    set_target_properties(parfisApp PROPERTIES SUFFIX "d")
    target_link_libraries(parfisApp PRIVATE parfis)
    add_dependencies(parfisApp parfis)
endif(BUILD_PARFISAPP)

# Logging level
if(NOT PARFIS_LOG_LEVEL)
    set(PARFIS_LOG_LEVEL 0)
endif(NOT PARFIS_LOG_LEVEL)
add_compile_definitions(PARFIS_LOG_LEVEL=${PARFIS_LOG_LEVEL})

# Doxygen
if(BUILD_DOXYGEN)
    message("Building doxygen")
    run_doxygen()
endif(BUILD_DOXYGEN)

# Sphinx
if(BUILD_SPHINX)
    message("Building sphinx")
    run_sphinx_build()
endif(BUILD_SPHINX)

# State type float or double
if(PARFIS_STATE_TYPE_DOUBLE)
    message("Using double for type parfis::state_t")
    add_compile_definitions(PARFIS_STATE_TYPE_DOUBLE)
else(PARFIS_STATE_TYPE_DOUBLE)
    message("Using float for type parfis::state_t")
endif(PARFIS_STATE_TYPE_DOUBLE)

# Google test
if(BUILD_GOOGLE_TEST)
    message("Build googletest")
    include(ExternalProject)
    ExternalProject_Add(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG main
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${parfis_SOURCE_DIR}/build/lib/googletest -DBUILD_SHARED_LIBS=ON
    )
    add_library(gtest SHARED IMPORTED)
    if(UNIX)
        set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${parfis_SOURCE_DIR}/build/lib/googletest/lib/libgtest.so)
    endif(UNIX)
endif(BUILD_GOOGLE_TEST)

# gtestAll
if(BUILD_GTESTALL)
    message("Build gtestAll")
    include(${parfis_SOURCE_DIR}/test/CMakeLists.txt)
endif()


message("VERSION = ${VERSION}")
message("PARFIS_LOG_LEVEL = ${PARFIS_LOG_LEVEL}")
