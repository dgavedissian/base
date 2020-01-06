add_executable(tests main.cpp)
target_compile_features(tests PRIVATE cxx_std_17)
target_link_libraries(tests dga-base)
