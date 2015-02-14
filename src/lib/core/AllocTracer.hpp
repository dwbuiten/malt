 /*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

#ifndef MATT_ALLOC_TRACER_HPP
#define MATT_ALLOC_TRACER_HPP

/*******************  FUNCTION  *********************/
//standard
#include <cstdio>
#include <cstdlib>
#include <cycle.h>
#include <string>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*********************  TYPES  **********************/
class Stack;
	
/********************  MACROS  **********************/
/**
 * Buffer size to accumulate the element to dump to trace file to not
 * write them one by one.
**/
#define MATT_ALLOC_TRACER_BUFFER_SIZE 4096

/********************  STRUCT  **********************/
/**
 * Define an event to dump in binary format in the trace file.
 * @todo Ideally use uint64_t for sizes to be more strict in the output file, for
 * example to be usable from arch32 when writted on arch64.
 * 
 * @brief Structure dumped to allocation trace files.
**/
struct AllocTracerChunk
{
	/** ID of the call stack, by default we consider its memory address which is uniq. **/
	size_t allocStackId;
	/** ID of the call stack, by default we consider its memory address which is uniq. **/
	size_t freeStackId;
	/** Size of the allocated chunk (requested size) **/
	size_t size;
	/** Timestamp of creation in ticks. **/
	ticks timestamp;
	/** Lifetime of the chunk in ticks. **/
	ticks lifetime;
};

/*********************  CLASS  **********************/
/**
 * @brief Class to manage the generation of allocation trace file.
**/
class AllocTracer
{
	public:
		AllocTracer(const std::string & file = "",size_t bufferSize = MATT_ALLOC_TRACER_BUFFER_SIZE);
		~AllocTracer(void);
		void open(const std::string & file);
		void close(void);
		void traceChunk( size_t allocStackId, size_t freeStackId, size_t size, ticks timestamp, ticks lifetime );
	private:
		void flush(void);
	private:
		/** Descriptor of the current output file. **/
		FILE * fp;
		/** Current position in the buffer, increase on traceChunk(). **/
		size_t pos;
		/** Size of the current buffer. Buffer is flushed when pos==size.**/
		size_t bufferSize;
		/** Pointer to the intenral buffer to accumulate before writing.**/
		AllocTracerChunk * buffer;
};

}

#endif //ATT_CALL_STACK_INFO_HPP
