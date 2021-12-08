cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
include(cmake/utils.cmake)
project(parfis LANGUAGES CXX C)

set_version()

option(BUILD_STATIC_LIB "Build static library" OFF)
option(BUILD_PARFISAPP "Build executable application" OFF)
option(HIDE_CXX_SYMBOLS "Build shared library and hide all C++ symbols" OFF)
option(STATE_TYPE_DOUBLE "Use double for state data type" OFF)

# core parfis
file(GLOB_RECURSE PARFIS_CORE_SOURCES ${parfis_SOURCE_DIR}/src/parfis/*.cpp ${parfis_SOURCE_DIR}/src/parfis/*.h)
if (BUILD_STATIC_LIB)
    add_library(parfis STATIC)
else (BUILD_STATIC_LIB)
    add_library(parfis SHARED)
endif (BUILD_STATIC_LIB)
target_sources(parfis PRIVATE ${PARFIS_CORE_SOURCES})
target_include_directories(parfis PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>  # <prefix>/include
)
set_output_directory(parfis ${parfis_SOURCE_DIR}/lib)

# parfisApp
if(BUILD_PARFISAPP)
    file(GLOB_RECURSE PARFIS_APP_SOURCES ${parfis_SOURCE_DIR}/src/parfisApp/*.cpp ${parfis_SOURCE_DIR}/src/parfisApp/*.h)
    add_executable(parfisApp)
    target_sources(parfisApp PRIVATE ${PARFIS_APP_SOURCES})
    target_include_directories(parfisApp PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>  # <prefix>/include
    )
    set_output_directory(parfisApp ${parfis_SOURCE_DIR})
    target_link_libraries(parfisApp PRIVATE parfis)
    add_dependencies(parfisApp parfis)
endif(BUILD_PARFISAPP)

add_compile_definitions(PARFIS_SHARED_LIB)
# Make double state type
if(STATE_TYPE_DOUBLE)
    message("Using double for type parfis::state_t")
    add_compile_definitions(PARFIS_STATE_TYPE_DOUBLE)
else(STATE_TYPE_DOUBLE)
    message("Using float for type parfis::state_t")
endif(STATE_TYPE_DOUBLE)

message("VERSION = ${VERSION}")
