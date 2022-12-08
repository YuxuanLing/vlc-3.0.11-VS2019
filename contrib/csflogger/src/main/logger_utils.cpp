// This utilities can be used across all platforms the logger supports.

#include "logger_utils.h"

#define MAX_LINE_LENGTH 61440
#define MAX_REPETITIONS 20

char * generate_message (int msg_len,const char *format, va_list args)
{
    /* Initially guess we need no more than LOG_MSG_LEN bytes. */
    int n, size = msg_len;
    char *p = NULL, *np = NULL;
    int numRepetitions = 0;

    //From note in MSDN: "vsnprintf_s...initialize the buffer to null prior to calling the function."
    if ((p = (char*) calloc(size, 1)) == NULL)
    {
        return NULL;
    }

    while (numRepetitions < MAX_REPETITIONS)
    {
        /* Try to print in the allocated space. */
        va_list args2;
        va_copy(args2, args);
#ifdef _WIN32
#pragma warning (push)
#pragma warning (disable : 4996)

        n = _vscprintf(format, args2);

        if (n <= -1)
        {
            //There seems to be problems with generate_message(LOG_MSG_LEN, "%ls", someWideString)
            //even if the buffer is big enough, depending on what the wideString contains. If it contains
            //simply English text then there doesn't seem to be a problem. However, if it contains, say Russian, then
            //_vscprintf, vsnprintf, _vsnprintf_s all return -1. Tried lots of things, like setting locale
            //on Windows, nothing seems to fix this. For the moment just making sure we bail if Windows
            //can't deal with the passed in format+args, i.e. returns -1 even though buffer is big enough, as we
            //were running out of memory.
            //The following for example causes the issue: L"\u044F\u0447\u0432\u044B\u0430\u0432\u0430\u0432\u043C";

            free(p);
            return NULL;
        }

        if (n < size)
        {
            n = vsnprintf (p, size, format, args2);
        }

#pragma warning (pop) 
#else
        n = vsnprintf (p, size, format, args2);
#endif

        va_end(args2);
        /* If that worked, return the string. */
        if (n > -1 && n < size)
        {
            *(p+n) = '\0';
            return p;
        }

        /* Else try again with more space. */
        if (n > -1)    /* glibc 2.1 */
        {
            size = n+1; /* precisely what is needed */
        }
        else if (size == MAX_LINE_LENGTH)
        {
            *(p + MAX_LINE_LENGTH - 1) = '\0';
            return p;
        }
        else           /* glibc 2.0 */
        {
            size *= 2;  /* twice the old size */
        }

        if (size > MAX_LINE_LENGTH)
        {
            size = MAX_LINE_LENGTH;
        }

        if ((np = (char*)realloc (p, size)) == NULL)
        {
            free(p);
            return NULL;
        }
        else
        {
            p = np;
            memset(p, '\0', size);
        }

        ++numRepetitions;
    }/* end while*/

    free(p);
    return NULL;
}

char *logger_trimwhitespace(char *str)
{
    std::locale loc;
    char * end = NULL;

    if (str == NULL) return NULL;

    // Trim leading space
    while(std::isspace(*str, loc)) str++;

    if(*str == '\0')  // All spaces?
    return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && std::isspace(*end, loc)) end--;

    // Write new null terminator
    *(end+1) = '\0';

    return str;
}

