Build Instructions

1. git clone --recursive https://github.com/donatasm/netuv.git
2. cd netuv/libuv
3. git clone https://git.chromium.org/external/gyp.git build/gyp
4. vcbuild static x64 release nobuild
5. libuv -> Reload Project
6. Configuration Debug: libuv -> Configuration Properties -> C/C++ -> All Options -> Runtime Library -> Multi-threaded Debug DLL (/MDd)
7. Configuration Release: libuv -> Configuration Properties -> C/C++ -> All Options -> Runtime Library -> Multi-threaded DLL (/MD)