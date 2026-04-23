/*

 MIT License
 
 Copyright © 2026 Samuel Venable
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
*/

#include "pidexepath.hpp"
#include <cerrno>
#if defined(_WIN32)
#include <vector>
#include <cstddef>
#include <cstdlib>
#include <windef.h>
#include <stringapiset.h>
#include <processthreadsapi.h>
#include <securitybaseapi.h>
#include <libloaderapi.h>
#include <handleapi.h>
#include <winbase.h>
#include <intsafe.h>
#include <winnt.h>
#include <ntdef.h>
#elif (defined(__APPLE__) && defined(__MACH__))
#include <climits>
#include <cstdlib>
#include <libproc.h>
#include <unistd.h>
#elif defined(__linux__)
#include <climits>
#include <cstdlib>
#include <unistd.h>
#elif defined(__FreeBSD__)
#include <cstddef>
#include <climits>
#include <cstdlib>
#include <sys/sysctl.h>
#elif defined(__DragonFly__)
#include <cstddef>
#include <climits>
#include <cstdlib>
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(__NetBSD__)
#include <cstddef>
#include <climits>
#include <cstdlib>
#include <sys/param.h>
#include <sys/sysctl.h>
#elif defined(__OpenBSD__)
#include <vector>
#include <sstream>
#include <cstddef>
#include <climits>
#include <cstdlib>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <kvm.h>
#elif defined(__sun)
#include <climits>
#include <cstdlib>
#include <libproc.h>
#include <unistd.h>
#endif

std::string get_executable_path(int process_id) {
  std::string path;
  if (process_id < -1) {
    errno = EINVAL;
    return path;
  }
  #if defined(_WIN32)
  auto narrow = [](std::wstring wstr) {
    if (wstr.empty()) return std::string("");
    int nbytes = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), nullptr, 0, nullptr, nullptr);
    std::vector<char> buf(nbytes);
    return std::string { buf.data(), (std::size_t)WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.length(), buf.data(), nbytes, nullptr, nullptr) };
  };
  auto open_process_with_debug_privilege = [](int process_id) {
    HANDLE process = nullptr;
    HANDLE hToken = nullptr;
    LUID luid;
    TOKEN_PRIVILEGES tkp;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
      if (LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid)) {
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = luid;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        if (AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), nullptr, nullptr)) {
          process = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)process_id);
        }
      }
      CloseHandle(hToken);
    }
    if (!process) {
      process = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)process_id);
    }
    return process;
  };
  if (process_id == -1 || process_id == (int)GetCurrentProcessId()) {
    wchar_t buffer[MAX_PATH];
    if (GetModuleFileNameW(nullptr, buffer, sizeof(buffer))) {
      wchar_t exe[MAX_PATH];
      if (_wfullpath(exe, buffer, MAX_PATH)) {
        path = narrow(exe);
      }
    }
  } else {
    HANDLE process = open_process_with_debug_privilege(process_id);
    if (!process) { 
      return path;
    }
    wchar_t buffer[MAX_PATH];
    DWORD size = sizeof(buffer);
    if (QueryFullProcessImageNameW(process, 0, buffer, &size)) {
      wchar_t exe[MAX_PATH];
      if (_wfullpath(exe, buffer, MAX_PATH)) {
        path = narrow(exe);
      }
    }
    CloseHandle(process);
  }
  #elif (defined(__APPLE__) && defined(__MACH__))
  char exe[PROC_PIDPATHINFO_MAXSIZE];
  if (proc_pidpath((process_id == -1) ? getpid() : process_id, exe, sizeof(exe)) > 0) {
    char buffer[PATH_MAX];
    if (realpath(exe, buffer)) {
      path = buffer;
    }
  }
  #elif defined(__linux__)
  char exe[PATH_MAX];
  if (process_id == -1 || process_id == getpid()) {
    if (realpath("/proc/self/exe", exe)) {
      path = exe;
    }
  } else {
    if (realpath((std::string("/proc/") + std::to_string(process_id) + 
      std::string("/exe")).c_str(), exe)) {
      path = exe;
    }
  }
  #elif defined(__FreeBSD__) || defined(__DragonFly__)
  int mib[4]; 
  std::size_t len = 0;
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC;
  mib[2] = KERN_PROC_PATHNAME;
  mib[3] = process_id;
  if (sysctl(mib, 4, nullptr, &len, nullptr, 0) == 0) {
    std::string strbuff;
    strbuff.resize(len, '\0');
    char *exe = strbuff.data();
    if (sysctl(mib, 4, exe, &len, nullptr, 0) == 0) {
      char buffer[PATH_MAX];
      if (realpath(exe, buffer)) {
        path = buffer;
      }
    }
  }
  #elif defined(__NetBSD__)
  int mib[4]; 
  std::size_t len = 0;
  mib[0] = CTL_KERN;
  mib[1] = KERN_PROC_ARGS;
  mib[2] = process_id;
  mib[3] = KERN_PROC_PATHNAME;
  if (sysctl(mib, 4, nullptr, &len, nullptr, 0) == 0) {
    std::string strbuff;
    strbuff.resize(len, '\0');
    char *exe = strbuff.data();
    if (sysctl(mib, 4, exe, &len, nullptr, 0) == 0) {
      char buffer[PATH_MAX];
      if (realpath(exe, buffer)) {
        path = buffer;
      }
    }
  }
  #elif defined(__OpenBSD__)
  auto is_exe = [](int process_id, std::string exe) {
    int cntp = 0;
    std::string res;
    kvm_t *kd = nullptr;
    kinfo_file *kif = nullptr;
    bool error = false;
    kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
    if (!kd) return res;
    if ((kif = kvm_getfiles(kd, KERN_FILE_BYPID, (process_id == -1) ? getpid() : process_id, sizeof(struct kinfo_file), &cntp))) {
      for (int i = 0; i < cntp && kif[i].fd_fd < 0; i++) {
        if (kif[i].fd_fd == KERN_FILE_TEXT) {
          struct stat st;
          fallback:
          char buffer[PATH_MAX];
          if (!stat(exe.c_str(), &st) && (st.st_mode & S_IXUSR) &&
            (st.st_mode & S_IFREG) && realpath(exe.c_str(), buffer) &&
            st.st_dev == (dev_t)kif[i].va_fsid && st.st_ino == (ino_t)kif[i].va_fileid) {
            res = buffer;
          }
          if (res.empty() && !error) {
            error = true;
            std::size_t last_slash_pos = exe.find_last_of("/");
            if (last_slash_pos != std::string::npos) {
              exe = exe.substr(0, last_slash_pos + 1) + kif[i].p_comm;
              goto fallback;
            }
          }
          break;
        }
      }
    }
    kvm_close(kd);
    return res;
  };
  auto envvar_value_from_process_id = [](int process_id, std::string name) {
    if (process_id == -1 || process_id == getpid()) {
      const char *cvalue = getenv(name.c_str());
      return std::string(cvalue ? cvalue : "");
    }
    auto environ_from_process_id = [](int process_id) {
      std::vector<std::string> vec;
      int cntp = 0;
      kvm_t *kd = nullptr;
      kinfo_proc *process_info = nullptr;
      kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
      if (!kd) {
        return vec;
      }
      if ((process_info = kvm_getprocs(kd, KERN_PROC_PID, process_id, sizeof(struct kinfo_proc), &cntp))) {
        char **env = kvm_getenvv(kd, process_info, 0);
        if (env) {
          for (int i = 0; env[i]; i++) {
            vec.push_back(env[i]);
          }
        }
      }
      kvm_close(kd);
      return vec;
    };
    auto string_split_by_first_equals_sign = [](std::string str) {
      std::size_t pos = 0;
      std::vector<std::string> vec;
      if ((pos = str.find_first_of("=")) != std::string::npos) {
        vec.push_back(str.substr(0, pos));
        vec.push_back(str.substr(pos + 1));
      }
      return vec;
    };
    std::string value;
    if (name.empty()) {
      return value;
    }
    std::vector<std::string> vec = environ_from_process_id(process_id);
    if (!vec.empty()) {
      for (std::size_t i = 0; i < vec.size(); i++) {
        std::vector<std::string> equalssplit = string_split_by_first_equals_sign(vec[i]);
        if (equalssplit.size() == 2) {
          if (equalssplit[0] == name) {
            value = equalssplit[1];
            break;
          }
        }
      }
    }
    return value;
  };
  int cntp = 0;
  kvm_t *kd = nullptr;
  kinfo_proc *process_info = nullptr;
  std::vector<std::string> buffer;
  bool error = false, retried = false;
  kd = kvm_openfiles(nullptr, nullptr, nullptr, KVM_NO_FILES, nullptr);
  if (!kd) {
    path.clear();
    return path;
  }
  if ((process_info = kvm_getprocs(kd, KERN_PROC_PID, (process_id == -1) ? getpid() : process_id, sizeof(struct kinfo_proc), &cntp))) {
    char **cmd = kvm_getargv(kd, process_info, 0);
    if (cmd) {
      for (int i = 0; cmd[i]; i++) {
        buffer.push_back(cmd[i]);
      }
    }
  }
  kvm_close(kd);
  if (!buffer.empty()) {
    std::string argv0;
    if (!buffer[0].empty()) {
      fallback:
      std::size_t slash_pos = buffer[0].find('/');
      std::size_t colon_pos = buffer[0].find(':');
      if (slash_pos == 0) {
        argv0 = buffer[0];
        path = is_exe(process_id, argv0);
      } else if (slash_pos == std::string::npos || slash_pos > colon_pos) { 
        std::string penv = envvar_value_from_process_id(process_id, "PATH");
        if (!penv.empty()) {
          retry:
          std::string tmp;
          std::stringstream sstr(penv);
          while (std::getline(sstr, tmp, ':')) {
            argv0 = tmp + "/" + buffer[0];
            path = is_exe(process_id, argv0);
            if (!path.empty()) break;
            if (slash_pos > colon_pos) {
              argv0 = tmp + "/" + buffer[0].substr(0, colon_pos);
              path = is_exe(process_id, argv0);
              if (!path.empty()) break;
            }
          }
        }
        if (path.empty() && !retried) {
          retried = true;
          penv = "/usr/bin:/bin:/usr/sbin:/sbin:/usr/X11R6/bin:/usr/local/bin:/usr/local/sbin";
          std::string home = envvar_value_from_process_id(process_id, "HOME");
          if (!home.empty()) {
            penv = home + "/bin:" + penv;
          }
          goto retry;
        }
      }
      if (path.empty() && slash_pos > 0) {
        std::string pwd = envvar_value_from_process_id(process_id, "PWD");
        if (!pwd.empty()) {
          argv0 = pwd + "/" + buffer[0];
          path = is_exe(process_id, argv0);
        }
        if (path.empty()) {
          if (process_id == -1 || process_id == getpid()) {
            char cwd[PATH_MAX];
            if (getcwd(cwd, PATH_MAX)) {
              argv0 = std::string(cwd) + "/" + buffer[0];
              path = is_exe(process_id, argv0);
            }
          } else {
            int mib[3];
            std::size_t len = 0;
            mib[0] = CTL_KERN;
            mib[1] = KERN_PROC_CWD;
            mib[2] = process_id;
            if (!sysctl(mib, 3, nullptr, &len, nullptr, 0)) {
              std::vector<char> vecbuff;
              vecbuff.resize(len);
              char *cwd = &vecbuff[0];
              if (!sysctl(mib, 3, cwd, &len, nullptr, 0)) {
                argv0 = std::string(cwd) + "/" + buffer[0];
                path = is_exe(process_id, argv0);
              }
            }
          }
        }
      }
    }
    if (path.empty() && !error) {
      error = true;
      buffer.clear();
      std::string underscore = envvar_value_from_process_id(process_id, "_");
      if (!underscore.empty()) {
        buffer.push_back(underscore);
        goto fallback;
      }
    }
  }
  if (!path.empty()) {
    errno = 0;
  }
  #elif defined(__sun)
  int err = 0;
  char exe[PATH_MAX];
  char buffer[PATH_MAX];
  struct ps_prochandle *P = nullptr;
  if (process_id == -1 || process_id == getpid()) {
    const char *execname = getexecname();
    if (execname) {
      if (realpath(execname, exe)) {
        path = exe;
        goto finish;
      }
    }
  }
  P = Pgrab((process_id == -1) ? getpid() : process_id, PGRAB_RDONLY, &err);
  if (P) {
    if (!err && !errno) {
      if (Pexecname(P, buffer, sizeof(buffer))) {
        if (realpath(buffer, exe)) {
          path = exe;
        }
      }
    }
    Pfree(P);
  }
  if (!path.empty()) {
    goto finish;
  }
  if (process_id == -1 || process_id == getpid()) {
    if (realpath("/proc/self/path/a.out", exe)) {
      path = exe;
    }
  } else {
    if (realpath((std::string("/proc/") + std::to_string(process_id) + 
      std::string("/path/a.out")).c_str(), exe)) {
      path = exe;
    }
  }
  finish:
  #endif
  return path;
}
