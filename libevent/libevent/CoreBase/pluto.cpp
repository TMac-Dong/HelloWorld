#include "Pluto.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "msg_id.h"
#include "bitcryto.h"
#include <stdlib.h>
#include <algorithm>
#include "memory_pool.h"
#pragma warning(disable:4996)
#include "my_stl.h"

namespace TMacZone
{

	using namespace std;
	static const unsigned char sg_mycryto_key[] = { 15, 180, 213, 37, 40, 98, 85, 7, 152, 223, 48, 168, 71, 102, 191, 194 };
	CBitCryto sg_mycryto((char*)sg_mycryto_key, sizeof(sg_mycryto_key)); //客户端服务器交互包加解密类
	CMemoryPool* CPluto::memPool = NULL;
	MyLock CPluto::m_mLock;

	CPluto::CPluto(uint32_t unBufferSize)
		: m_szBuffer(NULL)
		, m_unBufferSize(unBufferSize)
		, m_unLen(0)
		, m_unMaxLen(0)
		, m_unDecodeErrIdx(0)
		, m_bEncodeErr(false)
		
	{
		m_szBuffer = new char[unBufferSize];
		m_unBufferSize = unBufferSize;
	}


	CPluto::~CPluto()
	{
		SAFE_DELETE(m_szBuffer);
	}

#ifdef PLUTO_POOL_
	void* CPluto::operator new(size_t size)
	{
		m_mLock.Lock();
		if (memPool == NULL)
		{
			expandMemoryPool();
		}

		TMacZone::CMemoryPool* head = memPool;
		memPool = head->m_pNext;
		m_mLock.UnLock();
		return head;
	}

	void CPluto::operator delete(void* p, size_t size)
	{
		m_mLock.Lock();
		TMacZone::CMemoryPool* head = (TMacZone::CMemoryPool*)p;
		head->m_pNext = head;
		memPool = head;
		m_mLock.UnLock();
	}
#endif

	pluto_msgid_t CPluto::GetMsgId()
	{
		return sz_to_msgid<pluto_msgid_t>((unsigned char*)(m_szBuffer + MSGLEN_HEAD + MSGLEN_RESERVED));
	}

	CPluto& CPluto::Encode(pluto_msgid_t msg_id)
	{
		m_unLen = MSGLEN_HEAD + MSGLEN_RESERVED;
		(*this) << msg_id;

		return *this;
	}

	CPluto& CPluto::operator<< (uint8_t unData)
	{
		Resize(sizeof(unData));

		uint8_to_sz(unData, m_szBuffer + m_unLen);
		m_unLen += sizeof(unData);

		return *this;
	}

	CPluto& CPluto::operator<< (uint16_t unData)
	{
		Resize(sizeof(unData));

		uint16_to_sz(unData, m_szBuffer + m_unLen);
		m_unLen += sizeof(unData);

		return *this;
	}

	CPluto& CPluto::operator << (uint32_t unData)
	{
		Resize(sizeof(unData));

		uint32_to_sz(unData, m_szBuffer + m_unLen);
		m_unLen += sizeof(unData);

		return *this;
	}

	CPluto& CPluto::operator << (uint64_t unData)
	{

		Resize(sizeof(unData));

		uint64_to_sz(unData, m_szBuffer + m_unLen);
		m_unLen += sizeof(unData);

		return *this;
	}

	CPluto& CPluto::operator << (int8_t unData)
	{
		uint8_t n2 = (uint8_t)unData;
		(*this) << n2;
		return *this;
	}

	CPluto& CPluto::operator << (int16_t unData)
	{
		uint16_t n2 = (uint16_t)unData;
		(*this) << n2;
		return *this;
	}

	CPluto& CPluto::operator << (int32_t unData)
	{
		uint32_t n2 = (uint32_t)unData;
		(*this) << n2;
		return *this;
	}

	CPluto& CPluto::operator << (int64_t unData)
	{
		uint64_t n2 = (uint64_t)unData;
		(*this) << n2;
		return *this;
	}

	CPluto& CPluto::operator << (float32_t unData)
	{
		Resize(sizeof(unData));

		float32_to_sz(unData, m_szBuffer + m_unLen);
		m_unLen += sizeof(unData);
		return *this;
	}

	CPluto& CPluto::operator << (float64_t unData)
	{
		Resize(sizeof(unData));

		float64_to_sz(unData, m_szBuffer + m_unLen);
		m_unLen += sizeof(unData);


		return *this;
	}

	CPluto& CPluto::operator<<(bool bData)
	{
		uint8_t unVal = bData ? 1 : 0;
		Resize(sizeof(unVal));

		uint8_to_sz(unVal, m_szBuffer + m_unLen);
		m_unLen += sizeof(unVal);

		return *this;
	}

	CPluto& CPluto::operator<<(std::string str)
	{
		enum{ MAX_LEN = 65534, PRINT_LEN = 48, };
		Resize(sizeof(str));

		size_t size = str.size();
		uint16_t unSize = (uint16_t)size;
		if (size > MAX_LEN)
		{
			SetEncodeErr();
			PrintHex(m_szBuffer + m_unLen, PRINT_LEN);
			return *this;
		}

		*this << unSize;
		memcpy(m_szBuffer + m_unLen, str.c_str(), str.size());
		m_unLen += size;

		return *this;
	}

	CPluto& EndPluto(CPluto& u)
	{
		uint32_to_sz(u.GetLen(), u.GetRecvBuffer());
		char* str = u.GetRecvBuffer();
		str[MSGLEN_HEAD] = '\0';
		str[MSGLEN_HEAD + 1] = '\0';
		u.SetMaxLen(u.GetLen());

		if (u.GetMsgId() < MAX_CLIENT_SERVER_MSGID)
		{
			//客户端包需要加密
			sg_mycryto.Reset();
			for (uint32_t i = MSGLEN_TEXT_POS; i < u.GetLen(); ++i)
			{
				str[i] = sg_mycryto.Encode(str[i]);
			}
		}

		return u;
	}

	CPluto& CPluto::operator >> (uint8_t& unData)
	{
		uint32_t unNewLen = m_unLen + sizeof(unData);
		if (unNewLen > m_unMaxLen)
		{
			m_unDecodeErrIdx = m_unLen;
		}
		else
		{
			unData = sz_to_uint8((unsigned char*)m_szBuffer + m_unLen);
			m_unLen = unNewLen;
		}
		return *this;
	}

	CPluto& CPluto::operator >> (uint16_t& unData)
	{
		uint32_t unNewLen = m_unLen + sizeof(unData);
		if (unNewLen > m_unMaxLen)
		{
			m_unDecodeErrIdx = m_unLen;
		}
		else
		{
			unData = sz_to_uint16((unsigned char*)m_szBuffer + m_unLen);
			m_unLen = unNewLen;
		}
		return *this;
	}

	CPluto& CPluto::operator >> (uint32_t& unData)
	{
		uint32_t unNewLen = m_unLen + sizeof(unData);
		if (unNewLen > m_unMaxLen)
		{
			m_unDecodeErrIdx = m_unLen;
		}
		else
		{
			unData = sz_to_uint32((unsigned char*)m_szBuffer + m_unLen);
			m_unLen = unNewLen;
		}
		return *this;
	}

	CPluto& CPluto::operator >> (uint64_t& unData)
	{
		uint32_t unNewLen = m_unLen + sizeof(unData);
		if (unNewLen > m_unMaxLen)
		{
			m_unDecodeErrIdx = m_unLen;
		}
		else
		{
			unData = sz_to_uint64((unsigned char*)m_szBuffer + m_unLen);
			m_unLen = unNewLen;
		}
		return *this;
	}

	CPluto& CPluto::operator >> (int8_t& unData)
	{
		uint8_t n2;
		(*this) >> n2;
		unData = (int8_t)n2;
		return *this;
	}

	CPluto& CPluto::operator >> (int16_t& unData)
	{
		uint16_t n2;
		(*this) >> n2;
		unData = (int16_t)n2;
		return *this;
	}

	CPluto& CPluto::operator >> (int32_t& unData)
	{
		uint32_t n2;
		(*this) >> n2;
		unData = (int32_t)n2;
		return *this;
	}

	CPluto& CPluto::operator >> (int64_t& unData)
	{
		uint64_t n2;
		(*this) >> n2;
		unData = (int64_t)n2;
		return *this;
	}

	CPluto& CPluto::operator >> (float32_t& unData)
	{
		uint32_t unNewLen = m_unLen + sizeof(unData);
		if (unNewLen > m_unMaxLen)
		{
			m_unDecodeErrIdx = m_unLen;
		}
		else
		{
			unData = sz_to_float32((unsigned char*)m_szBuffer + m_unLen);
			m_unLen = unNewLen;
		}
		return *this;
	}

	CPluto& CPluto::operator >> (float64_t& unData)
	{
		uint32_t unNewLen = m_unLen + sizeof(unData);
		if (unNewLen > m_unMaxLen)
		{
			m_unDecodeErrIdx = m_unLen;
		}
		else
		{
			unData = sz_to_float64((unsigned char*)m_szBuffer + m_unLen);
			m_unLen = unNewLen;
		}
		return *this;
	}

	CPluto& CPluto::operator >> (bool& bData)
	{
		uint8_t unTmp;
		uint32_t unNewLen = m_unLen + sizeof(unTmp);
		if (unNewLen > m_unMaxLen)
		{
			m_unDecodeErrIdx = m_unLen;
		}
		else
		{
			unTmp = sz_to_uint8((unsigned char*)m_szBuffer + m_unLen);
			bData = (unTmp == 1) ? true : false;
			m_unLen = unNewLen;
		}
		return *this;
	}

	CPluto& CPluto::operator >> (std::string& str)
	{
		if (GetDecodeErrIdx() == 0)
		{
			uint16_t unSize;
			*this >> unSize;

			if (unSize + m_unLen > m_unMaxLen)
			{
				m_unDecodeErrIdx = m_unLen;
			}
			else
			{
				str.assign(m_szBuffer + m_unLen, unSize);
				m_unLen += unSize;
			}
		}

		return *this;
	}

	CPluto& CPluto::endPluto()
	{
		return EndPluto(*this);
	}

	void CPluto::Resize(uint32_t n)
	{
		if (m_unLen + n <= m_unBufferSize)
			return;

		//buff大小不足需要扩展

		enum{ resize_times = 2 }; //需要扩展buff时的倍数
		uint32_t unOldLen = m_unLen;
		uint32_t unNewLen = (m_unLen + n) * resize_times;
		//X86系统中一个内存页是0x1000
		enum { MIDDLE_SIZE = 4096, HIGH_SIZE = MIDDLE_SIZE * 16 };
		if (unNewLen < MIDDLE_SIZE)
		{
			m_unBufferSize = MIDDLE_SIZE;
		}
		else if (unNewLen > MIDDLE_SIZE)
		{
			m_unBufferSize = HIGH_SIZE;
		}
		else
		{
			m_unBufferSize = unNewLen;
		}

		char* pszBuff = new char[m_unBufferSize];
		memcpy(pszBuff, m_szBuffer, m_unLen);
		delete[] m_szBuffer;

		m_szBuffer = pszBuff;
	}

	void CPluto::expandMemoryPool()
	{
		size_t size = sizeof(TMacZone::CMemoryPool*) > sizeof(CPluto) ? sizeof(TMacZone::CMemoryPool*) : sizeof(CPluto);
		TMacZone::CMemoryPool* runner = (TMacZone::CMemoryPool*)new char[size];

		memPool = runner;
		enum { EXPAND_SIZE = 32, };
		for (int index = 0; index < EXPAND_SIZE; ++index)
		{
			runner->m_pNext = (TMacZone::CMemoryPool*)new char[size];
			runner = runner->m_pNext;
		}

		runner->m_pNext = NULL;
		
	}

#if (REVERSE_CONVERT == 1)

	void uint8_to_sz(uint8_t n, char* s)
	{
		s[0] = n;
	}

	void uint16_to_sz(uint16_t n, char* s)
	{
		s[0] = (n >> 8) & 0xff;
		s[1] = n & 0xff;
	}

	void uint32_to_sz(uint32_t n, char* s)
	{
		s[0] = (n >> 24) & 0xff;
		s[1] = (n >> 16) & 0xff;
		s[2] = (n >> 8) & 0xff;
		s[3] = n & 0xff;
	}

	void uint64_to_sz(uint64_t n, char* s)
	{
		s[0] = (n >> 56) & 0xff;
		s[1] = (n >> 48) & 0xff;
		s[2] = (n >> 40) & 0xff;
		s[3] = (n >> 32) & 0xff;
		s[4] = (n >> 24) & 0xff;
		s[5] = (n >> 16) & 0xff;
		s[6] = (n >> 8) & 0xff;
		s[7] = n & 0xff;
	}

	void float32_to_sz(float32_t n, char* s)
	{
		uint32_to_sz(*(uint32_t*)(void*)&n, s)
	}

	void float64_to_sz(float64_t n, char* s)
	{
		uint64_to_sz(*(uint64_t*)(void*)&n, s)
	}

	uint8_t sz_to_uint8(unsigned char* s)
	{
		return s[0];
	}

	uint16_t sz_to_uint16(unsigned char* s)
	{
		return (s[0] << 8) + s[1];
	}

	uint32_t sz_to_uint32(unsigned char* s)
	{
		return (s[0] << 24) + (s[1] << 16) + (s[2] << 8) + s[3];
	}

	uint64_t sz_to_uint64(unsigned char* s)
	{
		return (s[0] << 56) + (s[1] << 48) + (s[2] << 40) + (s[3] << 32) \
			+ (s[4] << 24) + (s[5] << 16) + (s[6] << 8) + s[7];
	}

	float32_t sz_to_float32(unsigned char* s)
	{
		uint32_t n = sz_to_uint32(s);
		return *(float32_t*)(void*)&n;
	}

	float64_t sz_to_float64(unsigned char* s)
	{
		uint64_t n = sz_to_uint64(s);
		return *(float64_t*)(void*)&n;
	}

#else

	void uint8_to_sz(uint8_t n, char* s)
	{
		s[0] = n;
	}

	void uint16_to_sz(uint16_t n, char* s)
	{
		*(uint16_t*)(void*)s = n;
	}

	void uint32_to_sz(uint32_t n, char* s)
	{
		*(uint32_t*)(void*)s = n;
	}

	void uint64_to_sz(uint64_t n, char* s)
	{
		*(uint64_t*)(void*)s = n;
	}

	void float32_to_sz(float32_t n, char* s)
	{
		*(float32_t*)(void*)s = n;
	}

	void float64_to_sz(float64_t n, char* s)
	{
		*(float64_t*)(void*)s = n;
	}

	uint8_t sz_to_uint8(unsigned char* s)
	{
		return s[0];
	}

	uint16_t sz_to_uint16(unsigned char* s)
	{
		return *(uint16_t*)(void*)s;
	}

	uint32_t sz_to_uint32(unsigned char* s)
	{
		return *(uint32_t*)(void*)s;
	}

	uint64_t sz_to_uint64(unsigned char* s)
	{
		return *(uint64_t*)(void*)s;
	}

	float32_t sz_to_float32(unsigned char* s)
	{
		return *(float32_t*)(void*)s;
	}

	float64_t sz_to_float64(unsigned char* s)
	{
		return *(float64_t*)(void*)s;
	}

#endif

	// 将值如0x12的char转换为字符串"12"
	void char_to_sz(unsigned char c, char* s)
	{
		//char tmp[3];
		//tmp[2] = '\0';
		//sprintf(tmp, "%02x", c);
		//s[0] = tmp[0];
		//s[1] = tmp[1];

		const static char char_map[] = "0123456789abcdef";

		unsigned char c1 = (c >> 4) & 0xf;
		unsigned char c2 = c & 0xf;
		s[0] = char_map[c1];
		s[1] = char_map[c2];

	}

	//将形如"12"的字符创转换为值为0x12的char
	unsigned char sz_to_char(char* s)
	{
		unsigned int i;
		sscanf(s, "%02x", &i);
		unsigned char c = (unsigned char)i;
		return c;
	}

	void PrintHex16(const char* s, size_t n)
	{
		char buf[16 * 3 + 3 + 16 + 1 + 1];
		memset(buf, ' ', sizeof(buf)-1);
		buf[sizeof(buf)-1] = '\0';

		for (size_t i = 0; i < n; ++i)
		{
			unsigned char c = s[i];
			char_to_sz(c, buf + i * 3);

			if (isprint(c))
			{
				buf[51 + i] = c;
			}
			else
			{
				buf[51 + i] = '.';
			}
		}

		//g_logger.NewLine() << buf << EndLine;
		printf("%s/n", buf);
	}

	void PrintHex(const char* s, size_t n)
	{
		size_t sixteen = 16;
		size_t count = n / sixteen + 1;

		for (size_t i = 0; i < count; ++i)
		{
			if (i == count - 1)
			{
				PrintHex16(s + i*sixteen, n % sixteen);
			}
			else
			{
				PrintHex16(s + i*sixteen, sixteen);
			}
		}

	}

	void PrintHexPluto(CPluto& c)
	{
		uint32_t n = std::max(c.GetLen(), c.GetMaxLen());
		PrintHex(c.GetBuffer(), n);
	}



};







