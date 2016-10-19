#pragma once
#include "db_operater.h"
#include "pthread.h"
#include <list>
#include "../CoreBase/pluto.h"
#include "db_world.h"
using namespace TMacZone;

class CGuardLock
{
public:
	CGuardLock(pthread_mutex_t lock)
	{
		m_lock = lock;
		pthread_mutex_lock(&m_lock);
	}
	~CGuardLock()
	{
		pthread_mutex_unlock(&m_lock);
	}

private:
	pthread_mutex_t m_lock;
};

class CPlutoList
{
public:
	CPlutoList()
		: m_bInit(false)
	{
		
	}

	~CPlutoList()
	{
		DestoryMutex();
	}

	bool InitMutex()
	{
		if (!m_bInit)
		{
			if (0 != pthread_mutex_init(&m_lock, NULL))
			{
				printf("pthread_mutex_init %d\n");
				return false;
			}

			m_bInit = true;
		}

		return m_bInit;
	}

	void DestoryMutex()
	{
		pthread_mutex_destroy(&m_lock);
		m_bInit = false;
	}

	void PushPluto(CPluto* p)
	{
		CGuardLock lock(m_lock);
		m_list.push_back(p);
	}

	CPluto* PopPluto()
	{
		CGuardLock lock(m_lock);
		if (m_list.size() > 0)
		{
			CPluto* p = m_list.front();
			m_list.pop_front();

			return p;
		}
		
		return NULL;
	}

	size_t Size()
	{
		CGuardLock lock(m_lock);
		return m_list.size();
	}

	bool Empty()
	{
		CGuardLock lock(m_lock);
		return m_list.size() == 0 ? true : false;
	}

private:
	bool m_bInit;
	pthread_mutex_t m_lock;
	std::list<CPluto*> m_list;
};


static CPlutoList g_SendPlutoList;
static CPlutoList g_RecvPlutoList;
static bool g_bShutDown(false);

class CDBTask
{
public:
	CDBTask(int nSeq) : m_Db(nSeq) {};
	~CDBTask() {};

	bool Connect(const char* pszHost, const char* pszAccount, const char* pszPwd, const char* pszConnDb, std::string& strError)
	{
		return m_Db.Connect(pszHost, pszAccount, pszPwd, pszConnDb, strError);
	}

	void Run()
	{
		while (true)
		{
			CPluto* p = g_RecvPlutoList.PopPluto();
			if (p == NULL)
			{
#ifdef _WIN32
				::_sleep(1000);
#else
				sleep(1);
#endif // _WIN32
			}
			else
			{
				CWorld::GetSingletonPtr()->HandleMessages(p);
			}
				
			if (g_bShutDown)
			{
				if (g_RecvPlutoList.Empty())
				{
					break;
				}
			}
		}
	}


private:
	CDBOperater	m_Db;
};