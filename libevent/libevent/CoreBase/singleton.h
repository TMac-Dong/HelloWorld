#ifndef     SINGLETON_H 
#define		SINGLETON_H 

#if			defined(ASSERT)
	#define		SIG_ASSERT ASSERT
#else
	#define		SIG_ASSERT(val)
#endif
#define NULL 0

template <typename T> class CSingleton
{
public:
	typedef T ThisClass;

protected:
	static T* m_pSingleton;

public:
	CSingleton( void )
	{
		SIG_ASSERT( !m_pSingleton );
#if defined( _MSC_VER ) && _MSC_VER < 1200	 
		int offset = (int)(T*)1 - (int)(CSingleton <T>*)(T*)1;
		m_pSingleton = (T*)((int)this + offset);
#else
		m_pSingleton = static_cast< T* >( this );
#endif
	}

	~CSingleton( void )
	{  
		SIG_ASSERT( m_pSingleton ); 
		m_pSingleton = 0;  
	}

	static T& GetSingleton( void )
	{	
		SIG_ASSERT( m_pSingleton ); 
		return ( *m_pSingleton ); 
	}

	static T* GetSingletonPtr( void )
	{ 
		return m_pSingleton;
	}
};

template <typename T >  T* CSingleton<T>::m_pSingleton=NULL ;
#endif 
