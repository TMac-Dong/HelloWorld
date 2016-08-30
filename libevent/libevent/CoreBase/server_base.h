#ifndef SERVER_BASE_H_
#define SERVER_BASE_H_

#include <vector>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/util.h>
#include <map>
#include "util.h"
namespace TMacZone
{
	class CMailBox;
	class CPluto;
	class CServerBase
	{
	public:
		CServerBase();
		~CServerBase();

		//生成监听fd
		int StartServer(std::string strIP, uint32_t unPort);
		
		//开启服务器服务
		bool Server(const char* pszAddr, uint32_t unPort);
		//关闭服务器服务
		void ShutdownServer();

		virtual int HandleSocketConnectioned(evutil_socket_t fd);
		virtual int HandleNewConnection(evutil_socket_t fd, struct sockaddr *sa, int socklen);
		virtual void HandleSocketClosed(evutil_socket_t fd);
		virtual void HandleCloseConnection();
		virtual void HandleSocketError();
		virtual void HandleSocketTimeOut();
		virtual void HandleReadSocket(struct bufferevent *bev);
		virtual void HandleWriteSocket();
		virtual void HandleEventTick();
	
	public:
		CMailBox* GetMailBoxFromFd(evutil_socket_t fd);
		CMailBox* GetMailBoxBySId(uint32_t unId);
		void AddConfigMB(CMailBox* pmb);
		void ConnectMailBoxs();
		virtual void OnShoutDownServer();
		void AddRecvMsg(CPluto* u);
		void HandleRecvMsg();

	private:
		int OnReadMessage(struct bufferevent *bev);
		int OnNewFdAccepted(evutil_socket_t fd, const char* pszAddr, uint32_t unPort);
		int OnCloseFd(evutil_socket_t fd);
		int OnConnectServer(CMailBox* pmb);
		bool CheckHeadSize(evutil_socket_t fd, CMailBox* pmb, uint32_t unHeadLen);

		void AddFdToMb(evutil_socket_t fd, CMailBox* pmb);
		void RemoveFd(evutil_socket_t fd);

	private:
		struct event_base* m_pMainBase;
		struct event* m_timer_event;
		struct evconnlistener* m_evconnlistener;

		std::vector<CPluto*>					m_recvMsgs;
		std::vector<CMailBox*>					m_vecConnMbs;			//需要连接
		std::map<evutil_socket_t, CMailBox*>	m_mMbs;					//连接上
		std::map<uint32_t, CMailBox*>			m_mMailBox;					//连接上
	};
};


#endif
