#!/bin/sh
cd "${0%/*}"
if [ $(uname) = "Darwin" ]; then
  clang++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall -arch arm64 -arch x86_64; ./a.out;
elif [ $(uname) = "Linux" ]; then
  if [ -f "/bin/g++" ]; then
    g++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./a.out;
  else
    clang++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall; ./a.out;
  fi
elif [ $(uname) = "FreeBSD" ]; then
  clang++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall -static; ./a.out;
elif [ $(uname) = "DragonFly" ]; then
  g++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./a.out;
elif [ $(uname) = "NetBSD" ]; then
  g++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./a.out;
elif [ $(uname) = "OpenBSD" ]; then
  clang++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall -lkvm -static; ./a.out;
elif [ $(uname) = "SunOS" ]; then
  g++ exe/exe.cpp main.cpp -o a.out -std=c++17 -Wall -static-libgcc; ./a.out;
else
  g++ exe/exe.cpp main.cpp -o a.out.exe -std=c++17 -Wall -static-libgcc -static-libstdc++ -static -Wl,--subsystem,console; ./a.out.exe;
fi
