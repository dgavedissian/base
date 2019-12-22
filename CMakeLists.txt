cmake_minimum_required(VERSION 3.8)
project(base)

# Determine if this is built as a subproject (using add_subdirectory)
# or if it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif()

add_library(dga-base INTERFACE)
target_sources(dga-base INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dga/aliases.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dga/barrier.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dga/fast_variant_visit.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dga/platform.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dga/scope.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dga/semaphore.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/dga/string_algorithms.h
)
target_include_directories(dga-base INTERFACE include)

if(MASTER_PROJECT)
  add_subdirectory(tests)
endif()
