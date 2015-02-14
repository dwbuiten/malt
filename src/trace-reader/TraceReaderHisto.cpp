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
#include "TraceReaderHisto.hpp"

/*******************  NAMESPACE  ********************/
namespace MATT
{

/*******************  FUNCTION  *********************/
TraceReaderHisto::TraceReaderHisto(HistoCriteria histCrit, Filter * filter): TraceReader(filter)
{
	this->histCrit = histCrit;
}

/*******************  FUNCTION  *********************/
void TraceReaderHisto::onStart(void)
{
	this->histo.clear();
}

/*******************  FUNCTION  *********************/
void TraceReaderHisto::onEnd(void)
{
	//open json array
	printf("{\n");
	
	//print
	for (TraceReaderHistoMap::const_iterator it = histo.begin() ; it != histo.end() ; ++it)
		printf("	%llu:%lu,\n",it->first,it->second);
	
	//closejson array
	printf("}\n");
}

/*******************  FUNCTION  *********************/
void TraceReaderHisto::onData(MATT::AllocTracerChunk& chunk)
{
	ticks indice;

	switch(histCrit)
	{
		case HISTO_SIZE:
			indice = chunk.size;
			break;
		case HISTO_LIFE:
			indice = chunk.lifetime;
			break;
		case HISTO_TIME:
			indice = chunk.timestamp;
			break;
	}
	
	TraceReaderHistoMap::iterator it = histo.find(indice);
	if (it == histo.end())
		histo[indice] = 1;
	else
		it->second++;
}

}
