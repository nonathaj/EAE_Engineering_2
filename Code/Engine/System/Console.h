#ifndef __SYSTEM_CONSOLE_H
#define __SYSTEM_CONSOLE_H

namespace System
{
	void ConsolePrint(bool displayFileAndLine, std::string i_file, const unsigned int i_line, std::string i_fmt, ...);
}

// This is how we handle variable argument pre-processor macros
// This is only supported by compilers that support the C99 standard
#if defined(_DEBUG) && !defined(DISABLE_DEBUG_PRINT)

#define DEBUG_PRINT(fmt,...) System::ConsolePrint(true, __FILE__, __LINE__, (fmt), __VA_ARGS__)
#define DEBUG_PRINT_NOFILE(fmt,...) System::ConsolePrint(false, nullptr, nullptr, (fmt), __VA_ARGS__)

#else

// This is one way we create a do nothing (NO OP) macro that doesn't
// generate a compiler warning or error
#define DEBUG_PRINT(fmt,...) void(0)
#define DEBUG_PRINT_NOFILE(fmt,...) void(0)

#endif


#endif // __SYSTEM_CONSOLE_H
