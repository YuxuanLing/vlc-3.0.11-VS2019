#include "csf/logger/LogController.hpp"
#include "csf/logger/LogAppender.hpp"
#include "csf/logger/BasicFileAppender.hpp"
#include "csf/logger/BufferedAppender.hpp"
#include "csf/logger/LoggerDefaults.hpp"
#include "csf/logger/Configuration.hpp"
#include "csf/logger/FileAppender.hpp"
#include "csf/logger/FileSystem.hpp"
#include "csf/logger/FileSystemProvider.hpp"
#include "csf/logger/FileUtils.hpp"
#include "csf/utils/ScopedLock.hpp"
#include "csf/utils/Mutex.hpp"
#include "csf/utils/ThreadPool.hpp"
#include "csf/utils/Thread.hpp"
#include "csf/utils/csf_predicates.h"
#include "csf/logger/Formatter.hpp"
#ifdef __ANDROID__
#include "csf/logger/AndroidLogCatAppender.hpp"
#endif
#include "csf/logger/ConsoleLogAppender.hpp"
#ifdef __APPLE__
#include "csf/logger/NSLogAppender.hpp"
#endif
#include "csf/utils/FormattedTimeUtils.h"


namespace CSF
{
	namespace csflogger
	{
		class LogController::Impl
		{
		public:
			
			Impl(SMART_PTR_NS::shared_ptr<FileSystemProvider> fileSystemProvider, LogAppenderType appenderType) :
				logAppenderType(appenderType),
				fileSystemProvider(fileSystemProvider),
				configuration(new Configuration(FileName, MaxFileSize, MaxNumberOfFiles, BufferSize)),
				logLevel(DefaultLogLevel),
				isInitCalled(false),
				threadIdHandlerFxn(NULL),
				canDeleteLogs(true),
				pIIDataHashEnable(false)
			{
				appender = buildAppender(configuration);
			}

			Impl(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystemProvider> fileSystemProvider) :
				logAppenderType(CSF::csflogger::DefaultLogAppenderType),
				fileSystemProvider(fileSystemProvider),
				configuration(configuration),
				logLevel(DefaultLogLevel),
				isInitCalled(false),
				threadIdHandlerFxn(NULL),
				canDeleteLogs(true),
				pIIDataHashEnable(false)
			{
				threadIdHandlerFxn = NULL;
				appender = buildAppender(configuration);
			}

			virtual ~Impl()
			{
			}

			void log(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
			{
				csf::csflogger::ScopedLock lock(logMutex);

				if( isLoggingAllowed() &&  
					logMessageLevel <= this->logLevel &&
					appender )
				{
						csf::csflogger::ScopedLock lock(logMutex);
						appender->append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
				}
			}

			void setAppender(SMART_PTR_NS::shared_ptr<LogAppender> appender)
			{
				// A LogAppender, such as the BufferedAppender can have it's own configuration object.
				// This can lead to a mismatch between the configuration of the controller and the configuration of the appender.
				// It is the caller's responsibility to ensure that the same configurations are in use.
				csf::csflogger::ScopedLock lock(logMutex);
				this->appender = appender;
			}

			SMART_PTR_NS::shared_ptr<LogAppender> getAppender()
			{
				csf::csflogger::ScopedLock lock(logMutex);
				return this->appender;
			}

			void setLogLevel(CSFLogLevel newLogLevel)
			{
				csf::csflogger::ScopedLock lock(logMutex);
				this->logLevel = newLogLevel;
			}

			void setAppenderType(const LogAppenderType type)
			{
				csf::csflogger::ScopedLock lock(logMutex);
				
				if(this->logAppenderType != type)
				{
					this->logAppenderType = type;
					appender = buildAppender(configuration);
				}
			}

			CSFLogLevel getLogLevel()
			{
				csf::csflogger::ScopedLock lock(logMutex);
				return this->logLevel;
			}

			void init(std::string fileName)
			{
				csf::csflogger::ScopedLock lock(logMutex);

                std::string bufferedLogMessage;
                auto bufferAppender = std::dynamic_pointer_cast<BufferedAppender>(appender);
                if (bufferAppender)
                {
                    bufferAppender->getBuffer(bufferedLogMessage);
                    bufferAppender->clearBuffer();
                }
                
                configuration->setFullPathFileName(fileName);
                appender = buildAppender(configuration);
                //appender->append("LogController", CSFLogLevel::CSF_LOG_INFO, __FILE__, __LINE__, __FUNCTION__, "***** LogController init , start logging *****");

                bufferAppender = std::dynamic_pointer_cast<BufferedAppender>(appender);
                if (bufferAppender)
                {
                    bufferAppender->appendToBuffer(bufferedLogMessage);
                }

                isInitCalled = true;
			}

			std::string getLogDirectory()
			{
				return configuration->getDirectory();
			}

			SMART_PTR_NS::shared_ptr<LogAppender> buildAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration)
			{		
				SMART_PTR_NS::shared_ptr<LogAppender> logAppender;
				fileSystem = fileSystemProvider->getFileSystem();

				switch(logAppenderType) 
				{
					#ifdef __ANDROID__
					case AndroidLogCat :
					{
						logAppender = SMART_PTR_NS::shared_ptr<LogAppender>(new AndroidLogCatAppender(configuration));	
						break;					
					}
					#endif
				    case BasicFile : 
				    {	
				    	logAppender = SMART_PTR_NS::shared_ptr<LogAppender>(new BasicFileAppender(fileSystem));
				    	break;	
				    }
				    case Buffered : 
				    {	
						logAppender = SMART_PTR_NS::shared_ptr<LogAppender>(new BufferedAppender(configuration,fileSystem));
				    	break;	
				    }
				    case File : 
				    {	
						logAppender = SMART_PTR_NS::shared_ptr<LogAppender>(new FileAppender(configuration, fileSystem));				    
						break;	
					}
				    case ConsoleLog :
				    {
				    	return SMART_PTR_NS::shared_ptr<LogAppender>(new ConsoleLogAppender(configuration, fileSystem));
				    }
				    #ifdef __APPLE__
				    case NSLog :
				    {
				    	return SMART_PTR_NS::shared_ptr<LogAppender>(new NSLogAppender(configuration, fileSystem));
				    }
				    #endif
				}
				if(threadIdHandlerFxn != NULL)
				{
					appender->setThreadIdHandler(threadIdHandlerFxn);
				}
				return logAppender;
			}

			void flush()
			{
				csf::csflogger::ScopedLock lock(logMutex);

				SMART_PTR_NS::shared_ptr<BufferedAppender> bufferAppender =  SMART_PTR_NS::dynamic_pointer_cast<BufferedAppender>(appender);
				if(bufferAppender)
				{
					bufferAppender->flushBuffer();
				}
			}

			void deleteLogs()
			{
				csf::csflogger::ScopedLock lock(logMutex);
				
                if (canDeleteLogs)
                {
                    if(fileSystem)
                    {
                        fileSystem->deleteLogs(configuration->getDirectory(), configuration->getFileName());
                    }
                }
			}

            void setCanDeleteLogs(bool canDeleteLogs)
            {
                this->canDeleteLogs = canDeleteLogs;
            }

			void deleteMatchingLogs(std::string pattern)
			{
				csf::csflogger::ScopedLock lock(logMutex);

				FileUtils::deleteMatchingLogs(configuration->getDirectory(), pattern);
			}

			bool getFileWritingAllowed()
			{
				csf::csflogger::ScopedLock lock(logMutex);
				return configuration->getFileWritingAllowed();
			}

			void setBufferSize(unsigned int sizeInMB)
			{
				csf::csflogger::ScopedLock lock(logMutex);

				if(sizeInMB == configuration->getBufferSize())
				{
					return;
				}
				flush();

				if(sizeInMB > 5)
				{
					sizeInMB = 5;
				}
				else if(sizeInMB == 0 )
				{
					sizeInMB = 1;
				}

				configuration->setBufferSize(sizeInMB);
				appender = buildAppender(configuration);
			}

			void setFileWritingAllowed(bool allowed)
			{
				csf::csflogger::ScopedLock lock(logMutex);
				configuration->setFileWritingAllowed(allowed);
			}

	        void setMaxNumberOfFiles(unsigned int numberOfFiles)
	        {
	            csf::csflogger::ScopedLock lock(logMutex);
	            configuration->setMaxNumberOfFiles(numberOfFiles);
	        }

			void deleteLogsIfWritingNotAllowed()
			{
				if(!getFileWritingAllowed())
				{
					fileSystemProvider->getFileSystem()->deleteLogs(configuration->getDirectory(), configuration->getFileName());
				}
			}

			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
			{
				this->threadIdHandlerFxn = threadIdHandlerFxn;
				appender->setThreadIdHandler(threadIdHandlerFxn);
			}

			virtual void clearThreadIdHandler()
			{
				this->threadIdHandlerFxn = NULL;
				appender->setThreadIdHandler(NULL);
			}

			void flushBufferIfAllowed()
			{
				if(getFileWritingAllowed())
				{
					flush();
				}
			}

			void setPIIDataHashEnablement(bool enable)
			{
			    pIIDataHashEnable = enable;
			}

			bool getPIIDataHashEnablement()
			{
			    return pIIDataHashEnable;
			}

		private:
			LogAppenderType logAppenderType;
			SMART_PTR_NS::shared_ptr<FileSystemProvider> fileSystemProvider;
			SMART_PTR_NS::shared_ptr<Configuration> configuration;
			csf::csflogger::Mutex logMutex;
			SMART_PTR_NS::shared_ptr<LogAppender> appender;
			CSFLogLevel logLevel;
			bool isInitCalled;
			ThreadIdHandlerFxn threadIdHandlerFxn;
			SMART_PTR_NS::shared_ptr<FileSystem> fileSystem;
			bool canDeleteLogs;
			bool pIIDataHashEnable;

			bool isLoggingAllowed()
			{
				return CSF::csflogger::LoggingAllowedBeforeLogControllerInitialization ? true : isInitCalled;
			}
		};
		

		LogController::LogController(SMART_PTR_NS::shared_ptr<FileSystemProvider> fileSystemProvider, LogAppenderType appenderType):pimpl(new Impl(fileSystemProvider, appenderType))	
		{
		}
		
		LogController::LogController():pimpl(new Impl(SMART_PTR_NS::shared_ptr<FileSystemProvider>(new FileSystemProvider), CSF::csflogger::DefaultLogAppenderType))
		{
		}

		LogController::LogController(SMART_PTR_NS::shared_ptr<Configuration> configuration) : pimpl(new Impl(configuration, SMART_PTR_NS::shared_ptr<FileSystemProvider>(new FileSystemProvider)))
		{
		}

		LogController::~LogController()
		{
		}

		SMART_PTR_NS::shared_ptr<LogController> LogController::instance;
		SMART_PTR_NS::shared_ptr<LogController> LogController::GetInstance()
		{
			static csf::csflogger::Mutex myMutex;
			csf::csflogger::ScopedLock lock(myMutex);

			if(!instance)
			{
				instance.reset(new LogController());
			}

			return instance;
		}

		void LogController::ResetInstance()
		{
			instance.reset();
		}

		void LogController::log(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
		{
			pimpl->log(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
		}

		void LogController::setAppender(SMART_PTR_NS::shared_ptr<LogAppender> appender)
		{
			pimpl->setAppender(appender);
		}

		SMART_PTR_NS::shared_ptr<LogAppender> LogController::getAppender()
		{
			return pimpl->getAppender();
		}

		void LogController::setLogLevel(CSFLogLevel newLogLevel)
		{
			pimpl->setLogLevel(newLogLevel);
		}		

		void LogController::setAppenderType(const LogAppenderType type)
		{
			pimpl->setAppenderType(type);
		}

		CSFLogLevel LogController::getLogLevel()
		{
			return pimpl->getLogLevel();
		}

		std::string LogController::getLogDirectory()
		{
			return pimpl->getLogDirectory();
		}

		void LogController::init(std::string fileName)
		{
			pimpl->init(fileName);
		}

		void LogController::flush()
		{
			pimpl->flush();
		}

		void LogController::deleteLogs()
		{
			pimpl->deleteLogs();
		}

        void LogController::setCanDeleteLogs(bool canDeleteLogs)
        {
            pimpl->setCanDeleteLogs(canDeleteLogs);
        }

		void LogController::deleteMatchingLogs(string pattern)
		{
			pimpl->deleteMatchingLogs(pattern);
		}

		void LogController::setBufferSize(unsigned int sizeInMB)
		{
			pimpl->setBufferSize(sizeInMB);	
		}
		bool LogController::getFileWritingAllowed()
		{
			return pimpl->getFileWritingAllowed();
		}

		void LogController::setFileWritingAllowed(bool allowed)
		{
			pimpl->setFileWritingAllowed(allowed);
		}

		void LogController::deleteLogsIfWritingNotAllowed()
		{
			pimpl->deleteLogsIfWritingNotAllowed();
		}

		void LogController::setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
		{
			pimpl->setThreadIdHandler(threadIdHandlerFxn);
		}

		void LogController::clearThreadIdHandler()
		{
			pimpl->clearThreadIdHandler();
		}

		void LogController::flushBufferIfAllowed()
		{
			pimpl->flushBufferIfAllowed();
		}

		void LogController::setMaxNumberOfFiles(unsigned int numberOfFiles)
		{
		    pimpl->setMaxNumberOfFiles(numberOfFiles);
		}
		
		void LogController::setPIIDataHashEnablement(bool enable)
		{
		    pimpl->setPIIDataHashEnablement(enable);
		}

		bool LogController::getPIIDataHashEnablement()
		{
		    return pimpl->getPIIDataHashEnablement();
		}
	}
}
