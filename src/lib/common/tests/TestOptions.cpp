/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 01/2014
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
#include <gtest/gtest.h>
#include "Options.hpp"

/***************** USING NAMESPACE ******************/
using namespace MATT;

/*******************  FUNCTION  *********************/
TEST(TestOptions,constructor)
{
	Options options;
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,save)
{
	Options options;
	options.dumpConfig("test-config-dump.ini");
}

/*******************  FUNCTION  *********************/
TEST(TestOptions,saveAndLoad)
{
	Options options;
	options.dumpConfig("test-config-load.ini");
	
	Options options2;
	options2.loadFromFile("test-config-load.ini");
	
	EXPECT_EQ(options,options2);
}
