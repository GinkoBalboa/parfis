# Collect all .h and .cpp files from parfis directory
file(GLOB_RECURSE GTESTALL_SOURCES LIST_DIRECTORIES true
    ${parfis_SOURCE_DIR}/test/cpp/*.h 
    ${parfis_SOURCE_DIR}/test/cpp/*.cpp)

# Enable testing
# enable_testing()
include_directories(${parfis_SOURCE_DIR}/build/lib)

# Make all tests target
# Make executable for all tests
add_executable(gtestAll ${GTESTALL_SOURCES})
message("Google test executable = gtestAll")
target_include_directories(gtestAll PUBLIC
    $<BUILD_INTERFACE:${parfis_SOURCE_DIR}/build/lib/googletest/include>
    $<INSTALL_INTERFACE:test/googletest/include>  # <prefix>/include
)
# Link with parfis library
set_target_properties(gtestAll PROPERTIES SUFFIX "d")
set_output_directory(gtestAll ${parfis_SOURCE_DIR}/build/bin/gtestAll)
target_link_libraries(gtestAll PUBLIC parfis gtest)
#add_dependencies(gtestAll parfis ${parfis_SOURCE_DIR}/test/googletest/lib/gtest)
# Add test
add_test(NAME gtestAll COMMAND gtestAll)
