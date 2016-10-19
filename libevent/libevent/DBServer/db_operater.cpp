#include "db_operater.h"
#include <stdio.h>
#include <string>
#define NULL 0

#include <stdio.h>
#include <stdlib.h>
#include <Winsock2.h>

CDBOperater::CDBOperater(int nSeq)
:	m_nSeq(nSeq)
,	m_bConnceted(false)
{
	m_pMysql = mysql_init(NULL);
}

CDBOperater::~CDBOperater()
{
	DisConnect();
}

bool CDBOperater::Connect(const char* pszHost, const char* pszAccount, const char* pszPwd, const char* pszConnDb, std::string& strError)
{
	int  timeout = 10;
	if (0 != mysql_options(m_pMysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout))
	{
		m_bConnceted = false;
		strError.assign(mysql_error(m_pMysql));
		printf("mysql_options failed!! error=%s\n", strError);
		return false;
	}

	if (0 != mysql_options(m_pMysql, MYSQL_SET_CHARSET_NAME, "utf8"))//ÉèÖÃ×Ö·û¼¯utf8
	{
		m_bConnceted = false;
		strError.assign(mysql_error(m_pMysql));
	}

	if (!mysql_real_connect(m_pMysql, pszHost, pszAccount, pszPwd, pszConnDb, 0, NULL, 0))
	{
		m_bConnceted = false;
		strError.assign(mysql_error(m_pMysql));
		printf("mysql_real_connect failed!! error=%s\n", strError);
		return false;
	}
	else
	{
		std::string str = "set global interactive_timeout=5";
		if (0 != mysql_real_query(m_pMysql, str.c_str(), (unsigned long)str.size()))
		{
			m_bConnceted = false;
			strError.assign(mysql_error(m_pMysql));
			printf("mysql_real_query set global interactive_timeout=5 failed!! error=%s\n", strError);
			return false;
		}
	}

	m_bConnceted = true;
	return true;
}

void CDBOperater::DisConnect()
{
	mysql_close(m_pMysql);
}

int CDBOperater::InsertSql(const char* pszSql)
{
	return ExecSql(pszSql);
}

int CDBOperater::DeleteSql(const char* pszSql)
{
	return ExecSql(pszSql);
}

int CDBOperater::UpdateSql(const char* pszSql)
{
	return ExecSql(pszSql);
}

int CDBOperater::ExecSql(const char* pszSql)
{
	if (!m_bConnceted)
	{
		printf("no connected\n");
		return -1;
	}

	if (0 != mysql_real_query(m_pMysql, pszSql, strlen(pszSql)))
	{
		printf("mysql_real_query failed!! sql = %s, error = %s\n", pszSql, mysql_error(m_pMysql));
		return -1;
	}

	int nRows = (int)mysql_affected_rows(m_pMysql);
	if (nRows <= 0)
		return -2;

	return nRows;
}

MYSQL_FIELD* CDBOperater::SelectSql(const char* pszSql)
{
	if (!m_bConnceted)
	{
		printf("no connected\n");
		return NULL;
	}

	if (0 != mysql_real_query(m_pMysql, pszSql, strlen(pszSql)))
	{
		printf("mysql_real_query failed!! sql = %s, error = %s\n", pszSql, mysql_error(m_pMysql));
		return NULL;
	}

	int nRows = (int)mysql_affected_rows(m_pMysql);
	if (nRows <= 0)
		return NULL;

	MYSQL_RES* res = mysql_store_result(m_pMysql);
	if (res == NULL)
	{
		printf("mysql_store_result! sql = %s, error = %s\n", pszSql, mysql_error(m_pMysql));
		return NULL;
	}

	MYSQL_FIELD * field = mysql_fetch_fields(res);
	return field;
}







