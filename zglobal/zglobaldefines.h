#ifndef ZGLOBALDEFINES_H
#define ZGLOBALDEFINES_H

// target platform defines
// define                   platform
// PLATFORM_LINUX           linux
// PLATFORM_WIN32           win32
// PLATFORM_MSDOS           ms-dos

// target system types
// define                   platform
// SYSTEM_CONSOLE           console mode
// SYSTEM_GUI               gui mode

// compiler specific #defines

// Visual Studio

#ifdef WIN32
#define PLATFORM_WIN32
#endif

#ifdef _CONSOLE
#define SYSTEM_CONSOLE
#endif

#ifdef _WINDOWS
#define SYSTEM_GUI
#endif

#if defined(PLATFORM_WIN32) && defined(SYSTEM_GUI)
#define PLATFORM_WIN32_GUI
#endif

// Visual Studio

// g++

#ifndef WIN32
#define PLATFORM_LINUX
#define SYSTEM_CONSOLE
#endif

// g++

#ifdef PLATFORM_LINUX && SYSTEM_CONSOLE
#define PLATFORM_LINUX_CONSOLE
#endif

#ifdef PLATFORM_WIN32 && SYSTEM_CONSOLE
#define PLATFORM_WIN32_CONSOLE
#endif

// Bit masks, 16 bits

#define BIT_0   1
#define BIT_1   2
#define BIT_2   4
#define BIT_3   8
#define BIT_4   16
#define BIT_5   32
#define BIT_6   64
#define BIT_7   128
#define BIT_8   256
#define BIT_9   512
#define BIT_10  1024
#define BIT_11  2048
#define BIT_12  4096
#define BIT_13  8192
#define BIT_14  16384
#define BIT_15  32768
#define BIT_16  65536
#endif
