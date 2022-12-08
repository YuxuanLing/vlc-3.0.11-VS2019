#pragma once

#include "csf/logger/CSFLog.h"

#include <string>

namespace CSF
{
namespace csflogger
{

	class CSF_LOGGER_API StringUtils
	{
	public:
		static std::string generateHashBasedOnSHA256(const std::string & str);

	private:
	};

}
}

