#include "log_level_time.h"
#ifdef _WIN32
#include <stdio.h>  
#include <stdarg.h> 
#include <time.h>
#include <windows.h>
clock_t c_start, c_end;
#elif __linux__
#include <stdio.h>
#include <stdarg.h> 
#include <sys/time.h>
#include <time.h>
struct timeval t_start, t_end;
#endif

#ifdef _WIN32
int gettimeofday(struct timeval* tp, void* tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

void log_write(const char* filepath, const char* filename, int lines, const char* functions, const char* format, ...)
{
    FILE* pFile = fopen(filepath, "a+");
    if (pFile == NULL)
    {
        return;
    }
    va_list arg;
    int done;
    struct timeval tv_;
    long tv_ms = 0;
    va_start(arg, format);
    time_t time_log = time(NULL);
    struct tm* tm_log = localtime(&time_log);    
    gettimeofday(&tv_, NULL);
    tv_ms = tv_.tv_usec / 1000;//»ñÈ¡ºÁÃë
    fprintf(pFile, "Data:%04d-%02d-%02d %02d:%02d:%02d.%ld File:%s Line:%d Function:%s \t", 
                   tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, 
                   tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec, tv_ms, filename, lines, functions);
    done = vfprintf(pFile, format, arg);
    fprintf(pFile, "\n");
    va_end(arg);
    fflush(pFile);
    fclose(pFile);
}

void log_print(const char* filename, int lines, const char* functions, const char* format, ...)
{
    va_list arg;
    int done;
    va_start(arg, format);
    time_t time_log = time(NULL);
    struct tm* tm_log = localtime(&time_log);
    printf("Data:%04d-%02d-%02d %02d:%02d:%02d File:%s Line:%d Function:%s \t", 
            tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday,
            tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec, filename, lines, functions);
    done = vprintf(format, arg);
    printf("\n");
    va_end(arg);
}

void log_record_begin()
{
#ifdef _WIN32
    c_start = clock();
#elif __linux__
    gettimeofday(&t_start, NULL);
#endif

}

void log_record_end(int flag, const char* filepath, const char* filename, int lines, const char* functions)
{
#ifdef _WIN32
    c_end = clock();
    double costtime = (double)(c_end - c_start);
#elif __linux__
    gettimeofday(&t_end, NULL);
    long start = ((long)t_start.tv_sec) * 1000 + (long)t_start.tv_usec / 1000;
    long end = ((long)t_end.tv_sec) * 1000 + (long)t_end.tv_usec / 1000;
    long costtime = end - start;
#endif
    switch (flag)
    {
    case 0:
    {
#ifdef _WIN32
        log_write(filepath, filename, lines, functions, "UsedTime:%.2fms", costtime);
#elif __linux__
        log_write(filepath, filename, lines, functions, "UsedTime:%ldms", costtime);
#endif // _WIN32
        break;
    }
    case 1:
    {
#ifdef _WIN32
        log_print(filename, lines, functions, "UsedTime:%.2fms", costtime);
#elif __linux__
        log_print(filename, lines, functions, "UsedTime:%ldms", costtime);
#endif // _WIN32
        break;
    }
    default:
    {
        break;
    }
    }
}