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

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

if(BUILD_DEBUG)
    message("Build Debug version")
    set(CMAKE_BUILD_TYPE Debug)
else()
    message("Build Release version")
    set(CMAKE_BUILD_TYPE Release)
endif()

# Core parfis
file(GLOB_RECURSE PARFIS_CORE_SOURCES ${parfis_SOURCE_DIR}/src/parfis/*.cpp ${parfis_SOURCE_DIR}/src/parfis/*.h)
if (BUILD_STATIC_LIB)
    message("Build static lib")
    add_library(parfis STATIC)
else ()
    message("Build shared lib")
    add_library(parfis SHARED)
    add_compile_definitions(PARFIS_SHARED_LIB)
    set_properties(parfis SHARED)
    set_target_properties(parfis PROPERTIES FILE_DIR ${parfis_SOURCE_DIR}/build/lib/parfis)
endif ()
target_sources(parfis PRIVATE ${PARFIS_CORE_SOURCES})
set_target_properties(parfis PROPERTIES PUBLIC_HEADER "parfis.h")
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
    set_properties(parfisApp EXECUTABLE)
    add_dependencies(parfisApp parfis)
    target_link_libraries(parfisApp PRIVATE parfis)
    add_dependencies(parfisApp parfis)
    get_target_property(PARFIS_FILE_DIR parfis FILE_DIR)
    get_target_property(PARFIS_FILE_NAME parfis FILE_NAME)
    # Copy shared libs under win
    if(WIN32)
        add_custom_command(TARGET parfisApp POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy 
            ${PARFIS_FILE_DIR}/${PARFIS_FILE_NAME} 
            ${parfis_SOURCE_DIR}/build/bin/parfisApp/${PARFIS_FILE_NAME})
    endif()
endif()

# Logging level
if(NOT PARFIS_LOG_LEVEL)
    set(PARFIS_LOG_LEVEL 0)
endif()
add_compile_definitions(PARFIS_LOG_LEVEL=${PARFIS_LOG_LEVEL})

# Doxygen
if(BUILD_DOXYGEN OR BUILD_SPHINX)
    message("Building doxygen")
    run_doxygen()
endif()

# Sphinx
if(BUILD_SPHINX)
    message("Building sphinx")
    run_sphinx_build()
endif()

# State type float or double
if(PARFIS_STATE_TYPE_DOUBLE)
    message("Using double for type parfis::state_t")
    add_compile_definitions(PARFIS_STATE_TYPE_DOUBLE)
else()
    message("Using float for type parfis::state_t")
endif()

# Google test
if(BUILD_GOOGLE_TEST OR BUILD_GTESTALL)
    message("Build googletest")
    include(ExternalProject)
    ExternalProject_Add(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG main
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${parfis_SOURCE_DIR}/build/lib/googletest 
                   -DBUILD_SHARED_LIBS=ON
		   -DCMAKE_DEBUG_POSTFIX=d
        BUILD_COMMAND ""
        BUILD_ALWAYS 1
        INSTALL_COMMAND cmake --build . --target install --config ${CMAKE_BUILD_TYPE}
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    add_library(gtest SHARED IMPORTED)
    set_target_properties(gtest PROPERTIES FILE_DIR ${parfis_SOURCE_DIR}/build/lib/googletest/lib)
    if(UNIX)
	if(BUILD_DEBUG)
            set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${parfis_SOURCE_DIR}/build/lib/googletest/lib/libgtestd.so)
	    set_target_properties(gtest PROPERTIES FILE_NAME libgtestd.so)
	else()
            set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${parfis_SOURCE_DIR}/build/lib/googletest/lib/libgtest.so)
	    set_target_properties(gtest PROPERTIES FILE_NAME libgtest.so)
	endif()
    elseif(WIN32)
        set_target_properties(gtest PROPERTIES FILE_DIR ${parfis_SOURCE_DIR}/build/lib/googletest/bin)
        if(BUILD_DEBUG)
            set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${parfis_SOURCE_DIR}/build/lib/googletest/lib/gtestd.lib)
            set_target_properties(gtest PROPERTIES FILE_NAME gtestd.dll)
        else()
            set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${parfis_SOURCE_DIR}/build/lib/googletest/lib/gtest.lib)
            set_target_properties(gtest PROPERTIES FILE_NAME gtest.dll)
        endif()
    endif()
endif()

# gtestAll
if(BUILD_GTESTALL)
    message("Build gtestAll")
    include(${parfis_SOURCE_DIR}/test/CMakeLists.txt)
endif()


message("VERSION = ${VERSION}")
message("PARFIS_LOG_LEVEL = ${PARFIS_LOG_LEVEL}")

get_target_property(GTEST_IMPORTED_LOCATION gtest IMPORTED_LOCATION)
get_target_property(GTEST_FILE_DIR gtest FILE_DIR)
get_target_property(GTEST_FILE_NAME gtest FILE_NAME)
get_target_property(PARFIS_FILE_DIR parfis FILE_DIR)
get_target_property(PARFIS_FILE_NAME parfis FILE_NAME)
message("GTEST_IMPORTED_LOCATION = ${GTEST_IMPORTED_LOCATION}")
message("GTEST_FILE_DIR = ${GTEST_FILE_DIR}")
message("GTEST_FILE_NAME = ${GTEST_FILE_NAME}")
message("PARFIS_FILE_DIR = ${PARFIS_FILE_DIR}")
message("PARFIS_FILE_NAME = ${PARFIS_FILE_NAME}")
