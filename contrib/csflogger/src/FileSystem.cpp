#include "csf/logger/FileSystem.hpp"
#include "csf/logger/FileUtils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string.h>
#include <errno.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

namespace CSF
{
	namespace csflogger
	{
		class FileSystem::Impl
		{

		public:
			

			~Impl()
			{
				closeFile();
			}

			#ifdef WIN32

			Impl() : bufferLength(1024), outFile(NULL)
			{
			}

		 	HANDLE outFile;
		 				
		 	std::string win32ErrorCodeToString(DWORD errorCode)
			{
				std::string returnValue;

				LPSTR messageBuffer;

				size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

				returnValue.assign(messageBuffer, size);

				if(size > 0)
				{
					LocalFree(messageBuffer);
				}

				return returnValue;
			}

			static std::wstring toWideString(const std::string& utf8String)
			{
				// Convert UTF-8 to UTF-16
				int size_needed = MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], (int)utf8String.size(), NULL, 0);
				std::wstring utf16String(size_needed, 0);
				MultiByteToWideChar(CP_UTF8, 0, &utf8String[0], (int)utf8String.size(), &utf16String[0], size_needed);
				return utf16String;				
			}

			void closeFile()
			{
				if(outFile)
				{
					writeToFile(buffer, 0, buffer.length());
					CloseHandle(outFile);
					outFile = NULL;
				}
			}

			bool createFile(const std::string & argFileName)
			{	
				if(!outFile)
				{   
					fileName = argFileName;
					outFile = CreateFile(LPCTSTR(toWideString(fileName).c_str()), FILE_APPEND_DATA , FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ , 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					return GetLastError() == ERROR_SUCCESS || GetLastError() == ERROR_ALREADY_EXISTS;
				}
		
				return outFile != NULL;
			}

			bool openOverwrite(const std::string & argFileName)
			{
				if(!outFile)
				{  
					fileName = argFileName;
					outFile = CreateFile(LPCTSTR(toWideString(fileName).c_str()), FILE_APPEND_DATA , FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ , 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					return GetLastError() == ERROR_SUCCESS || GetLastError() == ERROR_ALREADY_EXISTS;
				}
		
				return outFile != NULL;
			}

			void writeToFile(const std::string & content, unsigned int startPos, size_t length)
			{
				DWORD bytesWritten;
				WriteFile(outFile, content.c_str()+startPos, length, &bytesWritten, NULL);
			}

			bool deleteFile(const std::string & fileName)
			{
				return DeleteFile(LPCTSTR(toWideString(fileName).c_str())) != ERROR_SUCCESS;
			}

			bool renameFile(const std::string & currentFileName, const std::string & newFileName)
			{
				return MoveFile(LPCTSTR(toWideString(currentFileName).c_str()), LPCTSTR(toWideString(newFileName).c_str())) != ERROR_SUCCESS;
			}

			void printErrors(std::string name)
			{
				if(GetLastError() != ERROR_SUCCESS && GetLastError() != ERROR_ALREADY_EXISTS)
				{
					std::cout << "Error code " << name << " : " <<  win32ErrorCodeToString(GetLastError()) << std::endl;
				}
			}

			bool isOpen()
			{
				return outFile != NULL;
			}

			long long getFileSize()
			{

				if(isOpen())
				{
					ULARGE_INTEGER ui = {0};
					ui.LowPart = GetFileSize (outFile, &ui.HighPart);
					return ui.QuadPart == INVALID_FILE_SIZE ? 0 : ui.QuadPart;
				}
				else
				{
					return 0;
				}

			}

			std::string getLastError()
			{
				std::stringstream error;
				error << "Error code " << GetLastError() << " : " <<  win32ErrorCodeToString(GetLastError());
				return error.str();
			}

			#else
			
			Impl() : bufferLength(1024)
			{

			}

			std::ofstream outFile;

			void closeFile()
			{
				if (isOpen())
				{
					writeToFile(buffer, 0, buffer.length());
					outFile.close();
				}
			}

			bool createFile(const std::string & argFileName)
			{
				if (!outFile.is_open())
				{
					fileName = argFileName;
					std::ios_base::openmode mode = std::ios::out | std::ios::binary | std::ios::app;
					outFile.open(fileName.c_str(), mode);
				}
				return outFile.is_open();
			}

			bool openOverwrite(const std::string & argFileName)
			{
				if (!outFile.is_open())
				{
					fileName = argFileName;
					std::ios_base::openmode mode = std::ios::out | std::ios::binary | std::ios::trunc;
					outFile.open(fileName.c_str(), mode);
				}
				return outFile.is_open();
			}

			void writeToFile(const std::string & content, unsigned int startPos, size_t length)
			{
				outFile.write(content.c_str()+startPos, length);
    			if (!outFile.good())
	    		{
	    			// std::cout << "FileSystem::write - Error writing" << std::endl;
	    		}
				outFile.flush();
			}

			bool deleteFile(const std::string & fileName)
			{
				return remove(fileName.c_str()) == 0;
			}

			bool renameFile(const std::string & currentFileName, const std::string & newFileName)
			{
				return rename(currentFileName.c_str(), newFileName.c_str()) == 0;
			}

			bool isOpen()
			{
				return outFile.is_open();
			}

			long long getFileSize()
			{
				struct stat buf;
				if (stat(fileName.c_str(),&buf) < 0)
				{
					return 0;
				}
				return buf.st_size;
			}

			std::string getLastError()
			{
				return std::string(strerror(errno));
			}

			#endif

			void deleteLogs(const std::string& directory, const std::string& argFileName)
			{
				bool wasOpen = isOpen();
				closeFile();
				FileUtils::deleteLogs(directory,argFileName);
				
				if(wasOpen)
				{
					createFile(fileName);
				}
			}

			std::string buffer;
			unsigned int bufferLength;
			std::string fileName;

			void write(const std::string & content, unsigned int startPos, size_t length)
			{		
				if(isOpen())
				{
					if (buffer.length() + length > bufferLength)
					{
						writeToFile(buffer, 0, buffer.length());
						buffer = "";
					}

					if (length > bufferLength)
					{
						writeToFile(content, startPos, length);
					}

					else
					{
						if(startPos == 0 && length == content.length())
						{
							buffer.append(content);
						}
						else
						{
							buffer.append(content.substr(startPos,length));
						}
					}
				}				
			}

			void flush()
			{
				if(isOpen())
				{
					writeToFile(buffer, 0, buffer.length());
					buffer = "";
				}
			}
		};

		FileSystem::FileSystem():pimpl(new Impl())
		{
		}

		FileSystem::~FileSystem()
		{
		}
		
		bool FileSystem::open(const std::string & fileName)
		{
			return pimpl->createFile(fileName);
		}

		bool FileSystem::openOverwrite(const std::string & fileName)
		{
			return pimpl->openOverwrite(fileName);
		}

		bool FileSystem::deleteFile(const std::string & fileName)
		{
			if(FileUtils::fileExists(fileName))
			{
				return pimpl->deleteFile(fileName);
			}
			else
			{
				return true;
			}
		}

		bool FileSystem::renameFile(const std::string & currentFileName, const std::string & newFileName)
		{
			if(FileUtils::fileExists(currentFileName))
			{
				return pimpl->renameFile(currentFileName, newFileName);			
			}
			else
			{
				return true;
			}
		}

		void FileSystem::write(const std::string & content)
		{
			pimpl->write(content, 0, content.size());
		}

		void FileSystem::write(const std::string & content, unsigned int startPos, size_t length)
		{
			pimpl->write(content, startPos, length);
		}

		void FileSystem::close()
		{
			pimpl->closeFile();
		}

		long long FileSystem::getFileSize()
		{
			return pimpl->getFileSize();
		}

		void FileSystem::flush()
		{
			pimpl->flush();
		}

		std::string FileSystem::getLastError()
		{
			return pimpl->getLastError();
		}

		void FileSystem::deleteLogs(const std::string& directory, const std::string& fileName)
		{
			pimpl->deleteLogs(directory, fileName);
		}
	}
}
