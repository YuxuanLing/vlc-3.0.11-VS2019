#ifndef _LOG_LEVEL_TIME_H
#define _LOG_LEVEL_TIME_H

#ifndef G_LOG_LEVEL_TIME_H
#define G_LOG_LEVEL_TIME_H extern
#endif
#define LOG_WRITE(FILEPATH,FORMAT,...) log_write(FILEPATH,__FILE__,__LINE__,__FUNCTION__,FORMAT,##__VA_ARGS__)
#define LOG_PRINT(FORMAT,...) log_print(__FILE__,__LINE__,__FUNCTION__,FORMAT,##__VA_ARGS__)

#define LOG_RECORD_BEGIN log_record_begin()
#define LOG_WRITE_RECOED_END(FILEPATH) log_record_end(0,FILEPATH,__FILE__,__LINE__,__FUNCTION__)
#define LOG_PRINT_RECOED_END log_record_end(1,"",__FILE__,__LINE__,__FUNCTION__)

void log_write(const char* filepath, const char* filename, int lines, const char* functions, const char* format, ...);

void log_print(const char* filename, int lines, const char* functions, const char* format, ...);

void log_record_begin();

void log_record_end(int flag, const char* filepath, const char* filename, int lines, const char* functions);

#endif