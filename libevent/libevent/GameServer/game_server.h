#pragma  once
#include "../CoreBase/server_base.h"
using namespace TMacZone;

class CGameServer : public CServerBase
{
public:
	CGameServer();
	~CGameServer();

	virtual void HandleEventTick();
	virtual void HandleSocketTimeOut();
	virtual void OnShutDownServer();

};
