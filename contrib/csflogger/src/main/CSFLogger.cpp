#include "csf/logger/CSFLogger.h"
#include "csf/logger/CSFLogger.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h> /* For strstr() */
#include <string>
#include <map>

#ifdef WIN32
#include <atlbase.h> /* For MultiByteToWideChar & co*/
#endif

#include "logger_utils.h"

/*
 * Suppress this warning, which appears many times in clang from log4cxx.h:
 *
 *  warning: __declspec attribute 'dllimport' is not supported [-Wignored-attributes]
 */
#ifdef __APPLE__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#endif

#include <log4cxx/logger.h>  
#include <log4cxx/logmanager.h> 
#include <log4cxx/logstring.h>
#include <log4cxx/helpers/exception.h>
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/xml/domconfigurator.h"
#include "log4cxx/helpers/messagebuffer.h"
#include "log4cxx/helpers/transcoder.h"
#include "log4cxx/patternlayout.h"

#ifdef __APPLE__
#pragma clang diagnostic pop
#endif

#define LOG_MSG_LEN 4096

/*! @file CSFLogger.cpp
* CSFLogger implements CSFLogger header using log4cxx library logging
*	manages the logging services:
*			initialize the service based on a config file or using default values. 
*			returns loggers based on their name
*			set/get log level or a given loggers
*			capture the log statements into the ocnfigured destination (or appender)
*
*/


/* Helper functions to convert log levels between CSF logger and log4cxx log levels */
log4cxx::LevelPtr convert2cxxLogLevel(CSFLogLevel level);
CSFLogLevel convert2CSFLogLevel(int cxxLevel);


static bool initialized = false;

#ifdef DEBUG
static const CSFLogLevel kDefaultLogLevel = CSF_LOG_DEBUG;
static const log4cxx::LevelPtr kDefaultLog4cxxLevel = log4cxx::Level::getDebug();
#else
static const CSFLogLevel kDefaultLogLevel = CSF_LOG_INFO;
static const log4cxx::LevelPtr kDefaultLog4cxxLevel = log4cxx::Level::getInfo();
#endif

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

	std::string name;				/*! name represents the namespace of this specific logger */
	log4cxx::LoggerPtr csfLogger;


};

/**
* Convert a utf-8 string into a utf-16 wstring
* @param confFile - full path to configuration file location
* @param propertyFile - a PropertyConfigurator if true or DOMConfigurator if false
* 
*/
std::wstring toWideString(const std::string& utf8String)
{
#ifdef WIN32
	// Convert UTF-8 to UTF-16
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], (int)utf8String.size(), NULL, 0);
	std::wstring utf16String(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], (int)utf8String.size(), &utf16String[0], size_needed);
	return utf16String;
#else
	return L"";
#endif
}

std::string toString(const std::wstring& utf16String)
{
    // Convert UTF-16 to UTF-8
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], (int)utf16String.size(), NULL, 0, NULL, NULL);
    std::string utf8String(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], (int)utf16String.size(), &utf8String[0], size_needed, NULL, NULL);
    return utf8String;
}

/**
* Configure the appropriate log4cxx Configurator.
* @param confFile - full path to configuration file location
* @param propertyFile - a PropertyConfigurator if true or DOMConfigurator if false
* 
*/
void configurelog4cxx(std::string confFile, bool propertyFile)
{
	if(propertyFile)
	{
		/* Configure the logging framework based on the provided config file */
		
#ifdef WIN32
		/*
		On Windows the wstring version must be used in order to support international characters in utf-8 strings.
		There is a problem on Windows with the fstream(..) family from the std. If the string passed to these methods
		is utf-8 encoded they don't correctly interpret it. So characters such as � become ö.
		*/
		log4cxx::PropertyConfigurator::configure(toWideString(confFile));
#else
		log4cxx::PropertyConfigurator::configure(confFile);
#endif
	}
	else
	{
		/* the config file is an xml file */
		
#ifdef WIN32
		log4cxx::xml::DOMConfigurator::configure(toWideString(confFile));
#else
		log4cxx::xml::DOMConfigurator::configure(confFile);
#endif
	}
}

void CSFLogger_initialize(const char* confFile){	
}

/**
* CSFLogger_init - initializes the CSF logger based on properties in a config file or default to basic configuration if no file is provided
* @param confFile - full path to configuration file location 
* 
*/
void CSFLogger_init(const char* confFile){

	if(!initialized){

        if (confFile == NULL || confFile[0] == '\0')
        {
			try
            {
                /* Use Basic configuration to initialize the logging framework */
                log4cxx::BasicConfigurator::configure();
            }
            catch (...)
            {
                fprintf(stderr, "Logger is not initialized. log4cxx::BasicConfigurator::configure() threw a C++ exception.\n");
                return;
            }
        }else{
            try
            { 
                configurelog4cxx(confFile, strstr(confFile, ".xml") == NULL);

                /* Check if the property configuration was successful */
                if(log4cxx::Logger::getRootLogger() == NULL){ 
                    fprintf(stderr, "Logger is not initialized. Please check that \"%s\" exists.\n", confFile);
                    return;
                }    
            }
            catch (...)
            {
                fprintf(stderr, "Logger is not initialized. log4cxx::PropertyConfigurator::configure() threw a C++ exception. Please check that \"%s\" exists.\n", confFile);
                return;
            }    
	
			/* Get configured log4cxx loggers into our csf logger map */
			log4cxx::LoggerList cxxLoggers = log4cxx::LogManager::getCurrentLoggers();

			std::map<std::string, CSFLogger*>& loggers = getCSFLoggerList();

			log4cxx::LoggerList::iterator it = cxxLoggers.begin();

			while(it != cxxLoggers.end())
			{
				CSFLogger* cLogger = new CSFLogger;
				LOG4CXX_ENCODE_CHAR(loggerName,(*it)->getName());
		
				cLogger->name = loggerName;
				cLogger->csfLogger = (*it); 

				loggers.insert(std::pair<std::string, CSFLogger*>(loggerName,cLogger));

				it++;
		
			}
		}

		initialized = true;
	}
	
}


/**
* CSFLogger_getLogger() - gets the logger of the given name or creates a logger with the given name if it does not exists (under the right hierarchy).
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
            try
            {
                pLogger->csfLogger = log4cxx::LogManager::getLogger(name);
            }
            catch (...)
            {
                delete pLogger;
                return NULL;
            }
			
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
		try
		{
			rootLogger->csfLogger = log4cxx::Logger::getRootLogger();
		}
		catch (...)
        {
            delete rootLogger;
            return NULL;
       }

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


void CSFLogger_setLogLevel(CSFLogger *pLogger, CSFLogLevel level){

	if(pLogger != NULL)
	{
		try
		{
			pLogger->csfLogger->setLevel(convert2cxxLogLevel(level));
		}
		catch (...) 
		{
		}
	}
}

/**
* CSFLogger_getLogLevel - gets the log level for the given logger
* @param CSFLogger struct pointer of the logger to be queried
* @return CSFLogLevel enum value of this specific logger
*/


CSFLogLevel CSFLogger_getLogLevel(CSFLogger *pLogger){
	
	if(pLogger == NULL)
	{
		return kDefaultLogLevel;
	}

	int cxxLevel =0;
	
	try {
		cxxLevel = pLogger->csfLogger->getEffectiveLevel()->toInt();
	}
	catch (log4cxx::helpers::NullPointerException &) {
		
		fprintf(stderr, "Failed to get the log level due to an NullPointerException from log4cxx. \n");
		return kDefaultLogLevel;  /*return default. Once OFF level is supported we can return that instead. */
	}
	catch (...) {
		
		fprintf(stderr, "Failed to get the log level due to an exception from log4cxx. \n");
		return kDefaultLogLevel;  /*return default. Once OFF level is supported we can return that instead. */
	}

	return convert2CSFLogLevel(cxxLevel);
}

/**
* CSFLogger_getGlobalLogLevel - gets the log level for the log controller
* Provide backward compatable API for the new (secure) logger.
* @return CSFLogLevel enum value of the logcontroller
*/
CSFLogLevel CSFLogger_getGlobalLogLevel()
{
	return CSFLogger_getLogLevel(CSFLogger_getRootLogger());
}

void CSFLog_enableDetailLog(bool detail)
{
    CSFLogLevel level = detail? CSF_LOG_DEBUG:CSF_LOG_INFO;

    CSFLogger_setLogLevel(CSFLogger_getRootLogger(), level);
    std::map<std::string, CSFLogger*>&loggers =  getCSFLoggerList();

    std::map<std::string, CSFLogger*>::iterator it = loggers.begin();
    while (it != loggers.end())
    {
        CSFLogger_setLogLevel(it->second, level);
        ++it;
    }

    return;
}

void CSFLogV(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const char* format, va_list args)
{
    if (pLogger == NULL)
    {
        return;
    }
	
    try
    {
		log4cxx::LevelPtr level = convert2cxxLogLevel(priority);

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

		    if ((pTrimmedString != NULL) && (level != NULL))
		    {
			    pLogger->csfLogger->log(level, pTrimmedString, log4cxx::spi::LocationInfo(sourceFile, function, sourceLine ));
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

#ifdef __cplusplus
void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const std::string & message)
{
    if (pLogger == NULL)
    {
        return;
    }
	
    try
    {
		log4cxx::LevelPtr level = convert2cxxLogLevel(priority);

		if (level != NULL)
		{
		/*	On windows csfLogger->log is acualy implemented as utf-8
			placing a wstring into the messase will actaly mess up the logs
		*/
#ifdef WIN32
			pLogger->csfLogger->log(level, toString(message), log4cxx::spi::LocationInfo(sourceFile, function, sourceLine ));
#else
			pLogger->csfLogger->log(level, message, log4cxx::spi::LocationInfo(sourceFile, function, sourceLine ));
#endif
		}
    }
    catch (...)
    {
		fprintf(stderr, "Failed to log message due to an exception from log4cxx. \n");
    }
}

void CSFLog(CSFLogger *pLogger, CSFLogLevel priority, const char* sourceFile, int sourceLine, const char* function , const std::wstring & message)
{
    if (pLogger == NULL)
    {
        return;
    }
	
    try
    {
		log4cxx::LevelPtr level = convert2cxxLogLevel(priority);

		if (level != NULL)
		{
			pLogger->csfLogger->log(level, message, log4cxx::spi::LocationInfo(sourceFile, function, sourceLine ));
		}
    }
    catch (...)
    {
		fprintf(stderr, "Failed to log message due to an exception from log4cxx. \n");
    }
}

#endif

log4cxx::LevelPtr convert2cxxLogLevel(CSFLogLevel level)
{
	switch(level)
	{
		case CSF_LOG_OFF: return log4cxx::Level::getOff(); 
		case CSF_LOG_FATAL: return log4cxx::Level::getFatal();
		case CSF_LOG_ERROR: return log4cxx::Level::getError();
		case CSF_LOG_WARNING: return log4cxx::Level::getWarn();
		case CSF_LOG_INFO: return log4cxx::Level::getInfo();
		case CSF_LOG_DEBUG: return log4cxx::Level::getDebug();
		case CSF_LOG_TRACE: return log4cxx::Level::getTrace();
		default: return kDefaultLog4cxxLevel;
	}

	return kDefaultLog4cxxLevel;
}

CSFLogLevel convert2CSFLogLevel(int cxxLevel)
{
	switch(cxxLevel)
	{
		case log4cxx::Level::OFF_INT: 
			return CSF_LOG_OFF;
		case log4cxx::Level::FATAL_INT:
			return CSF_LOG_FATAL;
		case log4cxx::Level::ERROR_INT:
			return CSF_LOG_ERROR;
		case log4cxx::Level::WARN_INT:
			return CSF_LOG_WARNING;
		case log4cxx::Level::INFO_INT:
			return CSF_LOG_INFO;
		case log4cxx::Level::DEBUG_INT:
			return CSF_LOG_DEBUG;
		case log4cxx::Level::TRACE_INT:
		case log4cxx::Level::ALL_INT: /* ALL_INT is level less than trace but we will default it to trace*/
			return CSF_LOG_TRACE;
		default:
			return kDefaultLogLevel;
	}

	return kDefaultLogLevel;
}

