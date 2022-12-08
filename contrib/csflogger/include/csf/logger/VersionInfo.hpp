/*
 * VersionInfo.hpp
 *
 *  Created on: Nov 2, 2010
 *  This file is to be copied and included in each library or component which is separately
 *  buildable/releaseable.
 *  The CSF2G central build scripts will modify the contents of this file for every build.
 *  If you are not using the central builds, you can make changes to the individual text field
 *  contents as necessary for your builds.  Any such custom strings will be ignored and replaced
 *	when central builds are run - but central builds will never commit changes.
 */

/**
* To add version info to your project, copy this file and change the #ifndef below to use
* your project name, and change the namespace to be unique to your project.  You will also need
* the corresponding VersionInfo.cpp file and need to change the namespace there accordingly.
*/
#ifndef CSFLOGGER_VERSION_INFO_HPP_
#define CSFLOGGER_VERSION_INFO_HPP_

#include <string>
#include <vector>

namespace CSF
{
namespace csflogger
{

	class Dependency
	{
		public:
			Dependency(std::string name, std::string version):
				name(name),
				version(version)
			{
			}
			std::string name;
			std::string version;
	};


	class SvnInfo
	{
		public:
			std::string localPath;
			std::string url;
			std::string revision;

			SvnInfo(std::string localPath, std::string url, std::string revision):
				localPath(localPath),
				url(url),
				revision(revision)
			{

			}
	};

	class VersionInfo
	{
		public:

			static const int major;
			static const int minor;
			static const int patch;
			static const int build;
			static const std::string suffix;
			static const std::string svnUrl;			// complete svn url to identify branch
			static const std::string svnRevision;		// svn revision of local workspace where build was performed
			static const int svnModifiedFiles;			// nubmer of locally modified files in the workspace.  anything greater than 0 means this build cannot be reliably reproduced.
			static const std::string scriptSvnUrl;
			static const std::string scriptSvnRevision;
			static const std::string buildDate;		// Date and time (formatted string) when build took place
			static std::string logString;		// formatted string containing all the version info - for easy and consistent printing to logs
			static std::string versionString;	// version number string for this component/artifact
			static std::vector<SvnInfo> svnExterns;
			static std::vector<Dependency> depends;

			VersionInfo();

	};

}
}

#endif /* CSFLOGGER_VERSION_INFO_H_ */
