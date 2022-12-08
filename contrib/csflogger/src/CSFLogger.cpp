#include "csf/logger/CSFLogger.h"
#include "csf/logger/CSFLogger.hpp"
#include "csf/logger/LogController.hpp"
#include "csf/logger/LoggerDefaults.hpp"
#include "csf/logger/StringUtils.hpp"
#include "csf/utils/Mutex.hpp"
#include "csf/utils/ScopedLock.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h> /* For strstr() */
#include <string>
#include <map>

#include "logger_utils.h"

#ifdef __APPLE__
#include "ObjCHelpers.h"
#endif

#ifdef _WIN32
#include "Windows.h"
#endif

#define LOG_MSG_LEN 4096

static CSFLogger* rootLogger;

static std::map<std::string, CSFLogger*>& getCSFLoggerList() {
		
	static std::map<std::string, CSFLogger*> loggers;
    return loggers;
}

/*! @struct CSFLogger
* @brief encapsulates the different properties of a Logger.
* Properties such as name, log level, log destination, log message format, maximum number of lof files, and maximum size of a specific log file
*
*/

struct CSFLogger{

	std::string name;			/*! name represents the namespace of this specific logger */
	CSFLogLevel level;
    CSFLogger() :name(), level(CSF_LOG_DEBUG){}
};

const char* CSFLogLevel_toString(CSFLogLevel logLevel)
{
    switch(logLevel)
    {
        case CSF_LOG_OFF:       return "OFF"; break;
        case CSF_LOG_FATAL:     return "FATAL"; break;
        case CSF_LOG_ERROR:     return "ERROR"; break;
        case CSF_LOG_WARNING:   return "WARN"; break;
        case CSF_LOG_INFO:      return "INFO"; break;
        case CSF_LOG_DEBUG:     return "DEBUG"; break;
        case CSF_LOG_TRACE:     return "TRACE"; break;
        default: return "";
    }
}

/**
* CSFLogger_init - is depracated in the new logger.
* @param confFile - full path to configuration file location 
* 
*/
void CSFLogger_init(const char* fileName)
{   
}

void CSFLogger_initialize(const char* fileName)
{
    CSF::csflogger::LogController::GetInstance()->init(std::string(fileName));
}

/**
* CSFLogger_flush - write the log in buffer to the log file
* 
*/
void CSFLogger_flush()
{
    CSF::csflogger::LogController::GetInstance()->flush();
}

/**
* CSFLogger_getLogger() - gets the logger of the given name or creates a logger with the given name if it does not exist (under the right hierarchy).
* @param name character pointer that is unique for each logger
* @return CSFLogger struct pointer to the logger requested. return NULL if any error occurs such as logging framework was not initialized or the logger name was null.
*/
CSFLogger* CSFLogger_getLogger(const char* name)
{

	if(name == NULL)
	{
		fprintf(stderr, "Logger name can not be null");
		return NULL;
	}
	
	if(name[0] == '\0')
		return CSFLogger_getRootLogger();

	std::map<std::string, CSFLogger*>& loggers = getCSFLoggerList();
	std::map<std::string, CSFLogger*>::iterator it = loggers.find(name);
    if(it == loggers.end())
    {
    	CSFLogger *pLogger = new CSFLogger; 

    	//set CSFLogger properties
    	pLogger->name = name;
        pLogger->level = CSF::csflogger::DefaultLogLevel;

    	loggers.insert(std::pair<std::string, CSFLogger*>(name, pLogger));
    	
        return pLogger;
    }
    else
    {	
        return it->second;
    }

}

/**
* CSFLogger_getLogger() - gets the root logger.
* @return CSFLogger struct pointer to the root logger 
*/
CSFLogger* CSFLogger_getRootLogger()
{
	if(rootLogger == NULL)
	{
		rootLogger = new CSFLogger; 

		//set CSFLogger properties
		rootLogger->name = "";
        rootLogger->level = CSF::csflogger::DefaultLogLevel;
	}
	return rootLogger;

}


/**
* CSFLogger_getName - gets the name of the given logger
* @param CSFLogger struct pointer of the logger 
* @return Char pointer to the logger name
*/
const char* CSFLogger_getName(CSFLogger* pLogger)
{
	if(pLogger == NULL)
	{
		return NULL;
	}

	return pLogger->name.c_str();
}

/**
* CSFLogger_setLogLevel - sets the given log level for the given logger
* @param CSFLogger struct pointer of the logger whose log level will be updated
* @param CSFLogLevel enum value that the log level will be updated to
*/
void CSFLogger_setLogLevel(CSFLogger *pLogger, CSFLogLevel level)
{
	if(pLogger != NULL)
	{
		pLogger->level = level;
        //CSF::csflogger::LogController::GetInstance()->setLogLevel(level);
	}
}


/**
* CSFLogger_getGlobalLogLevel - gets the log level for the log controller
* @return CSFLogLevel enum value of the logcontroller
*/
CSFLogLevel CSFLogger_getGlobalLogLevel()
{
    return CSF::csflogger::LogController::GetInstance()->getLogLevel();
}

/**
* CSFLogger_getGlobalLogLevel - gets the log level for the log controller
* @param CSFLogLevel enum value that the log level will be updated to
*/
void CSFLogger_setGlobalLogLevel(CSFLogLevel level)
{
    CSF::csflogger::LogController::GetInstance()->setLogLevel(level);
}

/**
* CSFLogger_getLogLevel - gets the log level for the given logger
* @param CSFLogger struct pointer of the logger to be queried
* @return CSFLogLevel enum value of this specific logger
*/
CSFLogLevel CSFLogger_getLogLevel(CSFLogger *pLogger)
{
    return pLogger->level;
    //return CSF::csflogger::LogController::GetInstance()->getLogLevel();
}

void CSFLogV(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const char* format, va_list args)
{
    if (pLogger == NULL)
    {
        return;
    }
	
    try
    {
        //If a va_list type is passed into a sub-function (as it is in this case) need to make a copy.
        //See http://blog.julipedia.org/2011/09/using-vacopy-to-safely-pass-ap.html

        va_list args2;
        va_copy(args2, args);
        char * pAllocatedMessage = generate_message(LOG_MSG_LEN,format, args2);
        va_end(args2);

        if (pAllocatedMessage != NULL)
        {
            //Strip any CR/LF off the end of this string before logging to reduce number of blank lines
            //appearing in log file.

            char * pTrimmedString = logger_trimwhitespace(pAllocatedMessage);

		    if ((pTrimmedString != NULL))
		    {
                CSF::csflogger::LogController::GetInstance()->log(pLogger->name, priority, sourceFile, sourceLine, function, pTrimmedString );
		    }

            free(pAllocatedMessage);
        }
        else
        {
		    fprintf(stderr, "Failed to allocate buffer for logged message.\n");
        }
    }
    catch (...)
    {
		fprintf(stderr, "Failed to log message due to an exception from log4cxx. \n");
    }
}	

void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const char* format, ...)
{
    if (pLogger == NULL)
    {
        return;
    }

	va_list ap;

	va_start(ap, format);
	CSFLogV(pLogger, priority, sourceFile, sourceLine, function, format, ap);
	va_end(ap);
}

void CSFLogger_SetThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
{
    CSF::csflogger::LogController::GetInstance()->setThreadIdHandler(threadIdHandlerFxn);    
}

void CSFLogger_ClearThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
{
    CSF::csflogger::LogController::GetInstance()->clearThreadIdHandler();  
}

#ifdef __cplusplus
void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const std::string & message)
{
    if (pLogger == NULL)
    {
        return;
    }

    if (CSF::csflogger::LogController::GetInstance())
    {
        CSF::csflogger::LogController::GetInstance()->log(pLogger->name, priority, sourceFile, sourceLine, function, message );
    }
}

std::string toString(const std::wstring& utf16String)
{
    #ifdef _WIN32
    // Convert UTF-16 to UTF-8
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], (int)utf16String.size(), NULL, 0, NULL, NULL);
    std::string utf8String(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], (int)utf16String.size(), &utf8String[0], size_needed, NULL, NULL);
    return utf8String;
    #elif defined __APPLE__
    return CSFLoggerStdStringForStdWString(utf16String);
    #else
    // FIXME: This only works for ASCII characters.
    std::string out(utf16String.begin(), utf16String.end());
    return out;
    #endif
}

void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const std::wstring & message)
{
    if (pLogger == NULL)
    {
        return;
    }
	
    CSF::csflogger::LogController::GetInstance()->log(pLogger->name, priority, sourceFile, sourceLine, function, toString(message) );
}

void CSFLog(string pLoggerName, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function, const std::wstring & message)
{
	CSF::csflogger::LogController::GetInstance()->log(pLoggerName, priority, sourceFile, sourceLine, function, toString(message) );
}

#endif

static csf::csflogger::Mutex pIIDataMutex;
static std::map<std::string, std::string> *pIIDataList = NULL;

static std::map<std::string, std::string>* getPIIDataList()
{
    csf::csflogger::ScopedLock scopedLock(pIIDataMutex);
    if (NULL == pIIDataList)
    {
        pIIDataList = new std::map<std::string, std::string>();
    }
    return pIIDataList;
}

std::string CSFLogger_replacePIIData(std::string data)
{
    if (!CSF::csflogger::LogController::GetInstance()->getPIIDataHashEnablement())
        return data;

    if (data.empty())
        return "";

    static csf::csflogger::Mutex pIIDataMutex;
    std::map<std::string, std::string>* dataList = getPIIDataList();
    {
        csf::csflogger::ScopedLock scopedLock(pIIDataMutex);
        auto it = dataList->find(data);
        if (it !=dataList->end())
        {
            return "{!" + it->second + "!}";
        }
    }

    auto hash = CSF::csflogger::StringUtils::generateHashBasedOnSHA256(data);
    if (!hash.empty())
    {
            auto subHash = hash.substr(0,16);
            csf::csflogger::ScopedLock scopedLock(pIIDataMutex);
            dataList->emplace(data, subHash);
            return "{!" + subHash + "!}";
    }
    else
    {
            fprintf(stderr, "Failed to generate hash.\n");
            return "{!!}";
    }
}

std::map<std::string, std::string> CSFLogger_getPIIDataList()
{
    csf::csflogger::ScopedLock scopedLock(pIIDataMutex);
    return *getPIIDataList();
}
