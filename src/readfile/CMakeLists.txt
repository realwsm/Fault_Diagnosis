include_directories(${PROJECT_SOURCE_DIR}/src/include/)
aux_source_directory(. READFILE_SRC)

add_library(readfile SHARED ${READFILE_SRC})