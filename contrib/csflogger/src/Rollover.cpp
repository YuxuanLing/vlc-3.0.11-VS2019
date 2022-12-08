#include "csf/logger/Rollover.hpp"
#include "csf/logger/FileSystem.hpp"
#include "csf/logger/Configuration.hpp"
#include <sstream>
#include <string>
#include <vector>
#include "csf/logger/FileUtils.hpp"
#include <iostream>
#include <regex>
#include "csf/utils/FormattedTimeUtils.h"

namespace CSF
{
	namespace csflogger
	{		
		class Rollover::Impl
		{
		public:

			std::string fileName;
			std::string directory;
			SMART_PTR_NS::shared_ptr<FileSystem> fileSystem;
			long long maxSizeInBytes;
			long long writtenBytes;
			SMART_PTR_NS::shared_ptr<Configuration> configuration;
			SMART_PTR_NS::shared_ptr<Formatter> formatter;
			bool bufferedWrite;
			
			Impl(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem, SMART_PTR_NS::shared_ptr<Formatter> formatter, bool bufferedWrite):
																								fileSystem(fileSystem),
																								maxSizeInBytes((long long)configuration->getMaxFileSize() * 1024 * 1024),
																								writtenBytes(0),
																								configuration(configuration),
																								formatter(formatter),
																								bufferedWrite(bufferedWrite)
			{
				directory = configuration->getDirectory();
				fileName =  configuration->getFileName();
				FileUtils::createDirectory(directory);
			}

			~Impl()
			{

			}

			static std::string FileSeparator()
			{
			#ifdef _WIN32
				return "\\";
			#else
				return "/";
			#endif
			}

			static std::string getWritablePath()
			{
				std::string path = "";

			#ifdef IOS
				path = (std::string)getenv("HOME") + "/";
			#endif
				return path;
			}

			void write(const std::string & message, unsigned int startPos, size_t messageSize)
			{
                if(bufferedWrite)
                {
                    batchWrite(message, startPos, messageSize);
                }
                else
                {
                    continuousWrite(message, startPos, messageSize);
                }
			}

            void batchWrite(const std::string & message, unsigned int startPos, size_t messageSize)
            {
                fileSystem->open(directory + fileName);
                writtenBytes = fileSystem->getFileSize();
                    
                long long newSize = writtenBytes + messageSize;
                if(newSize > maxSizeInBytes)
                {
                    long long remainingCapacity = maxSizeInBytes - writtenBytes;
                    if (remainingCapacity > 0)
                    {
                        // write as much as will fit
                        long long bytesWrittenSoFar = fillFileToCapacity(message, startPos, messageSize, remainingCapacity);
                        // messageSize and startPos should change to reflect the remainder
						startPos += static_cast<unsigned int>(bytesWrittenSoFar);
						messageSize -= static_cast<size_t>(bytesWrittenSoFar);
                    }
                    doRollover();
                }

                fileSystem->write(message, startPos, messageSize);
                
                fileSystem->close();
            }

            void continuousWrite(const std::string & message, unsigned int startPos, size_t messageSize)
            {
                if(writtenBytes == 0)
                {
                    fileSystem->open(directory + fileName);
                    writtenBytes = fileSystem->getFileSize();
                }
                
                long long newSize = writtenBytes + messageSize;
                if(newSize > maxSizeInBytes)
                {
                    doRollover();
                    writtenBytes = messageSize;
                }
                else
                {
                    writtenBytes = newSize;
                }

                fileSystem->write(message, startPos, messageSize);
            }

			long long findEndLastLineThatFits(const std::string & message, unsigned int startPos, size_t messageSize, long long remainingCapacity)
			{
				long long lastLineEndPos = static_cast<long long>(startPos) + remainingCapacity;
				while (message[static_cast<unsigned int>(lastLineEndPos)] != '\n' && lastLineEndPos != 0)
				{
					lastLineEndPos--;
				}

				return (lastLineEndPos == 0) ? lastLineEndPos : lastLineEndPos+1 - startPos;
			}

			long long fillFileToCapacity(const std::string & message, unsigned int startPos, size_t messageSize, long long remainingCapacity)
			{
				long long newLength = findEndLastLineThatFits(message, startPos, messageSize, remainingCapacity);

				if (newLength == 0)
				{
					return 0;
				}

				fileSystem->write(message, startPos, static_cast<size_t>(newLength));
				
				return newLength;
			}

			std::string appendSuffix(std::string name, int value)
			{
				std::stringstream stream;
				stream << name;
				stream << "." << value;
				return stream.str();
			}

			void doRollover()
			{
				fileSystem->close();

				std::vector<std::string> errorMessages;
				unsigned int maxFiles = configuration->getMaxNumberOfFiles();

				removeResidualLogFiles(maxFiles, errorMessages);

				if(!fileSystem->deleteFile(directory + appendSuffix(fileName,maxFiles)))
				{
					errorMessages.push_back(formatter->formatMessage("csf.logger", CSF_LOG_WARNING, __FILE__ , __LINE__ , __FUNCTION__, "Error while deleting: " + appendSuffix(fileName,maxFiles) + " - " + fileSystem->getLastError()));
				}

				for (int i = maxFiles; i > 0; --i)
				{
					std::string oldFileName = fileName; 
					
					if(i-1 != 0)
					{
						oldFileName = appendSuffix(fileName,i-1);
					}

					if(!fileSystem->renameFile(directory + oldFileName, directory+ appendSuffix(fileName,i)))
					{
						errorMessages.push_back(formatter->formatMessage("csf.logger", CSF_LOG_WARNING, __FILE__ , __LINE__ , __FUNCTION__, "Error while renaming: " + appendSuffix(fileName,i) + " - "+ fileSystem->getLastError()));
					}
				}
				
				fileSystem->openOverwrite(directory + fileName);
				//errorMessages.push_back(formatter->formatMessage("csf.logger", CSF_LOG_DEBUG, __FILE__ , __LINE__ , __FUNCTION__, "Current Local Time: " + getFormattedLocalTimestamp()));

				if(errorMessages.size() > 0)
				{
					for (std::vector<std::string>::iterator error = errorMessages.begin(); error != errorMessages.end(); ++error)
					{
						fileSystem->write((*error), 0, (*error).length());
					}
					errorMessages.clear();
				}
			}

			void removeResidualLogFiles(unsigned int maxFiles, std::vector<std::string>& errorMessages)
			{
#if defined(_WIN32) || defined(LINUX) || (!defined(IOS) && defined(__APPLE__))
				std::vector<std::string> filesInDirectory = FileUtils::getFilesWithSubStr(directory, fileName);

				unsigned int existingNumberOfFiles = 0;
				std::for_each(filesInDirectory.begin(), filesInDirectory.end(), [&existingNumberOfFiles](const std::string& fileName)
				{
					if (std::regex_match(fileName, std::regex("^.+\\.[0-9]+$")))
						existingNumberOfFiles++;
				});

				if (existingNumberOfFiles > maxFiles)
				{
					for (auto i = existingNumberOfFiles; i > maxFiles; i--)
					{
						if (!fileSystem->deleteFile(directory + appendSuffix(fileName, i)))
						{
							errorMessages.push_back(formatter->formatMessage("csf.logger", CSF_LOG_WARNING, __FILE__, __LINE__, __FUNCTION__, "Error while deleting: " + appendSuffix(fileName, i) + " - " + fileSystem->getLastError()));
						}
					}
				}
#endif
			}
		};

		void Rollover::write(const std::string & logMessage)
		{
			pimpl->write(logMessage, 0, logMessage.length());
		}

		Rollover::Rollover(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem, SMART_PTR_NS::shared_ptr<Formatter> formatter, bool bufferedWrite):pimpl(new Impl( configuration, fileSystem, formatter, bufferedWrite))
		{

		}

		void Rollover::write(const std::string & logMessage, unsigned int startPos, size_t length)
		{
			pimpl->write(logMessage, startPos, length);
		}

		Rollover::~Rollover()
		{
		}

	}
}
