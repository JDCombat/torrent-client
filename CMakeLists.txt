cmake_minimum_required(VERSION 3.29)
project(torrent-client)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(Nowy_folder
    bencode.hpp
    main.cpp)
