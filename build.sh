#!/bin/sh
cd "${0%/*}"
if [ $(uname) = "Darwin" ]; then
  clang++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall -arch arm64 -arch x86_64; ./a.out;
elif [ $(uname) = "Linux" ]; then
  if [ -f "/bin/g++" ]; then
    g++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./a.out;
  else
    clang++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall; ./a.out;
  fi
elif [ $(uname) = "FreeBSD" ]; then
  clang++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall -lelf -lkvm -lpthread -static; ./a.out;
elif [ $(uname) = "DragonFly" ]; then
  g++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc -static-libstdc++ -lkvm -lpthread -static; ./a.out;
elif [ $(uname) = "NetBSD" ]; then
  g++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc -static-libstdc++ -lkvm -lpthread -static; ./a.out;
elif [ $(uname) = "OpenBSD" ]; then
  clang++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall -lkvm -lpthread -static; ./a.out;
elif [ $(uname) = "SunOS" ]; then
  g++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc -lkvm -lproc; ./a.out;
else
  g++ pidpath/pidpath.cpp process/process.cpp main.cpp -o a.out.exe -std=c++17 -Wall -static-libgcc -static-libstdc++ -static -lntdll -Wl,--subsystem,console; ./a.out.exe;
fi
