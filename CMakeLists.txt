cmake_minimum_required(VERSION 2.8.1)
project(client)


set (SOURCE_DIR             ${PROJECT_SOURCE_DIR}/src)
set (INCLUDE_DIR            ${PROJECT_SOURCE_DIR}/include)

set(CMAKE_CXX_STANDARD 11)

include_directories(${INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -ffunction-sections -O0")




#client
add_executable(client
        ${SOURCE_DIR}/main.cpp
        ${SOURCE_DIR}/message.cpp
        ${SOURCE_DIR}/cJSON.c
)
#target_include_directories(client)

#server

# Library dependent

#target_link_libraries(client ${PROTOBUF_LIBRARIES})

