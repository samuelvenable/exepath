#!/bin/sh
cd "${0%/*}"
if [ $(uname) = "Darwin" ]; then
  clang++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall -arch arm64 -arch x86_64; ./__getexecname;
elif [ $(uname) = "Linux" ]; then
  if [ -f "/bin/g++" ]; then
    g++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./__getexecname;
  else
    clang++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall; ./__getexecname;
  fi
elif [ $(uname) = "FreeBSD" ]; then
  clang++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall -static; ./__getexecname;
elif [ $(uname) = "DragonFly" ]; then
  g++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./__getexecname;
elif [ $(uname) = "NetBSD" ]; then
  g++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall -static-libgcc -static-libstdc++ -static; ./__getexecname;
elif [ $(uname) = "OpenBSD" ]; then
  clang++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall -lkvm -static; ./__getexecname;
elif [ $(uname) = "SunOS" ]; then
  g++ __getexecname/external.cpp external.cpp -o __getexecname -std=c++17 -Wall -static-libgcc -lproc; ./__getexecname;
else
  g++ __getexecname/external.cpp external.cpp -o __getexecname.exe -std=c++17 -Wall -static-libgcc -static-libstdc++ -static -Wl,--subsystem,console; ./__getexecname.exe;
fi
