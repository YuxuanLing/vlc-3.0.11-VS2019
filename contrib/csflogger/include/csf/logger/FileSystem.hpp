#pragma once

#include "csf/logger/CSFLog.h"
#include "csf/logger/LogAppender.hpp"
#include "csf/logger/SmartPointer.hpp"

#include <string>

namespace CSF
{
	namespace csflogger
	{
		class CSF_LOGGER_API FileSystem
		{
		public:
			
			FileSystem();

			virtual ~FileSystem();
			virtual bool open(const std::string & fileName);
			virtual bool openOverwrite(const std::string & fileName);
			virtual void write(const std::string & content);
			virtual void write(const std::string & content, unsigned int startPos, size_t length);
			virtual void close();
			virtual bool deleteFile(const std::string & fileName);
			virtual void deleteLogs(const std::string& directory, const std::string& fileName);
			virtual bool renameFile(const std::string & currentFileName, const std::string & newFileName);
			virtual long long getFileSize();
			virtual void flush();
			virtual std::string getLastError();

		private:
			class Impl;
			SMART_PTR_NS::shared_ptr<Impl> pimpl;
		};
	}
}