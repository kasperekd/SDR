cmake_minimum_required(VERSION 3.10)
project(QAMModem)

set(CMAKE_C_STANDARD 11)

add_library(qam SHARED src/qam/qam.c)
add_library(utils SHARED src/utils/utils.c)

include_directories(src/include)

add_executable(qam_main src/qam_main.c)
add_executable(deqam_main src/deqam_main.c)

target_link_libraries(qam_main qam utils m)
target_link_libraries(deqam_main qam utils m)