# Base library
# Written by David Avedissian (c) 2018-2020 (git@dga.dev)

# Pull google test.
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.10.0.tar.gz
    URL_MD5 ecd1fa65e7de707cd5c00bdac56022cd
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Add tests
macro(dga_add_test NAME)
    add_executable(${NAME} ${NAME}.cpp)
    target_compile_features(${NAME} PRIVATE cxx_std_17)
    target_link_libraries(${NAME} dga-base gtest gmock gtest_main)
    gtest_add_tests(
        TARGET ${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endmacro()

dga_add_test(flags_test)
dga_add_test(scope_test)
dga_add_test(string_algorithms_test)
dga_add_test(result_test)
