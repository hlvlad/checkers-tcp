cmake_minimum_required(VERSION 3.25)

project(PackUnpack)

set(HEADERS
    include/pack.h
)

set(SOURCES
    src/pack.cpp
)

add_library(PackUnpack ${HEADERS} ${SOURCES})
target_include_directories(PackUnpack PUBLIC include)