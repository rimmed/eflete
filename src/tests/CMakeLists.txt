include_directories(${GTEST_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(COMMON_TEST_LINK_LIBRARIES
   libeflete
   GTest::GTest
   GTest::Main
   gmock)

add_subdirectory(filesystem)
add_subdirectory(project)
