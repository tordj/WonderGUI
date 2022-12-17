#include <string.h>

#include "gfxstream.h"

#include <wg_gfxstreamloopwrapper.h>
#include <wg_gfxstreampump.h>
#include <wg_gfxstreamwriter.h>

#include <wg_string.h>

using namespace wg;

GfxStreamTest::GfxStreamTest()
{

	ADD_TEST(streamLoopWrapperTest);

}

GfxStreamTest::~GfxStreamTest()
{
}


bool GfxStreamTest::init(std::ostream& output)
{
	return true;
}


bool GfxStreamTest::streamLoopWrapperTest(std::ostream& output)
{
	Blob_p pBlob = loadBlob("resources/teststream.wax");

	char * pOutputBuffer = new char[pBlob->size()+10000];			// Some bytes margin, just in case.
	char * pOutputWrite = pOutputBuffer;
	
	
	
	const int loopBufferSize = GfxStream::c_maxBlockSize+2;
	
	char loopBuffer[loopBufferSize];
	
	char * pBufferWrite = loopBuffer;
	char * pBufferRead = loopBuffer;
		
	auto pStreamLoopWrapper = GfxStreamLoopWrapper::create(loopBuffer, loopBuffer+loopBufferSize,
														   [&pBufferWrite] () { return pBufferWrite; },
														   [&pBufferRead] (const void * pReadPos) { pBufferRead = (char *) pReadPos; } );

	auto pStreamWriter = GfxStreamWriter::create([&pOutputWrite](int nBytes, const void * pBytes) {
		memcpy( pOutputWrite, pBytes, nBytes), pOutputWrite += nBytes;
		
	} );
	
	auto pStreamPump = GfxStreamPump::create( {pStreamLoopWrapper, pStreamLoopWrapper->output}, {pStreamWriter, pStreamWriter->input} );
	
	
	char * pBlobRead = (char *) pBlob->begin();
	char * pBlobEnd = (char *) pBlob->end();
	
	while( pBlobRead < pBlobEnd )
	{

		int leftToRead = pBlobEnd - pBlobRead;

		if( pBufferRead <= pBufferWrite )
		{
			int endSpace = loopBuffer + loopBufferSize - pBufferWrite;
			if( pBufferRead < loopBuffer + 2 )
				endSpace -= 2;
			
			if( endSpace > 0 )
			{
				int amount = std::min( endSpace, leftToRead );
				memcpy( pBufferWrite, pBlobRead, amount );
				pBufferWrite += amount;
				pBlobRead += amount;
				leftToRead -= amount;
				
				if( pBufferWrite == loopBuffer + loopBufferSize )
					pBufferWrite = loopBuffer;
			}
		}
		

		if( pBufferRead > pBufferWrite )
		{
			int amount = std::min(int(pBufferRead - pBufferWrite - 2), leftToRead);
			if( amount > 0 )
			{
				memcpy( pBufferWrite, pBlobRead, amount );
				pBufferWrite += amount;
				pBlobRead += amount;
				leftToRead -= amount;
			}
		}

		
/*
		* pBufferWrite++ = * pBlobRead++;
		if( pBufferWrite == loopBuffer + loopBufferSize )
			pBufferWrite = loopBuffer;
*/

		pStreamPump->pumpAll();
	}

	
	int newSize = pOutputWrite - pOutputBuffer;
	
	TEST_ASSERT( newSize == pBlob->size() );
	
	char * pOrg = (char *) pBlob->data();
	char * pCopy = (char *) pOutputBuffer;
	
	for( int i = 0 ; i < pBlob->size() ; i++ )
	{
		TEST_ASSERT( * pCopy++ == * pOrg++ );
	}

	return true;
}

