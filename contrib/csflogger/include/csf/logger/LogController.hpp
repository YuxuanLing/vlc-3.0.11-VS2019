#pragma once

#include "csf/logger/CSFLog.h"
#include "csf/logger/SmartPointer.hpp"
#include "csf/logger/LoggerDefaults.hpp"
#include <string>

namespace CSF
{
	namespace csflogger
	{
		class LogAppender;
		class FileSystemProvider;
		class Configuration;

		//class CSF_LOGGER_API LogController
		class LogController
		{
		public:
			
			LogController();
			LogController(SMART_PTR_NS::shared_ptr<FileSystemProvider> fileSystemProvider, LogAppenderType appenderType);
			LogController(SMART_PTR_NS::shared_ptr<Configuration> configuration);

			virtual ~LogController();

			static SMART_PTR_NS::shared_ptr<LogController> GetInstance();
			static void ResetInstance();
	
			virtual void init(std::string fileName);
			
			void setAppender(SMART_PTR_NS::shared_ptr<LogAppender> appender);
			SMART_PTR_NS::shared_ptr<LogAppender> getAppender();

			//This function is virtual so that it can be mocked, and tested from the LoggerService
			virtual void setLogLevel(CSFLogLevel logLevel);
			virtual void setAppenderType(const LogAppenderType type);
			CSFLogLevel getLogLevel();	

			virtual bool getFileWritingAllowed();
			virtual void setFileWritingAllowed(bool enable);
			virtual std::string getLogDirectory();
			virtual void log(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage);
			virtual void flush();
			virtual void deleteLogs();
			virtual void setCanDeleteLogs(bool canDeleteLogs);
			virtual void deleteMatchingLogs(std::string pattern);
			virtual void setBufferSize(unsigned int sizeInMB);
			virtual void deleteLogsIfWritingNotAllowed();
			virtual void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn);
			virtual void clearThreadIdHandler();
			virtual void flushBufferIfAllowed();
			virtual void setMaxNumberOfFiles(unsigned int numberOfFiles);

			virtual void setPIIDataHashEnablement(bool enable);
			virtual bool getPIIDataHashEnablement();
		private:
			class Impl;
			SMART_PTR_NS::shared_ptr<Impl> pimpl;
			static SMART_PTR_NS::shared_ptr<LogController> instance;
		};
	}
}
