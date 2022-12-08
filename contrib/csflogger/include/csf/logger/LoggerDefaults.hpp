#pragma once

#include "CSFLog.h"
#include "LogAppender.hpp"
#include "LogAppenderType.hpp"
#include <string>
#include <iostream>

using namespace std;

namespace CSF
{
	namespace csflogger
	{
	
	//After discussion with Diego, we may add more directives here to force the configurion.
	//If no "forceful" directives are set at pre-compile time, then default to platform specific configurations. 
	#ifdef FORCE_LOGGER_INIT_FILEAPPENDER
		const std::string FileName = "cpve.log";
		const unsigned int MaxFileSize = 10;
		const unsigned int MaxNumberOfFiles = 5;
		const unsigned int BufferSize = 5;
		const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
		const LogAppenderType DefaultLogAppenderType = File;
		const bool LoggingAllowedBeforeLogControllerInitialization = true;
	#else 
		#ifdef DEBUG 
			#ifdef _WIN32
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 10;
				const unsigned int BufferSize = 5;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = File;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;
			#elif __APPLE__
				#ifdef IOS
					const std::string FileName = "cpve.log";
					const unsigned int MaxFileSize = 10;
					const unsigned int MaxNumberOfFiles = 5;
					const unsigned int BufferSize = 1;
					const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
					const LogAppenderType DefaultLogAppenderType = NSLog;
					const bool LoggingAllowedBeforeLogControllerInitialization = true;
				#else
					const std::string FileName = "cpve.log";
					const unsigned int MaxFileSize = 10;
					const unsigned int MaxNumberOfFiles = 10;
					const unsigned int BufferSize = 5;
					const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
					const LogAppenderType DefaultLogAppenderType = NSLog;
					const bool LoggingAllowedBeforeLogControllerInitialization = true;
				#endif
			#elif __ANDROID__
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 5;
				const unsigned int BufferSize = 1;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = AndroidLogCat;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;
			#elif __linux__
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 5;
				const unsigned int BufferSize = 1;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = File;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;
			#else
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 5;
				const unsigned int BufferSize = 5;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = File;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;
			#endif
		#else 
			#ifdef _WIN32
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 10;
				const unsigned int BufferSize = 5;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = Buffered;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;  
			#elif __APPLE__
				#ifdef IOS
					const std::string FileName = "cpve.log";
					const unsigned int MaxFileSize = 10;
					const unsigned int MaxNumberOfFiles = 5;
					const unsigned int BufferSize = 1;
					const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
					const LogAppenderType DefaultLogAppenderType = Buffered;
					const bool LoggingAllowedBeforeLogControllerInitialization = true;
				#else
					const std::string FileName = "cpve.log";
					const unsigned int MaxFileSize = 10;
					const unsigned int MaxNumberOfFiles = 10;
					const unsigned int BufferSize = 5;
					const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
					const LogAppenderType DefaultLogAppenderType = Buffered;
					const bool LoggingAllowedBeforeLogControllerInitialization = true;
				#endif
			#elif __ANDROID__
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 5;
				const unsigned int BufferSize = 1;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = Buffered;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;
			#elif __linux__
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 5;
				const unsigned int BufferSize = 5;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = Buffered;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;
			#else
				const std::string FileName = "cpve.log";
				const unsigned int MaxFileSize = 10;
				const unsigned int MaxNumberOfFiles = 5;
				const unsigned int BufferSize = 1;
				const CSFLogLevel DefaultLogLevel = CSF_LOG_DEBUG;
				const LogAppenderType DefaultLogAppenderType = File;
				const bool LoggingAllowedBeforeLogControllerInitialization = true;
			#endif
		#endif
	#endif
	}
}


