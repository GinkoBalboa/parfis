# Set output directory of target, ignoring debug or release
function(set_output_directory target dir)
  set_target_properties(${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${dir}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${dir}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${dir}
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dir}
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${dir}
    LIBRARY_OUTPUT_DIRECTORY ${dir}
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${dir}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${dir}
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dir}
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${dir}
    ARCHIVE_OUTPUT_DIRECTORY ${dir}
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${dir}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${dir}
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${dir}
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${dir})
endfunction(set_output_directory)

# Set binary files extension
function(set_properties target type)
    if(${type} MATCHES "SHARED")
        if(UNIX)
            if(BUILD_DEBUG)
                set_target_properties(
                    ${target} 
                    PROPERTIES 
                        SUFFIX "d.so"
                        FILE_NAME "lib${target}d.so"
                        CMAKE_C_FLAGS_DEBUG "-g -DDEBUG"
                        CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
            else()
                set_target_properties(
                    ${target} 
                    PROPERTIES 
                        SUFFIX ".so"
                        FILE_NAME "lib${target}.so")
            endif()
        elseif(WIN32)
            if(BUILD_DEBUG)
                set_target_properties(
                    ${target} 
                    PROPERTIES 
                        SUFFIX "d.dll"
                        FILE_NAME "${target}d.dll"
                        CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MDd"
                        CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
            else()
                set_target_properties(
                    ${target} 
                    PROPERTIES 
                        SUFFIX ".dll"
                        FILE_NAME "${target}.dll"
                        CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MD"
                        CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
            endif()
        endif()
    elseif(${type} MATCHES "EXECUTABLE")
        if(UNIX)
            if(BUILD_DEBUG)
                set_target_properties(
                    ${target} 
                    PROPERTIES 
                        SUFFIX "d"
                        FILE_NAME "${target}d"
                        CMAKE_C_FLAGS_DEBUG "-g -DDEBUG"
                        CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
            endif()
        elseif(WIN32)
            if(BUILD_DEBUG)
                set_target_properties(
                    ${target} 
                    PROPERTIES 
                        SUFFIX "d.exe"
                        FILE_NAME "${target}d.exe"
                        CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd"
                        CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
            else()
                set_target_properties(
                    ${target} 
                    PROPERTIES 
                        SUFFIX ".exe"
                        FILE_NAME "${target}.exe"
                        CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT"
                        CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
            endif()
        endif()
    endif()
endfunction(set_properties)

function(set_version)
    set(GIT_REV "no-rev")
    execute_process(
        COMMAND git log --pretty=format:'%h' -n 1
        OUTPUT_VARIABLE GIT_REV ERROR_QUIET)
    # Check whether we got any revision (which isn't
    # always the case, e.g. when someone downloaded a zip
    # file from Github instead of a checkout)
    if(UNIX)
        execute_process(
            COMMAND bash -c "git diff --quiet --exit-code || echo +"
            OUTPUT_VARIABLE GIT_DIFF)
    elseif(WIN32)
        execute_process(
            COMMAND git diff --quiet --exit-code; if ($LASTEXITCODE=1) {echo +}
            OUTPUT_VARIABLE GIT_DIFF)
    endif()
    
    execute_process(
        COMMAND git describe --tags
        OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
    if ("${GIT_TAG}" STREQUAL "")
        set(GIT_TAG "N/A")
    endif()
        
    
    execute_process(
        COMMAND git describe --tags --abbrev=0
        OUTPUT_VARIABLE VERSION ERROR_QUIET)
    if ("${VERSION}" STREQUAL "")
        set(VERSION "N/A")
    endif()

    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH)

    string(STRIP "${GIT_REV}" GIT_REV)
    # GIT_REV string has quotes
    if(NOT "${GIT_REV}" STREQUAL "")
        string(REPLACE "\'" "" GIT_REV ${GIT_REV})
        string(REPLACE "\"" "" GIT_REV ${GIT_REV})
    endif()
    string(STRIP "${GIT_DIFF}" GIT_DIFF)
    string(STRIP "${GIT_TAG}" GIT_TAG)
    string(STRIP "${VERSION}" VERSION)
    string(STRIP "${GIT_BRANCH}" GIT_BRANCH)

    set(BUILD_CONFIG_STR "Release")
    if(BUILD_DEBUG)
        set(BUILD_CONFIG_STR "Debug")
    endif()

    set(VERSION_H "\
#ifndef PARFIS_VERSION_H
#define PARFIS_VERSION_H
#define GIT_REV \"${GIT_REV}${GIT_DIFF}\"
#define GIT_TAG \"${GIT_TAG}\"
#define GIT_BRANCH \"${GIT_BRANCH}\"
#define VERSION \"${VERSION}\"
#define BUILD_CONFIG \"${BUILD_CONFIG_STR}\"
#endif // PARFIS_VERSION_H
")

    message("VERSION_H=\n${VERSION_H}")
    set(VERSION "${VERSION}" PARENT_SCOPE)
    set(GIT_TAG "${GIT_TAG}" PARENT_SCOPE)
    set(GIT_REV "${GIT_REV}${GIT_DIFF}" PARENT_SCOPE)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h)
        file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h OLD_VERSION_H)        
    else()
        set(OLD_VERSION_H "")
    endif()
    if(NOT "${VERSION_H}" STREQUAL "${OLD_VERSION_H}")
        file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/include/parfis/version.h "${VERSION_H}")
    endif()
endfunction(set_version)


function (run_doxygen)
  find_package(Doxygen REQUIRED)

  if (NOT DOXYGEN_DOT_FOUND)
    message(FATAL_ERROR "Command `dot` not found.  Please install graphviz.")
  endif (NOT DOXYGEN_DOT_FOUND)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/doxygen)
  configure_file(
    ${parfis_SOURCE_DIR}/doc/doxygen/Doxyfile.in
    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  add_custom_target(doc_doxygen ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generate C APIs documentation."
    VERBATIM)
endfunction (run_doxygen)

# This runs sphins from python
function (run_sphinx)

    # configured documentation tools and intermediate build results
    set(SPHINX_SOURCE_DIR ${parfis_SOURCE_DIR}/doc/sphinx/source)
    
    # HTML output directory
    set(SPHINX_HTML_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc/sphinx/html)

    configure_file(
        ${parfis_SOURCE_DIR}/doc/sphinx/conf.py.in
        ${parfis_SOURCE_DIR}/doc/sphinx/source/conf.py
        @ONLY)

    add_custom_command(
        TARGET doc_doxygen POST_BUILD
        COMMAND python3 -m sphinx 
                        -b html ${SPHINX_SOURCE_DIR} ${SPHINX_HTML_DIR}
        VERBATIM)
endfunction (run_sphinx)

# This runs sphinx-build
function (run_sphinx_build)
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
    find_package(Sphinx REQUIRED)
    
    if(NOT DEFINED SPHINX_THEME)
        set(SPHINX_THEME default)
    endif()
    
    if(NOT DEFINED SPHINX_THEME_DIR)
        set(SPHINX_THEME_DIR)
    endif()
    
    # configured documentation tools and intermediate build results
    set(SPHINX_SOURCE_DIR ${parfis_SOURCE_DIR}/doc/sphinx/source)
    
    # Sphinx cache with pickled ReST documents
    set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/sphinx/_doctrees")
    
    # HTML output directory
    set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/doc/sphinx/html")
    
    configure_file(
        ${parfis_SOURCE_DIR}/doc/sphinx/conf.py.in
        ${parfis_SOURCE_DIR}/doc/sphinx/source/conf.py
        @ONLY)          
    
    add_custom_target(doc_sphinx ALL
    ${SPHINX_EXECUTABLE}
        -q -b html
        -c "${SPHINX_SOURCE_DIR}"
        -d "${SPHINX_CACHE_DIR}"
        "${SPHINX_SOURCE_DIR}"
        "${SPHINX_HTML_DIR}"
    COMMENT "Building HTML documentation with Sphinx")
    add_dependencies(doc_sphinx doc_doxygen)

    add_custom_target(copy-doxygen-html-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
        ${parfis_SOURCE_DIR}/build/doc/doxygen/html/ 
        ${parfis_SOURCE_DIR}/build/doc/sphinx/html/doxygen/
        DEPENDS doc_sphinx)

endfunction (run_sphinx_build)


# This sets up the setup.py for the python module
function(generate_setup_python)
    configure_file(
        ${parfis_SOURCE_DIR}/python-package/setup.py.in
        ${parfis_SOURCE_DIR}/python-package/setup.py
        @ONLY)
endfunction(generate_setup_python)