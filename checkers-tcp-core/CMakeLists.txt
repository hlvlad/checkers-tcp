cmake_minimum_required(VERSION 3.25)
project(CheckersTcpCore)

set(CMAKE_CXX_STANDARD 20)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

set(HEADERS
    include/board.h
    include/checkers_engine.h
    include/message.h
)

set(SOURCES
    src/checkers_engine.cpp
    src/message.cpp
)

add_library(CheckersTcpCore ${HEADERS} ${SOURCES})
target_include_directories(CheckersTcpCore PUBLIC include)
target_link_libraries(CheckersTcpCore PRIVATE spdlog::spdlog PackUnpack)