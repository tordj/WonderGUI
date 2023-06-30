/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_gfxstreamedgemap.h>
#include <wg_gfxbase.h>

namespace wg
{

const TypeInfo GfxStreamEdgemap::TYPEINFO = { "GfxStreamEdgemap", &Edgemap::TYPEINFO };


//____ create() ______________________________________________________________

GfxStreamEdgemap_p	GfxStreamEdgemap::create( GfxStreamEncoder * pEncoder, const Blueprint& blueprint )
{
	if( !pEncoder )
		return nullptr;
	
	if( !_validateBlueprint(blueprint) )
		return nullptr;
	
	auto p = GfxStreamEdgemap_p( new GfxStreamEdgemap(pEncoder, blueprint) );
	pEncoder->flush();
	return p;
}

GfxStreamEdgemap_p GfxStreamEdgemap::create( GfxStreamEncoder * pEncoder, const Edgemap::Blueprint& blueprint, WaveOrigo origo, 
												const float * pSamples, int edges, int edgePitch, int samplePitch)
{
	if( !pEncoder )
		return nullptr;

	if( !_validateBlueprint(blueprint) )
		return nullptr;

	if(edges >= blueprint.segments )
		return nullptr;

	auto p = GfxStreamEdgemap_p( new GfxStreamEdgemap(pEncoder, blueprint) );

	p->_importSamples(origo, pSamples, 0, edges, 0, p->m_size.w+1, edgePitch, samplePitch);
	p->m_nbRenderSegments = edges+1;
	pEncoder->flush();
	return p;
}

GfxStreamEdgemap_p GfxStreamEdgemap::create( GfxStreamEncoder * pEncoder, const Edgemap::Blueprint& blueprint, WaveOrigo origo, 
												const spx * pSamples, int edges, int edgePitch, int samplePitch)
{
	if( !pEncoder )
		return nullptr;

	if( !_validateBlueprint(blueprint) )
		return nullptr;

	if(edges >= blueprint.segments )
		return nullptr;
	
	auto p = GfxStreamEdgemap_p( new GfxStreamEdgemap(pEncoder, blueprint) );

	p->_importSamples(origo, pSamples, 0, edges, 0, p->m_size.w+1, edgePitch, samplePitch);
	p->m_nbRenderSegments = edges+1;
	pEncoder->flush();
	return p;
}

//____ constructor ___________________________________________________________

GfxStreamEdgemap::GfxStreamEdgemap( GfxStreamEncoder * pEncoder, const Blueprint& bp) : Edgemap(bp)
{
	m_pEncoder = pEncoder;
	m_inStreamId = pEncoder->allocObjectId();

	// Setup buffers
	
	int sampleArraySize = (bp.size.w+1) * bp.segments * sizeof(spx);
	int colorArraySize = bp.colors ? bp.segments*sizeof(HiColor) : 0;
	int gradientArraySize = bp.gradients ? bp.segments*sizeof(Gradient) : 0;

	int bytes = sampleArraySize + colorArraySize + gradientArraySize;
	
	m_pBuffer = new char[bytes];
			
	auto pBuffer = m_pBuffer;
	
	if( gradientArraySize > 0 )
	{
		m_pGradients = (Gradient*) pBuffer;
		memcpy(m_pGradients, bp.gradients, gradientArraySize);
		pBuffer += gradientArraySize;
	}

	if( colorArraySize > 0 )
	{
		m_pColors = (HiColor*) pBuffer;
		memcpy(m_pColors, bp.colors, colorArraySize);
		pBuffer += colorArraySize;
	}
	
	m_pSamples = (spx*) pBuffer;
	
	_sendCreateEdgemap(m_pEncoder);
}

//____ destructor ____________________________________________________________

GfxStreamEdgemap::~GfxStreamEdgemap()
{
	delete [] m_pBuffer;

	* m_pEncoder << GfxStream::Header{ GfxChunkId::DeleteEdgemap, GfxStream::SpxFormat::Int32_dec, 2 };
	* m_pEncoder << m_inStreamId;
}

//____ typeInfo() ____________________________________________________________

const TypeInfo& GfxStreamEdgemap::typeInfo(void) const
{
	return TYPEINFO;
}

//____ setRenderSegments() ____________________________________________________

bool GfxStreamEdgemap::setRenderSegments(int nSegments)
{
	if (!Edgemap::setRenderSegments(nSegments))
		return false;

	*m_pEncoder << GfxStream::Header{ GfxChunkId::SetEdgemapRenderSegments, GfxStream::SpxFormat::Int32_dec, 4 };
	*m_pEncoder << m_inStreamId;
	*m_pEncoder << (int16_t) nSegments;
	return true;
}


//____ importSamples() _________________________________________________________

bool GfxStreamEdgemap::importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( pSource == nullptr || edgeBegin < 0 || edgeBegin > edgeEnd || edgeEnd > (m_nbSegments-1) || sampleBegin < 0 || sampleBegin > sampleEnd || sampleEnd > (m_size.w+1) )
		return false;

	_importSamples( origo, pSource, edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
	m_pEncoder->flush();
	return true;
}

bool GfxStreamEdgemap::importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( pSource == nullptr || edgeBegin < 0 || edgeBegin > edgeEnd || edgeEnd > (m_nbSegments-1) || sampleBegin < 0 || sampleBegin > sampleEnd || sampleEnd > (m_size.w+1) )
		return false;

	_importSamples( origo, pSource, edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
	m_pEncoder->flush();
	return true;
}


//____ exportSamples() _________________________________________________________

bool GfxStreamEdgemap::exportSamples( WaveOrigo origo, spx * pDestination, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	//TODO: Implement!!!
	
	return false;
}

bool  GfxStreamEdgemap::exportSamples( WaveOrigo origo, float * pDestination, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	//TODO: Implement!!!
	
	return false;
}

//____ _importSamples() ________________________________________________________

void GfxStreamEdgemap::_importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( samplePitch == 0 )
		samplePitch = 1;
	
	
	if( edgePitch == 0 )
		edgePitch = samplePitch * (sampleEnd - sampleBegin);
	
	spx mul = (origo == WaveOrigo::Top || origo == WaveOrigo::MiddleDown) ? 1 : -1;
	spx offset = 0;
	
	if( origo == WaveOrigo::Bottom )
		offset = m_size.h*64;
	else if( origo == WaveOrigo::MiddleDown || origo == WaveOrigo::MiddleUp )
		offset = m_size.h*32;

	for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
	{
		const spx * pSrc = pSource + edgePitch * (edge-edgeBegin);
		spx * pDst = m_pSamples + edge*(m_size.w+1) + sampleBegin;

		for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
		{
			* pDst++ = (* pSrc * mul) + offset;
			pSrc += samplePitch;
		}
	}

	_sendSamples(m_pEncoder, edgeBegin, edgeEnd, sampleBegin, sampleEnd );
}

void GfxStreamEdgemap::_importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( samplePitch == 0 )
		samplePitch = 1;
	
	
	if( edgePitch == 0 )
		edgePitch = samplePitch * (sampleEnd - sampleBegin);

	spx mul = (origo == WaveOrigo::Top || origo == WaveOrigo::MiddleDown) ? 1 : -1;
	spx offset = 0;

	if( origo == WaveOrigo::Bottom )
		offset = m_size.h*64;
	else if( origo == WaveOrigo::MiddleDown || origo == WaveOrigo::MiddleUp )
		offset = m_size.h*32;

	if( origo == WaveOrigo::MiddleDown || origo == WaveOrigo::MiddleUp )
		mul *= m_size.h*32;
	else
		mul *= m_size.h*64;
	
	for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
	{
		const float * pSrc = pSource + edgePitch * (edge-edgeBegin);
		spx * pDst = m_pSamples + edge*(m_size.w+1) + sampleBegin;

		for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
		{
			* pDst++ = (* pSrc * mul) + offset;
			pSrc += samplePitch;
		}
	}

	_sendSamples(m_pEncoder, edgeBegin, edgeEnd, sampleBegin, sampleEnd );
}

//____ _sendCreateEdgemap() ___________________________________________________

void GfxStreamEdgemap::_sendCreateEdgemap( GfxStreamEncoder* pEncoder )
{
	int nbColors = ( m_pColors != nullptr ) ? m_nbSegments : 0;
	int nbGradients = ( m_pGradients != nullptr ) ? m_nbSegments : 0;


	int blockSize = 14 + nbColors * 8 + nbGradients * 34;
	
	*pEncoder << GfxStream::Header{ GfxChunkId::CreateEdgemap, GfxStream::SpxFormat::Int32_dec, blockSize };
	*pEncoder << m_inStreamId;
	*pEncoder << m_size;
	*pEncoder << (uint16_t) m_nbSegments;
	*pEncoder << (bool) nbColors;
	*pEncoder << (bool) nbGradients;

	for( int i = 0 ; i < nbColors ; i++ )
		* pEncoder << m_pColors[i];
	
	for( int i = 0 ; i < nbGradients ; i++ )
		* pEncoder << m_pGradients[i];
}

//____ _sendSamples() ___________________________________________________

void GfxStreamEdgemap::_sendSamples( GfxStreamEncoder* pEncoder, int edgeBegin, int edgeEnd,
											int sampleBegin, int sampleEnd )
{
	// Send header
	
	int beginBlockSize = 2 + 1 + 1 + 2 + 2;
	
	*pEncoder << GfxStream::Header{ GfxChunkId::BeginEdgemapUpdate, GfxStream::SpxFormat::Int32_dec, beginBlockSize };
	*pEncoder << m_inStreamId;
	*pEncoder << (uint8_t) edgeBegin;
	*pEncoder << (uint8_t) edgeEnd;
	*pEncoder << (uint16_t) sampleBegin;
	*pEncoder << (uint16_t) sampleEnd;
	
	
	// Check sample max/min and max/min diff against old samples
	// and find best spxFormat for transfer.
	
	const int add = (1 << 21);
	int spxMask = 0;

	for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
	{
		spx * pNewSample = m_pSamples + edge*(m_nbSegments-1) + sampleBegin;

		for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
		{
			int value = * pNewSample++;
			spxMask |= value + add;
		}
	}

	GfxStream::SpxFormat spxFormat = _findBestPackFormat(spxMask);
	
	
	
	// Compress bytes for transfer

	int bufferBytes = GfxStream::spxSize(spxFormat)*(edgeEnd-edgeBegin)*(sampleEnd-sampleBegin);
	
	int8_t * pBuffer = (int8_t*) GfxBase::memStackAlloc(bufferBytes);
	
	
	switch( spxFormat )
	{
		case GfxStream::SpxFormat::Int32_dec:
		{
			// No compression needed, just replace old
			
			spx * pPacked = (spx *) pBuffer;
			for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
			{
				spx * pNew = m_pSamples + edge*(m_size.w+1) + sampleBegin;

				for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
					* pPacked++ = * pNew++;
			}
			break;
		}
			
		case GfxStream::SpxFormat::Uint16_dec:
		{
			uint16_t * pPacked = reinterpret_cast<uint16_t*>(pBuffer);

			for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
			{
				spx * pNew = m_pSamples + edge*(m_size.w+1) + sampleBegin;

				for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
					* pPacked++ = (uint16_t) * pNew++;
			}
			break;
		}

		case GfxStream::SpxFormat::Int16_int:
		{
			int16_t * pPacked = reinterpret_cast<int16_t*>(pBuffer);
			for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
			{
				spx * pNew = m_pSamples + edge*(m_size.w+1) + sampleBegin;

				for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
					* pPacked++ = (int16_t) ((* pNew++) >> 6);
			}
			break;
		}

		case GfxStream::SpxFormat::Uint8_int:
		{
			uint8_t * pPacked = reinterpret_cast<uint8_t*>(pBuffer);
			for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
			{
				spx * pNew = m_pSamples + edge*(m_size.w+1) + sampleBegin;

				for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
					* pPacked++ = (uint8_t) ((* pNew++) >> 6);
			}
			break;
		}
	}
	
	// Stream data
	
	const int8_t * pSrc = pBuffer;
		
	int dataSize = bufferBytes;

	int maxDataSize = ((int)(GfxStream::c_maxBlockSize - sizeof(GfxStream::Header))) & 0xFFFC;		// We can not have fractional spx in buffer.
	
	while( dataSize > 0 )
	{
		uint16_t chunkSize = std::min(dataSize, maxDataSize);
		dataSize -= chunkSize;

		*pEncoder << GfxStream::Header{ GfxChunkId::EdgemapSamples, spxFormat, uint16_t((chunkSize+1)&0xFFFE) };

		*pEncoder << GfxStream::WriteBytes{ chunkSize, (void *) pSrc };
		pSrc += chunkSize;
	}
		
	pEncoder->align();
	
	// Stream footer
	
	*pEncoder << GfxStream::Header{ GfxChunkId::EndEdgemapUpdate, GfxStream::SpxFormat::Int32_dec, 0 };

	// Release temp buffer

	GfxBase::memStackFree(bufferBytes);
}

GfxStream::SpxFormat GfxStreamEdgemap::_findBestPackFormat( int spxMask )
{
	if( (spxMask & 0xFFDFC03F) == 0 )
		return GfxStream::SpxFormat::Uint8_int;						// Fits in uint8_t without binals.
	
	if( (spxMask & 0xFFDF0000) == 0 )
		return GfxStream::SpxFormat::Uint16_dec;					// Fits in uint16_t with binals.

	if( (spxMask & 0xFFC0003F) == 0 )
		return GfxStream::SpxFormat::Int16_int;						// Fits in int16_t without binals.

	return GfxStream::SpxFormat::Int32_dec;							// We need 32 bits.
}

} // namespace wg

