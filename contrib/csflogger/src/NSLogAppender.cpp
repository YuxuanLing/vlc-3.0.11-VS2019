#include "csf/logger/NSLogAppender.hpp"
#include "csf/logger/DefaultFormatter.hpp"
#include "csf/logger/Configuration.hpp"
#include <string>
#include <stdio.h>

#ifdef SYNERGY_LITE
#include <stdio.h>
#endif

#ifdef __APPLE__
#include "ObjCHelpers.h"
#endif

namespace CSF
{
	namespace csflogger
	{
		class NSLogAppender::Impl
		{
		public:

			SMART_PTR_NS::shared_ptr<Formatter> formatter;
			SMART_PTR_NS::shared_ptr<Configuration> configuration;
			
			Impl(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<Formatter> formatter) :
				formatter(formatter),
				configuration(configuration)
			{	
			}

			~Impl()
			{
			}

			void append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
			{
				if (configuration->getFileWritingAllowed())
				{
					#ifdef __APPLE__
					::LogToNSLog(formatter->formatMessage(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage));
					#endif

				}
			}

			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
			{
				formatter->setThreadIdHandler(threadIdHandlerFxn);
			}
		};


		NSLogAppender::NSLogAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem) :
        FileAppender(configuration, fileSystem),
        pimpl(new Impl(configuration, SMART_PTR_NS::shared_ptr<Formatter>(new DefaultFormatter)))
		{
		}

		NSLogAppender::~NSLogAppender()
		{
		}

		void NSLogAppender::append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
		{
			pimpl->append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
            FileAppender::append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
		}

		void NSLogAppender::setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
		{
			pimpl->setThreadIdHandler(threadIdHandlerFxn);
		}
	}
}
