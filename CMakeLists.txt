cmake_minimum_required(VERSION 3.13)

# set project name
set(TARGET_NAME parallel_concurrent_transformation)
project (${TARGET_NAME})

set(CMAKE_CXX_STANDARD 17)

# setting output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build)

# adding library
set(LINK_LIBRARIES pthread)

# adding /include dir  
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# collecting headers and sources
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCE_EXTENSION)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/include HEADER_EXTENSION)

# adding headers and sources
add_executable(${TARGET_NAME} ${SOURCE_EXTENSION} ${HEADER_EXTENSION})

# linking the libraries
target_link_libraries(${TARGET_NAME} ${LINK_LIBRARIES})

