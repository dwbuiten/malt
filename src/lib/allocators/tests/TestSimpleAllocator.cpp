/*****************************************************
             PROJECT  : MATT
             VERSION  : 0.1.0-dev
             DATE     : 02/2015
             AUTHOR   : Valat Sébastien
             LICENSE  : CeCILL-C
*****************************************************/

/********************  HEADERS  *********************/
//gtest
#include <gtest/gtest.h>
//current
#include <allocators/SimpleAllocator.hpp>

/***************** USING NAMESPACE ******************/
using namespace MATT;

/********************** CONSTS **********************/
const char CST_VALUE_1[] = "=============================================================== MATT MEMORY =====================================================================\n\
Internal memory : allocated = 256.0 Ko , unused = 256.0 Ko\n\
=================================================================================================================================================\n";

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,constructor)
{
	SimpleAllocator alloc;
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc1)
{
	SimpleAllocator alloc;
	void * ptr = alloc.malloc(16);
	EXPECT_NE((void*)NULL,ptr);
	alloc.free(ptr);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc2)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(16);
	alloc.free(ptr1);
	void * ptr2 = alloc.malloc(16);
	EXPECT_EQ(ptr1,ptr2);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc3)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(16);
	void * ptr2 = alloc.malloc(16);
	void * ptr3 = alloc.malloc(16);
	EXPECT_NE(ptr1,ptr2);
	EXPECT_NE(ptr1,ptr3);
	EXPECT_NE(ptr2,ptr3);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc4)
{
	SimpleAllocator alloc;
	void * ptr[2048];
	for (int i = 0 ; i < 2048 ; i++)
	{
		ptr[i] = alloc.malloc(64);
		memset(ptr[i],0,64);
	}
	for (int i = 0 ; i < 2048 ; i++)
		alloc.free(ptr[i]);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,alloc5)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(16);
	alloc.free(ptr1);
	void * ptr2 = alloc.malloc(64);
	EXPECT_NE(ptr1,ptr2);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,mallocSeq1)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(4*sizeof(int));
	memset(ptr1,0,4*sizeof(int));
	void * ptr2 = alloc.malloc(8*sizeof(int));
	memset(ptr2,0,8*sizeof(int));
	void * ptr3 = alloc.malloc(16*sizeof(int));
	memset(ptr3,0,16*sizeof(int));
	void * ptr4 = alloc.malloc(32*sizeof(int));
	memset(ptr4,0,32*sizeof(int));
	void * ptr5 = alloc.malloc(48*sizeof(int));
	memset(ptr5,0,48*sizeof(int));
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,reallocSeq1)
{
	SimpleAllocator alloc;
	void * ptr1 = alloc.malloc(4*sizeof(int));
	memset(ptr1,0,4*sizeof(int));
	void * ptr2 = alloc.realloc(ptr1,8*sizeof(int));
	memset(ptr2,0,8*sizeof(int));
	void * ptr3 = alloc.realloc(ptr2,16*sizeof(int));
	memset(ptr3,0,16*sizeof(int));
	void * ptr4 = alloc.realloc(ptr3,32*sizeof(int));
	memset(ptr4,0,32*sizeof(int));
	void * ptr5 = alloc.realloc(ptr4,48*sizeof(int));
	memset(ptr5,0,48*sizeof(int));
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,refillNonMultiple)
{
	//to check move of non used to small segment from cur => freelist
	SimpleAllocator alloc;
	for (int i = 0 ; i < 1000 ; i++)
		alloc.malloc(560);
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,getTotalAndInUseAndUnusedMemory)
{
	SimpleAllocator alloc;
	EXPECT_EQ(0,alloc.getTotalMemory());
	EXPECT_EQ(0,alloc.getUnusedMemory());
	EXPECT_EQ(0,alloc.getInuseMemory());
	alloc.malloc(32);
	#ifdef NDEBUG
		int extra = 0;
	#else
		int extra = sizeof(size_t);
	#endif
	EXPECT_EQ(MATT_ALLOC_SYS_REQ_SIZE,alloc.getTotalMemory());
	EXPECT_EQ(MATT_ALLOC_SYS_REQ_SIZE-32-sizeof(Chunk)-extra,alloc.getUnusedMemory());
	EXPECT_EQ(32+sizeof(Chunk)+extra,alloc.getInuseMemory());
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,getMaxSize)
{
	SimpleAllocator alloc;
	EXPECT_EQ(MATT_ALLOC_SYS_REQ_SIZE,alloc.getMaxSize());
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,printStat)
{
	SimpleAllocator alloc;
	alloc.malloc(32);
	
	std::stringstream out;
	alloc.printState(out);
	
	EXPECT_EQ(CST_VALUE_1,out.str());
}

/*******************  FUNCTION  *********************/
TEST(SimpleAllocator,tooLarge)
{
	SimpleAllocator alloc;
	void * ptr = alloc.malloc(MATT_ALLOC_SYS_REQ_SIZE * 8);
	memset(ptr,0,MATT_ALLOC_SYS_REQ_SIZE * 8);
}
