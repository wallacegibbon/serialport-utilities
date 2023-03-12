#include "common_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#if defined(_WIN32) || defined(_WIN64)
#include "windows.h"
#else
#include "unistd.h"
#endif

int sleep_milliseconds(int milliseconds) {
#if defined(_WIN32) || defined(_WIN64)
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}

void exit_info(int code, const char *format, ...) {
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	exit(code);
}

