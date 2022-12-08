#include "csf/logger/BufferedAppender.hpp"
#include "csf/logger/FileSystem.hpp"
#include "csf/logger/Rollover.hpp"
#include "csf/logger/DefaultFormatter.hpp"
#include "csf/logger/Configuration.hpp"
#include <sstream>
#include <iostream>
#include <string>

namespace CSF
{
	namespace csflogger
	{	
		class BufferedAppender::Impl
		{
		public:
			
			Impl(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem, SMART_PTR_NS::shared_ptr<Formatter> formatter)
				: formatter(formatter),
				  rollover(new Rollover(configuration, fileSystem, formatter, true)), 
				  configuration(configuration),
				  BEGIN_SENTINEL(configuration->getBeginSentinel()/*"\nMUIPICNIRP_ILLUN_ODECNOC\n"*/),
				  END_SENTINEL(configuration->getEndSentinel()/*"\nATIMERTXE_ILLUN_ODECNOC\n"*/),
				  bufferLimit(1024*1024*configuration->getBufferSize()+(unsigned int)(BEGIN_SENTINEL.size()+END_SENTINEL.size()))

			{
				this->fileSystem = fileSystem;
				buffer.reserve(bufferLimit);
				clearBuffer();
			}

			~Impl()
			{
				flushBufferIfAllowed();
			}

			void append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
			{
				appendToBuffer(formatter->formatMessage(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage));
			}

			void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
			{
				formatter->setThreadIdHandler(threadIdHandlerFxn);
			}

			void appendToBuffer(const std::string & logMessage)
			{
				if (buffer.size() + logMessage.size() >= bufferLimit)
				{
					flushBufferIfAllowed();
				}

				buffer.insert(buffer.size() - END_SENTINEL.size(), logMessage);
			}

			void flushBufferIfAllowed()
			{
				if (configuration->getFileWritingAllowed())
				{
					flushBuffer();
				}
				else
				{
					clearBuffer();
				}
			}

            void getBuffer(std::string& logMessage)
            {
                size_t bufferSize = buffer.size();
                if (bufferSize > BEGIN_SENTINEL.size() + END_SENTINEL.size())
                {
                    size_t lengthString = buffer.size() - (BEGIN_SENTINEL.size() + END_SENTINEL.size());
                    logMessage = buffer.substr((unsigned int)BEGIN_SENTINEL.size(), lengthString);
                }
            }

            void flushBuffer()
            {
                size_t bufferSize = buffer.size();
                if (bufferSize > BEGIN_SENTINEL.size() + END_SENTINEL.size())
                {
                    size_t lengthString = buffer.size() - (BEGIN_SENTINEL.size() + END_SENTINEL.size());
                    rollover->write(buffer, (unsigned int)BEGIN_SENTINEL.size(), lengthString);
                    clearBuffer();
                }
			}

			void clearBuffer()
			{
				buffer.clear();
				createMemorySentinel(); 
			}

			void createMemorySentinel()
			{
				char temp[256];
				int index = 0;
				for(size_t i = BEGIN_SENTINEL.size(); i > 0; --i)
				{
					temp[index] = BEGIN_SENTINEL[i-1];
					index++;
				}
				for(size_t i = END_SENTINEL.size(); i > 0; --i)
				{
					temp[index] = END_SENTINEL[i-1];
					index++;
				}
				temp[index] = '\0';
				buffer.append(temp);
			}

		private:
			SMART_PTR_NS::shared_ptr<Formatter> formatter;
			SMART_PTR_NS::shared_ptr<FileSystem> fileSystem;				
			SMART_PTR_NS::shared_ptr<Rollover> rollover;
			SMART_PTR_NS::shared_ptr<Configuration> configuration;
			std::string buffer;				
			const std::string BEGIN_SENTINEL;
		    const std::string END_SENTINEL;
		    unsigned int bufferLimit;
		};

		BufferedAppender::BufferedAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem) 
			:pimpl(new Impl(configuration, fileSystem, SMART_PTR_NS::shared_ptr<Formatter>(new DefaultFormatter)))
		{
		}

		BufferedAppender::BufferedAppender(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem, SMART_PTR_NS::shared_ptr<Formatter> formatter) 
			:pimpl(new Impl(configuration, fileSystem, formatter))
		{
		}

		BufferedAppender::~BufferedAppender()
		{
		}

		void BufferedAppender::append(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage)
		{
			pimpl->append(loggerName, logMessageLevel, sourceFile, sourceLine, function, logMessage);
		}

        void BufferedAppender::appendToBuffer(const std::string & logMessage)
        {
            pimpl->appendToBuffer(logMessage);
        }

		void BufferedAppender::setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn)
		{
			pimpl->setThreadIdHandler(threadIdHandlerFxn);
		}

        void BufferedAppender::getBuffer(std::string& logMessage)
        {
            pimpl->getBuffer(logMessage);
        }

		void BufferedAppender::flushBuffer()
		{
			pimpl->flushBuffer();
		}

        void BufferedAppender::clearBuffer()
        {
            pimpl->clearBuffer();
        }
	}
}