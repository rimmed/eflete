#add headers to target to show them in IDE

PROTOBUF_GENERATE_CPP(PROJECT_PROTO_CPP PROJECT_PROTO_HPP project/project.proto)

add_library(libeflete STATIC
   ${PROJECT_PROTO_CPP}
   project/project.cpp
   project/project.hpp
   )

target_include_directories(libeflete PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(libeflete ${PROTOBUF_LIBRARY})
