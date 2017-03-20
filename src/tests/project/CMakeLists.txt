set(TEST_NAME project)

PROTOBUF_GENERATE_CPP(TEST_SAVER_PROTO_CPP TEST_SAVER_PROTO_HPP test_saver.proto)

set(TEST_SOURCES
   test_project.cpp
   test_saver.cpp
   ${TEST_SAVER_PROTO_CPP}
   )

#add headers to target to show them in IDE
set(TEST_HEADERS
   )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

include(eflete_add_test)
