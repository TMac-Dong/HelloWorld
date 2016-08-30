#include "server_config.h"
#include "tinyxml2.h"
#include <assert.h>
namespace TMacZone
{
	std::map<uint32_t, SObject*> CServerConfig::ServerList;
	CServerConfig::CServerConfig()
	{

	}

	CServerConfig::~CServerConfig()
	{

	}

	void CServerConfig::LoadConfig()
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError err_code = doc.LoadFile("server_config.xml");
		GameAssert(err_code == 0,"");
	
		tinyxml2::XMLElement* root = doc.RootElement();
		tinyxml2::XMLElement* pNode = root->FirstChildElement("server");
		while (pNode)
		{
			SObject* obj = new SObject();
			obj->m_unId = (uint32_t)pNode->IntAttribute("server_id");
			obj->m_strServerName = pNode->Attribute("server_name");
			
			if (strcmp(obj->m_strServerName.c_str(), "gameserver"))
			{
				obj->m_eServerType = SERVER_TYPE_GS;
			}
			else if (strcmp(obj->m_strServerName.c_str(), "dbserver"))
			{
				obj->m_eServerType = SERVER_TYPE_DB;
			}

			obj->m_strAddr = pNode->Attribute("server_addr");
			obj->m_unPort = (uint32_t)pNode->IntAttribute("server_port");
			
			ServerList.insert(std::make_pair(obj->m_unId, obj));
			pNode = pNode->NextSiblingElement("server");
		}

	}

	void CServerConfig::ReleaseConfig()
	{
		std::map<uint32_t, SObject*>::iterator iter = ServerList.begin();
		for (; iter != ServerList.end(); ++iter)
		{
			delete iter->second;
			iter->second = NULL;
		}
		
		ServerList.clear();
	}

};