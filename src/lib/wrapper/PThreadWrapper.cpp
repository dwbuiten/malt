/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//standard
#include "PThreadWrapper.hpp"
#include <cstdio>
#include <cassert>
//libc POSIX.1, here we use GNU specific RTLD_NEXT (need _GNU_SOURCE)
#include <dlfcn.h>

namespace MATT
{

/********************* GLOBALS **********************/
/**
 * Static instance of thread tracker data.
**/
static PThreadWrapperData gblPThreadWrapperData = {1,1,NULL,PTHREAD_MUTEX_INITIALIZER};

/*******************  FUNCTION  *********************/
/**
 * Function called on thread exit via pthread_key system.
 * Argument is not used, only defined for signature compatibility.
**/
void pthreadWrapperOnExit(void *)
{
	pthread_mutex_lock(&(gblPThreadWrapperData.lock));
	//fprintf(stderr,"Destroy thread : %d / %d !\n",gblPThreadWrapperData.threadCount,gblPThreadWrapperData.maxThreadCount);
	gblPThreadWrapperData.threadCount--;
	assert(gblPThreadWrapperData.threadCount >= 1);
	pthread_mutex_unlock(&(gblPThreadWrapperData.lock));
}

/*******************  FUNCTION  *********************/
/**
 * Function to be used as wrapper in pthread_create to capture the thread init, update
 * counters and call the real user function.
 * @param arg Muse get a pointer to newly allocated PThreadWrapperArg structure.
**/
void * pthreadWrapperStartRoutine(void * arg)
{
	//update counters
	pthread_mutex_lock(&(gblPThreadWrapperData.lock));
	gblPThreadWrapperData.threadCount++;
	if (gblPThreadWrapperData.threadCount > gblPThreadWrapperData.maxThreadCount)
		gblPThreadWrapperData.maxThreadCount = gblPThreadWrapperData.threadCount;
	pthread_mutex_unlock(&(gblPThreadWrapperData.lock));

	//setup the key to get destructor call on thread exit (capture function finish or pthread_exit)
	pthread_setspecific(gblPThreadWrapperData.key, (void*)0x1);
	
	//fprintf(stderr,"Create thread : %d / %d !\n",gblPThreadWrapperData.threadCount,gblPThreadWrapperData.maxThreadCount);
	
	//run child
	PThreadWrapperArg * subarg = (PThreadWrapperArg *)arg;
	void * res = subarg->routine(subarg->arg);
	
	//delete
	delete subarg;
	
	//return
	return res;
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the maximum number of alive thread during program exacution.
**/
int PThreadWrapper::getMaxThreadCount(void)
{
	return gblPThreadWrapperData.maxThreadCount;
}

/*******************  FUNCTION  *********************/
/**
 * @return Return the current alive thread count.
**/
int PThreadWrapper::getThreadCount(void)
{
	return gblPThreadWrapperData.threadCount;
}

}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of thread_create method.
**/
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg)
{
	//init
	if (MATT::gblPThreadWrapperData.pthread_create == NULL)
	{
		 MATT::gblPThreadWrapperData.pthread_create = (MATT::PthreadCreateFuncPtr)dlsym(RTLD_NEXT,"pthread_create");
		 pthread_key_create(&MATT::gblPThreadWrapperData.key,MATT::pthreadWrapperOnExit);
	}
	
	//prepare args
	MATT::PThreadWrapperArg * subarg = new MATT::PThreadWrapperArg;
	subarg->arg = arg;
	subarg->routine = start_routine;

	//call
	return MATT::gblPThreadWrapperData.pthread_create(thread,attr,MATT::pthreadWrapperStartRoutine,subarg);
}

