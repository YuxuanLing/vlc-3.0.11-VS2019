/*! @file CSFLoggerCommon.h
* CSFLoggerCommon currently define macros to get CSFLogger DLL working on Windows
*
*	Created: Feb. 1st,2011
*/

#ifndef CSF_LOGGER_COMMON_H
#define CSF_LOGGER_COMMON_H
#define  CSF_LOGGER_EXPORTS
#if defined(_WIN32) && defined(_DLL)
	#if !defined(CSF_LOGGER_DLL) && !defined(CSF_LOGGER_STATIC)
		#define CSF_LOGGER_DLL
	#endif
#endif

#if defined(_WIN32) 
	#if defined(CSF_LOGGER_DLL)
		#if defined(CSF_LOGGER_EXPORTS)
			#define CSF_LOGGER_API __declspec(dllexport)
		#else
			#define CSF_LOGGER_API __declspec(dllimport)
		#endif
	#endif
#else
	#if __GNUC__ >= 4
		#define CSF_LOGGER_API __attribute__ ((visibility("default")))
	#endif
#endif

#if !defined(CSF_LOGGER_API)
	#define CSF_LOGGER_API
#endif

#if defined(_MSC_VER)
	#pragma warning(disable:4251) /* ... needs to have dll-interface warning */
#endif

#endif //CSF_LOGGER_COMMON_H

