#pragma once
#include "util.h"
#include "my_stl.h"
#include "enum_header.h"

namespace TMacZone
{
	struct SObject
	{
		SObject() : m_unId(0), m_eServerType(SERVER_TYPE_INVALIAD), m_strServerName(std::string()), m_strAddr(std::string()), m_unPort(0) {}

		uint32_t			m_unId;	
		ENUM_SERVER_TYPE	m_eServerType;
		std::string			m_strServerName;
		std::string			m_strAddr;
		uint32_t			m_unPort;
	};

	class CServerConfig
	{
	public:
		CServerConfig();
		~CServerConfig();

		static void LoadConfig();
		static void ReleaseConfig();

		static SObject* GetServerConfig(uint32_t unServerId)
		{
			std::map<uint32_t, SObject*>::iterator iter = ServerList.find(unServerId);
			if (iter != ServerList.end())
			{
				return iter->second;
			}

			return NULL;
		}

		static std::map<uint32_t, SObject*>& GetServerConfig()
		{
			return ServerList;
		}

	private:
		
		static std::map<uint32_t, SObject*>	ServerList;
	};

};



