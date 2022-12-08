#pragma once
#include "csf/logger/CSFLog.h"
#include <string>

namespace CSF
{
	namespace csflogger
	{
		class CSF_LOGGER_API LogAppender
		{
		public:
			virtual void append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage) = 0;	
			virtual void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn) = 0;
		};
	}
}