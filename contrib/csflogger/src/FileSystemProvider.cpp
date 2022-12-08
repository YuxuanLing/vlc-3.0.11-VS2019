#include "csf/logger/FileSystemProvider.hpp"
#include "csf/logger/FileSystem.hpp"

namespace CSF
{
	namespace csflogger
	{
		FileSystemProvider::FileSystemProvider(){}

		FileSystemProvider::~FileSystemProvider(){}

		SMART_PTR_NS::shared_ptr<FileSystem> FileSystemProvider::getFileSystem()
		{
			return SMART_PTR_NS::shared_ptr<FileSystem>(new FileSystem());
		}
	}
}