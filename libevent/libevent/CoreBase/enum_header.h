#ifndef ENUM_HEADER_H_
#define ENUM_HEADER_H_

enum MAILBOX_CLIENT_TYPE
{
	MAILBOX_CLIENT_UNAUTHZ = 0,         //来自于客户端的连接,未验证
	MAILBOX_CLIENT_AUTHZ = 1,           //来自于客户端的连接,已验证
	MAILBOX_CLIENT_TRUSTED = 0xf,       //来自于服务器端的可信任连接
};

enum ENUM_SERVER_TYPE
{
	SERVER_TYPE_INVALIAD = -1,
	SERVER_TYPE_GS = 0,
	SERVER_TYPE_DB = 1,
};

#endif