#ifndef CSFLOGGER_HPP
#define CSFLOGGER_HPP

#ifdef __cplusplus

#include <string>
#include <sstream>
#include <map>
#include "CSFLog.h"

/**
* CSFLog - logs a message to a given CSFLogger with given log level priority if the logger's log level is at least the given priority.
*			Only available in CSFLogger library if built as a C++ implematation, and then only supported if consuming API is compiled for C++.
*
* @param pLogger - CSFLogger pointer to the specific logger
* @param priority - LogLevel enum priority: CSFTRACE, CSFDEBUG, CSFINFO, CSFWARNING, CSFERROR, CSFFATAL
* @param sourceFile - character pointer to capture the file name of the originating source code using __FILE__ macro
* @param sourceLine - integer using __LINE__ macro
* @param function - char pointer to capture the function name of the originating source code using __FUNCTION__ macro
* @param message - std::string to be logged
* @param 
*
*/
CSF_LOGGER_API void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const std::string & message);

/**
* CSFLog - logs a message to a given CSFLogger with given log level priority if the logger's log level is at least the given priority.
*			Only available in CSFLogger library if built as a C++ implematation, and then only supported if consuming API is compiled for C++.
*
* @param pLogger - CSFLogger pointer to the specific logger
* @param priority - LogLevel enum priority: CSFTRACE, CSFDEBUG, CSFINFO, CSFWARNING, CSFERROR, CSFFATAL
* @param sourceFile - character pointer to capture the file name of the originating source code using __FILE__ macro
* @param sourceLine - integer using __LINE__ macro
* @param function - char pointer to capture the function name of the originating source code using __FUNCTION__ macro
* @param message - std::wstring to be logged
* @param 
*
*/
CSF_LOGGER_API void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const std::wstring & message);

/**
* CSFLog - logs a message to a given CSFLogger with given log level priority if the logger's log level is at least the given priority.
*			Only available in CSFLogger library if built as a C++ implematation, and then only supported if consuming API is compiled for C++.
*
* @param pLoggerName - CSFLogger name 
* @param priority - LogLevel enum priority: CSFTRACE, CSFDEBUG, CSFINFO, CSFWARNING, CSFERROR, CSFFATAL
* @param sourceFile - character pointer to capture the file name of the originating source code using __FILE__ macro
* @param sourceLine - integer using __LINE__ macro
* @param function - char pointer to capture the function name of the originating source code using __FUNCTION__ macro
* @param message - std::wstring to be logged
* @param
*
*/
CSF_LOGGER_API void CSFLog(std::string pLoggerName, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function, const std::wstring & message);

#define CSFLogFatalS(pLogger, message)  { std::ostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_FATAL,   __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogErrorS(pLogger, message)  { std::ostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_ERROR,   __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogWarnS(pLogger, message)   { std::ostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_WARNING, __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogInfoS(pLogger, message)   { std::ostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_INFO,    __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogDebugS(pLogger, message)  { if(CSFLog_isDebugEnabled(pLogger)) { std::ostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_DEBUG, __FILE__ , __LINE__ , __FUNCTION__ , _oss.str());  } }
#define CSFLogTraceS(pLogger, message)  { if(CSFLog_isTraceEnabled(pLogger)) { std::ostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_TRACE, __FILE__ , __LINE__ , __FUNCTION__ , _oss.str());  } }

#define CSFLogFatalWS(pLogger, message)  { std::wostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_FATAL,   __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogErrorWS(pLogger, message)  { std::wostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_ERROR,   __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogWarnWS(pLogger, message)   { std::wostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_WARNING, __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogInfoWS(pLogger, message)   { std::wostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_INFO,    __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
#define CSFLogDebugWS(pLogger, message)  { if(CSFLog_isDebugEnabled(pLogger)) { std::wostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_DEBUG, __FILE__ , __LINE__ , __FUNCTION__ , _oss.str());  } }
#define CSFLogTraceWS(pLogger, message)  { if(CSFLog_isTraceEnabled(pLogger)) { std::wostringstream _oss; _oss << message; CSFLog(pLogger, CSF_LOG_TRACE, __FILE__ , __LINE__ , __FUNCTION__ , _oss.str());  } }

CSF_LOGGER_API std::map<std::string, std::string> CSFLogger_getPIIDataList();
CSF_LOGGER_API std::string CSFLogger_replacePIIData(std::string data);

#endif //__cplusplus
#endif /* CSFLOGGER_HPP */
