cmake_minimum_required(VERSION 3.13)
project(testecg)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_PREFIX_PATH "C:/mingw64")

add_executable(testecg main.cpp)

include_directories(C:/mingw64/include)

target_link_libraries(testecg -lOpenGL32 -lfreeGLUT -lglu32)