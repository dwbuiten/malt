/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <cstring>
#include <cassert>
#include <common/Debug.hpp>
#include "AllocTracer.hpp"
#include <allocators/NoFreeAllocator.hpp>

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
/**
 * Constructor of the trace file manager. It mostly setup the local parameters
 * and open the trace file.
 * @param file Define the path to the output file to use.
 * @param bufferSize define the size of the buffer to use.
**/
AllocTracer::AllocTracer(const std::string& file,size_t bufferSize)
{
	this->bufferSize = bufferSize;
	this->buffer = new AllocTracerChunk[bufferSize];
	this->pos = 0;
	this->fp = NULL;
	
	if ( ! file.empty() )
		this->open(file);
}

/*******************  FUNCTION  *********************/
/**
 * Destructor of the trace file manager, it mostly close the current trace file
 * and delete the local buffer.
**/
AllocTracer::~AllocTracer(void)
{
	this->close();
	delete [] buffer;
}

/*******************  FUNCTION  *********************/
/**
 * Function used to open and prepare the trace file.
 * @param file Define the file path to use.
**/
void AllocTracer::open(const std::string& file)
{
	this->close();
	
	//open
	this->fp = fopen(file.c_str(),"w");
	
	//check error
	if (this->fp == NULL)
		MATT_ERROR_ARG("Failed to open file %1 : %2\n").arg(file).argStrErrno();
	
	//reset pos in buffer
	this->pos = 0;
}

/*******************  FUNCTION  *********************/
/**
 * Close the trace file but take care of flushing the current buffer before
 * doing it.
**/
void AllocTracer::close(void)
{
	//nothing to do
	if (fp == NULL)
		return;
	
	this->flush();
	fclose(fp);
	this->fp = NULL;
}

/*******************  FUNCTION  *********************/
/**
 * Add a trace event for the given memory chunk. It can generate a write() to the file
 * if the buffer is full.
 * @param allocStack Define the call stack responsible of the allocation of the current chunk.
 * @param freeStack Define the call stack responsible of the deallocation of the current chunk.
 * @param size Define the requested size of the chuink at allocation time.
 * @param timestamp Define the allocation timestamp.
 * @param lifetime Define the lifetime of the current chunk.
**/
void AllocTracer::traceChunk( size_t allocStackId, size_t freeStackId, size_t size, ticks timestamp, ticks lifetime )
{
	//check errors
	assert(pos < bufferSize);
	
	//write value
	AllocTracerChunk & entry = buffer[pos];
	entry.allocStackId = allocStackId;
	entry.freeStackId = freeStackId;
	entry.lifetime = lifetime;
	entry.size = size;
	entry.timestamp = timestamp;
	
	//inc pos
	this->pos++;

	//need flush
	if (pos == bufferSize)
		this->flush();
}

/*******************  FUNCTION  *********************/
/**
 * Function responsible to flush the buffer content to the file and reset
 * the internal counter.
**/
void AllocTracer::flush(void)
{
	//check
	assert(pos <= bufferSize);
	
	//no file
	if (this->fp == NULL)
		return;
	
	//write
	size_t size = fwrite(buffer,sizeof(buffer[0]),pos,fp);
	assumeArg(size == pos,"Failed to write all datas with fwrite, check for interuption, need a loop here for some thread context. %1 != %2").arg(size).arg(pos).end();
	
	//reset pos
	this->pos = 0;
}

}
