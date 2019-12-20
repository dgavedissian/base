cmake_minimum_required(VERSION 3.8)
project(base)

# Determine if this is built as a subproject (using add_subdirectory)
# or if it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif()

add_library(dga-base
    include/dga/aliases.h
    include/dga/barrier.h
    include/dga/fast_variant_visit.h
    include/dga/platform.h
    include/dga/scope.h
    include/dga/semaphore.h
    include/dga/string_algorithms.h
)
target_include_directories(dga-base PUBLIC include)
set_target_properties(dga-base PROPERTIES LINKER_LANGUAGE CXX)
target_compile_features(dga-base PUBLIC cxx_std_17)
set_target_properties(dga-base PROPERTIES CXX_EXTENSIONS OFF)

if(MASTER_PROJECT)
  add_subdirectory(tests)
endif()