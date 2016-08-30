#include "db_world.h"
#include "../CoreBase/msg_id.h"

CDBWorld::CDBWorld()
{

}

CDBWorld::~CDBWorld()
{

}

void CDBWorld::InitWorld()
{
	CWorld::InitWorld();
}

int CDBWorld::HandleMessages(CPluto* u)
{
	pluto_msgid_t msg_id = u->GetMsgId();
	if (msg_id < 0)
		return -1;

	switch ((ENUM_PLUTO_MSG_ID)msg_id)
	{
	case MAX_CLIENT_SERVER_TEST:
	{	
		
		printf("MAX_CLIENT_SERVER_TEST successed!!!\n");
		break;
	}
	default:
	break;
	}
	return 0;
}






