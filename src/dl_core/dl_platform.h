// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_PLATFORM_H
#define DL_CORE_PLATFORM_H

// NOTE: This header may be included by any file, without linking to dl_core. Therefore, do not
// declare any function that requires a compiled implementation.
//

#if defined(_WIN32)
    #define _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR 
    #ifndef WINVER
        #define WINVER              0x0601      // Windows 7
    #endif
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT        0x0601      // _WIN32_WINNT_WIN7
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef NOGDI
        #define NOGDI
    #endif
    #ifndef _USE_MATH_DEFINES
        #define _USE_MATH_DEFINES
    #endif
    #ifndef _CRT_SECURE_NO_WARNINGS
        #define _CRT_SECURE_NO_WARNINGS 1
    #endif
    #ifndef _CRT_SECURE_NO_DEPRECATE
        #define _CRT_SECURE_NO_DEPRECATE 1
    #endif
    #ifndef _CRT_NONSTDC_NO_DEPRECATE
        #define _CRT_NONSTDC_NO_DEPRECATE 1
    #endif
    #include <windows.h>
#elif defined(__linux__)
    #include <sys/utsname.h>
    #include <sys/stat.h>
    #include <dlfcn.h>
    #include <signal.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <pthread.h>
#elif defined(__APPLE__)
    #include <mach/mach.h>
    #include <mach/mach_time.h>
    #include <mach-o/dyld.h>
    #include <sys/utsname.h>
    #include <sys/stat.h>
    #include <dlfcn.h>
    #include <signal.h>
    #include <unistd.h>
    #include <dirent.h>
    #include <pthread.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <cassert>
#include <cerrno>
#include <cstddef>
#include <ctime>
#include <cfloat>
#include <climits>
#include <cmath>
#include <new>

#if defined(_WIN32)
    #define DL_LIBEXT                   ".dll"
    typedef LARGE_INTEGER                PlatformTime;
#elif defined(__linux__)
    #define DL_LIBEXT                   ".so"
    typedef timespec                     PlatformTime;
#elif defined(__APPLE__)
    #define DL_LIBEXT                   ".dylib"
    typedef uint64_t                     PlatformTime;
#else
    #error "Unknown platform."
#endif

#if defined(_WIN32)
    #if DL_IS_RELEASE
        #define DL_DEBUG_BREAK()
    #else
        #define DL_DEBUG_BREAK()        DebugBreak()
    #endif
    #define DL_UTF8_CONSOLE()           SetConsoleOutputCP(CP_UTF8)
    #define DL_HMODULE                  HMODULE
    #define DL_HSYMBOL                  void*
    extern "C" IMAGE_DOS_HEADER          __ImageBase;
    #define DL_DEFMODULE                ( DL_HMODULE )&__ImageBase
    #define DL_DLOPEN(path)             (path.isEmpty() ? DL_DEFMODULE : LoadLibraryW(path.toUtf16().as<LPCWSTR>()))
    #define DL_DLCLOSE(mod)             FreeLibrary(mod)
    #define DL_DLCLOSE_OK               TRUE
    #define DL_DLSYM(mod,name)          GetProcAddress(mod ? mod : DL_DEFMODULE, name.buf())
#else
    #if DL_IS_RELEASE
        #define DL_DEBUG_BREAK()
    #else
        #define DL_DEBUG_BREAK()        asm("int $3")
    #endif
    #define DL_UTF8_CONSOLE()
    #define DL_HMODULE                  void*
    #define DL_HSYMBOL                  void*
    #define DL_DEFMODULE                RTLD_DEFAULT
    #define DL_DLOPEN(path)             dlopen(path.isEmpty() ? NULL : path.buf(), RTLD_LAZY|RTLD_GLOBAL)
    #define DL_DLCLOSE(mod)             dlclose(mod)
    #define DL_DLCLOSE_OK               0
    #define DL_DLSYM(mod,name)          dlsym(mod ? mod : DL_DEFMODULE, name.buf())
#endif

#endif // DL_CORE_PLATFORM_H
