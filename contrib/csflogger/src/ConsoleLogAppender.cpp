#include "csf/logger/ConsoleLogAppender.hpp"
#include "csf/logger/DefaultFormatter.hpp"
#include "csf/logger/Configuration.hpp"
#include <string>
#include <stdio.h>

#ifdef SYNERGY_LITE
#include <stdio.h>
#endif

namespace CSF
{
	namespace csflogger
	{

		

		class ConsoleLogAppender::Impl
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
                    printf("%s",formatter->formatMessage(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage).c_str());
				}
			}

			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
			{
				formatter->setThreadIdHandler(threadIdHandlerFxn);
			}
		};


		ConsoleLogAppender::ConsoleLogAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem) :
        FileAppender(configuration, fileSystem),
        pimpl(new Impl(configuration, SMART_PTR_NS::shared_ptr<Formatter>(new DefaultFormatter)))
		{
		}

		ConsoleLogAppender::~ConsoleLogAppender()
		{
		}

		void ConsoleLogAppender::append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
		{
			pimpl->append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
            FileAppender::append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
		}

		void ConsoleLogAppender::setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
		{
			pimpl->setThreadIdHandler(threadIdHandlerFxn);
		}
	}
}
