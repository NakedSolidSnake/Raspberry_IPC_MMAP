cmake_minimum_required(VERSION 3.7.0)

project(mmap)

set (CMAKE_C_FLAGS "-D_GNU_SOURCE -Wall -std=c99")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_BUILD_TYPE Debug)

#build led_process
add_executable(led_process "")

target_link_libraries(led_process PUBLIC hardware)

target_sources(led_process
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src/led_process.c
  )

target_include_directories(led_process 
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib  
  )

install(TARGETS led_process DESTINATION bin)

#built_button_process
add_executable(button_process "")

target_link_libraries(button_process PUBLIC hardware)

target_sources(button_process
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src/button_process.c
  )

target_include_directories(button_process 
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/lib  
  )
  
install(TARGETS button_process DESTINATION bin)

#build launch_processes
add_executable(launch_processes "")

target_sources(launch_processes
  PUBLIC 
  ${CMAKE_CURRENT_LIST_DIR}/src/launch_processes.c
  )
 
install(TARGETS launch_processes DESTINATION bin)

file(COPY config/kill_process.sh DESTINATION bin)