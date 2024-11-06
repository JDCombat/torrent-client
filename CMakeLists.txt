cmake_minimum_required(VERSION 3.29)
project(torrent-client)

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(torrent-client
        main.cpp
        bencode.h
)
