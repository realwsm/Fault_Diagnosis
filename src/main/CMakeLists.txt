#include_directories(${PROJECT_SOURCE_DIR}/src/include/)
include_directories(${PROJECT_SOURCE_DIR}/src/include/)
aux_source_directory(. MAIN_SRC)

add_executable(SelectP ${MAIN_SRC})
set(Lib_All readfile)
target_link_libraries(SelectP ${Lib_All})