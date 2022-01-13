cmake_minimum_required(VERSION 3.2)
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
        include/message.h
        include/pack.h
        include/board.h
        include/checkers_engine.h
        )

set(SOURCES
        src/main.cpp
        src/game_session.cpp
        src/socket.cpp
        src/message.cpp
        src/checkers_engine.cpp
        )
add_executable(CheckersTcpServer ${HEADERS} ${SOURCES})
target_link_libraries(CheckersTcpServer PRIVATE spdlog::spdlog)