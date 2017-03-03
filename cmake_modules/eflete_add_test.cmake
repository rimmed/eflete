add_executable(test_${TEST_NAME} EXCLUDE_FROM_ALL
   ${TEST_SOURCES}
   ${TEST_HEADERS}
   )

target_link_libraries(test_${TEST_NAME}
   ${COMMON_TEST_LINK_LIBRARIES}
   )

add_test(${TEST_NAME} test_${TEST_NAME} DEPENDS add_dependencies)
add_dependencies(check test_${TEST_NAME})
