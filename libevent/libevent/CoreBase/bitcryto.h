#ifndef __BITCRYTO__HEAD__
#define __BITCRYTO__HEAD__

#ifdef _WIN32
#pragma warning (disable:4786)
#pragma warning (disable:4503)
#endif

namespace TMacZone
{

    //一个简单的移位加密算法
    class CBitCryto
    {
        public:
            CBitCryto(const char* szKey, size_t nSize);
            ~CBitCryto();

        public:
            unsigned char Encode(unsigned char c);
            unsigned char Decode(unsigned char c);

        public:
            inline void Reset()
            {
                m_nIdx = 0;
            }

        private:
            unsigned char* m_pszKey;
            size_t m_nKeySize;
            size_t m_nIdx;

    };

};

#endif
