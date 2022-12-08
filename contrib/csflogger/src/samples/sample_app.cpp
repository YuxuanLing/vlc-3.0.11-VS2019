#include "csf/logger/CSFLogger.h"
#include "csf/logger/CSFLog.h"
#include "csf/logger/VersionInfo.hpp"
#include "csf/logger/CSFLogger.hpp"


#include <iostream>

#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <string>

#ifdef USE_LOG4CXX
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/logger.h>
#endif


using namespace std;


/*
* sample app to demonstrate CSFLogger capabilities and usage.
* requires CSFLogger-log4cxx library and log4cxx Library
*/

int main(int argc, char** argv)
{
	/* Testing to see that no issues are caused by calling CSFLogger_getLogger before CSFLogger_init */
	CSFLogger_initialize(".\\Log\\cpve.log");
	CSFLogger* cpve = CSFLogger_getLogger("TestCpve");
	CSFLogWarn(cpve,"Testing writing log statement before init");
	/* init the logging framework using a config file */
	//CSFLogger_init("csflogger-log4cxx.config");
	/*init the logging framework using XML file to support UTF-8 */
	//CSFLogger_init("csflogger-log4cxx.xml");

	/* get root logger which was defined in the config file */
	CSFLogger* rLogger = CSFLogger_getRootLogger();
	CSFLogFatal(rLogger,"Testing the csflogger implementation with root logger - Fatal level.");
	CSFLogError(rLogger,"Testing Error level");
	CSFLogWarn(rLogger,"Testing Warning level");
	CSFLogInfo(rLogger,"Testing Info level");
	CSFLogDebug(rLogger,"Testing Debug level. This is the default level for root logger");
	CSFLogTrace(rLogger,"Testing Trace level. This statement should NOT show up!");
	

	/* get csf logger if it is already defined in the config file. 
	Otherwise, it will create a new logger which will inherit root logger properties */	
	CSFLogger* csf = CSFLogger_getLogger("csf");
	CSFLogInfo(csf,"Testing the csflogger implementation with csf logger");
	//CSFLogInfo(csf, "This default configuration allows you to log csf-cxx.log file.");

	CSFLogFatal(csf,"Testing Fatal level with csf logger.");
	CSFLogError(csf,"Testing Error level with csf logger");
	CSFLogWarn(csf,"Testing Warning level with csf logger");
	CSFLogInfo(csf,"Testing Info level with csf logger");
	CSFLogDebug(csf,"Testing Debug level. . This is the default level for csf logger!");
	CSFLogTrace(csf,"Testing Trace level. This statement should NOT show up in csf logger!");

	/* Get the log level of csf logger*/
	CSFLogLevel myLevel = CSFLogger_getLogLevel(csf);

	CSFLogInfo(csf, "The current log level for csf is: %d", myLevel);

	/* Testing creating new logger that is not in the config file. It hsould inherit properties of the root logger. */
	CSFLogger* appLogger = CSFLogger_getLogger("csf.appLogger");
	CSFLogInfo(appLogger, "Creating new logger which does not exist in the config but will inherit csf logger properties: csf.appLogger");
			
	CSFLogFatal(appLogger,"Testing the csflogger implementation with app logger - Fatal level.");
	CSFLogError(appLogger,"Testing Error level with app logger");
	CSFLogWarn(appLogger,"Testing Warning level with app logger");
	CSFLogInfo(appLogger,"Testing Info level with app logger");
	CSFLogDebug(appLogger,"Testing debug level. This is the default level for app logger");
	CSFLogTrace(appLogger,"Testing Trace level. This statement should NOT show up with app logger!");

	/* set the log level to Trace for the applogger's parent logger "csf" */
	CSFLogger_setLogLevel(csf, CSF_LOG_TRACE);
			
	CSFLogTrace(csf,  "** set its level to TRACE level **");

	CSFLogFatal(appLogger,"Testing the csflogger-log4cxx implementation with app logger - Fatal level.");
	CSFLogError(appLogger,"Testing Error level in app logger");
	CSFLogWarn(appLogger,"Testing Warning level in app logger");
	CSFLogInfo(appLogger,"Testing Info level in app logger");
	CSFLogDebug(appLogger,"Testing debug level. This is the default level for app logger");
	CSFLogTrace(appLogger,"Testing Trace level. This statement SHOULD not show up!");

    /* Printing version info */
//      CSF::csflogger::VersionInfo csfloggerInfo;
//      CSFLogInfo(l, csfloggerInfo.logString.c_str());

	/* set the log level to OFF and test that log statements are not printed out */
	CSFLogTrace(csf,  "** set its level to OFF level **");
	CSFLogger_setLogLevel(csf, CSF_LOG_OFF);
	CSFLogFatal(csf,"Testing Fatal level. This statement should show up!");
	CSFLogTrace(csf,"Testing Trace level. This statement SHOULD NOT show up!");

	CSFLogger_setLogLevel(csf, CSF_LOG_TRACE);
	CSFLogTrace(csf,  "** set log level TRACE level **");
	int test = 10;
	CSFLogFatalS(csf, "Testing string stream - adding int = %d"<< test); 
    CSFLogFatalWS(csf, "Testing string stream - adding int = %d"<< test);
    std::wstring russianStringWStr = L"\u044F\u0447\u0432\u044B\u0430\u0432\u0430\u0432\u043C\u043C\u0438\u043C\u0438\u044F\u0412\u042B\u0412\u042B\u042B\u042B\u0410\u0430\u044B\u0447";

    //CSFLogDebug(l, "%ls", russianStringWStr.c_str());

	return 0;
}
