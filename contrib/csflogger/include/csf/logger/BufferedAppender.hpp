#pragma once

#include "csf/logger/CSFLog.h"
#include "csf/logger/LogAppender.hpp"
#include "csf/logger/SmartPointer.hpp"

#include <string>

namespace CSF
{
	namespace csflogger
	{
		class FileSystem;
		class Configuration;
		class Formatter;

		class CSF_LOGGER_API BufferedAppender : public LogAppender
		{
		public:
			
			BufferedAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem);
			BufferedAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem, SMART_PTR_NS::shared_ptr<Formatter> formatter);

			virtual ~BufferedAppender();

			void append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage);
			void appendToBuffer(const std::string & logMessage);
			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn);
            void getBuffer(std::string& logMessage);
			void flushBuffer();		
			void clearBuffer();		
		private:
			class Impl;
			SMART_PTR_NS::shared_ptr<Impl> pimpl;
		};
	}
}