cmake_minimum_required(VERSION 3.16)
project(PlutoSDR CXX C)

find_library(LIBIIO_LIBRARIES iio)
message(${LIBIIO_LIBRARIES})

include_directories(src/include)
add_library(my_init SHARED src/my_init/my_init.cpp)
add_library(my_utils SHARED src/my_utils/my_utils.cpp)
add_library(my_config SHARED src/my_config/my_config.cpp)

set(MAIN_SOURCE_FILES
    src/main.cpp
)

add_executable(main ${MAIN_SOURCE_FILES})

target_link_libraries(main my_init my_utils my_config ${LIBIIO_LIBRARIES} m)