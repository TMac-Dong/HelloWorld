#ifndef MY_LOCK_H_
#define MY_LOCK_H_
#include "pthread.h"

class MyLock
{
	pthread_mutex_t m_Mutex;
public:
	MyLock()
	{
		pthread_mutex_init(&m_Mutex, NULL);
	}
	~MyLock()
	{
		pthread_mutex_destroy(&m_Mutex);
	}

	void Lock()
	{
		pthread_mutex_lock(&m_Mutex);
	}
	void UnLock() 
	{
		pthread_mutex_unlock(&m_Mutex);
	}
};
#endif
