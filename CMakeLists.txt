cmake_minimum_required(VERSION 3.15)
project(P2P_FileTransferProtocol VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard to Modern C++ (C++17 is great for modern features & cross-platform support)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories so our compiler can find files in the 'include' folder
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define source files
set(SOURCES
    src/socket.cpp
    # We will add socket.cpp, chunker.cpp, etc. here as we build them
)

# Create the executable binary
add_executable(p2p_transfer ${SOURCES})

# Cross-platform networking libraries link check
if(WIN32)
    # Windows requires linking against Winsock2
    target_link_libraries(p2p_transfer PRIVATE Ws2_32)
endif()