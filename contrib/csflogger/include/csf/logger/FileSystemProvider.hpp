#pragma once

#include "csf/logger/CSFLog.h"
#include "csf/logger/SmartPointer.hpp"

namespace CSF
{
	namespace csflogger
	{
		class FileSystem;
		class CSF_LOGGER_API FileSystemProvider
		{
		public:
			FileSystemProvider();
			virtual ~FileSystemProvider();
			virtual SMART_PTR_NS::shared_ptr<FileSystem> getFileSystem();
		};
	}
}