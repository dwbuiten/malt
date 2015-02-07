/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_MUTEX_PTHREAD_HPP
#define MATT_MUTEX_PTHREAD_HPP

/********************  HEADERS  *********************/
#include "pthread.h"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
struct StaticMutexPthread
{
	void lock(void);
	void unlock(void);
	void destroy(void);
	bool tryLock(void);
	pthread_mutex_t mutex;
};

/*********************  CLASS  **********************/
class MutexPthread
{
	public:
		MutexPthread(void);
		~MutexPthread(void);
		void lock(void);
		void unlock(void);
		bool tryLock(void);
	private:
		pthread_mutex_t mutex;
};

/*******************  FUNCTION  *********************/
inline void StaticMutexPthread::destroy(void )
{
	pthread_mutex_destroy(&mutex);
}

/*******************  FUNCTION  *********************/
inline void StaticMutexPthread::lock(void )
{
	pthread_mutex_lock(&mutex);
}

/*******************  FUNCTION  *********************/
inline void StaticMutexPthread::unlock(void )
{
	pthread_mutex_unlock(&mutex);
}

/*******************  FUNCTION  *********************/
inline bool StaticMutexPthread::tryLock(void)
{
	return pthread_mutex_trylock(&mutex);
}

/*******************  FUNCTION  *********************/
inline MutexPthread::MutexPthread(void )
{
	pthread_mutex_init(&mutex,NULL);
}

/*******************  FUNCTION  *********************/
inline MutexPthread::~MutexPthread(void )
{
	pthread_mutex_destroy(&mutex);
}

/*******************  FUNCTION  *********************/
inline void MutexPthread::lock(void )
{
	pthread_mutex_lock(&mutex);
}

/*******************  FUNCTION  *********************/
inline void MutexPthread::unlock(void )
{
	pthread_mutex_unlock(&mutex);
}

/*******************  FUNCTION  *********************/
inline bool MutexPthread::tryLock(void )
{
	return pthread_mutex_trylock(&mutex);
}

}

#endif //MATT_MUTEX_PTHREAD_HPP
