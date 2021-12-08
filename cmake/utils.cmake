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

function(set_version)
    execute_process(COMMAND git log --pretty=format:'%h' -n 1
                    OUTPUT_VARIABLE GIT_REV
                    ERROR_QUIET)
    # Check whether we got any revision (which isn't
    # always the case, e.g. when someone downloaded a zip
    # file from Github instead of a checkout)
    if ("${GIT_REV}" STREQUAL "")
        set(GIT_REV "N/A")
        set(GIT_DIFF "N/A")
        set(GIT_TAG "N/A")
        set(GIT_BRANCH "N/A")
        set(VERSION "N/A")
    else()
        execute_process(
            COMMAND bash -c "git diff --quiet --exit-code || echo +"
            OUTPUT_VARIABLE GIT_DIFF)
        execute_process(
            COMMAND git describe --tags
            OUTPUT_VARIABLE GIT_TAG ERROR_QUIET)
        execute_process(
            COMMAND git describe --tags --abbrev=0
            OUTPUT_VARIABLE VERSION ERROR_QUIET)
        execute_process(
            COMMAND git rev-parse --abbrev-ref HEAD
            OUTPUT_VARIABLE GIT_BRANCH)

        string(STRIP "${GIT_REV}" GIT_REV)
        string(SUBSTRING "${GIT_REV}" 1 7 GIT_REV)
        string(STRIP "${GIT_DIFF}" GIT_DIFF)
        string(STRIP "${GIT_TAG}" GIT_TAG)
        string(STRIP "${VERSION}" VERSION)
        string(STRIP "${GIT_BRANCH}" GIT_BRANCH)
    endif()

    set(VERSION_H "\
#ifndef PARFIS_VERSION_H
#define PARFIS_VERSION_H
#define GIT_REV \"${GIT_REV}${GIT_DIFF}\"
#define GIT_TAG \"${GIT_TAG}\"
#define GIT_BRANCH \"${GIT_BRANCH}\"
#define VERSION \"${VERSION}\"
#endif // PARFIS_VERSION_H
")
    set(VERSION "${VERSION}" PARENT_SCOPE)

    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h)
        file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h OLD_VERSION_H)        
    else()
        set(OLD_VERSION_H "")
    endif()
    if(NOT "${VERSION_H}" STREQUAL "${OLD_VERSION_H}")
        file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/include/version.h "${VERSION_H}")
    endif()
endfunction(set_version)