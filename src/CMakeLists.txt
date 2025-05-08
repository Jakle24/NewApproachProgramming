cmake_minimum_required(VERSION 3.10)
project(NewApproachProgramming)

set(CMAKE_CXX_STANDARD 17)

# Add source files
set(SOURCES
    src/main.cpp
    src/TCPServer.cpp
    src/TCPClient.cpp
    src/LogProcessor.cpp
)

# Include headers
include_directories(src)

add_executable(NewApproachProgramming ${SOURCES})

# Link Winsock2 on Windows
if (WIN32)
    target_link_libraries(NewApproachProgramming ws2_32)
endif()