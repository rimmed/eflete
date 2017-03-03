cmake_minimum_required(VERSION 3.0)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

project(Eflete)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)

find_package(GTest REQUIRED)

enable_testing()

include_directories(
   ${CMAKE_SOURCE_DIR}/src/modules/project
   )

add_custom_target(check
   COMMAND ${CMAKE_CTEST_COMMAND}
   COMMENT "Running tests")

add_subdirectory(src)
