#include "db_server.h"
#include "db_world.h"
#include "../CoreBase/my_stl.h"
#include "../CoreBase/server_config.h"
#include <pthread.h>
#include <mysql.h>
#include "db_task.h"
void* RunTask(void* arg)
{
	int nSeq = *(int*)arg;
	if (nSeq < 0)
	{
		printf("RunTask Error! pthread_id =%d\n", nSeq);
		return NULL;
	}

	if (nSeq > 0)
	{
#ifdef _WIN32
		// fix the redis_connet crash
		::Sleep(2000 * nSeq);
#endif
	}

	mysql_thread_init();
	CDBTask dbTask(nSeq);
	std::string strError;
	if (!dbTask.Connect("127.0.0.1", "root", "", "card", strError))
	{
		printf("dbTask.Connect failed!! error=%s\n", strError.c_str());
		mysql_thread_end();
		return NULL;
	}
	dbTask.Run();
	mysql_thread_end();
	
	return NULL;
}

int main(int argc, char** argv)
{
	if (sizeof(argv) < 1)
	{
		return -1;	
	}
	uint32_t unServerId = (uint32_t)atoi(argv[1]);

	CDBWorld* pWorld = new CDBWorld();
	CDBServer* pServer = new CDBServer();
	pWorld->SetServer(pServer);
	pWorld->InitWorld();
	SObject* pObject = CServerConfig::GetServerConfig(unServerId);

	if (!g_RecvPlutoList.InitMutex() || !g_SendPlutoList.InitMutex())
	{
		printf("pthread_mutex_t is not init\n");
		return -1;
	}
	std::vector<pthread_t> ThreadList;
	for (int index = 0; index < 3; ++index)
	{
		int* pTaskNum = new int(index);
		pthread_t pthread_id;
		if (pthread_create(&pthread_id, NULL, RunTask, pTaskNum) != 0)
		{
			printf("pthread_join error:%d,%s\n", errno, strerror(errno));
			return -2;
		}
		ThreadList.push_back(pthread_id);
	}
	
	pServer->Server(pObject->m_strAddr.c_str(), pObject->m_unPort);

	for (size_t index = 0; index < ThreadList.size(); ++index)
	{
		if (0 != pthread_join(ThreadList[index], NULL))
		{
			printf("pthread_join error:%d,%s\n", errno, strerror(errno));
			return -1;
		}
	}


	SAFE_DELETE(pWorld);
	SAFE_DELETE(pServer);
	return 0;
}