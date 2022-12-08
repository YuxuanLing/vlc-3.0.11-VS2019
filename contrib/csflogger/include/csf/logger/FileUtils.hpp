#pragma once

#include "csf/logger/CSFLog.h"

#include <string>
#include <vector>

namespace CSF
{
namespace csflogger
{

	class CSF_LOGGER_API FileUtils
	{
	public:
		/**
		 * Creates a directory using the given 'directoryPath'. It will create a directory's parents
		 * if they do not already exist.
		 *
		 * @return true if the directory has been created, false otherwise
		 */
		static bool createDirectory(const std::string& directoryPath);

		/**
		 * Removes the directory as given by 'directoryPath'. If the given directory is not empty,
		 * the contents will be recursively deleted.
		 */
		static void removeDirectory(const std::string& directoryPath);
		static void deleteLogs(const std::string& directory, const std::string& fileName);
		static void deleteMatchingLogs(const std::string& directory, const std::string& pattern);
		static std::string FileSeparator();
		static std::string getWritablePath();
		static bool fileExists(const std::string & fileName);
		static std::vector<std::string> getFilesWithSubStr(std::string directoryPath, const std::string& subStr);

	private:
		/**
		 * Recursively create a directory hierarchy using 'directoryPath'
		 *
		 * @return true if the directory (and any required parent directories) were successfully 
		 *         created
		 */
		static bool createDirectoryStructure(const std::string& directoryPath);
	};

}
}
