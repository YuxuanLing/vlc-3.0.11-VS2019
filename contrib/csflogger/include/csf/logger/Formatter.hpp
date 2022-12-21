#pragma once
#include <string>
#include "csf/logger/CSFLog.h"
#include <sstream>

namespace CSF
{
	namespace csflogger
	{
		class CSF_LOGGER_API Formatter
		{
		public:
            virtual ~Formatter() { }

			virtual std::string formatMessage(const std::string & loggerName, CSFLogLevel logMessageLevel, const std::string & sourceFile, int sourceLine, const std::string & function, const std::string & logMessage) =0;
			virtual void setThreadIdHandler(ThreadIdHandlerFxn threadIdHandlerFxn) =0;

            static std::string formatPathAndLine(const std::string & sourceFile, int sourceLine)
            {
                static const std::string padding = "                                        ";
                std::string fullString;

                char buffer[16] = {0};

#ifdef WIN32
                _snprintf_s(buffer, sizeof(buffer)-1, _TRUNCATE, "%d", sourceLine);
#else
                snprintf(buffer, sizeof(buffer)-1, "%d", sourceLine);
#endif

                fullString = sourceFile + "(" + buffer + ")";

                int length = (int)fullString.size();

                if (length > 40)
                {
                    return fullString.substr(length - 40, 40);
                }
                else if (length < 40)
                {
                    return fullString + padding.substr(0, 40 - length);
                }
                else
                {
                    return fullString;
                }
            }
		};
	}
}
