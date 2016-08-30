#include "exception.h"

#pragma warning(disable:4996)
namespace TMacZone
{
	CException::CException(int nCode, const std::string& strMsg)
		: m_nCode(nCode), m_strMsg(strMsg)
	{

	}

	CException::CException(int nCode, const char* pszMsg)
		: m_nCode(nCode), m_strMsg(pszMsg)
	{

	}
	
	CException::~CException()
	{
		
	}



	void ThrowException(int n, const char* pszMsg, ...)
	{
		char szTemp[512];
		memset(szTemp,0,sizeof(szTemp));
		va_list ap;
		va_start(ap, pszMsg);
		vsnprintf(szTemp, sizeof(szTemp) - 1,pszMsg, ap);
		va_end(ap);


		throw CException(n,szTemp);
	}

}
		
		
