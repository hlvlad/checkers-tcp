cmake_minimum_required(VERSION 3.25)
project(CheckersTcpServer)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
set(CMAKE_PREFIX_PATH third-party)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

set(HEADERS
        include/game_session.h
        include/socket.h
        include/message_handler.h
)

set(SOURCES
        src/main.cpp
        src/game_session.cpp
        src/socket.cpp
        src/message_handler.cpp
)

add_executable(CheckersTcpServer ${HEADERS} ${SOURCES})
target_include_directories(CheckersTcpServer PRIVATE include)
target_link_libraries(CheckersTcpServer PRIVATE spdlog::spdlog CheckersTcpCore PackUnpack)