#include "csf/logger/StringUtils.hpp"
#include <sstream>
#include <iostream>
#include <map>
#include <iomanip>

#if !defined(VDI_THIN_CLIENT) && !defined(CISCO_VDI_HVD_AGENT)
//#include "openssl/evp.h"
#endif

namespace CSF
{
namespace csflogger
{
    std::string StringUtils::generateHashBasedOnSHA256(const std::string & str)
    {
#if defined(VDI_THIN_CLIENT) || defined(CISCO_VDI_HVD_AGENT)
        return "";
#else
#if 0	
        unsigned char hash[EVP_MAX_MD_SIZE];
		unsigned int len;

		EVP_MD_CTX *mdctx;

		if ((mdctx = EVP_MD_CTX_create()) == NULL)
		{
			return "";
		}

		if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))
		{
			return "";
		}

		if (1 != EVP_DigestUpdate(mdctx, str.c_str(), str.length()))
		{
			return "";
		}

		if (1 != EVP_DigestFinal_ex(mdctx, hash, &len))
		{
			return "";
		}

		EVP_MD_CTX_destroy(mdctx);

        std::ostringstream oss;
		for(unsigned int i = 0; i < len; ++i)
		{
		      oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
		}

        return oss.str();
#endif
        return "";		
#endif
    }
}
}