/* Base library
 * Written by David Avedissian (c) 2018-2020 (git@dga.dev)  */
#pragma once

// Determine C++ version.
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#define DGA_HAS_CPP17
#endif

// Determine architecture.
#if defined(__x86_64__) || defined(_M_X64)
#define DGA_ARCH_64
#elif defined(__i386__) || defined(_M_IX86)
#define DGA_ARCH_32
#endif

// Determine platform.
#define DGA_WIN32 0
#define DGA_MACOS 1
#define DGA_LINUX 2
#if defined(__WIN32__) || defined(_WIN32)
#define DGA_PLATFORM DGA_WIN32
#elif defined(__APPLE_CC__)
#define DGA_PLATFORM DGA_MACOS
#elif defined(__EMSCRIPTEN__)
#define DGA_PLATFORM DGA_LINUX
#define DGA_EMSCRIPTEN
#elif defined(__linux__) && !defined(__ANDROID__)
#define DGA_PLATFORM DGA_LINUX
#endif

// Determine compiler.
// Note that clang-format breaks the libstdc++ detection when using clang.
// clang-format off
#define __VERSION(major, minor, patch) (major * 1000 + minor * 100 + patch * 10)
#if defined(_MSC_VER)
#define DGA_MSVC
#elif defined(__clang__)
#define DGA_CLANG __VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)
#if __has_include(<__config>)
#define DGA_LIBCPP _LIBCPP_VERSION
#elif __has_include(<bits/c++config.h>)
#define DGA_LIBSTDCPP __VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#else
#error This standard library is not recognised. Clang is only supported with libc++ or libstdc++.
#endif
#elif defined(__GNUC__)
#define DGA_GCC __VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#define DGA_LIBSTDCPP DGA_GCC
#endif
// clang-format on

// Define platform/build dependent visibility macro helpers.
#if defined(DGA_DLL)
#if DGA_PLATFORM == DGA_WIN32
#define DGA_HELPER_API_EXPORT __declspec(dllexport)
#define DGA_HELPER_API_IMPORT __declspec(dllimport)
#else
#define DGA_HELPER_API_EXPORT
#define DGA_HELPER_API_IMPORT
#endif
#ifdef DGA_BUILD
#define DGA_API DGA_HELPER_API_EXPORT
#else
#define DGA_API DGA_HELPER_API_IMPORT
#endif
#else
#define DGA_API
#endif

// Define deprecated marker.
#ifdef DEPRECATED
#undef DEPRECATED
#endif
#if defined(DGA_GCC) || defined(DGA_CLANG)
#define DEPRECATED __attribute__((deprecated))
#elif defined(DGA_MSVC)
#define DEPRECATED __declspec(deprecated)
#else
#define DEPRECATED
#endif

// Define unreachable marker.
#if defined(DGA_GCC) || defined(DGA_CLANG)
#define DGA_UNREACHABLE __builtin_unreachable()
#elif defined(DGA_MSVC)
#define DGA_UNREACHABLE __assume(0)
#else
#define DGA_UNREACHABLE void
#endif
