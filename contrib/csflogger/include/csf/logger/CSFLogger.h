#ifndef CSFLOGGER_H
#define CSFLOGGER_H

#include "CSFLog.h"

#include <stdio.h>
#include <stdarg.h>

/*! @file CSFLogger.h
* CSFLogger manages the logging services:
*			initialize the service based on a config file or using default values. 
*			initiates a specific logger based on a namespace
*			sets and gets log level
*			capture the log statements to destinations specified by config file or given implementation
*
*	Created: June 7th,2010
*	Modified: Jan 13th, 2011 (major refactoring)
*/

#ifdef __cplusplus
extern "C"{
#endif

/**
* CSFLog - logs a message to a given CSFLogger with given log level priority if the logger's log level is at least the given priority.
*			  supports printf-like function arguments input. 
*	e.g. CSFLog(myLogger, CSF_LOG_DEBUG, __FILE__, __LINE__, __FUNCTION__, "this is my log message where I want to include key info %s", myVar);
*
* @param pLogger - CSFLogger pointer to the specific logger
* @param priority - LogLevel enum priority: CSFTRACE, CSFDEBUG, CSFINFO, CSFWARNING, CSFERROR, CSFFATAL
* @param sourceFile - character pointer to capture the file name of the originating source code using __FILE__ macro
* @param sourceLine - integer using __LINE__ macro
* @param function - char pointer to capture the function name of the originating source code using __FUNCTION__ macro
* @param format - printf-like format that captures the log message and optionally contain embedded format tags that are substitued by the values specified in the subsequent argument(s)
* @param ... - printf-like extra arguments whose values will be included in the format argument
*
*/
CSF_LOGGER_API void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const char* format, ...);

/**
* CSFLogV - logs a message to a given CSFLogger with given log level priority if the logger's log level is at least the given priority.
 			supports printf-like function arguments input. 
  
         void myPrintFunction(CSFLogger * pLogger, const char * pSourceFile, int sourceLine, const char * function, const char * pFormat, ...) {
	       va_list ap;
           va_start(ap, pFormat);

           CSFLogV(pLogger, priority, pSourceFile, sourceLine, function, pFormat, ap);
           va_end(ap);
         }
 
  @param pLogger - CSFLogger pointer to the specific logger
  @param priority - LogLevel enum priority: CSFTRACE, CSFDEBUG, CSFINFO, CSFWARNING, CSFERROR, CSFFATAL
  @param sourceFile - character pointer to capture the file name of the originating source code using _FILE_ macro
  @param sourceLine - integer using _LINE_ macro
  @param function - char pointer to capture the function name of the originating source code using __FUNCTION__ macro
  @param pFormat - printf-like format that captures the log message and optionally contain embedded format tags that are substitued by the values specified in the subsequent argument(s)
  @param args - va_list argument (see example in brief above).
 */
CSF_LOGGER_API void CSFLogV(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* tag , const char* format, va_list args);

/**
* CSFLogFatal - Macro to log a message to a given CSFLogger with FATAL priority if the logger's log level is at least FATAL
* supports printf-like function arguments. e.g. CSFLogFatal(myLogger, "this is my log message where I want to include key info %s", myVar);
*
* @param pLogger - CSFLogger pointer to the specific logger
* @param format - printf-like format that captures the log message and optionally contain embedded format tags that are substitued by the values specified in the subsequent argument(s)
* @param ... - printf-like extra arguments whose values will be included in the format argument
*
*/
#define CSFLogFatal(pLogger, format, ...) CSFLog(pLogger, CSF_LOG_FATAL, __FILE__ , __LINE__ , __FUNCTION__ , format , ## __VA_ARGS__ )
#define CSFLogFatalV(pLogger, format, va_list_arg) CSFLogV(pLogger, CSF_LOG_FATAL, __FILE__ , __LINE__ , __FUNCTION__ , format , va_list_arg )

/**
* CSFLogError - same as CSFLogFatal but for ERROR log level
*
*/
#define CSFLogError(pLogger, format, ...) CSFLog(pLogger, CSF_LOG_ERROR, __FILE__ , __LINE__ , __FUNCTION__, format , ## __VA_ARGS__ )
#define CSFLogErrorV(pLogger, format, va_list_arg) CSFLogV(pLogger, CSF_LOG_ERROR, __FILE__ , __LINE__ , __FUNCTION__ , format , va_list_arg )

/**
* CSFLogWarn - same as CSFLogFatal but for WARNING log level
*
*/
#define CSFLogWarn(pLogger, format, ...) CSFLog(pLogger, CSF_LOG_WARNING, __FILE__ , __LINE__ , __FUNCTION__ , format , ## __VA_ARGS__ )
#define CSFLogWarnV(pLogger, format, va_list_arg) CSFLogV(pLogger, CSF_LOG_WARNING, __FILE__ , __LINE__ , __FUNCTION__ , format , va_list_arg )

/**
* CSFLogInfo - same as CSFLogFatal but for INFO log level
*
*/
#define CSFLogInfo(pLogger, format, ...) CSFLog(pLogger, CSF_LOG_INFO, __FILE__ , __LINE__ , __FUNCTION__ , format , ## __VA_ARGS__ )
#define CSFLogInfoV(pLogger, format, va_list_arg) CSFLogV(pLogger, CSF_LOG_INFO, __FILE__ , __LINE__ , __FUNCTION__ , format , va_list_arg )

/**
* CSFLogDebug - same as CSFLogFatal but for DEBUG level
* 
*/
#define CSFLogDebug(pLogger, format, ...) if(CSFLog_isDebugEnabled(pLogger)) CSFLog(pLogger, CSF_LOG_DEBUG, __FILE__ , __LINE__ , __FUNCTION__ , format , ## __VA_ARGS__ )
#define CSFLogDebugV(pLogger, format, va_list_arg) if(CSFLog_isDebugEnabled(pLogger)) CSFLogV(pLogger, CSF_LOG_DEBUG, __FILE__ , __LINE__ , __FUNCTION__ , format , va_list_arg )

/**
* CSFLogTrace - same as CSFLogFatal but for TRACE level
* 
*/
#define CSFLogTrace(pLogger, format, ...) if(CSFLog_isTraceEnabled(pLogger)) CSFLog(pLogger, CSF_LOG_TRACE, __FILE__ , __LINE__ , __FUNCTION__ , format , ## __VA_ARGS__ )
#define CSFLogTraceV(pLogger, format, va_list_arg) if(CSFLog_isTraceEnabled(pLogger)) CSFLogV(pLogger, CSF_LOG_TRACE, __FILE__ , __LINE__ , __FUNCTION__ , format , va_list_arg )

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* CSFLOGGER_H */
