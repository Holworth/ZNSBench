cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

PROJECT(Simulator-thirdparty)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

INCLUDE(ExternalProject)

IF(TARGET gflags)
  MESSAGE("[ZNSSimulator/third-party] gflags target existed, re-use it!")
ELSE()
  MESSAGE("[ZNSSimulator/third-party] gflags target not exist, let's build it!")
  ExternalProject_Add(gflags-project
      SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gflags
      CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/gflags -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR} -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS=-fPIC -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}
      BUILD_IN_SOURCE 0
      BUILD_COMMAND $(MAKE)
      INSTALL_COMMAND $(MAKE) install
  )
  ADD_LIBRARY(gflags STATIC IMPORTED GLOBAL)
  ADD_DEPENDENCIES(gflags gflags-project)
  SET_TARGET_PROPERTIES(gflags PROPERTIES
    IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libgflags.a
    INCLUDE_DIRECTORIES ${CMAKE_BINARY_DIR}/include
    POSITION_INDEPENDENT_CODE ON
  )
ENDIF() # end target