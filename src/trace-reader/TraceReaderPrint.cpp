/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//STD
#include <cstdio>
//matt internals
#include "TraceReaderPrint.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
TraceReaderPrint::TraceReaderPrint(Filter * filter): TraceReader(filter)
{
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onStart(void)
{
	//open json array
	printf("/* [  [ 'STACK_ID', CHUNK_SIZE , CHUNK_TIMESTAMP , CHUNK_LIFETIME ] , ....   ] */\n\n");
	printf("[\n\t");
	this->first = true;
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onEnd(void)
{
	//closejson array
	printf("\n]\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderPrint::onData(MATT::AllocTracerChunk& chunk)
{
	if (!first)
		printf(",\n\t");
	//printf("{\n\t\tallocStack: %p\n\t\tsize: %lu\n\t\ttimestamp: %llu\n\t\tlifetime: %llu\n\t}",buffer[i].allocStack,buffer[i].size,buffer[i].timestamp,buffer[i].lifetime);
	printf("['%lu',%lu,%llu,%llu]",chunk.allocStackId,chunk.size,chunk.timestamp,chunk.lifetime);
	first = false;
}

}
