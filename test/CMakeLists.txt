# Collect all .h and .cpp files from parfis directory
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true
    ${parfis_SOURCE_DIR}/test/src/*.h 
    ${parfis_SOURCE_DIR}/test/src/*.cpp)

# Enable testing
# enable_testing()
include_directories(${parfis_SOURCE_DIR}/lib/googletest/include)

# Make all tests target
# Make executable for all tests
add_executable(gtestAll ${SOURCES})
message("Google test executable = gtestAll")
target_include_directories(gtestAll PUBLIC
    $<BUILD_INTERFACE:${parfis_SOURCE_DIR}/lib/googletest/include>
    $<INSTALL_INTERFACE:lib/googletest/include>  # <prefix>/include
)
# Link with parfis library
set_output_directory(gtestAll ${parfis_SOURCE_DIR})
target_link_libraries(gtestAll PUBLIC parfis gtest)
#add_dependencies(gtestAll parfis ${parfis_SOURCE_DIR}/test/googletest/lib/gtest)
# Add test
add_test(NAME gtestAll COMMAND gtestAll)
