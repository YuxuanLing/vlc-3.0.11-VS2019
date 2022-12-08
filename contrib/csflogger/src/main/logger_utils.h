#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h> /* For strstr() */
#include <string>
#include <locale>

#ifndef va_copy
#if WIN32
//This is safe on Windows (32-bit and x64), but might not be safe for other platforms
#define va_copy(dst, src)       memcpy(&(dst), &(src), sizeof(va_list))
#else
//GCC defines va_copy, so this might be a non Windows, non GCC compiler, so you'll need to do some digging to
//understand if what we've done for Windows is OK, or if you need a different impl.
#error va_copy not supported on this platform, please investigate what the impl might need to be on your platform
#endif
#endif


char * generate_message (int msg_len, const char *format, va_list args);
char *logger_trimwhitespace(char *str);
