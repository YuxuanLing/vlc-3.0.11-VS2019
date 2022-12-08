#include "csf/logger/CSFLog.h"
#include "csf/logger/CSFLogger.h"
/**
* CSFLog_getCSFLogger - gets the logger of the given name or creates a logger with the given name if it does not exists (under the right hierarchy).
* @param name - character pointer that is unique for each logger
* @return CSFLogger* - struct pointer to the logger requested
*/
CSFLogger* CSFLog_getCSFLogger(const char* name)
{
	return CSFLogger_getLogger(name);
}

/**
* CSFLog_isLogLevel - returns non-zero value if at least the given log level is set for the given CSF Logger.
* @param pLogger - CSFLogger struct pointer to the logger 
* @param level - CSFLogLevel enum of log levels. 
*				 Valid values are: CSF_FATAL, CSF_ERROR, CSF_WARNING, CSF_INFO, CSF_DEBUG, CSF_TRACE
* @return int - Non-zero value if at least the given log level is set for the given CSF Logger. Otherwise, returns 0. 
*/
int CSFLog_isLogLevel(CSFLogger* pLogger, CSFLogLevel level)
{
	if(level <= CSFLogger_getLogLevel(pLogger)) return 1;
    return 0;
}

/**
* CSFLog_isDebugEnabled - returns non-zero value if at least the debug level is set for the given CSF Logger.
* @param pLogger - CSFLogger struct pointer to the logger 
* @return int - Non-zero value if at least the debug level is set for the given CSF Logger. Otherwise, returns 0. 
*/
int CSFLog_isDebugEnabled(CSFLogger* pLogger)
{
	if(CSFLogger_getLogLevel(pLogger) >= CSF_LOG_DEBUG) return 1;
    return 0;
}

/**
* CSFLog_isTraceEnabled - returns non-zero value if at least the trace level is set for the given CSF Logger.
* @param pLogger - CSFLogger struct pointer to the logger 
* @return int - Non-zero value if at least the trace level is set for the given CSF Logger. Otherwise, returns 0. 
*/
int CSFLog_isTraceEnabled(CSFLogger* pLogger)
{
	if(CSFLogger_getLogLevel(pLogger) >= CSF_LOG_TRACE) return 1;
	return 0;
}
