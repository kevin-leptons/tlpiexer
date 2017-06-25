cmake_minimum_required(VERSION 3.0)
project(tlpiexer)

# flags
set(CMAKE_C_FLAGS "-std=gnu99 -Wall -g3 -lrt -pthread -lcrypt -ldl -lrt")

# define
add_definitions(-D_FILE_OFFSET_BITS=64)

# setup build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# setup rpath
#SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
#SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
#set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")

# setup header files
include_directories(include)

# setup sub directory
add_subdirectory(src)
