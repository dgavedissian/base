cmake_minimum_required(VERSION 3.15)
project(cpp-utils)

set(CMAKE_CXX_STANDARD 17)

add_library(barrier include/barrier/barrier.h)
target_include_directories(barrier PUBLIC include)
set_target_properties(barrier PROPERTIES LINKER_LANGUAGE CXX)

add_library(fast_variant_visit include/fast_variant_visit/fast_variant_visit.h)
target_include_directories(fast_variant_visit PUBLIC include)
set_target_properties(fast_variant_visit PROPERTIES LINKER_LANGUAGE CXX)

add_library(semaphore include/semaphore/semaphore.h)
target_include_directories(semaphore PUBLIC include)
set_target_properties(semaphore PROPERTIES LINKER_LANGUAGE CXX)

add_library(str_algorithms include/str_algorithms/str_algorithms.h)
target_include_directories(str_algorithms PUBLIC include)
set_target_properties(str_algorithms PROPERTIES LINKER_LANGUAGE CXX)

add_executable(cpp-utils main.cpp)
target_link_libraries(cpp-utils barrier fast_variant_visit semaphore str_algorithms)