
#ifndef EXCEPTION_H_
#define EXCEPTION_H_
#include <string>
#include <stdarg.h>
namespace TMacZone
{
	class CException
	{
	public:
		CException(int nCode, const std::string& strMsg);
		CException(int nCode, const char* pszMsg);
		~CException();

	public:
		inline int GetCode() const
		{
			return m_nCode;
		}

		inline std::string GetMsg() const
		{
			return m_strMsg;
		}

	private:
		int m_nCode;
		std::string m_strMsg;
	};

	inline void ThrowException(int n, const std::string& s)
	{
		throw CException(n, s);
	}

	extern void ThrowException(int n, const char* pszMsg, ...);
};
#endif