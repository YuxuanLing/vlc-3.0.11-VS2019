#pragma once

#include "csf/logger/CSFLog.h"
#include "csf/logger/SmartPointer.hpp"
#include <string>

namespace CSF
{
	namespace csflogger
	{
		class CSF_LOGGER_API Configuration
		{
		public:
			Configuration(std::string fileName, unsigned int maxFileSize,unsigned int maxNumberOfFiles, unsigned int bufferSize, std::string bSentinel = "\nMUIPICNIRP_ILLUN_ODECNOC\n", std::string eSentinel = "\nATIMERTXE_ILLUN_ODECNOC\n");
			virtual ~Configuration();
			virtual std::string getFullPathFileName();
			virtual void setFullPathFileName(std::string fileName);
			virtual std::string getDirectory();
			virtual std::string getFileName();
			virtual unsigned int getMaxFileSize();
			virtual unsigned int getMaxNumberOfFiles();
			virtual unsigned int getBufferSize(); 
			virtual bool getFileWritingAllowed();
			virtual void setFileWritingAllowed(bool);
			void setBufferSize(unsigned int bufferSize);
			void setMaxNumberOfFiles(unsigned int numberOfFiles);
			virtual std::string getBeginSentinel();
			virtual std::string getEndSentinel();
		private:
			class Impl;
			SMART_PTR_NS::shared_ptr<Impl> pimpl;
		};
	}
}
