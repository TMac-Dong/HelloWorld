#include "game_server.h"
#include "../CoreBase/enum_header.h"
#include "../CoreBase/pluto.h"
#include "../CoreBase/msg_id.h"
#include "../CoreBase/mail_box.h"

CGameServer::CGameServer()
{

}

CGameServer::~CGameServer()
{

}

void CGameServer::HandleEventTick()
{
	CServerBase::HandleEventTick();
	static uint32_t unSendCount = 0;

	CMailBox* pmb = GetMailBoxBySId(SERVER_TYPE_DB);
	if (pmb && unSendCount < 4)
	{
		unSendCount++;
		CPluto* u = new CPluto();
		u->Encode(MAX_CLIENT_SERVER_TEST);
		*u << 1;
		*u << EndPluto;

		pmb->PushPluto(u);
	}



}

void CGameServer::HandleSocketTimeOut()
{
	CServerBase::HandleSocketTimeOut();
}

void CGameServer::OnShutDownServer()
{
	CServerBase::OnShoutDownServer();
}