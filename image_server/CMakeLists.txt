cmake_minimum_required(VERSION 2.8)
project(process_image)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -I/usr/include/python3.10 -Wno-unused-result -Wsign-compare -g -fopenmp -fstack-protector-strong -Wformat -Werror=format-security -DNDEBUG -g -fwrapv -O2 -Wall")

find_package(PkgConfig)
pkg_check_modules(PC_ZeroMQ QUIET zmq)

find_path(ZeroMQ_INCLUDE_DIR NAMES zmq.hpp PATHS ${PC_ZeroMQ_INCLUDE_DIRS})
find_library(ZeroMQ_LIBRARY NAMES zmq PATHS ${PC_ZeroMQ_LIBRARY_DIRS})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(process_image SHARED
            ProcessImage.cpp
            linked_list.h
            thread_utils.h
            PyImgReader.hpp
            PyImgWriter.hpp)

target_link_libraries(process_image PRIVATE ${OpenCV_LIBS} ${ZeroMQ_LIBRARY} -L/usr/lib/python3.10/config-3.10-x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -lpython3.10 -lcrypt -ldl -lpthread -lm -lm)
