include_directories(${PROJECT_SOURCE_DIR}/src/include/)
aux_source_directory(. IISF_FID_SRC)

add_library(IisfFid SHARED ${IISF_FID_SRC})