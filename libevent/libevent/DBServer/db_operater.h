#pragma once
#include <my_global.h>
#include <mysql.h>
#include <string>
class CDBOperater
{
public:
	CDBOperater(int nSeq);
	~CDBOperater();

	bool Connect(const char* pszHost, const char* pszAccount, const char* pszPwd, const char* pszConnDb,std::string& strError);
	void DisConnect();

	int InsertSql(const char* pszSql);
	int DeleteSql(const char* pszSql);
	int UpdateSql(const char* pszSql);
	int ExecSql(const char* pszSql);
	MYSQL_FIELD* SelectSql(const char* pszSql);

private:
	MYSQL*		m_pMysql;
	int			m_nSeq;			// Ïß³ÌºÅ
	bool		m_bConnceted;

	CDBOperater(CDBOperater& other);
	CDBOperater& operator=(CDBOperater& other);
};