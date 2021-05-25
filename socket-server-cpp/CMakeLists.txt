cmake_minimum_required(VERSION 3.16)

set(PROJ_NAME "darksky-socket-server")
project(${PROJ_NAME})

# The version number.
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set (${PROJ_NAME}_VERSION_MAJOR 1)
set (${PROJ_NAME}_VERSION_MINOR 0)
set (${PROJ_NAME}_BUILD_SHA ${GIT_COMMIT_HASH})

find_package(sdbus-c++ REQUIRED)

# configure a header file to pass some of the CMake settings
configure_file (
  "${PROJECT_SOURCE_DIR}/socket-server-version.hpp.in"
  "${PROJECT_BINARY_DIR}/socket-server-version.hpp"
)

# Setup include directories
include_directories(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_BINARY_DIR}
)

# Add Debug logger
set(DEBUGLOGGER_SRCS "debuglogger/debuglogger.cpp")

# Add socket-server
set(SOCKET_SERVER_SRCS
  "src/socket-server.cpp"
  "src/server.cpp"
)

set(SOURCE_FILES ${DEBUGLOGGER_SRCS}
                 ${SOCKET_SERVER_SRCS}
)

execute_process(COMMAND sdbus-c++-xml2cpp --verbose --adaptor=net.pdev.test.h ${CMAKE_SOURCE_DIR}/interfaces/net.pdev.test.xml)

add_executable(${PROJ_NAME} ${SOURCE_FILES})

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJ_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(${PROJ_NAME}
    PUBLIC z
    PRIVATE SDBusCpp::sdbus-c++
)

install(TARGETS ${PROJ_NAME} DESTINATION bin)
