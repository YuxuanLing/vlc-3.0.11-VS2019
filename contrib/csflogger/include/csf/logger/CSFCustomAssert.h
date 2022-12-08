#ifndef CSFCUSTOMLOGGER_H
#define CSFCUSTOMLOGGER_H

/****************************************************************************************************************************
*
*  1. Ensure your project includes the jabber-global-project-settings & csf2g-log4cxx property sheets in Debug and Release.
*
*  2. #include the csf-logger:
*     csf-logger\include\csf\logger\CSFLogger.hpp
*
*  3. Create a static logger instance:
*     static CSFLogger* theLogger = CSFLogger_getLogger("theLogger");
*
*  4. Invoke the jabberAssert macro:
*     jabberAssert(false, theLogger, "The Log Message");
*
*  Debug mode	: Throw standard assert where Aborting will crash the application.
*  Release mode	: Throw standard assert where Aborting will crash the application.
*  FCS mode		: Log a FATAL line to the logger and continue.
*
****************************************************************************************************************************/

#ifdef WIN32
	#include <crtdefs.h>
#endif

#undef  jabberAssert


/******************   Debug mode   ******************/

#ifndef NDEBUG
#ifdef WIN32
	#define jabberAssert(_Expression, _Logger, _Message) assert(_Expression)
	#define jabberQuickAssert(_Expression) jabberAssert(_Expression, 0, "")
#else
	#define jabberAssert(_Expression, _Logger, _Message) ((void)0)
	#define jabberQuickAssert(_Expression) ((void)0)
#endif
#endif

/****************   End Debug mode   ****************/



/*****************   Release mode   *****************/

#if (defined(NDEBUG) && defined(INTERNAL_BUILD))

#ifdef WIN32
	#ifdef  __cplusplus
		extern "C" {
	#endif
			_CRTIMP void __cdecl _wassert(_In_z_ const wchar_t * _Message, _In_z_ const wchar_t *_File, _In_ unsigned _Line);
	#ifdef  __cplusplus
		}
	#endif

	#define jabberAssert(_Expression, _Logger, _Message) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression) _CRT_WIDE(#_Message), _CRT_WIDE(__FILE__), __LINE__), 0) )
	#define jabberQuickAssert(_Expression) jabberAssert(_Expression, 0, "")
#else
	#define jabberAssert(_Expression, _Logger, _Message) ((void)0)
	#define jabberQuickAssert(_Expression) ((void)0)
#endif

#endif  /* NDEBUG && INTERNAL_BUILD */

/***************   End Release mode   ***************/



/*******************   FCS mode   *******************/

#if defined(NDEBUG) && !defined(INTERNAL_BUILD)
	#define jabberAssert(_Expression, _Logger, _Message) if (!_Expression) { std::ostringstream _oss; _oss << _Message; CSFLog(_Logger, CSF_LOG_FATAL,   __FILE__ , __LINE__ , __FUNCTION__ , _oss.str()); }
	#define jabberQuickAssert(_Expression) ((void)0)
#endif

/*****************   End FCS mode   *****************/

#endif  /* CSFCUSTOMLOGGER_H */
