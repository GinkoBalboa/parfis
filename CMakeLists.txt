cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 17)
include(cmake/Utils.cmake)
project(parfis LANGUAGES CXX C)

set_version()

option(BUILD_LIB "Build parfis library" ON)
option(PARFIS_STATE_TYPE_DOUBLE "Use double for state data type" ON)
option(BUILD_DEBUG "Build debug version" OFF)
option(BUILD_PARFISAPP "Build executable application" OFF)
option(BUILD_DOXYGEN "Build documentation C++ code using Doxygen." OFF)
option(BUILD_SPHINX "Build documentation using Sphinx." OFF)
option(BUILD_GOOGLE_TEST "Build google test" OFF)
option(BUILD_GTESTALL "Build gtestAll" OFF)
option(COPY_LIB_PYTHON "Copy parfis lib for python module" OFF)
option(NBSPHINX_EXECUTE "Should nbsphinx be executed when generating doc." ON)

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
if (BUILD_LIB)
    message("Build shared lib")
    add_library(parfis SHARED)
    add_compile_definitions(PARFIS_SHARED_LIB)
    if(PARFIS_STATE_TYPE_DOUBLE)
        message("Using double for type parfis::state_t")
        add_compile_definitions(STATE_TYPE_DOUBLE)
        set_properties(parfis SHARED "64")
    else()
        message("Using float for type parfis::state_t")
        set_properties(parfis SHARED "32")
    endif()
    target_sources(parfis PRIVATE ${PARFIS_CORE_SOURCES})
    set_target_properties(parfis PROPERTIES PUBLIC_HEADER "parfis.h")
    set_target_properties(parfis PROPERTIES FILE_DIR ${parfis_SOURCE_DIR}/build/lib/parfis)
    target_include_directories(parfis PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/parfis>
        $<INSTALL_INTERFACE:lib/parfis/include>  # <prefix>/include
    )
    set_output_directory(parfis ${parfis_SOURCE_DIR}/build/lib/parfis)
    get_target_property(PARFIS_FILE_DIR parfis FILE_DIR)
    get_target_property(PARFIS_FILE_NAME parfis FILE_NAME)
    message("PARFIS_FILE_DIR = ${PARFIS_FILE_DIR}")
    message("PARFIS_FILE_NAME = ${PARFIS_FILE_NAME}")
endif()

# Copy shared libs for the python-package
if(BUILD_LIB AND COPY_LIB_PYTHON)
    add_custom_command(TARGET parfis POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${PARFIS_FILE_DIR}/${PARFIS_FILE_NAME} 
        ${parfis_SOURCE_DIR}/python-package/parfis/clib/${PARFIS_FILE_NAME})
endif()


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
    set_properties(parfisApp EXECUTABLE "")
    add_dependencies(parfisApp parfis)
    target_link_libraries(parfisApp PRIVATE parfis)
    add_dependencies(parfisApp parfis)
    get_target_property(PARFIS_FILE_DIR parfis FILE_DIR)
    get_target_property(PARFIS_FILE_NAME parfis FILE_NAME)
    # Copy shared libs under win for the parfisApp
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
    if(NBSPHINX_EXECUTE)
        set(NBSPHINX_EXECUTE_STR "always")
    else()
        set(NBSPHINX_EXECUTE_STR "never")
    endif(NBSPHINX_EXECUTE)
    message("NBSPHINX_EXECUTE_STR=${NBSPHINX_EXECUTE_STR}")
    run_sphinx_build()
endif()

# Google test
if(BUILD_GOOGLE_TEST OR BUILD_GTESTALL)
    if (BUILD_GOOGLE_TEST)
        message("Build googletest")
        include(ExternalProject)
        ExternalProject_Add(googletest
            GIT_REPOSITORY https://github.com/google/googletest
            GIT_TAG main
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${parfis_SOURCE_DIR}/build/lib/googletest 
                    -DBUILD_SHARED_LIBS=ON
            BUILD_COMMAND ""
            BUILD_ALWAYS 1
            INSTALL_COMMAND cmake --build . --target install --config ${CMAKE_BUILD_TYPE}
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()
    add_library(gtest SHARED IMPORTED)
    if(UNIX)
        set_target_properties(gtest PROPERTIES FILE_DIR ${parfis_SOURCE_DIR}/build/lib/googletest/lib)
        set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${parfis_SOURCE_DIR}/build/lib/googletest/lib/libgtest.so)
        set_target_properties(gtest PROPERTIES FILE_NAME libgtest.so)
    elseif(WIN32)
        set_target_properties(gtest PROPERTIES FILE_DIR ${parfis_SOURCE_DIR}/build/lib/googletest/bin)
        set_target_properties(gtest PROPERTIES IMPORTED_LOCATION ${parfis_SOURCE_DIR}/build/lib/googletest/lib/gtest.lib)
        set_target_properties(gtest PROPERTIES FILE_NAME gtest.dll)
    endif()
    get_target_property(GTEST_IMPORTED_LOCATION gtest IMPORTED_LOCATION)
    get_target_property(GTEST_FILE_DIR gtest FILE_DIR)
    get_target_property(GTEST_FILE_NAME gtest FILE_NAME)
    message("GTEST_IMPORTED_LOCATION = ${GTEST_IMPORTED_LOCATION}")
    message("GTEST_FILE_DIR = ${GTEST_FILE_DIR}")
    message("GTEST_FILE_NAME = ${GTEST_FILE_NAME}")
endif()

# gtestAll
if(BUILD_GTESTALL)
    message("Build gtestAll")
    include(${parfis_SOURCE_DIR}/test/CMakeLists.txt)
endif()

message("VERSION = ${VERSION}")
message("PARFIS_LOG_LEVEL = ${PARFIS_LOG_LEVEL}")
