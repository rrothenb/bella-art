// Copyright 2019 Diffuse Logic, all rights reserved.

#ifndef DL_CORE_DEFINES_H
#define DL_CORE_DEFINES_H

// NOTE: This header may be included by any file, without linking to core.lib. Therefore, do not
// declare any function that requires a compiled implementation.
//

// Stringify.
//
#define DL_STRINGIFY_IMPL(x) DL_STRINGIFY(x)
#define DL_STRINGIFY(x) #x

// Configuration.
//
#define DL_CONFIG_DEBUG                         0
#define DL_CONFIG_PSEUDODEBUG                   1
#define DL_CONFIG_RELEASE                       2
#if defined(_DEBUG)
    #define DL_CONFIG                           DL_CONFIG_DEBUG
    #define DL_CONFIG_NAME                      "debug"
#elif defined(PSEUDODEBUG)
    #define DL_CONFIG                           DL_CONFIG_PSEUDODEBUG
    #define DL_CONFIG_NAME                      "pseudodebug"
#elif defined(NDEBUG)
    #define DL_CONFIG                           DL_CONFIG_RELEASE
    #define DL_CONFIG_NAME                      "release"
#endif
#if !defined(DL_CONFIG)
    #error "Unknown configuration. We support [_DEBUG|NDEBUG]."
#endif
#define DL_IS_DEBUG                             (DL_CONFIG == DL_CONFIG_DEBUG)
#define DL_IS_PSEUDODEBUG                       (DL_CONFIG == DL_CONFIG_PSEUDODEBUG)
#define DL_IS_RELEASE                           (DL_CONFIG == DL_CONFIG_RELEASE)

// Platform.
//
#define DL_PLATFORM_WIN32                       0
#define DL_PLATFORM_LINUX                       1
#define DL_PLATFORM_MACOS                       2
#if defined(_WIN32)
    #define DL_PLATFORM                         DL_PLATFORM_WIN32
    #define DL_PLATFORM_NAME                    "win32"
#elif defined(__linux__)
    #define DL_PLATFORM                         DL_PLATFORM_LINUX
    #define DL_PLATFORM_NAME                    "linux"
#elif defined(__APPLE__)
    #define DL_PLATFORM                         DL_PLATFORM_MACOS
    #define DL_PLATFORM_NAME                    "macos"
#endif
#if !defined(DL_PLATFORM)
    #error "Unknown platform. We support [_WIN32|__linux__|__APPLE__]."
#endif
#define DL_IS_WIN32                             (DL_PLATFORM == DL_PLATFORM_WIN32)
#define DL_IS_LINUX                             (DL_PLATFORM == DL_PLATFORM_LINUX)
#define DL_IS_MACOS                             (DL_PLATFORM == DL_PLATFORM_MACOS)

// Architecture.
//
#define DL_ARCH_X86 0
#define DL_ARCH_X64 1
#define DL_ARCH_ARM64 2
#if defined(_WIN64)
    #define DL_ARCH                             DL_ARCH_X64
    #define DL_ARCH_NAME                        "x64"
#elif defined(_WIN32)
    #define DL_ARCH                             DL_ARCH_X86
    #define DL_ARCH_NAME                        "x86"
#elif DL_IS_LINUX
    #if defined(__amd64__) || \
        defined(__x86_64__)
        #define DL_ARCH                         DL_ARCH_X64
        #define DL_ARCH_NAME                    "x64"
    #elif defined(__i386__)
        #define DL_ARCH                         DL_ARCH_X86
        #define DL_ARCH_NAME                    "x86"
    #endif
#elif DL_IS_MACOS
    #if defined(__aarch64__)
        #define DL_ARCH                         DL_ARCH_ARM64
        #define DL_ARCH_NAME                    "arm64"
    #elif defined(__amd64__)
        #define DL_ARCH                         DL_ARCH_X64
        #define DL_ARCH_NAME                    "x64"
    #elif defined(__i386__)
        #define DL_ARCH                         DL_ARCH_X86
        #define DL_ARCH_NAME                    "x86"
    #endif
#endif
#if !defined(DL_ARCH)
    #error "Unknown architecture. We support [_WIN64|_WIN32] on Windows, [__amd64__|__x86_64__|__i386__] on linux, and [__aarch64__|__amd64__|__i386__] on macos."
#endif
#define DL_IS_X86                               (DL_ARCH == DL_ARCH_X86)
#define DL_IS_X64                               (DL_ARCH == DL_ARCH_X64)
#define DL_IS_ARM64                             (DL_ARCH == DL_ARCH_ARM64)
#define DL_IS_64BIT                             (DL_IS_X64 || DL_IS_ARM64)

// Compiler/toolset.
//
#define DL_COMPILER_MSVC                        1
#define DL_COMPILER_CLANG                       2
#define DL_COMPILER_GCC                         3
#define DL_IS_MSVC                              0
#define DL_IS_CLANG                             0
#define DL_IS_GCC                               0
#if defined(_MSC_VER)
    #define DL_COMPILER                         DL_COMPILER_MSVC
    #undef  DL_IS_MSVC
    #define DL_IS_MSVC                          1
#elif defined(__clang__)
    #define DL_COMPILER                         DL_COMPILER_CLANG
    #undef  DL_IS_CLANG
    #define DL_IS_CLANG                         1
#elif defined(__GNUC__)
    #define DL_COMPILER                         DL_COMPILER_GCC
    #undef  DL_IS_GCC
    #define DL_IS_GCC                           1
#endif
#if !defined(DL_COMPILER)
    #error "Unknown compiler. We support [_MSC_VER|__clang__|__GNUC__]."
#endif

// Compiler version.
//
#if DL_IS_MSVC
    #define DL_MSC_VER_6                        1200 // Visual Studio 6.0
    #define DL_MSC_VER_2002                     1300 // Visual Studio .NET 2002 (7.0)
    #define DL_MSC_VER_2003                     1310 // Visual Studio .NET 2003 (7.1)
    #define DL_MSC_VER_2005                     1400 // Visual Studio 2005 (8.0)
    #define DL_MSC_VER_2008                     1500 // Visual Studio 2008 (9.0)
    #define DL_MSC_VER_2010                     1600 // Visual Studio 2010 (10.0)
    #define DL_MSC_VER_2012                     1700 // Visual Studio 2012 (11.0)
    #define DL_MSC_VER_2013                     1800 // Visual Studio 2013 (12.0)
    #define DL_MSC_VER_2015                     1900 // Visual Studio 2015 (14.0)
    #define DL_MSC_VER_2017_MIN                 1910 // Visual Studio 2017 RTW (15.0)
    #define DL_MSC_VER_2017_MAX                 1916 // Visual Studio 2017 version 15.9
    #define DL_MSC_VER_2019_MIN                 1920 // Visual Studio 2019 RTW (16.0)
    #define DL_MSC_VER_2019_MAX                 1929 // Visual Studio 2019 version 16.10, 16.11
    #define DL_MSC_VER_2022_MIN                 1930 // Visual Studio 2022 RTW (17.0)
    //#define DL_MSC_VER_2022_MAX               1935 // Visual Studio 2022 version 17.5

    #define DL_IS_V80                           (_MSC_VER == DL_MSC_VER_2005)
    #define DL_IS_V90                           (_MSC_VER == DL_MSC_VER_2008)
    #define DL_IS_V100                          (_MSC_VER == DL_MSC_VER_2010)
    #define DL_IS_V110                          (_MSC_VER == DL_MSC_VER_2012)
    #define DL_IS_V120                          (_MSC_VER == DL_MSC_VER_2013)
    #define DL_IS_V140                          (_MSC_VER == DL_MSC_VER_2015)
    #define DL_IS_V141                          (_MSC_VER >= DL_MSC_VER_2017_MIN && _MSC_VER <= DL_MSC_VER_2017_MAX)
    #define DL_IS_V142                          (_MSC_VER >= DL_MSC_VER_2019_MIN && _MSC_VER <= DL_MSC_VER_2019_MAX)
    #define DL_IS_V143                          (_MSC_VER >= DL_MSC_VER_2022_MIN /*&& _MSC_VER <= DL_MSC_VER_2022_MAX*/)

    #define DL_IS_AT_LEAST_V80                  (_MSC_VER >= DL_MSC_VER_2005)
    #define DL_IS_AT_LEAST_V90                  (_MSC_VER >= DL_MSC_VER_2008)
    #define DL_IS_AT_LEAST_V100                 (_MSC_VER >= DL_MSC_VER_2010)
    #define DL_IS_AT_LEAST_V110                 (_MSC_VER >= DL_MSC_VER_2012)
    #define DL_IS_AT_LEAST_V120                 (_MSC_VER >= DL_MSC_VER_2013)
    #define DL_IS_AT_LEAST_V140                 (_MSC_VER >= DL_MSC_VER_2015)
    #define DL_IS_AT_LEAST_V141                 (_MSC_VER >= DL_MSC_VER_2017_MIN)
    #define DL_IS_AT_LEAST_V142                 (_MSC_VER >= DL_MSC_VER_2019_MIN)
    #define DL_IS_AT_LEAST_V143                 (_MSC_VER >= DL_MSC_VER_2022_MIN)
    #define DL_COMPILER_VERSION                 _MSC_FULL_VER

    #if DL_IS_V80
        #define DL_COMPILER_VERSION_STRING      "MSVC 8 (VS2005, v80)"
        #define DL_TOOLSET_NAME                 "v80"
    #elif DL_IS_V90
        #define DL_COMPILER_VERSION_STRING      "MSVC 9 (VS2008, v90)"
        #define DL_TOOLSET_NAME                 "v90"
    #elif DL_IS_V100
        #define DL_COMPILER_VERSION_STRING      "MSVC 10 (VS2010, v100)"
        #define DL_TOOLSET_NAME                 "v100"
    #elif DL_IS_V110
        #define DL_COMPILER_VERSION_STRING      "MSVC 11 (VS2012, v110)"
        #define DL_TOOLSET_NAME                 "v110"
    #elif DL_IS_V120
        #define DL_COMPILER_VERSION_STRING      "MSVC 12 (VS2013, v120)"
        #define DL_TOOLSET_NAME                 "v120"
    #elif DL_IS_V140
        #define DL_COMPILER_VERSION_STRING      "MSVC 14 (VS2015, v140)"
        #define DL_TOOLSET_NAME                 "v140"
    #elif DL_IS_V141
        #define DL_COMPILER_VERSION_STRING      "MSVC 15 (VS2017, v141)"
        #define DL_TOOLSET_NAME                 "v141"
    #elif DL_IS_V142
        #define DL_COMPILER_VERSION_STRING      "MSVC 16 (VS2019, v142)"
        #define DL_TOOLSET_NAME                 "v142"
    #elif DL_IS_V143
        #define DL_COMPILER_VERSION_STRING      "MSVC 17 (VS2022, v143)"
        #define DL_TOOLSET_NAME                 "v143"
    #else
        #error "Unknown MSVC version. You probably need to update the Compiler version section in util/defines.h."
    #endif
#elif DL_IS_CLANG
    //  NOTE WELL: these are set arbitrarily by vendors (i.e. Apple), so they are not reliable. For
    //  anything that matters, you should use clang's built-in feature-checking macros instead.
    #define DL_COMPILER_VERSION                 (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__)
    #define DL_COMPILER_VERSION_STRING          "Clang/LLVM " __VERSION__
    #define DL_TOOLSET_NAME                     "clang"
#elif DL_IS_GCC
    //  Version logic from https://gcc.gnu.org/onlinedocs/gcc-4.2.1/cpp/Common-Predefined-Macros.html.
    #define DL_COMPILER_VERSION                 (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
    #define DL_COMPILER_VERSION_STRING          "GNU GCC/G++ " __VERSION__
    #define DL_TOOLSET_NAME                     "gcc"
#endif

// Build spec.
//
#define DL_BUILDSPEC \
    DL_PLATFORM_NAME "-" DL_CONFIG_NAME "-" DL_ARCH_NAME "-" DL_TOOLSET_NAME

// Various macros.
//
#if DL_PLATFORM == DL_PLATFORM_WIN32
    #define DL_UNUSED(x)                        (void)x;
    #define DL_DLLIMPORT                        __declspec(dllimport)
    #define DL_DLLEXPORT                        __declspec(dllexport)
    #define DL_FORCE_EXPORT                     __declspec(dllexport)
    #define DL_NOINL                            __declspec(noinline)
    #define DL_FI                               __forceinline
    #define DL_RESTRICT                         __restrict
    #define DL_FUNCNAME                         __FUNCTION__
    #define DL_THREADFUNC                       __declspec(thread)
    #define DL_ALIGN(N)                         alignas(N)
    #define DL_DISABLE_ALIGN_WARNING            _Pragma("warning( disable : 4324 )")
    #define DL_DEFAULT_ALIGN_WARNING            _Pragma("warning( default : 4324 )")
#else
    #define DL_UNUSED(x)                        (void)x;
    #define DL_DLLIMPORT
    //#define DL_DLLEXPORT                        __attribute__((visibility("default"))) __attribute__((used))
    #define DL_DLLEXPORT                        __attribute__((visibility("default")))
    #define DL_FORCE_EXPORT                     __attribute__((visibility("default"))) __attribute__((used))
    #define DL_NOINL                            __attribute__((noinline))
    #define DL_FI                               inline __attribute__((always_inline))
    #define DL_RESTRICT                         __restrict
    #define DL_FUNCNAME                         __PRETTY_FUNCTION__
    #define DL_THREADFUNC                       __thread
    #define DL_ALIGN(N)                         alignas(N)
    #define DL_DISABLE_ALIGN_WARNING
    #define DL_DEFAULT_ALIGN_WARNING
#endif

#define DL_C_EXPORT extern "C" DL_FORCE_EXPORT

#if defined(DL_BUILD_STATIC)
    #define DL_API
#elif defined(DL_BUILD_SHARED)
    #define DL_API DL_DLLEXPORT
#elif defined(DL_USE_SHARED)
    #define DL_API DL_DLLIMPORT
#else
    #define DL_API
#endif

// Endianness. If you need to build for big endian, you must define DL_ENDIANNESS=1. Otherwise,
// builds are always done for little endian.
//
#define DL_ENDIAN_LITTLE                        0
#define DL_ENDIAN_BIG                           1
#ifndef DL_ENDIANNESS
    #define DL_ENDIANNESS                       DL_ENDIAN_LITTLE
#endif
#if DL_ENDIANNESS == DL_ENDIAN_LITTLE
    #define DL_IS_ENDIAN_LITTLE                 1
    #define DL_IS_ENDIAN_BIG                    0
#elif DL_ENDIANNESS == DL_ENDIAN_BIG
    #define DL_IS_ENDIAN_LITTLE                 0
    #define DL_IS_ENDIAN_BIG                    1
#else
    #error "Unknown endianness. We support [DL_ENDIAN_LITTLE=0|DL_ENDIAN_BIG=1]."
#endif

// OpenMP. Set DL_USE_OPENMP=0 to disable.
//
#ifndef DL_USE_OPENMP
    #define DL_USE_OPENMP                       1
#endif
#if DL_USE_OPENMP && defined(_OPENMP)
    #if defined(_MSC_VER)
        #define DL_OMP_THREAD_INDEX                     omp_get_thread_num()
        #define DL_OMP_GET_MAX_THREADS                  omp_get_max_threads()
        #define DL_OMP_PARALLEL_FOR                     __pragma(omp parallel for)
      //#define DL_OMP_PARALLEL_FOR_REDUCTION_SUM( x )  __pragma(omp parallel for reduction(+:x)) // seems unlikely this will be possible for other than windows, so let's dedicate a variable name with next line
        #define DL_OMP_PARALLEL_FOR_REDUCTION_SUM       __pragma(omp parallel for reduction(+:ompReductionVar))
    #else
        #define DL_OMP_GET_MAX_THREADS                  omp_get_thread_num() /* not sure about this */
        #define DL_OMP_GET_MAX_THREADS                  omp_get_max_threads() /* not sure about this */
        #define DL_OMP_PARALLEL_FOR                     _Pragma("omp parallel for")
        #define DL_OMP_PARALLEL_FOR_REDUCTION_SUM       _Pragma("omp parallel for reduction(+:ompReductionVar)")
    #endif
#else
    #define DL_OMP_THREAD_INDEX                         0 // NOTE: when no OMP it's ok to use 0
    #define DL_OMP_GET_MAX_THREADS                      1 // NOTE: when no OMP it's ok to use 1
    #define DL_OMP_PARALLEL_FOR
    #define DL_OMP_PARALLEL_FOR_REDUCTION_SUM
#endif

#define DL_OMP_FOR                              DL_OMP_PARALLEL_FOR for

// C++11 features.
//
#if DL_IS_MSVC
    #if _MSC_VER < 1600
        #define DL_NO_STATIC_ASSERT
        #define DL_NO_NULLPTR
        #define DL_NO_STDINT_H
    #endif
    #if _MSC_VER < 1700
        #define DL_NO_FINAL
    #endif
    #if (_MSC_VER < 1910) || (_MSC_FULL_VER < 190024210)
        #define DL_NO_CONSTEXPR
    #endif
    #if _MSC_FULL_VER < 180020827
        #define DL_NO_EXPLICIT_CONVERSION_OPERATORS
    #endif
    #define _DISABLE_CONSTEXPR_MUTEX_CONSTRUCTOR // Seems necessary after visual studio 17.11.3
#elif DL_IS_CLANG
    #if !__has_include(<stdint.h>)
        #define DL_NO_STDINT_H
    #endif
    #if !__has_feature(cxx_nullptr)
        #define DL_NO_NULLPTR
    #endif
    #if !__has_feature(cxx_constexpr) || !__has_feature(__cxx_generic_lambdas__) || !__has_feature(__cxx_relaxed_constexpr__)
        #define DL_NO_CONSTEXPR
    #endif
    #if !__has_feature(cxx_override_control)
        #define DL_NO_FINAL
    #endif
    #if !__has_feature(cxx_static_assert)
        #define DL_NO_STATIC_ASSERT
    #endif
    #if !__has_feature(cxx_explicit_conversions)
        #define DL_NO_EXPLICIT_CONVERSION_OPERATORS
    #endif
#elif DL_IS_GCC
    #if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
        #define DL_GCC_CXX11
    #endif
    #if !defined(DL_GCC_CXX11) || (DL_COMPILER_VERSION < 40600)
        #define DL_NO_NULLPTR
        #define DL_NO_CONSTEXPR
    #endif
    #if !defined(__cpp_constexpr) || (__cpp_constexpr < 201304)
        #define DL_NO_CONSTEXPR
    #endif
    #if !defined(DL_GCC_CXX11) || (DL_COMPILER_VERSION < 40700)
        #define DL_NO_FINAL
    #endif
    #if !defined(DL_GCC_CXX11) || (DL_COMPILER_VERSION < 40300)
        #define DL_NO_STATIC_ASSERT
    #endif
    #if !defined(DL_GCC_CXX11) || (DL_COMPILER_VERSION < 40500)
        #define DL_NO_EXPLICIT_CONVERSION_OPERATORS
    #endif
#endif

// Standard definitions.
//
#include <cstddef>
#include <climits>
#if !defined(__STDC_LIMIT_MACROS)
    #define __STDC_LIMIT_MACROS
#endif
#if defined(DL_NO_STDINT_H)
    #error "No inttypes/stdint, please use a newer compiler."
#endif
#if DL_IS_LINUX && !defined(__STDC_FORMAT_MACRO)
    #define __STDC_FORMAT_MACROS
#endif
#include <cinttypes>
#include <cstdint>

// nullptr.
//
#if defined(DL_NO_NULLPTR)
#   if defined(__cplusplus)
namespace std { class nullptr_t { }; } extern std::nullptr_t nullptr;
#   else
#       define nullptr 0
#   endif
#endif

// constexpr.
//
#if defined(DL_NO_CONSTEXPR)
    #define DL_CONSTEXPR
#else
    #define DL_CONSTEXPR constexpr
#endif

// final.
//
#if defined(DL_NO_FINAL)
    #define DL_FINAL
#else
    #define DL_FINAL final
#endif

// Explicit conversion operators.
//
#if defined(DL_NO_EXPLICIT_CONVERSION_OPERATORS)
    #define DL_EXPLICIT
#else
    #define DL_EXPLICIT explicit
#endif

// DL assert. TODO: create a "release with assert" config (log using DL_FUNCNAME/stringify).
//
#include <cassert>
#if DL_IS_RELEASE
    #define DL_ASSERT(x)
#else
    #define DL_ASSERT(x) assert(x)
#endif

// static_assert.
//
#if defined(DL_NO_STATIC_ASSERT)
    // NOTE: this will report as "use of undefined type 'dl::StaticAssert<false>'" with the
    // diagnostic indicating the line number where DL_SASSERT was used, as well as the point
    // of instantiation, if the failing assertion resides in a template type or function.
    namespace dl {
    template<bool T> struct StaticAssert;
    template<> struct StaticAssert<true> { };
    }
    #define DL_SASSERT(expr) (dl::StaticAssert<!!(expr)>())
#else
    #define DL_SASSERT(expr) static_assert(expr,"static_assert failed: '" DL_STRINGIFY(expr) "'")
#endif

// Portable casting macros, these are easier to type, easier to search for, and work in C or C++.
// Please try to use them wherever you would otherwise have used a plain C-style case.
//
#ifdef __cplusplus
    #define DL_DECONST(T, expr) (const_cast<T>(expr))
    #define DL_CAST(T, expr)    (static_cast<T>(expr))
    #define DL_COERCE(T, expr)  (reinterpret_cast<T>(expr))
    #define DL_FORCE(T, expr)   ((T)(expr))
#else
    #define DL_DECONST(T, expr) ((T)(expr))
    #define DL_CAST(T, expr)    ((T)(expr))
    #define DL_COERCE(T, expr)  ((T)(expr))
    #define DL_FORCE(T, expr)   ((T)(expr))
#endif

// General typedefs. These are not intended for allowing to switch types; rather they are to allow
// for defense against types of differing sizes on different compilers/platforms. Because writing
// and reading of files depends on these sizes, they should NEVER be changed.
//
// Char and Byte are defined as int8_t and uint8_t, since these are guaranteed to be the advertised
// size; this means, though, that there may be some cases where you will have to use the native char
// type, because it is not defined to be the same as int8_t.
//
namespace dl {

typedef std::nullptr_t  Null;
typedef int8_t          Char;
typedef uint8_t         Byte;
typedef bool            Bool;
typedef int64_t         Int;
typedef uint64_t        UInt;
typedef double          Real;
typedef size_t          Size;
typedef void*           Ptr;

typedef int8_t          Int8;
typedef int16_t         Int16;
typedef int32_t         Int32;
typedef int64_t         Int64;
typedef uint8_t         UInt8;
typedef uint16_t        UInt16;
typedef uint32_t        UInt32;
typedef uint64_t        UInt64;
typedef float           Float;
typedef double          Double;

typedef UInt64          Dim;

} // dl

// Define general hash function in the dl::ds namespace.
//
#include <functional>
namespace dl {
namespace ds {
template<typename T> static inline UInt64 hash( T t ) { return std::hash<T>()( t ); }
} // ds
} // dl

#endif // DL_CORE_DEFINES_H
