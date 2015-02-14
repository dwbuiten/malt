/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//matt internals
#include "TraceReader.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/********************  MACROS  **********************/
#define BUFFER_SIZE 1024

/*******************  FUNCTION  *********************/
Filter::Filter()
{
	this->at = 0;
	this->filterSize = 0;
	this->filterStack = NULL;
}

/*******************  FUNCTION  *********************/
bool Filter::accept(MATT::AllocTracerChunk& chunk)
{
	return (filterSize == 0 || filterSize == chunk.size) 
	    && (filterStack == NULL || filterStack == chunk.allocStackId)
		&& (at == 0 || (chunk.timestamp <= at && chunk.timestamp + chunk.lifetime >= at));
}



/*******************  FUNCTION  *********************/
TraceReader::TraceReader(Filter * filter)
{
	this->filter = filter;
}

/*******************  FUNCTION  *********************/
TraceReader::~TraceReader(void)
{

}

/*******************  FUNCTION  *********************/
bool TraceReader::run(const char* fname)
{
	//error
	if (fname == NULL)
	{
		fprintf(stderr,"Missing arguement, please provide the trace file as parameter.\n");
		return false;
	}
	
	//open the file
	FILE * fp = fopen(fname,"r");
	if (fp == NULL)
	{
		perror(fname);
		return false;
	}
	
	//run
	this->run(fp);
	
	//close
	fclose(fp);
	return true;
}

/*******************  FUNCTION  *********************/
void TraceReader::run(FILE* fp)
{
	//error
	if (fp == NULL)
		return;
	
	//handler
	this->onStart();
	
	//loop
	MATT::AllocTracerChunk buffer[BUFFER_SIZE];
	while (!feof(fp))
	{
		size_t size = fread(buffer,sizeof(buffer[0]),BUFFER_SIZE,fp);
		for (size_t i = 0 ; i < size ; i++)
		{
			bool acc = false;

			//check accept
			if (this->filter == NULL)
				acc = true;
			else
				acc = this->filter->accept(buffer[i]);

			//eat if accepted
			if (acc)
				this->onData(buffer[i]);
		}
	}
	
	//handler
	this->onEnd();
}

}
