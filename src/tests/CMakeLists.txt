include_directories(${GTEST_INCLUDE_DIR})
set(COMMON_TEST_LINK_LIBRARIES
   libeflete
   GTest::GTest
   GTest::Main)

add_subdirectory(project)
