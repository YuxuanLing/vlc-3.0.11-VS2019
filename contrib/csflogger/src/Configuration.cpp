#include "csf/logger/Configuration.hpp"
#include <sstream>
#include <iostream>
#include <string>
#include "csf/logger/FileUtils.hpp"

namespace CSF
{
	namespace csflogger
	{		
		class Configuration::Impl
		{
		public:

			Impl(std::string fileName, unsigned int maxFileSize, unsigned int maxNumberOfFiles, unsigned int bufferSize, std::string bSentinel, std::string eSentinel):
				fullPathfileName(fileName), maxFileSize(maxFileSize), maxNumberOfFiles(maxNumberOfFiles), bufferSize(bufferSize), fileWritingAllowed(true), beginSentinel(bSentinel), endSentinel(eSentinel)
			{
				if(this->maxFileSize < this->bufferSize)
				{
					this->bufferSize = this->maxFileSize;
				}
			}

			~Impl()
			{
			}

			std::string getFullPathFileName()
			{
				return fullPathfileName;
			}

			void setFullPathFileName(std::string filename)
			{
				fullPathfileName = filename;

				//Ensure that directory and fileName are populated after the method exits...
				directory.clear();
				fileName.clear();
				loadDirectoryAndFileName();
			}

			unsigned int getMaxFileSize()
			{
				return maxFileSize;
			}

			unsigned int getMaxNumberOfFiles()
			{
				return maxNumberOfFiles;
			}

            void setMaxNumberOfFiles(unsigned int numberOfFiles)
            {
                maxNumberOfFiles = numberOfFiles;
            }

			unsigned int getBufferSize()
			{
				return bufferSize;
			}

			void  setBufferSize(unsigned int argBufferSize)
			{
				bufferSize = argBufferSize;
			}

			std::string getDirectory()
			{
				loadDirectoryAndFileName();	
				return directory;
			}

			std::string getFileName()
			{
				loadDirectoryAndFileName();	
				return fileName;
			}

			void loadDirectoryAndFileName()
			{
				if(directory.empty() || fileName.empty())
				{
					std::size_t found = fullPathfileName.find_last_of(FileUtils::FileSeparator());
					
					if (found == std::string::npos)
					{
						fileName = fullPathfileName;
						directory = FileUtils::getWritablePath();
					}
					else
					{
						fileName = fullPathfileName.substr(found+1);
						directory = fullPathfileName.substr(0,found+1);
					}
				}
			}
			
			bool getFileWritingAllowed()
			{
				return fileWritingAllowed;
			}

			void setFileWritingAllowed(bool allowed)
			{
				fileWritingAllowed = allowed;
			}

			std::string getBeginSentinel()
			{
				return beginSentinel;
			}

			std::string getEndSentinel()
			{
				return endSentinel;
			}

		private:
			std::string fullPathfileName;
			std::string directory;
			std::string fileName;
			unsigned int maxFileSize;
			unsigned int maxNumberOfFiles;
			unsigned int bufferSize;
			bool fileWritingAllowed;
			std::string beginSentinel;
			std::string endSentinel;
		};

		Configuration::Configuration(std::string fileName, unsigned int maxFileSize,unsigned int maxNumberOfFiles, unsigned int bufferSize, std::string bSentinel, std::string eSentinel):
					 pimpl(new Impl(fileName,maxFileSize,maxNumberOfFiles,bufferSize, bSentinel, eSentinel))
		{		
		}

		Configuration::~Configuration()
		{
		}

		std::string Configuration::getFullPathFileName()
		{
			return pimpl->getFullPathFileName();
		}

		void Configuration::setFullPathFileName(std::string fileName)
		{
			pimpl->setFullPathFileName(fileName);
		}

		std::string Configuration::getDirectory()
		{
			return pimpl->getDirectory();
		}

		std::string Configuration::getFileName()
		{
			return pimpl->getFileName();
		}

		unsigned int Configuration::getMaxFileSize()
		{
			return pimpl->getMaxFileSize();
		}

		unsigned int Configuration::getMaxNumberOfFiles()
		{
			return pimpl->getMaxNumberOfFiles();
		}

		unsigned int Configuration::getBufferSize()
		{
			return pimpl->getBufferSize();
		}

		bool Configuration::getFileWritingAllowed()
		{
			return pimpl->getFileWritingAllowed();
		}

		void Configuration::setFileWritingAllowed(bool allowed)
		{
			pimpl->setFileWritingAllowed(allowed);
		}

		void  Configuration::setBufferSize(unsigned int bufferSize)
		{
			return pimpl->setBufferSize(bufferSize);
		}

		void Configuration::setMaxNumberOfFiles(unsigned int numberOfFiles)
		{
		    return pimpl->setMaxNumberOfFiles(numberOfFiles);
		}

		std::string Configuration::getBeginSentinel()
		{
			return pimpl->getBeginSentinel();
		}

		std::string Configuration::getEndSentinel()
		{
			return pimpl->getEndSentinel();
		}
	}
}
