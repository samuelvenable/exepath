#!/bin/sh
cd "${0%/*}"
if [ $(uname) = "Darwin" ]; then
  clang++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall -arch arm64 -arch x86_64; ./getexecname;
elif [ $(uname) = "Linux" ]; then
  if [ -f "/bin/g++" ]; then
    g++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./getexecname;
  else
    clang++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall; ./getexecname;
  fi
elif [ $(uname) = "FreeBSD" ]; then
  clang++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall -static; ./getexecname;
elif [ $(uname) = "DragonFly" ]; then
  g++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./getexecname;
elif [ $(uname) = "NetBSD" ]; then
  g++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./getexecname;
elif [ $(uname) = "OpenBSD" ]; then
  clang++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall -lkvm -static; ./getexecname;
elif [ $(uname) = "SunOS" ]; then
  g++ getexecname/external.cpp external.cpp -o getexecname -std=c++17 -Wall -static-libgcc -lproc; ./getexecname;
else
  g++ getexecname/external.cpp external.cpp -o getexecname.exe -std=c++17 -Wall -static-libgcc -static-libstdc++ -static -Wl,--subsystem,console; ./getexecname.exe;
fi
