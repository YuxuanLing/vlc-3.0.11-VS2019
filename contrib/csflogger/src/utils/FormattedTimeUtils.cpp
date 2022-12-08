#include "csf/utils/FormattedTimeUtils.h"
#ifdef _WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#include <unistd.h>
    #if defined(LINUX) || defined(ANDROID)
    #    if defined(__LP64__) || defined(JETS_CLIENT)
    #        include <time.h>
    #    else
    #        include <time64.h>
    #    endif
    #endif
#endif
#include <iomanip>
#include <chrono>
#include <vector>
#include <cstring>
#include <sstream>

namespace CSF
{
namespace csflogger
{

    std::string getFormattedLocalTimestamp()
    {
#ifdef WIN32
        // Windows implementation
        // GetSystemTime supplies a SYSTEMTIME type with the time in UTC,
        // so we only have to format the fields
        char str[32];
        SYSTEMTIME systemTime;
        ::GetLocalTime(&systemTime);
        _snprintf_s(str, 32, "%4d-%02d-%02d %02d:%02d:%02d,%03d",
            systemTime.wYear, systemTime.wMonth, systemTime.wDay,
            systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);
        return std::string(str);
#elif __APPLE__ 
        //OSX and iOS implementation
        struct tm local_tm_struct;
        struct tm gm_now_tm_struct;

        timeval curTime;
        gettimeofday(&curTime, NULL);
        int milli = curTime.tv_usec / 1000;
        gmtime_r(&curTime.tv_sec, &gm_now_tm_struct);
        localtime_r(&curTime.tv_sec, &local_tm_struct);

        std::stringstream stream;
        stream << std::setw(4) << ( 1900 + local_tm_struct.tm_year );
        stream << "-";
        stream << std::setw(2) << std::setfill('0') << (1+local_tm_struct.tm_mon );
        stream << "-";
        stream << std::setw(2) << std::setfill('0') << local_tm_struct.tm_mday;
        stream << " ";
        stream << std::setw(2) << std::setfill('0') << local_tm_struct.tm_hour;
        stream << ":";
        stream << std::setw(2) << std::setfill('0') << local_tm_struct.tm_min;
        stream << ":";
        stream << std::setw(2) << std::setfill('0') << gm_now_tm_struct.tm_sec;
        stream << ",";
        stream << std::setw(3) << std::setfill('0') << milli;
        return stream.str();

#else
        // Android and Linux implementation
        timeval curTime;
        gettimeofday(&curTime, NULL);
        struct tm local_tm_struct;
        int milli = curTime.tv_usec / 1000;
        memset (&local_tm_struct, 0, sizeof (tm));
        localtime_r(&curTime.tv_sec, &local_tm_struct);

        std::stringstream stream;
        stream << std::setw(4) << ( 1900 + local_tm_struct.tm_year ) ;
        stream << "-";
        stream << std::setw(2) << std::setfill('0') << (1+local_tm_struct.tm_mon ) ;
        stream << "-";
        stream << std::setw(2) << std::setfill('0') << local_tm_struct.tm_mday ;
        stream << " ";
        stream << std::setw(2) << std::setfill('0') << local_tm_struct.tm_hour ;
        stream << ":";
        stream << std::setw(2) << std::setfill('0') << local_tm_struct.tm_min ;
        stream << ":";
        stream << std::setw(2) << std::setfill('0') << local_tm_struct.tm_sec ;
        stream << ",";
        stream << std::setw(3) << std::setfill('0') << milli;
        return stream.str();
#endif
    }

    std::string getFormattedUTCTimestamp()
    {
#if defined(JETS_CLIENT)
        using ms_time_t = time_t;  // millisecondsF
#elif defined(LINUX) || defined(ANDROID)
        using ms_time_t = int64_t;  // millisecondsF
#else
        using ms_time_t = time_t;  // millisecondsF
#endif

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
        ms_time_t epoch = ms.count();

        ms_time_t timeInSeconds = epoch / 1000;

        ms_time_t milliseconds = epoch % 1000;

        tm buf = {};

#ifdef _WIN32
        gmtime_s(&buf, &timeInSeconds);
#elif defined(JETS_CLIENT)
        gmtime_r(&timeInSeconds, &buf);
#elif !defined(__LP64__) && (defined(LINUX) || defined(ANDROID))
        gmtime64_r(&timeInSeconds, &buf);
#else
        gmtime_r(&timeInSeconds, &buf);
#endif

        const auto size = sizeof("1999-12-12 12:12:12,000");
        const auto format = "%Y-%m-%d %H:%M:%S,";

        std::vector<char> timeStr;
        timeStr.resize(size);
        strftime(timeStr.data(), size, format, &buf);
        
        std::stringstream milliStream;
        milliStream << std::setw(3) << std::setfill('0') << milliseconds;

        return std::string(timeStr.data()) + milliStream.str();
    }

}
}
