#ifndef PLUTO_H_
#define PLUTO_H_
#include "util.h"
#include "my_lock.h"

namespace TMacZone
{
	class CMemoryPool;
};

namespace TMacZone
{
	enum
	{
		MSGLEN_HEAD = 4,                                              //消息包头长度
		MSGLEN_RESERVED = 2,                                              //保留2位,可用作版本或其他
		MSGLEN_MSGID = 2,                                              //消息id长度
		MSGLEN_TEXT_POS = MSGLEN_HEAD + MSGLEN_RESERVED + MSGLEN_MSGID,   //正文开始的位置
		MSGLEN_MAX = 65000,                                          //消息包最大长度

		PLUTO_CLIENT_MSGLEN_MAX = 65000,                                          //客户端包的最大长度
		PLUTO_MSGLEN_HEAD = MSGLEN_HEAD,                                    //便于其他模块引用
		PLUTO_FILED_BEGIN_POS = MSGLEN_HEAD + MSGLEN_RESERVED + MSGLEN_MSGID,   //字段开始的位置,此前的位置都是协议自己需要的
	};


	extern bool IsValidMsgid(uint16_t msgid);
	extern void uint8_to_sz(uint8_t n, char* s);
	extern void uint16_to_sz(uint16_t n, char* s);
	extern void uint32_to_sz(uint32_t n, char* s);
	extern void uint64_to_sz(uint64_t n, char* s);
	extern void float32_to_sz(float32_t n, char* s);
	extern void float64_to_sz(float64_t n, char* s);
	extern uint8_t sz_to_uint8(unsigned char* s);
	extern uint16_t sz_to_uint16(unsigned char* s);
	extern uint32_t sz_to_uint32(unsigned char* s);
	extern uint64_t sz_to_uint64(unsigned char* s);
	extern float32_t sz_to_float32(unsigned char* s);
	extern float64_t sz_to_float64(unsigned char* s);
	//将值如0x12的char转换为字符串"12"
	extern void char_to_sz(unsigned char c, char* s);
	//将形如"12"的字符创转换为值为0x12的char
	extern unsigned char sz_to_char(char* s);
	extern void PrintHex16(const char* s, size_t n);
	extern void PrintHex(const char* s, size_t n);
	template<typename T>
	T sz_to_msgid(unsigned char* s);

	template<>
	inline uint16_t sz_to_msgid<uint16_t>(unsigned char* s)
	{
		return sz_to_uint16(s);
	}

	enum {DEFAULT_PLUTO_BUFFER = 256,};
	class CPluto
	{
	public:
		CPluto(uint32_t unBufferSize = DEFAULT_PLUTO_BUFFER);
		~CPluto();

	#ifdef PLUTO_POOL_
		void *operator new(size_t size);
		void operator delete(void* p, size_t size);
	#endif

	public:
	
		inline const char* GetBuffer() const
		{
			return m_szBuffer;
		}

		inline char* GetRecvBuffer()
		{
			return m_szBuffer;
		}

		inline uint32_t GetBufferSize()
		{
			return m_unBufferSize;
		}

		inline void SetBufferSize(uint32_t unBufferSize)
		{
			m_unBufferSize = unBufferSize;
		}

		inline uint32_t GetLen()
		{
			return m_unLen;
		}
	
		inline void SetLen(uint32_t unLen)
		{
			m_unLen = unLen;
		}

		inline uint32_t GetMaxLen()
		{
			return m_unMaxLen;
		}

		inline void SetMaxLen(uint32_t unMaxLen)
		{
			m_unMaxLen = unMaxLen;
		}

		inline void EndRecv(uint32_t unLen)
		{
			m_unLen = unLen;
			m_unMaxLen = unLen;
		}

		inline uint32_t GetDecodeErrIdx()
		{
			return m_unDecodeErrIdx;
		}

		inline void SetDecodeErrIdx(uint32_t unDecodeErrIdx)
		{
			m_unDecodeErrIdx = unDecodeErrIdx;
		}

		inline bool GetIsEncodeErr()
		{
			return m_bEncodeErr;
		}

		inline void SetEncodeErr(bool bError = false)
		{
			m_bEncodeErr = bError;
		}


	public:
		pluto_msgid_t GetMsgId();

		//输入
		CPluto& Encode(pluto_msgid_t msg_id);
		CPluto& operator<<(uint8_t unData);
		CPluto& operator<<(uint16_t unData);
		CPluto& operator<<(uint32_t unData);
		CPluto& operator<<(uint64_t unData);
		CPluto& operator<<(int8_t unData);
		CPluto& operator<<(int16_t unData);
		CPluto& operator<<(int32_t unData);
		CPluto& operator<<(int64_t unData);
		CPluto& operator<<(float32_t unData);
		CPluto& operator<<(float64_t unData);
		CPluto& operator<<(bool bData);
		CPluto& operator<<(std::string str);

		friend CPluto& EndPluto(CPluto& p);
		CPluto& operator>>(uint8_t& unData);
		CPluto& operator>>(uint16_t& unData);
		CPluto& operator>>(uint32_t& unData);
		CPluto& operator>>(uint64_t& unData);
		CPluto& operator>>(int8_t& unData);
		CPluto& operator>>(int16_t& unData);
		CPluto& operator>>(int32_t& unData);
		CPluto& operator>>(int64_t& unData);
		CPluto& operator>>(float32_t& unData);
		CPluto& operator>>(float64_t& unData);
		CPluto& operator>>(bool& bData);
		CPluto& operator>>(std::string& str);

	public:
		CPluto& endPluto();

	private:
		//自动调节buffersize的大小
		void Resize(uint32_t n);

	private:
		char*		m_szBuffer;
		static	TMacZone::CMemoryPool* memPool;
		static  void expandMemoryPool();

		static	MyLock		m_mLock;
		uint32_t	m_unBufferSize;
		uint32_t	m_unLen;			//实际大小
		uint32_t	m_unMaxLen;			//实际最大长度
		uint32_t	m_unDecodeErrIdx;	//解析错误的位置
		bool		m_bEncodeErr;		//编码是否出错

		//限制类外相互赋值，不实现
		CPluto(const CPluto& other);
		CPluto& operator=(const CPluto& other);

	};
}
#endif