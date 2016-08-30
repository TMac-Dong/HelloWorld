#include "server_base.h"
#include "exception.h"
#include "mail_box.h"
#include "pluto.h"
#include "world.h"

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif
namespace TMacZone
{

	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sa, int socklen, void *user_data);

	static void readcb(struct bufferevent *bev, void *ptr);

	static void eventcb(struct bufferevent *bev, short events, void *user_data);

	static void signalcb(evutil_socket_t sig, short events, void *user_data);

	static void	writecb(struct bufferevent *bev, void *user_data);

	static void	timercb(evutil_socket_t fd, short events, void *user_data);

	CServerBase::CServerBase()
		: m_pMainBase(NULL)
		, m_timer_event(NULL)
		, m_evconnlistener(NULL)
	{
		
	}
	CServerBase::~CServerBase()
	{

	}

	int CServerBase::StartServer(std::string strIP, uint32_t unPort)
	{
#ifdef _WIN32
		WSADATA wsaData;
		WORD wVersionRequested = MAKEWORD(1, 1);
		WSAStartup(wVersionRequested, &wsaData);
#endif // _WIN32
		m_pMainBase = event_base_new();
		if (m_pMainBase == NULL)
		{
			printf("event_base_new:failed!!\n");
			return -1; 
		}
		
		m_timer_event = event_new(m_pMainBase, -1, EV_PERSIST, timercb, this);
		if (m_timer_event == NULL)
			return -1;
	
		struct timeval va;
		va.tv_sec = 0;
		va.tv_usec = 5 * 1000;
		int ret = event_add(m_timer_event, &va);
		if (ret != 0)
		{
			printf("event_add error\n");
			return -1;
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = PF_INET;
		addr.sin_port = htons(unPort);

		if (strIP.size() == 0)
			addr.sin_addr.s_addr = INADDR_ANY;
		else
			addr.sin_addr.s_addr = inet_addr(strIP.c_str());
		
		m_evconnlistener = evconnlistener_new_bind(m_pMainBase, listener_cb, (void*)this, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,-1,(struct sockaddr*)&addr,sizeof(addr));
		if (m_evconnlistener == NULL)
		{
			printf("new_bind error :%s\n", evutil_socket_error_to_string(EVUTIL_SOCKET_ERROR()));

			return -1;
		}

		evutil_socket_t fd = evconnlistener_get_fd(m_evconnlistener);
		evutil_make_socket_nonblocking(fd);

		enum {_BUFF_SIZE = 174760};
		int buffsize = _BUFF_SIZE;
		setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&buffsize, sizeof(int));
		setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&buffsize, sizeof(int));


		printf("StartServer Successed!!\n");
		return 0;
	}

	bool CServerBase::Server(const char* pszAddr, uint32_t unPort)
	{
		if (StartServer(pszAddr, unPort) != 0)
		{
			printf("StartServer failed!\n");
			return false;
		}

		ConnectMailBoxs();

		event_base_dispatch(m_pMainBase);
		int nCode = event_base_got_break(m_pMainBase);
		int nCode1 = event_base_got_exit(m_pMainBase);
		if (m_timer_event)
		{
			event_del(m_timer_event);
			event_free(m_timer_event);
			m_timer_event = NULL;
		}

		if (m_pMainBase)
		{
			event_base_free(m_pMainBase);
			m_pMainBase = NULL;
		}

		OnShoutDownServer();
		return true;
	}

	void CServerBase::ShutdownServer()
	{
	#ifdef _WIN32
		WSACleanup();
	#endif

		event_base_free(m_pMainBase);
	}

	int CServerBase::HandleSocketConnectioned(evutil_socket_t fd)
	{
		CMailBox* pmb = GetMailBoxFromFd(fd);
		if (pmb)
		{
			if (!pmb->GetConnected())
			{
				pmb->SetConnected(true);
				printf("Connected successed!!\n");
				return 0;
			}
		}
		return -1;
	}

	int CServerBase::HandleNewConnection(evutil_socket_t fd, struct sockaddr *sa, int socklen)
	{
		enum {MAX_ACCEPT = 5000};
		if (m_mMbs.size() > MAX_ACCEPT)
		{
			evutil_closesocket(fd);
			return -1;
		}
		
		sockaddr_in* the_addr = (struct sockaddr_in*)sa;
		char* pszClientAddr = inet_ntoa(the_addr->sin_addr);
		uint32_t unPort = ntohs(the_addr->sin_port);
		printf("CServerBase::HandleNewConnection：fd:%d; Connected from %s(port=%d);\n", fd, pszClientAddr, unPort);

		evutil_make_socket_nonblocking(fd);
		return OnNewFdAccepted(fd, pszClientAddr, unPort);
	}

	void CServerBase::HandleSocketClosed(evutil_socket_t fd)
	{
		evutil_closesocket(fd);
		CMailBox* mb = GetMailBoxFromFd(fd);
		if (mb)
		{
			mb->SetConnected(false);
		}

		OnCloseFd(fd);
	}

	void CServerBase::HandleCloseConnection()
	{

	}

	void CServerBase::HandleSocketError()
	{

	}

	void CServerBase::HandleSocketTimeOut()
	{

	}

	void CServerBase::HandleWriteSocket()
	{

	}

	void CServerBase::HandleEventTick()
	{
		std::map<evutil_socket_t, CMailBox*>::iterator iter = m_mMbs.begin();
		for ( ; iter != m_mMbs.end(); ++iter)
		{
			if (iter->second->GetConnected())
				iter->second->SendAll();
		}

		HandleRecvMsg();

	}

	void CServerBase::OnShoutDownServer()
	{
#ifndef _WIN32
		sleep(2);
#else
		::Sleep(2000);
#endif
	}

	void CServerBase::AddRecvMsg(CPluto* u)
	{
		m_recvMsgs.push_back(u);
	}

	void CServerBase::HandleRecvMsg()
	{
		while (!m_recvMsgs.empty())
		{
			printf("HandleRecvMsg\n");
			CWorld::GetSingletonPtr()->HandleMessages(m_recvMsgs.front());
			m_recvMsgs.erase(m_recvMsgs.begin());
		}
	}

	CMailBox* CServerBase::GetMailBoxFromFd(evutil_socket_t fd)
	{
		std::map<evutil_socket_t, CMailBox*>::iterator iter = m_mMbs.find(fd);
		if (iter != m_mMbs.end())
		{
			return iter->second;
		}

		return NULL;
	}

	CMailBox* CServerBase::GetMailBoxBySId(uint32_t unId)
	{
		std::map<uint32_t, CMailBox*>::iterator iter1 = m_mMailBox.find(unId);
		if (iter1 != m_mMailBox.end())
			return iter1->second;

		return NULL;
	}

	void CServerBase::AddConfigMB(CMailBox* pmb)
	{
		m_vecConnMbs.push_back(pmb);
	}

	void CServerBase::ConnectMailBoxs()
	{
		std::vector<CMailBox*>::iterator iter = m_vecConnMbs.begin();
		for (; iter != m_vecConnMbs.end(); ++iter)
		{
			OnConnectServer(*iter);
			//iter = m_vecConnMbs.erase(iter);
		}
	}

	void CServerBase::HandleReadSocket(struct bufferevent *bev)
	{
		evutil_socket_t fd = bufferevent_getfd(bev);
		OnReadMessage(bev);
	}

	bool CServerBase::CheckHeadSize(evutil_socket_t fd, CMailBox* pmb, uint32_t unHeadLen)
	{
		if (unHeadLen < MSGLEN_TEXT_POS)
		{
			evutil_closesocket(fd);
			return false;
		}

		if (pmb->GetAuthz() != MAILBOX_CLIENT_TRUSTED)
		{
			if (unHeadLen > MSGLEN_MAX)
			{
				return false;
			}
		}

		return true;
	}

#define PLUTO_MSGLEN_HEAD 4
	int CServerBase::OnReadMessage(struct bufferevent *bev)
	{
		//接受完成
		evutil_socket_t fd = bufferevent_getfd(bev);
		CMailBox* pmb = GetMailBoxFromFd(fd);
		if (pmb == NULL)
		{
			return -1;
		}

		struct evbuffer* input = bufferevent_get_input(bev);
		ev_ssize_t buffer_len = evbuffer_get_length(input);
		if (buffer_len == 0)
		{
			return -1;
		}

		CPluto* u = pmb->GetRecvPluto();
		if (u == NULL)
		{
			char szHead[PLUTO_MSGLEN_HEAD];
			ev_ssize_t nLen = evbuffer_copyout(input, szHead, PLUTO_MSGLEN_HEAD);
			if (nLen < PLUTO_MSGLEN_HEAD)
			{
				printf("Get PLUTO_MSGLEN_HEAD failed!\n");
				return -1;
			}
			if (strlen(szHead) == 0)
			{
				printf("PLUTO_MSG_HEAD is null!\n");
				return -1;
			}

			int ret = evbuffer_drain(input, nLen);
			if (ret != 0)
			{
				printf("drain failed!\n");
				return -1;
			}

			if (!CheckHeadSize(fd,pmb, sz_to_uint32((unsigned char*)szHead)))
				return -2;

			u = new CPluto(buffer_len);
			char* pszRecvBuffer = u->GetRecvBuffer();
			memcpy(pszRecvBuffer, szHead, PLUTO_MSGLEN_HEAD);
			ev_ssize_t nWant = buffer_len - PLUTO_MSGLEN_HEAD;
			nLen = evbuffer_copyout(input, pszRecvBuffer + PLUTO_MSGLEN_HEAD, nWant);
		
			if (nLen > 0)
			{
				evbuffer_drain(input, nLen);
				if (nLen == nWant)
				{
					u->EndRecv(nLen);
					AddRecvMsg(u);
					pmb->SetRecvPluto(NULL);
				}
				else
				{
					//接受不完整留在下一次接受
					u->SetLen(PLUTO_MSGLEN_HEAD + nLen);
					pmb->SetRecvPluto(u);
					return nLen + PLUTO_MSGLEN_HEAD;
				}
			}
		}
		else
		{
			//TO DO
			uint32_t unRecvLen = u->GetLen();
			char* pszBuffer = u->GetRecvBuffer();
			ev_ssize_t unReadLen = evbuffer_copyout(input, pszBuffer + unRecvLen, buffer_len);
			ev_ssize_t nWant = buffer_len - unRecvLen;
			evbuffer_drain(input, unReadLen);
			
			if (nWant == unReadLen)
			{
				u->EndRecv(buffer_len);
				AddRecvMsg(u);
				pmb->SetRecvPluto(NULL);
			}
			else
			{
				//接受不完整留在下一次接受
				u->SetLen(PLUTO_MSGLEN_HEAD + unReadLen);
				pmb->SetRecvPluto(u);
				return unRecvLen + unReadLen;
			}
		}

		return 0;
	}

	int CServerBase::OnNewFdAccepted(evutil_socket_t fd, const char* pszAddr, uint32_t unPort)
	{	
		enum {MIN_ID = 100};

		CMailBox* pmb = new CMailBox(fd + MIN_ID, pszAddr, unPort);
		AddFdToMb(fd, pmb);

		pmb = GetMailBoxFromFd(fd);
		if (pmb)
		{
			struct bufferevent* bev = bufferevent_socket_new(m_pMainBase, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
			bufferevent_setcb(bev, readcb, NULL, eventcb, (void*)this);
			bufferevent_enable(bev, EV_READ | EV_WRITE | EV_PERSIST);

			pmb->SetBufferEvent(bev);
			pmb->SetAuthz(MAILBOX_CLIENT_TRUSTED);
		}


		return 0;
	}

	int CServerBase::OnCloseFd(evutil_socket_t fd)
	{
		
		return 0;
	}


	int CServerBase::OnConnectServer(CMailBox* pmb)
	{
		int fd = pmb->ConnectServer();
		if ( fd > 0)
		{
			bufferevent* bev = bufferevent_socket_new(m_pMainBase, fd, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
			GameAssert(bev, "");
			
			bufferevent_setcb(bev, readcb, NULL, eventcb, this);
			bufferevent_enable(bev, EV_READ|EV_WRITE|EV_PERSIST);


			sockaddr_in the_addr;
			memset((char*)&the_addr, 0, sizeof(the_addr));

			the_addr.sin_addr.s_addr = inet_addr(pmb->GetClientAddr().c_str());
			the_addr.sin_port = htons(pmb->GetClientPort());
			the_addr.sin_family = AF_INET;

			int ret = bufferevent_socket_connect(bev, (sockaddr*)&the_addr, sizeof(struct sockaddr));
			if (ret != 0)
			{
				pmb->SetBufferEvent(NULL);
				return -1;
			}

			pmb->SetBufferEvent(bev);
			AddFdToMb(fd, pmb);
			return 0;
		}
		
		return -1;
	}

	void CServerBase::AddFdToMb(evutil_socket_t fd, CMailBox* pmb)
	{
		std::map<evutil_socket_t, CMailBox*>::iterator iter = m_mMbs.find(fd);
		if (iter != m_mMbs.end())
		{
			//异常情况,有一个老的mb未删除
			CMailBox* p2 = iter->second;
			if (p2 != pmb)
			{
				delete p2;
				iter->second = pmb;
			}
			printf("CServerBase::addFdAndMb_err", "desc=old_fd_mb; fd=%d\n", fd);
		}
		else
		{
			pmb->SetFd(fd);
			m_mMbs.insert(std::make_pair(fd, pmb));
			m_mMailBox.insert(std::make_pair(pmb->GetMailBoxSId(), pmb));
		}

		printf("AddFdToMb :fd = %d, Addr = %s, port=%d, auth = %d\n", fd, pmb->GetClientAddr().c_str(), pmb->GetClientPort(), pmb->GetAuthz());
	}

	void CServerBase::RemoveFd(evutil_socket_t fd)
	{
		std::map<evutil_socket_t, CMailBox*>::iterator iter = m_mMbs.find(fd);
		if (iter != m_mMbs.end())
		{
			m_mMbs.erase(iter);
			
			std::map<uint32_t, CMailBox*>::iterator iter1 = m_mMailBox.find((uint32_t)iter->second->GetMailBoxSId());
			if (iter1 != m_mMailBox.end())
			{
				m_mMailBox.erase(iter1);	
			}
		}
	}

	void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sa, int socklen, void *user_data)
	{
		CServerBase* pServer = (CServerBase*)user_data;
		pServer->HandleNewConnection(fd, sa, socklen );
	}

	void readcb(struct bufferevent *bev, void *ptr)
	{
		CServerBase* pServer = (CServerBase*)ptr;
		pServer->HandleReadSocket(bev);
	}

	void eventcb(struct bufferevent *bev, short events, void *user_data)
	{
		CServerBase* pServer = (CServerBase*)user_data;
		evutil_socket_t fd = bufferevent_getfd(bev);
		bool bFinished(false);
		if (events & BEV_EVENT_CONNECTED)
		{
			//BEV_EVENT_CONNECTED 是连接完成之后发过来的
			pServer->HandleSocketConnectioned(fd);
			bFinished = true;
		}
		else if (events & BEV_EVENT_EOF)
		{
			pServer->HandleSocketClosed(fd);
			bFinished = true;
		}
		else if (events & BEV_EVENT_ERROR)
		{
			pServer->HandleSocketError();
			bFinished = true;
		}
		else if (events & BEV_EVENT_TIMEOUT)
		{
			//超时不处理
		}

		if (!bFinished)
		{
			bufferevent_free(bev);
		}
	}

	void signalcb(evutil_socket_t sig, short events, void *user_data)
	{

	}

	void	writecb(struct bufferevent *bev, void *user_data)
	{
		struct evbuffer* output = bufferevent_get_output(bev);
		if (evbuffer_get_length(output) == 0)
		{
			bufferevent_free(bev);
		}
		else
		{
			CServerBase* pServer = (CServerBase*)user_data;
			pServer->HandleWriteSocket();
		}
	}

	void	timercb(evutil_socket_t fd, short events, void *user_data)
	{
		CServerBase* pServer = (CServerBase*)user_data;
		pServer->HandleEventTick();
	}
};