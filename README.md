# Cross-Platform: Executable Path from Self or PID
```c
// A reimplementation of the Solaris and illumos `getexecname()` function for a wide variety of platforms
// Supports Windows, macOS, Linux, FreeBSD, DragonFly BSD, NetBSD, OpenBSD, Solaris, illumos, and Android
#include <__getexecname/external.h> // return the absolute path name to an executable file from PID
#include <__getexecname/internal.h> // return the absolute path name to the current executable file
// If PID argument is -1 or omitted from the function, return the absolute path name to the current executable file
const char *external = __getexecname(int pid = -1); // return the absolute path name to an executable file from PID
const char *internal = __getexecname();             // return the absolute path name to the current executable file
```
