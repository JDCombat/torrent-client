# Torrent client

A proof of concept torrent client fo a school project

**[Bittorrent protocol](https://www.bittorrent.org/beps/bep_0003.html)**
**[UTP protocol](https://www.bittorrent.org/beps/bep_0029.html)**

![it's so over](https://pbs.twimg.com/media/F6aX6mJWYAAvYnl?format=jpg&name=small)

Well that was quite of a ride, huh?
A status of what works then:

## What works
- [x] Opening a torrent file
- [x] Reading and decoding it
- [x] Parsing it to get data easily
- [x] Calculating SHA1 hash of info dictionary
- [x] Making an HTTP announce request
- [x] Decoding response from announce and getting a list of peers
- [x] Trying NATPMP to open a port for connecting (won't work everywhere)
- [x] Partial: Opening an utp connection to peers

## What doesn't work
- At the time of writing still can't do a handshake with peer
- Downloading obviously because of the above
- As well as uploading
- Requests to UDP announces (might add soon because it's ez)
- Telling apart UTP and TCP peers
- For the reason above only trying UTP because it's more popular in the public

## System requirements
Works for sure on Linux and POSIX systems. Windows currently doesn't work because of the sockets :(

You need
```requirements
openssl cmake
```

# Usage
Open a file and start operating on it
```terminaloutput
torrent-client -f <file to open>
```
Get a list of avalible args
```terminaloutput
torrent-client -h
```

The program will print all the information about the file and prompt for continuation.
After confirmation requests announce


Networks are hard :(