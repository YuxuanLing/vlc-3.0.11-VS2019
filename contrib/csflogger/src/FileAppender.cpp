#include "csf/logger/FileAppender.hpp"
#include "csf/logger/FileSystem.hpp"
#include "csf/logger/DefaultFormatter.hpp"
#include "csf/logger/Rollover.hpp"
#include "csf/logger/Configuration.hpp"
#include <string>
#include <sstream>

using namespace std;

namespace CSF
{
	namespace csflogger
	{

		class FileAppender::Impl
		{
		public:
			SMART_PTR_NS::shared_ptr<Configuration> configuration; 
			SMART_PTR_NS::shared_ptr<Formatter> formatter;
			SMART_PTR_NS::shared_ptr<Rollover> rollover;

			Impl(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem):
																  configuration(configuration),
																  formatter(new DefaultFormatter), 
																  rollover(new Rollover(configuration,fileSystem,formatter))
			{
			}

			~Impl()
			{
			}

			void append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
			{
				string message = formatter->formatMessage(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
				if (configuration->getFileWritingAllowed())
				{
					rollover->write(message);
				}
			}

			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
			{
				formatter->setThreadIdHandler(threadIdHandlerFxn);
			}

		};

		FileAppender::FileAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem):pimpl(new Impl(configuration,fileSystem))
		{
		}

		FileAppender::~FileAppender()
		{
		}

		void FileAppender::append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
		{
			pimpl->append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
		}

		void FileAppender::setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
		{
			pimpl->setThreadIdHandler(threadIdHandlerFxn);
		}
	}
}