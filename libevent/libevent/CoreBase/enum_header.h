#ifndef ENUM_HEADER_H_
#define ENUM_HEADER_H_

enum MAILBOX_CLIENT_TYPE
{
	MAILBOX_CLIENT_UNAUTHZ = 0,         //�����ڿͻ��˵�����,δ��֤
	MAILBOX_CLIENT_AUTHZ = 1,           //�����ڿͻ��˵�����,����֤
	MAILBOX_CLIENT_TRUSTED = 0xf,       //�����ڷ������˵Ŀ���������
};

enum ENUM_SERVER_TYPE
{
	SERVER_TYPE_INVALIAD = -1,
	SERVER_TYPE_GS = 0,
	SERVER_TYPE_DB = 1,
};

#endif