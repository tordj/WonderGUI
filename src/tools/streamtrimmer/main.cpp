
#include <cstdlib>
#include <stdio.h>


#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>

#include <wondergfx.h>
#include <wondergfxstream.h>


using namespace wg;
using namespace std;


//____ main() _________________________________________________________________

int main ( int argc, char** argv )
{
	if( argc < 2 )
	{
		cerr << string("ERROR: Input file not specified\n");
		return 1;
	}

	std::ifstream input( argv[1], ifstream::binary );
	if( !input )
	{
		cerr << "ERROR: File not found\n";
		return 2;
	}
	
	input.seekg(0, input.end);
	int length = input.tellg();
	input.seekg(0, input.beg);
		
	std::ofstream trimmed( string("trimmed_") + argv[1], fstream::binary );
	std::ofstream log( string("log_") + argv[1], fstream::binary );
	std::ofstream log_trimmed( string("log_trimmed_") + argv[1], fstream::binary );

	
	GfxBase::init();
	
	auto pStreamReader = GfxStreamReader::create( [&](int amount, void * pDest){
		int toRead = min(length,amount);
		input.read((char *)pDest, toRead);
		length -= toRead;
		return toRead;
	} );

	
	auto pTrimmedLogger = GfxStreamLogger::create(log_trimmed);
	
	auto pTrimmedWriter = GfxStreamWriter::create( [&](int amount, const void * pDest){
		trimmed.write((const char*)pDest, amount);
	} );

	auto pTrimmedSplitter = GfxStreamSplitter::create( {{pTrimmedLogger, pTrimmedLogger->input}, {pTrimmedWriter, pTrimmedWriter->input}});
	
	auto pTrimEncoder = FastGfxStreamEncoder::create( {pTrimmedSplitter, pTrimmedSplitter->input} );
	
	auto pFactory = GfxStreamSurfaceFactory::create(pTrimEncoder);
	auto pDevice = GfxStreamDevice::create(pTrimEncoder);
	
	
	auto pPlayer = GfxStreamPlayer::create(pDevice, pFactory);
	
	auto pStraightLogger = GfxStreamLogger::create( log );

	auto pInputSplitter = GfxStreamSplitter::create( {{pPlayer, pPlayer->input}} );
//	auto pInputSplitter = GfxStreamSplitter::create( {{pStraightLogger, pStraightLogger->input}, {pPlayer, pPlayer->input}} );
	
	auto pStreamPump = GfxStreamPump::create( {pStreamReader, pStreamReader->output}, {pInputSplitter, pInputSplitter->input} );
	
	
	pDevice->defineCanvas(CanvasRef::Canvas_1, {240,240}, 64);
	pDevice->defineCanvas(CanvasRef::Canvas_2, {240,240}, 64);

	pStraightLogger->setDisplayOffset(false);
	pStraightLogger->setDisplayChunkNb(false);
	pTrimmedLogger->setDisplayOffset(false);
	pTrimmedLogger->setDisplayChunkNb(false);

	
	
	pStreamPump->pumpAll();
	
	input.close();
	trimmed.close();
	log.close();
	log_trimmed.close();
	
    return 0;
}




