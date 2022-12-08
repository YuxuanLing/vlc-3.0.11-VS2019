#pragma once

namespace CSF
{
	namespace csflogger
	{
		
		enum LogAppenderType
		{ 
			#ifdef __ANDROID__
			AndroidLogCat, 
			#endif

			#ifdef __APPLE__
			NSLog,
			#endif

			BasicFile,
			Buffered,
			File,
            ConsoleLog
		};
	}
}
