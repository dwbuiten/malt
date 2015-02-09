/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//matt config
#include <config.h>
//matt common
#include <common/Debug.hpp>
//libunwind
#ifdef MATT_HAVE_LIBUNWIND
	#include <libunwind.h>
#endif //MATT_HAVE_LIBUNWIND
//local
#include "BacktraceLibunwind.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
#ifdef MATT_HAVE_LIBUNWIND
int BacktraceLibunwind::backtrace(void** buffer, int maxDepth)
{
	unw_cursor_t    cursor;
	unw_context_t   context;

	unw_getcontext(&context);
	unw_init_local(&cursor, &context);

	int depth = 0;
	
	//skip first for compat with glibc backtrace and loop
	while(depth < maxDepth && unw_step(&cursor) > 0) {
		unw_word_t  pc;
		//char        fname[64];

		unw_get_reg(&cursor, UNW_REG_IP, &pc);

		//fname[0] = '\0';
		//(void) unw_get_proc_name(&cursor, fname, sizeof(fname), &offset);
		//printf ("%p : (%s+0x%x) [%p]\n", pc, fname, offset, pc);
		buffer[depth] = (void*)pc;
		depth++;
	};

	//remove 0xfffffffff
	if (depth < maxDepth)
		depth--; 

	return depth;
}
#else //MATT_HAVE_LIBUNWIND
int BacktraceLibunwind::backtrace(void** buffer, int maxDepth)
{
	MATT_FATAL("Libunwind support wasn't compiled for you version of MATT please recompile or use the default glibc backtracing method !");
	return 0;
}
#endif //MATT_HAVE_LIBUNWIND

}
