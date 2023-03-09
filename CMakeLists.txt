cmake_minimum_required(VERSION 3.8)
project(Simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_BUILD_TYPE "RelWithDebInfo")
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

find_package(Threads)

add_subdirectory(third-party)

get_target_property(gflags_INCLUDE_DIR gflags INCLUDE_DIRECTORIES)
include_directories(${gflags_INCLUDE_DIR})
list(APPEND THIRDPARTY_LIBS gflags)

set(
  SOURCE_FILE 
  src/zbd_fs.cc
  src/histogram.cc
)
add_library(zbd_fs ${SOURCE_FILE})

add_executable(zns_bench src/zns_bench.cc)
target_link_libraries(zns_bench zbd_fs zbd ${THIRDPARTY_LIBS} ${CMAKE_THREAD_LIBS_INIT})