#include <csf/logger/VersionInfo.hpp>
#include <sstream>


// To incorporate version info into your project, you need to
// change the following to a unique namespace
using namespace CSF::csflogger;

// You may manually edit the version numbers and suffix below, but they can and will be
// modified by the csf build tools, so make sure you know what you are doing.

/* Begin generated initializers - do not edit */
const int VersionInfo::major = 0;
const int VersionInfo::minor = 12;
const int VersionInfo::patch = 3;
const int VersionInfo::build = 0;
const std::string VersionInfo::suffix = "";

const std::string VersionInfo::svnUrl = "unknown";
const std::string VersionInfo::svnRevision = "unknown";
const int VersionInfo::svnModifiedFiles = 0;

const std::string VersionInfo::scriptSvnUrl = "unknown";
const std::string VersionInfo::scriptSvnRevision = "unknown";

const std::string VersionInfo::buildDate = "unknown";
std::string VersionInfo::versionString = "unknown";
std::string VersionInfo::logString = "unknown";

std::vector<Dependency> VersionInfo::depends;
std::vector<SvnInfo> VersionInfo::svnExterns;
/* End generated initializers - do not edit */


VersionInfo::VersionInfo()
{
	/* Begin generated constructor body - do not edit */
	// We need to initialize the contents of any vectors here
	/* End generated constructor body - do not edit */

	// The build tools (if and when used to update version and dependency info)
	// will generate the versionString and logString.  But, we want those strings to be
	// reliably available - even when the csf common build tools are not used.
	// So we provide a backup mechanism which ensures that these are always set and always
	// reflect the proper version number at least.
	// TODO: Need to add a name member var so the version string has some context in this
	// default scenario.  (i.e. version 1.2 of what?)
	if (versionString.compare("unknown") == 0)
	{
		// if the version string has not been filled out, then we asume the logString is
		// also not filled out and will generate these here.
		std::stringstream version, log;
		version << major << "." << minor << "." << "(" << patch << "." << build;
		if (suffix.compare("snapshot") == 0)
		{
			version << "-snapshot";
		}
		version << ")";
		versionString = version.str();

		log << "*****************************\n\tcsflogger-log4cxx Version: " << versionString << "\n*****************************\n";
		logString = log.str();
	}
}
