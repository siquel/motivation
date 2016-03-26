#pragma once

#define MOTI_COMPILER_MSVC             0

#define MOTI_PLATFORM_WINDOWS          0
#define MOTI_PLATFORM_LINUX            0

#define MOTI_ARCH_32BIT                0
#define MOTI_ARCH_64BIT                0

#if defined(_MSC_VER)
#   undef MOTI_COMPILER_MSVC
#   define MOTI_COMPILER_MSVC _MSC_VER
#else
#error   "Unsupported compiler"
#endif

#if defined(__x86_64__)    || \
	defined(_M_X64)        || \
	defined(__aarch64__)   || \
	defined(__64BIT__)     || \
	defined(__mips64)      || \
	defined(__powerpc64__) || \
	defined(__ppc64__)     || \
	defined(__LP64__)
#   undef MOTI_ARCH_64BIT
#   define MOTI_ARCH_64BIT 1
#else
#   undef MOTI_ARCH_32BIT
#   define MOTI_ARCH_32BIT 1
#endif

#if defined(_WIN32) || defined(_WIN64)
#     undef MOTI_PLATFORM_WINDOWS
#     ifndef NOMINMAX
#         define NOMINMAX
#     endif
#     if !defined(WINVER) && !defined(_WIN32_WINNT)
          // win 7
#         define WINWER 0x0601 
#         define _WIN32_WINNT 0x0601
#     endif // def winver && winnt
#     define MOTI_PLATFORM_WINDOWS _WIN32_WINNT
#elif defined(__linux__)
#     undef MOTI_PLATFORM_LINUX
#     define MOTI_PLATFORM_LINUX       1
#endif