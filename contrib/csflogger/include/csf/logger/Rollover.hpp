#pragma once

#include <string>
#include "csf/logger/CSFLog.h"
#include "csf/logger/SmartPointer.hpp"
#include "csf/logger/Formatter.hpp"

namespace CSF
{
	namespace csflogger
	{
		class FileSystem;
		class Configuration;

		class CSF_LOGGER_API Rollover
		{
		public:
			Rollover(SMART_PTR_NS::shared_ptr<Configuration> configuration, SMART_PTR_NS::shared_ptr<FileSystem> fileSystem, SMART_PTR_NS::shared_ptr<Formatter> formatter, bool atomicWrite=false);
		
			virtual ~Rollover();

			void write(const std::string & logMessage);
			void write(const std::string & logMessage, unsigned int startPos, size_t length);

		private:
			class Impl;
			SMART_PTR_NS::shared_ptr<Impl> pimpl;
		};
	}
}