cmake_minimum_required(VERSION 3.29)
project(torrent-client)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

include_directories(.)
add_executable(torrent-client
        main.cpp
        bencode.h
)
target_link_libraries(torrent-client PRIVATE cpr::cpr)
