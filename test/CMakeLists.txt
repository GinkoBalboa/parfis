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
get_target_property(GTEST_IMPORTED_LOCATION gtest IMPORTED_LOCATION)
get_target_property(GTEST_FILE_DIR gtest FILE_DIR)
get_target_property(GTEST_FILE_NAME gtest FILE_NAME)
get_target_property(PARFIS_FILE_DIR parfis FILE_DIR)
get_target_property(PARFIS_FILE_NAME parfis FILE_NAME)

set_properties(gtestAll EXECUTABLE)
set_output_directory(gtestAll ${parfis_SOURCE_DIR}/build/bin/gtestAll)
target_link_libraries(gtestAll PUBLIC parfis ${GTEST_IMPORTED_LOCATION})
add_dependencies(gtestAll parfis gtest)
add_custom_command(TARGET gtestAll POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${GTEST_FILE_DIR}/${GTEST_FILE_NAME} 
        ${parfis_SOURCE_DIR}/build/bin/gtestAll/${GTEST_FILE_NAME})
add_custom_command(TARGET gtestAll POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy 
        ${PARFIS_FILE_DIR}/${PARFIS_FILE_NAME} 
        ${parfis_SOURCE_DIR}/build/bin/gtestAll/${PARFIS_FILE_NAME})
# Add test
add_test(NAME gtestAll COMMAND gtestAll)
