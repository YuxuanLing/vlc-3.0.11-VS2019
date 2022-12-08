#include "csf/logger/BasicFileAppender.hpp"
#include "csf/logger/FileSystem.hpp"
#include "csf/utils/ScopedLock.hpp"
#include "csf/utils/Mutex.hpp"

#include "csf/logger/DefaultFormatter.hpp"

#include <sstream>
#include <iostream>
#include <string>

namespace CSF
{
	namespace csflogger
	{
		class BasicFileAppender::Impl
		{
		public:
			SMART_PTR_NS::shared_ptr<Formatter> formatter;
			csf::csflogger::Mutex myMutex;

			Impl(SMART_PTR_NS::shared_ptr<FileSystem> fileSystem):formatter(new DefaultFormatter)
			{
				this->fileSystem = fileSystem;
			}

			~Impl()
			{

			}

			static std::string getWritablePath()
			{
				std::string path = "";

			#ifdef IOS
				path = (std::string)getenv("HOME") + "/";
			#endif
				return path;
			}

			void append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
			{
				csf::csflogger::ScopedLock lock(myMutex); // Locking  here as it improves performance rather than locking in filesystem
				fileSystem->open(getWritablePath()+"jabberlog.log");
				fileSystem->write(formatter->formatMessage(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage));
			}

			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
			{
				formatter->setThreadIdHandler(threadIdHandlerFxn);
			}

		private:
			SMART_PTR_NS::shared_ptr<FileSystem> fileSystem;

		};


		BasicFileAppender::BasicFileAppender(SMART_PTR_NS::shared_ptr<FileSystem> fileSystem):pimpl(new Impl(fileSystem))
		{
		}

		BasicFileAppender::~BasicFileAppender()
		{
		}

		void BasicFileAppender::append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
		{
			pimpl->append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
		}

		void BasicFileAppender::setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
		{
			pimpl->setThreadIdHandler(threadIdHandlerFxn);
		}
	}
}