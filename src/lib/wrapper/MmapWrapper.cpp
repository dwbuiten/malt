/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cassert>
#include <cycle.h>
#include "MmapWrapper.hpp"
#include <portability/OS.hpp>
#include <common/Debug.hpp>
#include <sys/mman.h>

namespace MATT
{

/********************  MACROS  **********************/
#define MATT_MMAP_WRAPPER_NO_ACTION do{}while(0)
#define MATT_MMAP_WRAPPER(preaction,call,action,ret) \
	/*call hoook*/\
	MATT::MmapHooksInfos infos;\
\
	if (MATT::gblMmapWrapperState.status == MATT::ALLOC_WRAP_NOT_INIT)\
		MATT::mmapWrapperInit();\
\
	/*run the default function*/\
	MATT_ASSERT(MATT::gblMmapWrapperState.status > MATT::ALLOC_WRAP_INIT_SYMBOL);\
\
	bool reenter = MATT::gblReenter;\
	/*enter exit*/\
	if (!reenter && MATT::gblMmapWrapperState.hooks != NULL)\
	{\
		MATT::gblReenter = true;\
		if (MATT::gblMmapWrapperState.enterExit)\
		{\
			infos.retaddr = __builtin_extract_return_addr(__builtin_return_address(0));\
			infos.func = (void*)__func__;\
			MATT::gblMmapWrapperState.hooks->onMmapEnterFunction(infos);\
		}\
		preaction;\
	}\
\
	ticks t = getticks();\
	call;\
	t = getticks() - t;\
	infos.calltime = t;\
\
	/*enter exit*/\
	if (!reenter && MATT::gblMmapWrapperState.hooks != NULL)\
	{\
		action;\
		if (MATT::gblMmapWrapperState.enterExit)\
			MATT::gblMmapWrapperState.hooks->onMmapExitFunction(infos);\
		MATT::gblReenter = false;\
	}\
\
	/*return segment to user*/\
	ret

/********************* GLOBALS **********************/
MmapWrapperState gblMmapWrapperState = {ALLOC_WRAP_NOT_INIT,MATT_STATIC_MUTEX_INIT,NULL};
static __thread bool gblReenter = false;

/*******************  FUNCTION  *********************/
void mmapWrapperInit(void)
{
	//secure in case of first call in threads
	gblMmapWrapperState.lock.lock();

	if (gblMmapWrapperState.status == ALLOC_WRAP_NOT_INIT)
	{
		//update state
		gblMmapWrapperState.status = ALLOC_WRAP_INIT_SYMBOL;
		
		//search addresses
		gblMmapWrapperState.functions.mmap = OS::dlsymNext<MmapFuncPtr>("mmap");
		gblMmapWrapperState.functions.munmap = OS::dlsymNext<MunmapFuncPtr>("munmap");
		gblMmapWrapperState.functions.mremap = OS::dlsymNext<MremapFuncPtr>("mremap");

		//init profiler
		gblMmapWrapperState.status = ALLOC_WRAP_INIT_HOOKS;
		
		//init internal alloc
		gblMmapWrapperState.hooks = mmapHookInit();

		//check mode
		gblMmapWrapperState.enterExit = MATT::gblMmapWrapperState.hooks->mmapCallEnterExit();

		//final state
		gblMmapWrapperState.status = ALLOC_WRAP_READY;
	}

	//secure in case of first call in threads
	gblMmapWrapperState.lock.unlock();
}

}

/*******************  FUNCTION  *********************/
/**
 * Wrapper of the pvalloc function to capture allocations. The original symbol will be
 * search by dlsym() in AllocWrapperGlobal::init() .
**/
void *mmap(void *start, size_t length, int prot,int flags,int fd, off_t offset)
{
	MATT_MMAP_WRAPPER(
		MATT_MMAP_WRAPPER_NO_ACTION,
		void * ret = MATT::gblMmapWrapperState.functions.mmap(start,length,prot,flags,fd,offset),
		MATT::gblMmapWrapperState.hooks->onMmap(infos,ret,start,length,prot,flags,fd,offset),
		return ret;
	);
}