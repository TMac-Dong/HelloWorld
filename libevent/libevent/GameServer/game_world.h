#ifndef GAME_WORLD_H_
#define GAME_WORLD_H_

#include "../CoreBase/world.h"
using namespace TMacZone;
class CGameWorld : public CWorld
{
public:
	CGameWorld();
	~CGameWorld();

	virtual void InitWorld();
	int HandleMessages(CPluto* u);


};
#endif