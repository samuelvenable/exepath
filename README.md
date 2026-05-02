# Cross-Platform: Executable Path from Self or PID
- A reimplementation of the Solaris and illumos `getexecname()` function for a wide variety of platforms
- Supports Windows, macOS, Linux, FreeBSD, DragonFly BSD, NetBSD, OpenBSD, Solaris, illumos, and Android
- `#include <__getexecname/external.h>` to return the absolute path name to an executable file from PID
- `#include <__getexecname/internal.h>` to return the absolute path name to the current executable file
```cpp
// If PID argument is negative one or omitted completely return the absolute path name to the current executable file
const char *__external = __getexecname(int pid = -1); // return the absolute path name to an executable file from PID
const char *__internal = __getexecname();             // return the absolute path name to the current executable file
```
