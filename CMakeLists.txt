cmake_minimum_required(VERSION 3.13)  # CMake version check
project(QuizCli)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES main.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -lrt -lPocoNetSSL -lPocoFoundation -lPocoNet -lPocoUtil")
set(SOURCE_FILES main.cpp)
# Add executable target with source files listed in SOURCE_FILES variable
add_executable(quizCli.a ${SOURCE_FILES})
# set the POCO paths and libs
set(POCO_PREFIX "/usr/local") # the directory containing "include" and "lib"
set(POCO_INCLUDE_DIR"${POCO_PREFIX}/include")
set(POCO_LIB_DIR "${POCO_PREFIX}/lib")
set(POCO_LIBS
        "${POCO_LIB_DIR}/libPocoNet.so"
        "${POCO_LIB_DIR}/libPocoUtil.so"
        "${POCO_LIB_DIR}/libPocoFoundation.so")
