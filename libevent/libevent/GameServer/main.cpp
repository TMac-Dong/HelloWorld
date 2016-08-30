#include "game_server.h"
#include "game_world.h"
#include "../CoreBase/world.h"
#include "../CoreBase/my_stl.h"
#include "../CoreBase/server_config.h"

int main(int argc,char** argv)
{
	if (sizeof(argv) < 1)
	{
		return -1;
	}
	uint32_t unServerId = (uint32_t)atoi(argv[1]);

	CGameWorld* pWorld = new CGameWorld();
	CGameServer* pServer = new CGameServer();
	pWorld->SetServer(pServer);
	pWorld->InitWorld();

	SObject* pObject = CServerConfig::GetServerConfig(unServerId);
	pServer->Server(pObject->m_strAddr.c_str(), pObject->m_unPort);


	SAFE_DELETE(pServer);
	SAFE_DELETE(pWorld);
	return 0;
}