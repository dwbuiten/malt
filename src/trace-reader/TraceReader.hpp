/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_TRACE_READER_HPP
#define MATT_TRACE_READER_HPP

/********************  HEADERS  *********************/
//matt internals
#include <core/AllocTracer.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  STRUCT  **********************/
struct Filter
{
	Filter();
	bool accept (MATT::AllocTracerChunk & chunk);
	size_t filterStack;
	size_t filterSize;
	ticks at;
};

/*********************  CLASS  **********************/
class TraceReader
{
	public:
		TraceReader(Filter * filter = NULL);
		virtual ~TraceReader(void);
		bool run(const char * filepath);
		void run(FILE * fp);
	protected:
		virtual void onStart(void) {};
		virtual void onData(MATT::AllocTracerChunk & chunk){};
		virtual void onEnd(void) {};
		Filter * filter;
};

}

#endif //MATT_TRACE_READER_HPP
