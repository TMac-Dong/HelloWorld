#ifndef MAIL_BOX_H_
#define MAIL_BOX_H_
#include "util.h"
#include "enum_header.h"
#include <vector>
#include <event2/bufferevent.h>
#include "my_stl.h"

namespace TMacZone
{
	class CPluto;
	class CMailBox
	{
	public:
		CMailBox(uint16_t unId, const char* pszAddr, uint32_t unPort);
		~CMailBox();

		uint16_t GetMailBoxSId()
		{
			return m_unBoxSId;
		}
		void SetFd(uint64_t unFd)
		{
			m_unFd = unFd;
		}

		uint64_t GetFd()
		{
			return m_unFd;
		}

		void SetConnected(bool bConnected)
		{
			m_bConnected = bConnected;
		}

		bool GetConnected()
		{
			return m_bConnected;
		}

		void SetAuthz(MAILBOX_CLIENT_TYPE eAuthz)
		{
			m_eAuthz = eAuthz;
		}

		MAILBOX_CLIENT_TYPE GetAuthz()
		{
			return m_eAuthz;
		}

		std::string GetClientAddr()
		{
			return m_strAddr;
		}

		uint32_t GetClientPort()
		{
			return m_unPort;
		}

		void PushPluto(CPluto* p)
		{
			m_SendTo.push_back(p);
		}

		void SetBufferEvent(bufferevent* pBev)
		{
			m_bev = pBev;
		}

		bufferevent* GetBufferEvent()
		{
			return m_bev;
		}

		void SetRecvPluto(CPluto* pluto)
		{
			m_pPluto = pluto;
		}

		CPluto* GetRecvPluto()
		{
			return m_pPluto;
		}


		int SendAll();
		int ConnectServer();

	private:
		uint16_t	m_unBoxSId;
		uint64_t	m_unFd;
		std::string m_strAddr;
		uint32_t	m_unPort;
		bool		m_bConnected;
		MAILBOX_CLIENT_TYPE		m_eAuthz;
		time_t		m_tConnectTime;
		bufferevent*	m_bev;
		uint32_t	m_unSendPos;			//send阻塞时发送到的位置
		CPluto*		m_pPluto;				//正在接受的pluto

		std::vector<CPluto*> m_SendTo;
		
	private:
		CMailBox(const CMailBox&);
		CMailBox& operator=(const CMailBox&);
	};
}
#endif