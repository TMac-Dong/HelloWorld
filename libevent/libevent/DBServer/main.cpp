#include "db_server.h"
#include "db_world.h"
#include "../CoreBase/my_stl.h"
#include "../CoreBase/server_config.h"

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
	pServer->Server(pObject->m_strAddr.c_str(), pObject->m_unPort);

	SAFE_DELETE(pWorld);
	SAFE_DELETE(pServer);
	return 0;
}