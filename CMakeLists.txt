cmake_minimum_required(VERSION 3.21.4 FATAL_ERROR)
project(cuda_test VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CUDA_COMPILER "/usr/local/cuda-11.5/bin/nvcc")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/name.basics.tsv
    ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/title.basics.tsv
    ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# enable_testing()
enable_language(CUDA)

add_executable(cuda_test main.cpp)
add_subdirectory(kernels)

set_target_properties(cuda_test PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(cuda_test kernels)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
