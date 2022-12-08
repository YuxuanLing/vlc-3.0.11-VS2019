#include "csf/logger/FileUtils.hpp"
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#include <cstring>
#endif

#ifdef WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

namespace CSF
{
namespace csflogger
{
	
	std::string FileSeparator()
	{
	#ifdef _WIN32
		return "\\";
	#else
		return "/";
	#endif
	}

	#ifdef _WIN32
 	std::wstring toWideString(const std::string& utf8String)
	{
		// Convert UTF-8 to UTF-16
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], (int)utf8String.size(), NULL, 0);
		std::wstring utf16String(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], (int)utf8String.size(), &utf16String[0], size_needed);
		return utf16String;				
	}

	std::wstring FileSeparatorW()
	{
		return L"\\";
	}

	 std::string toString(const std::wstring& utf16String)
    {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], (int)utf16String.size(), NULL, 0, NULL, NULL);
        std::string utf8String(size_needed, 0);
        WideCharToMultiByte(CP_UTF8, 0, &utf16String[0], (int)utf16String.size(), &utf8String[0], size_needed, NULL, NULL);
        return utf8String;
    }
	#endif

	bool FileUtils::fileExists(const std::string & fileName)
	{
	#ifdef _WIN32

		struct _stat statInfo;
		return (_wstat (toWideString(fileName).c_str(), &statInfo) == 0);

	#else

		struct stat statInfo;
		return (stat (fileName.c_str(), &statInfo) == 0);

	#endif
	}
	
	std::string getParentDirectory(const std::string & directoryPath)
	{
		std::size_t found = directoryPath.find_last_of(FileSeparator());

        if (std::string::npos == found)
		{
			// No parent directory
			return "";
		}
		else if (directoryPath.size()-1 == found)
		{
			return directoryPath.substr(0, directoryPath.size() - 1);
		}

		return directoryPath.substr(0, found);
	}

	std::string trimLastTrailingSlash(const std::string & directoryPath)
	{
		std::size_t found = directoryPath.find_last_of(FileSeparator());

		if (directoryPath.size()-1 == found)
		{
			return directoryPath.substr(0, directoryPath.size() - 1);
		}
		return directoryPath;
	}

	bool FileUtils::createDirectory(const std::string & directoryPath)
	{
		bool dirCreated = false;
		std::string directory = trimLastTrailingSlash(directoryPath);

		if (!directory.empty() && !fileExists(directory))
		{
			dirCreated = createDirectoryStructure(directory);
		}

		return dirCreated;
	}

	bool FileUtils::createDirectoryStructure(const std::string & directoryPath)
	{
		bool dirCreated = false;
		std::string directory = trimLastTrailingSlash(directoryPath);

		// base case
		if (directory.empty() || fileExists(directory))
		{
			dirCreated = true;
		}
		// recursive case
		else
		{
			std::string parentDirectory = getParentDirectory(directory);
			if(createDirectoryStructure(parentDirectory))
			{
			#ifdef _WIN32
				dirCreated = (::CreateDirectoryW(toWideString(directoryPath).c_str(), NULL) == 0) ? false : true;
			#else
				dirCreated = (mkdir(directoryPath.c_str(), S_IRWXU | S_IRGRP | S_IROTH) == 0) ? true : false;
			#endif
			}
			else
			{
				dirCreated = false;
			}
		}

		return dirCreated;
	}

	void createParentDirectory(const std::string & filePath)
	{
		FileUtils::createDirectory(getParentDirectory(filePath));
	}

	#ifdef _WIN32
	void FileUtils::removeDirectory(const std::string& directoryPath)
	{
		if (directoryPath.empty())
		{
			return;
		}

		WIN32_FIND_DATAW findData = {0};
		HANDLE hFind = INVALID_HANDLE_VALUE;
		
		std::wstring wdirectoryPath = toWideString(directoryPath);
		std::wstring searchString = wdirectoryPath + L"\\*";
		
		hFind =	FindFirstFileW(searchString.c_str(), &findData);

		if (hFind != INVALID_HANDLE_VALUE)
		{	
			// Remove the content of the directory
			do 
			{
				if (0 == wcscmp(findData.cFileName, L".") || 0 == wcscmp(findData.cFileName, L".."))
				{
					continue;
				}

				std::wstring fullPath = wdirectoryPath + FileSeparatorW() + findData.cFileName;
				if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					removeDirectory(toString(fullPath));
				}
				else
				{
					::DeleteFileW(fullPath.c_str());
				}
			} while (FindNextFileW(hFind, &findData) != 0);

			FindClose(hFind);
			
		}

		// Directory is empty so we can RemoveDirectory
		::RemoveDirectoryW(toWideString(directoryPath).c_str());
	}
	#else
	void FileUtils::removeDirectory(const std::string& directoryPath)
	{
		if (directoryPath.empty())
		{
			return;
		}

		DIR* dirPtr = opendir(directoryPath.c_str());
		struct dirent* dirEntry;

		if (dirPtr)
		{	
			while ( (dirEntry = readdir(dirPtr)) != NULL)
			{
				if (0 == strcmp(dirEntry->d_name, ".") || 0 == strcmp(dirEntry->d_name, ".."))
				{
					continue;
				}

				std::string fullPath = directoryPath + FileSeparator() + dirEntry->d_name;
				
				bool wasFileDeleted = (remove(fullPath.c_str()) == 0);
				if (!wasFileDeleted)
				{
					removeDirectory(fullPath);
				}
			}
			closedir(dirPtr);
			rmdir(directoryPath.c_str());
		}
	}
	#endif

	std::string FileUtils::FileSeparator()
	{
		#ifdef _WIN32
			return "\\";
		#else
			return "/";
		#endif
	}

	std::string FileUtils::getWritablePath()
	{
		std::string path = "";

		#ifdef IOS
			path = (std::string)getenv("HOME") + "/";
		#endif
			return path;
	}


    void deleteFile(std::string fileName)
    {
        if(FileUtils::fileExists(fileName))
        {
            if( remove( fileName.c_str() ) != 0 )
                perror( "Error deleting file" );
        }
    }


#ifdef _WIN32
	std::string getCurrentWorkingDirectory()
	{
	    char working_directory[MAX_PATH+1] ;
	    GetCurrentDirectoryA( sizeof(working_directory), working_directory ) ; // **** win32 specific ****
	    return working_directory ;
	}

	std::vector<std::string> getFilesFromDirectory(std::string directoryPath, const std::string& extension)
	{
		std::vector<std::string> filenames;
		WIN32_FIND_DATAW findData = {0};
		HANDLE hFind = INVALID_HANDLE_VALUE;
		
		if (directoryPath.length() > MAX_PATH)
		{
			return filenames;
		}

		if(directoryPath.empty())
		{
			directoryPath = getCurrentWorkingDirectory();
		}

		std::wstring searchString =  toWideString(directoryPath) + L"\\*";
		
		hFind =	FindFirstFileW(searchString.c_str(), &findData);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do 
			{
				
				if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
				{
						
					// If extension is empty wcsstr returns findData.cFileName
					if (wcsstr(findData.cFileName, toWideString(extension).c_str()) != NULL)
					{

						filenames.push_back(directoryPath + FileSeparator() + toString(findData.cFileName));
					}
				}

			} while (FindNextFileW(hFind, &findData) != 0);
		}

		FindClose(hFind);
		return filenames;
	}
#else
	std::vector<std::string> getFilesFromDirectory(std::string directoryPath, const std::string& extension)
	{
		std::vector<std::string> filenames;

		if (directoryPath.empty())
		{
			directoryPath = ".";
		}

		DIR* dirPtr = opendir(directoryPath.c_str());
		struct dirent* dirEntry;

		if (dirPtr)
		{	
			while ( (dirEntry = readdir(dirPtr)) != NULL)
			{
				if (0 == strcmp(dirEntry->d_name, ".") || 0 == strcmp(dirEntry->d_name, ".."))
				{
					continue;
				}
				
				std::string fullPath;

				if(directoryPath == ".")
				{
					fullPath = dirEntry->d_name;
				}
				else
				{
					fullPath = directoryPath + FileSeparator() + dirEntry->d_name;
				}
				
				
				struct stat statInfo;
				if (stat(fullPath.c_str(), &statInfo) == 0)
				{
					if (!S_ISDIR(statInfo.st_mode) && strstr(dirEntry->d_name, extension.c_str()) != NULL)
					{
						filenames.push_back(fullPath);
					}
				}
			}

			closedir(dirPtr);
		}
		return filenames;
	}	
#endif

	std::vector<std::string> FileUtils::getFilesWithSubStr(std::string directoryPath, const std::string& subStr)
    {
	    return getFilesFromDirectory(directoryPath, subStr);
    }

	void FileUtils::deleteLogs(const std::string& directory, const std::string& fileName)
	{
		if(fileName.empty())
		{
			return;
		}

		std::vector<std::string>  filesInDirectory =  getFilesFromDirectory(directory, fileName);
        
        for (size_t i = 0; i < filesInDirectory.size(); ++i)
        {
           deleteFile(filesInDirectory.at(i)); 
        }

	}

	static std::string getFilenameFromFullPath(const std::string & pathtofile)
	{
		std::size_t pos_last_seperator = pathtofile.find_last_of(FileUtils::FileSeparator());

		if (pos_last_seperator != std::string::npos)
		{
			return pathtofile.substr(pos_last_seperator + 1);
		}
		else
		{
			return pathtofile;
		}
	}

	static bool wildcardMatch(const std::string & pathtofile, const std::string & pattern)
	{
		//1. We are only interested in the filename, not the fill path.  Strip out the filename.
		std::string filename = getFilenameFromFullPath(pathtofile);
		
		if (pattern.empty())
		{
			// All files match an empty pattern.
			return true;
		}

		size_t star_pos = pattern.find("*");
		if (star_pos == std::string::npos)
		{
			// Require an exact match if there is no wildcard.
			// Compare returns 0 if the strings are equal.
			return filename.compare(pattern) == 0;
		}
		else
		{	
			std::string beforeWildcard = pattern.substr(0, star_pos);
			std::string afterWildcard = pattern.substr(star_pos + 1, pattern.length() - star_pos + 1);
			
			// If any of the above are longer than the filename then we have no match, so return.
			// Testing the pattern here also prevents "filename.compare(...)" throwing "out-of_bouds" exceptions. 
			if (afterWildcard.length() > filename.length() ||
				beforeWildcard.length() > filename.length())
			{
				return false;
			}
			else
			{
				// Okay... Compare prefix, or postfix, or both if nessesary to the filename.
				if (beforeWildcard.empty())
				{
					return filename.compare(filename.length() - afterWildcard.length(), afterWildcard.length(), afterWildcard) == 0;
				}
				else if (afterWildcard.empty())
				{
					return filename.compare(0, beforeWildcard.length(), beforeWildcard) == 0;
				}
				else
				{
					return filename.compare(0, beforeWildcard.length(), beforeWildcard) == 0 &&
						filename.compare(filename.length() - afterWildcard.length(), afterWildcard.length(), afterWildcard) == 0;
				}
			}
		}
	}

	void FileUtils::deleteMatchingLogs(const std::string& directory, const std::string& pattern)
	{
		//Get all the files, regardless of extention.  That much does the hard work of giving us a list of files regardless of platform.
		std::vector<std::string>  filesInDirectory = getFilesFromDirectory(directory, "");
		
		for (size_t i = 0; i < filesInDirectory.size(); ++i)
		{
			if (wildcardMatch(filesInDirectory.at(i), pattern)) //Match?
			{
				deleteFile(filesInDirectory.at(i));
			}
		}

	}


} // end namespace csflogger
} // end namespace CSF
