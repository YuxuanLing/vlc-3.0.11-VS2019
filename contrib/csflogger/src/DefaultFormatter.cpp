#include "csf/logger/DefaultFormatter.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include <iomanip>
#include <cstring>
#ifndef _WIN32
    // Visual Studio 2008 has an issue with this header as it is only available
    // on C++11. Most compilers will include it automatically except for GCC running
    // on SLED. Surrounding this with a guard to stop it from being included on
    // Windows. Visual Studio 2013 should include this header automatically but for
    // the moment while the tranistion to 2013 and away from 2008 is in progress this
    // is still needed.
    #include <stdint.h>
#endif // _WIN32
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "csf/utils/FormattedTimeUtils.h"

namespace CSF
{
	namespace csflogger
	{
		static const size_t MAX_THREAD_ID_LENGTH = 500;

		void defaultThreadHandlerFxn(char * threadId)
		{
			uintptr_t id;
		#ifdef _WIN32
			id = ::GetCurrentThreadId();
		#elif defined __APPLE__
			id = reinterpret_cast<uintptr_t> (pthread_self());
		#else
			id = pthread_self();
		#endif

			std::stringstream stream;
		  	stream << "0x" 
				<< std::setfill ('0') << std::setw(sizeof(uintptr_t)*2) 
		        << std::hex << id;
		    //strcpy(threadId, stream.str().substr(0,MAX_THREAD_ID_LENGTH).c_str());
			strcpy_s(threadId, MAX_THREAD_ID_LENGTH, stream.str().substr(0, MAX_THREAD_ID_LENGTH).c_str());
		}

		class DefaultFormatter::Impl
		{
		public:
			
			std::string formatMessage(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
			{
				std::string logLine;

                char threadId[MAX_THREAD_ID_LENGTH];
				threadIdHandlerFxn(threadId);
                logLine = getFormattedLocalTimestamp() + " " + formatLogLevel(logMessageLevel) + " [" + threadId + "] [" + Formatter::formatPathAndLine(sourceFile, sourceLine) + "] [" + loggerName + "] [" + function + "] - " + logMessage + "\n";
                return logLine;
			}

			std::string formatLogLevel(CSFLogLevel logLevel)
			{
			    switch(logLevel)
			    {
			        case CSF_LOG_INFO:      return "INFO "; break;
			        case CSF_LOG_DEBUG:     return "DEBUG"; break;
			        case CSF_LOG_WARNING:   return "WARN "; break;
			        case CSF_LOG_ERROR:     return "ERROR"; break;
			        case CSF_LOG_FATAL:     return "FATAL"; break;
			        case CSF_LOG_TRACE:     return "TRACE"; break;
			        case CSF_LOG_OFF:       return "OFF"; break;
			        default: return "";
			    }
			}

			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
			{
				if(threadIdHandlerFxn != NULL)
				{
					this->threadIdHandlerFxn = threadIdHandlerFxn;
				}
				else
				{
					this->threadIdHandlerFxn = &defaultThreadHandlerFxn;
				}
			}

			Impl() : threadIdHandlerFxn(&defaultThreadHandlerFxn)
			{

			}

			~Impl()
			{

			}

		private:
			ThreadIdHandlerFxn threadIdHandlerFxn;
		};

		std::string DefaultFormatter::formatMessage(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
		{
			return pimpl->formatMessage(loggerName,logMessageLevel,sourceFile,sourceLine,function,logMessage);
		}
		void DefaultFormatter::setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
		{
			pimpl->setThreadIdHandler(threadIdHandlerFxn);
		}

		DefaultFormatter::DefaultFormatter() : 
            pimpl(new Impl())
		{
		}

		DefaultFormatter::~DefaultFormatter()
		{
		}

	}
}
