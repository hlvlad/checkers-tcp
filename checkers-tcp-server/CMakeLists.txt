cmake_minimum_required(VERSION 3.2)
project(checkers_server_tcp)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
set(CMAKE_PREFIX_PATH third-party)

if(NOT TARGET spdlog)
    find_package(spdlog REQUIRED)
endif()

set(HEADERS
        game_session.h
        socket.h
        message.h
        include/pack.h
        new_misc.h
        checkers_engine.h
        )

set(SOURCES
        main.cpp
        game_session.cpp
        socket.cpp
        message.cpp
        checkers_engine.cpp
        )
add_executable(checkers_server_tcp ${HEADERS} ${SOURCES})
target_link_libraries(checkers_server_tcp PRIVATE spdlog::spdlog)