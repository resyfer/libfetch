#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#ifndef __LIBFETCH_GLOBAL_H
#define __LIBFETCH_GLOBAL_H

#define NAME "libfetch"
#define VERSION "v1.0.0"

#ifdef __unix__
	#ifdef __linux__
		#define OS "Linux"
	#else
		#define OS "Unix"
	#endif
#elif __APPLE__
	#define OS "Darwin"
#elif _WIN32
	#define OS "Win32"
#endif

#define MAX 1024

#endif