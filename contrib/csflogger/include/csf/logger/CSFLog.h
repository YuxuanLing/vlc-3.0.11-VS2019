#ifndef CSFLOG_H
#define CSFLOG_H

#include "CSFLoggerCommon.h"
#include "CSFCustomAssert.h"

/*! @file CSFLog.h
*	@brief contains Macros that will allow you to log messages to a specified CSFLogger with given log level priority, tag and format.
*	It also contains functions to get a logger with a specified name and to check if a log level will be captured in the logs.
*
* Created: June, 7th, 2010
* Modified: Jan 13th, 2011 (macro refactoring)
* Modified: Apr 6th, 2012 (changed bool it int to make 'C' compatible).
* Modified: Apr 18th, 2012 (changed where functions are declared and how headers are included by each other. Moved some declarations to new headers).
*	
*/

/*! @struct CSFLogger
* @brief encapsulates the different properties of a Logger.
* Properties such as name, and depending on the implementation it will encapsulate an object that allows log management of log level, etc.
*
*/

#ifdef __cplusplus
extern "C"{
#endif

/**
* CSFLogLevel is a enum defining different log levels.
* Each log statement will be associated with a log level. 
* Every logger will have a log level assigned that can be used to filter log messages:
*		e.g. If the logger's log level is CSF_LOG_DEBUG, the logs will include messages from debug, info, notice warning, error, and critical
*			 If the logger's log level is CSF_LOG_FATAL, then only the fatal log messages will be captured
* Please refer to Log Parser Spec for log level guidelines
*/
typedef enum{
	CSF_LOG_OFF =0,
	CSF_LOG_FATAL,
	CSF_LOG_ERROR,
	CSF_LOG_WARNING,
	CSF_LOG_INFO,
	CSF_LOG_DEBUG,
	CSF_LOG_TRACE
} CSFLogLevel;

struct CSF_LOGGER_API CSFLogger;
typedef struct CSFLogger CSFLogger;

CSF_LOGGER_API const char* CSFLogLevel_toString(CSFLogLevel logLevel);


typedef void (*ThreadIdHandlerFxn)(char*);
/**
*
* CSFLogger_SetThreadIdHandler - initializes the handler used to set thread id used in logs.
* @param threadIdHandlerFxn - a function pointer to a method taking a char *, this will be invoked on every log statement and should set the Id/name for the calling thread. The handler should manage any padding/trimming to ensure fixed width columns in the log if required
* 
*/
CSF_LOGGER_API void CSFLogger_SetThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn);

/**
*
* CSFLogger_ClearThreadIdHandler - clears the handler used to set thread id used in logs back to the default
*/
CSF_LOGGER_API void CSFLogger_ClearThreadIdHandler();

/**
*
* CSFLogger_init - initializes the CSF logger based on properties in a config file or default to basic configuration if no file is provided
* @param confFile - full path to configuration file location 
* 
*/
CSF_LOGGER_API void CSFLogger_init(const char* confFile); 


/**
* CSFLogger_init - initializes the CSF logger based on log file name. If no file is provide, the logger 
* @param confFile - full path to log file file location 
* 
*/
CSF_LOGGER_API void CSFLogger_initialize(const char* confFile); 

/**
* CSFLogger_flush- write the log in buffer to the log file
* 
*/
CSF_LOGGER_API void CSFLogger_flush(); 

/**
* CSFLogger_getLogger - gets the logger of the given name or creates a logger with the given name if it does not exists (under the right hierarchy).
* @param name - character pointer that is unique for each logger
* @return CSFLogger* - struct pointer to the logger requested
*/
CSF_LOGGER_API CSFLogger* CSFLogger_getLogger(const char* name);

/**
* CSFLogger_getRootLogger - returns the root logger
* @return CSFLogger* - struct pointer to the root logger
*/
CSF_LOGGER_API CSFLogger* CSFLogger_getRootLogger(void);


/*
* CSFLogger_getName - gets the name of the given CSF logger struct
* @param CSFLogger struct pointer to the specific logger
* @return character pointer containing the name of the logger
*/
CSF_LOGGER_API const char* CSFLogger_getName(CSFLogger* pLogger);

/**
* CSFLogger_setLogLevel - sets the given log level for the given logger
* @param CSFLogger struct pointer of the logger whose log level will be updated
* @param CSFLogLevel enum value that the log level will be updated to
*/
CSF_LOGGER_API void CSFLogger_setLogLevel(CSFLogger *pLogger, CSFLogLevel level);

/**
* CSFLogger_getGlobalLogLevel - gets the log level for the log controller
* @return CSFLogLevel enum value of the logcontroller
*/
CSF_LOGGER_API CSFLogLevel CSFLogger_getGlobalLogLevel();

/**
* CSFLogger_getGlobalLogLevel - gets the log level for the log controller
* @param CSFLogLevel enum value that the log level will be updated to
*/
CSF_LOGGER_API void CSFLogger_setGlobalLogLevel(CSFLogLevel level);

/**
* CSFLogger_getLogLevel - gets the log level for the given logger
* @param CSFLogger struct pointer of the logger to be queried
* @return CSFLogLevel enum value of this specific logger
*/
CSF_LOGGER_API CSFLogLevel CSFLogger_getLogLevel(CSFLogger *pLogger);

/**
* CSFLog_getCSFLogger - gets the logger of the given name or creates a logger with the given name if it does not exists (under the right hierarchy).
* @param name - character pointer that is unique for each logger
* @return CSFLogger* - struct pointer to the logger requested
*/
CSF_LOGGER_API CSFLogger* CSFLog_getCSFLogger(const char* name); 

/**
* CSFLog_isLogLevel - returns non-zero value if at least the given log level is set for the given CSF Logger.
* @param pLogger - CSFLogger struct pointer to the logger 
* @param level - CSFLogLevel enum of log levels. 
*				 Valid values are: CSF_FATAL, CSF_ERROR, CSF_WARNING, CSF_INFO, CSF_DEBUG, CSF_TRACE
* @return int - Non-zero value if at least the given log level is set for the given CSF Logger. Otherwise, returns 0. 
*/
CSF_LOGGER_API int CSFLog_isLogLevel(CSFLogger* pLogger, CSFLogLevel level);

/**
* CSFLog_isDebugEnabled - returns non-zero value if at least the debug level is set for the given CSF Logger.
* @param pLogger - CSFLogger struct pointer to the logger 
* @return int - Non-zero value if at least the debug level is set for the given CSF Logger. Otherwise, returns 0. 
*/
CSF_LOGGER_API int CSFLog_isDebugEnabled(CSFLogger* pLogger);

/**
* CSFLog_isTraceEnabled - returns non-zero value if at least the trace level is set for the given CSF Logger.
* @param pLogger - CSFLogger struct pointer to the logger 
* @return int - Non-zero value if at least the trace level is set for the given CSF Logger. Otherwise, returns 0. 
*/
CSF_LOGGER_API int CSFLog_isTraceEnabled(CSFLogger* pLogger);

#ifdef __cplusplus
}
#endif



#endif /* CSFLOG_H */
