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
#define PLATFORM_LINUX
#define SYSTEM_CONSOLE

#ifdef PLATFORM_LINUX && SYSTEM_CONSOLE
#define PLATFORM_LINUX_CONSOLE
#endif

#endif
