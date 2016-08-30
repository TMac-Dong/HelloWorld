#include "mail_box.h"
#include "pluto.h"

#ifndef _WIN32
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <WinSock2.h>
#endif
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <event2/buffer.h>

namespace TMacZone
{
	CMailBox::CMailBox(uint16_t unId, const char* pszAddr, uint32_t unPort)
		: m_unBoxSId(unId), m_unFd(0), m_strAddr(pszAddr),
		m_unPort(unPort), m_bConnected(false), m_eAuthz(MAILBOX_CLIENT_UNAUTHZ),
		m_tConnectTime(0), m_bev(NULL), m_unSendPos(0), m_pPluto(NULL)
	{

	};

	CMailBox::~CMailBox()
	{
		SAFE_DELETE(m_pPluto);
	};


	int CMailBox::SendAll()
	{
		if (m_bev == NULL)
			return -1;

		evbuffer* output = bufferevent_get_output(m_bev);
		if (GetConnected())
		{
			while (!m_SendTo.empty())
			{
				CPluto* p = m_SendTo.front();
				struct evbuffer_iovec vec[1];
				uint32_t unWantSend = p->GetLen() - m_unSendPos;
				int ret = evbuffer_reserve_space(output, unWantSend, vec, 1);
				if (ret == -1)
				{
					return -2;
				}

				size_t unSendRet = vec[0].iov_len > unWantSend ? unWantSend : vec[0].iov_len;
				vec[0].iov_len = unSendRet;

				unsigned char* upChar = (unsigned char*)vec[0].iov_base;
				memset(upChar, 0, sizeof(upChar));
				memcpy(upChar, p->GetBuffer() + m_unSendPos, unWantSend);
				ret = evbuffer_commit_space(output, vec, 1);
				if (ret == -1)
				{
					printf("evbuffer_commit_space error=%s: fd = %d, msgid=%d,wantSendSize = %d, sendPos = %d, addr = %s\n", (intptr_t)evutil_socket_error_to_string((intptr_t)evutil_socket_geterror(m_unFd)), m_unFd, p->GetMsgId(), unSendRet,
						m_unSendPos, m_strAddr);
					
					return -3;
				}

				if (unSendRet != unWantSend)
				{
					uint16_t mbid = GetMailBoxSId();

					printf("CMailBox::sendAll", "nSendRet != nSendWant error.L#3. fd=%d, error:%s, GetLen:%d, m_nSendPos:%d, nSendWant:%d, nSendRet:%d\n"
						, m_unFd
						, (intptr_t)evutil_socket_error_to_string((intptr_t)evutil_socket_geterror(m_unFd))
						, p->GetLen()
						, m_unSendPos
						, unWantSend
						, unSendRet
						);

					//error handle
					if (mbid == 0 && GetAuthz() != MAILBOX_CLIENT_TRUSTED)
					{
						delete p;
						m_SendTo.erase(m_SendTo.begin());
						//客户端连接不重发了,直接关掉
						return -1;
					}

					if (unSendRet >= 0)
					{
						//阻塞了,留到下次继续发送
						m_unSendPos += unSendRet;

						return 0;
					}
				}

				delete p;
				m_SendTo.erase(m_SendTo.begin());
			}

		}
		return 0;
	}

	int CMailBox::ConnectServer()
	{
		m_unFd = 0;
		m_bev = 0;

		time_t  now = time_t(NULL);
		if (m_tConnectTime - now > 5)
			return -1;

		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1)
			return -1;
		
		evutil_make_socket_nonblocking(fd);

		return fd;
	}

	
};