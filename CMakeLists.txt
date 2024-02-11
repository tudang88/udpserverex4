cmake_minimum_required(VERSION 3.28.1)
project(udpserver VERSION 0.1.0 LANGUAGES C)

set(CLIENT_PROJECT "udpclient")
set(SERVER_PROJECT "udpserver")
set(MY_LIB "common_lib")
# #################
# library
# #################
set(MY_LIB_SOURCES
    lib/message.c
    lib/utils.c)
add_library(${MY_LIB} STATIC ${MY_LIB_SOURCES})
set_target_properties(${MY_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
target_include_directories(${MY_LIB} PUBLIC ${PROJECT_SOURCE_DIR}/inc)

find_library(${MY_LIB} PATHS ${PROJECT_SOURCE_DIR}/lib)

# #################
# client
# #################
set(CLIENT_SOURCES
    client/main.c
    client/client.c)

add_executable(${CLIENT_PROJECT} ${CLIENT_SOURCES})
target_include_directories(${CLIENT_PROJECT} PUBLIC ${PROJECT_SOURCE_DIR}/inc)
target_link_libraries(${CLIENT_PROJECT} PRIVATE ${MY_LIB})
set_target_properties(${CLIENT_PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)

# #################
# server
# #################
set(SERVER_SOURCES
    server/main.c
    server/server.c)
add_executable(${SERVER_PROJECT} ${SERVER_SOURCES})
target_include_directories(${SERVER_PROJECT} PUBLIC ${PROJECT_SOURCE_DIR}/inc)
target_link_libraries(${SERVER_PROJECT} PRIVATE ${MY_LIB})
set_target_properties(${SERVER_PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/out)
