#ifndef __LIBFETCH_GLOBAL_H
#define __LIBFETCH_GLOBAL_H

#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define MAX 1024

#ifndef OS
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
#endif

#endif