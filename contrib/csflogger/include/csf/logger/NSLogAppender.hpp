#pragma once

#include "csf/logger/CSFLog.h"
#include "csf/logger/LogAppender.hpp"
#include "csf/logger/SmartPointer.hpp"
#include "csf/logger/FileAppender.hpp"

#include <string>

namespace CSF
{
	namespace csflogger
	{
		class FileSystem;
		class Configuration;
		class Formatter;

		class CSF_LOGGER_API NSLogAppender : public FileAppender
		{
		public:
			
			NSLogAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem);

			virtual ~NSLogAppender();

			void append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage);
			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn);
		private:
			class Impl;
			SMART_PTR_NS::shared_ptr<Impl> pimpl;
		};
	}
}