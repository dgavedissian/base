cmake_minimum_required(VERSION 3.8)
project(base)

# Determine if this is built as a subproject (using add_subdirectory)
# or if it is the master project.
set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif()

# Base library.
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

# Patch tool.
macro(make_patch_tool_available)
  if(WIN32)
    # Hilariously, Windows won't run the tool without admin privileges if it's called patch.exe. Must be some kind of
    # heuristic against viruses.
    set(PATCH_TOOL ${CMAKE_CURRENT_SOURCE_DIR}/tools/ptool.exe)
  else()
    set(PATCH_TOOL patch)
  endif()
endmacro()

if(MASTER_PROJECT)
  add_subdirectory(tests)
endif()
