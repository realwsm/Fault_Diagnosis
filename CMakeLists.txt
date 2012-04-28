project(Fault_Diagnosis CXX)
cmake_minimum_required(VERSION 2.6)

#set(PROJECT_BINARY_DIR ${PROJECT_HOME_DIRECTORY}/Build/bin/)
#set(PROJECT_SOURCE_DIR ${PROJECT_HOME_DIRECTORY}/src/)
set(EXECUTABLE_OUTPUT_PATH ${Fault_Diagnosis_SOURCE_DIR}/BUILD/)
set(ARCHIVE_OUTPUT_DIRECTORY ${Fault_Diagnosis_SOURCE_DIR}/BUILD/)
set(LIBRARY_OUTPUT_DIRECTORY ${Fault_Diagnosis_SOURCE_DIR}/BUILD/)

add_subdirectory(src)
