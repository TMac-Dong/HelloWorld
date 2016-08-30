#include "world.h"
#include "config_manager.h"
#include "server_base.h"
#include "mail_box.h"

namespace TMacZone
{
	CWorld::CWorld()
		: m_pServer(NULL)
		, m_pConfigManager(NULL)
	{
	}

	CWorld::~CWorld()
	{
		m_pConfigManager->RealeseAllConfig();
		SAFE_DELETE(m_pConfigManager);
	}

	void CWorld::InitWorld()
	{
		GameAssert(m_pServer,"");
		if (m_pConfigManager == NULL)
			m_pConfigManager = new CConfigManager();
		
		m_pConfigManager->LoadAllConfig();

		InitMailBoxs();
	}


	void CWorld::InitMailBoxs()
	{
		std::map<uint32_t, SObject*>::iterator iter = CServerConfig::GetServerConfig().begin();
		for (; iter != CServerConfig::GetServerConfig().end(); ++iter)
		{
			CMailBox* pmb = new CMailBox(iter->second->m_unId, iter->second->m_strAddr.c_str(), iter->second->m_unPort);

			m_pServer->AddConfigMB(pmb);
		}
	}

};










