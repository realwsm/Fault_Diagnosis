project(FaultDiagnosis CXX)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_C_COMPILER "clang++")
set(CMAKE_CXX_COMPILER "clang++")
#set(PROJECT_BINARY_DIR ${PROJECT_HOME_DIRECTORY}/Build/bin/)
#set(PROJECT_SOURCE_DIR ${PROJECT_HOME_DIRECTORY}/src/)

IF(MYPROJECT_DEBUG)
  add_definitions(-DDEBUG)
ENDIF(MYPROJECT_DEBUG)

set(EXECUTABLE_OUTPUT_PATH ${FaultDiagnosis_SOURCE_DIR}/BUILD/)
set(ARCHIVE_OUTPUT_DIRECTORY ${FaultDiagnosis_SOURCE_DIR}/BUILD/)
set(LIBRARY_OUTPUT_PATH ${FaultDiagnosis_SOURCE_DIR}/BUILD/)

add_subdirectory(src)
