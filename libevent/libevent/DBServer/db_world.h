#ifndef DB_WORLD_H_
#define DB_WORLD_H_

#include "../CoreBase/world.h"
#include "../CoreBase/pluto.h"
using namespace TMacZone;

class CDBWorld : public CWorld
{
public:
	CDBWorld();
	~CDBWorld();

	virtual void InitWorld();	
	virtual int HandleMessages(CPluto* u);

private:
	
};
#endif