cmake_minimum_required(VERSION 3.25)
project(CheckersTcp)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/third-party/spdlog)

add_subdirectory(third-party/pack_unpack)

add_subdirectory(checkers-tcp-core)
add_subdirectory(checkers-tcp-client)
add_subdirectory(checkers-tcp-server)
