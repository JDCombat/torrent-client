cmake_minimum_required(VERSION 3.29)
project(torrent-client)

set(CMAKE_CXX_STANDARD 20)

add_link_options(-lssl -lcrypto)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG dd967cb48ea6bcbad9f1da5ada0db8ac0d532c06) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(portfwd GIT_REPOSITORY https://github.com/RJ/libportfwd.git
        GIT_TAG 0e93633e7fddd1146eabd502a96923730ec60b85)
FetchContent_MakeAvailable(portfwd)

include_directories(.)
add_executable(torrent-client
        main.cpp
        bencode.h
        downloader.h
        clientSocket.h
        serverSocket.h
)
target_link_libraries(torrent-client PRIVATE cpr::cpr)
target_link_libraries(torrent-client PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(torrent-client PRIVATE portfwd)
