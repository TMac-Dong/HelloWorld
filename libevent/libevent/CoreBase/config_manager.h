#pragma once
#include "server_config.h"

namespace TMacZone
{
	class CConfigManager
	{
	public:
		CConfigManager() {};
		~CConfigManager() {};

		void LoadAllConfig()
		{
			CServerConfig::LoadConfig();
		}

		void RealeseAllConfig()
		{
			CServerConfig::ReleaseConfig();
		}
	};
}
