#ifndef WORLD_H_
#define WORLD_H_
#include "my_stl.h"
#include "singleton.h"

namespace TMacZone{
	class CServerBase;
	class CConfigManager;
	class CMailBox;
	class CPluto;
	class CWorld : public CSingleton<CWorld>
	{
	public:
		CWorld();
		~CWorld();

		void SetServer(CServerBase* pServer)
		{
			m_pServer = pServer;
		}

		CServerBase* GetServer()
		{
			return m_pServer;
		}

		virtual void InitWorld();
		
		void InitMailBoxs();
		virtual int HandleMessages(CPluto* u) = 0;

	private:
		CServerBase*		m_pServer;
		CConfigManager*		m_pConfigManager;
	};
}

#endif