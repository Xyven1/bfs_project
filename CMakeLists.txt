cmake_minimum_required(VERSION 3.21.4 FATAL_ERROR)
project(bfs_project VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/name.basics.tsv
    ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/title.basics.tsv
    ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

# enable_testing()

add_executable(main main.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
