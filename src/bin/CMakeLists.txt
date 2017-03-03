#add headers to target to show them in IDE
add_executable(eflete
   eflete_main.cpp
   )
target_link_libraries(eflete libeflete)
