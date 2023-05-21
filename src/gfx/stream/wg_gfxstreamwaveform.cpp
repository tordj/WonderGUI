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

#include <wg_gfxstreamwaveform.h>

namespace wg
{

const TypeInfo GfxStreamWaveform::TYPEINFO = { "GfxStreamWaveform", &Waveform::TYPEINFO };


//____ create() ______________________________________________________________

GfxStreamWaveform_p	GfxStreamWaveform::create( GfxStreamEncoder * pEncoder, const Blueprint& blueprint )
{
	if( !pEncoder )
		return nullptr;
	
	if( !_validateBlueprint(blueprint) )
		return nullptr;
	
	auto p = GfxStreamWaveform_p( new GfxStreamWaveform(pEncoder, blueprint) );
	pEncoder->flush();
	return p;
}

GfxStreamWaveform_p GfxStreamWaveform::create( GfxStreamEncoder * pEncoder, const Waveform::Blueprint& blueprint, WaveOrigo origo, 
												const float * pSamples, int edges, int edgePitch, int samplePitch)
{
	if( !pEncoder )
		return nullptr;

	if( !_validateBlueprint(blueprint) )
		return nullptr;

	if(edges >= blueprint.segments )
		return nullptr;

	auto p = GfxStreamWaveform_p( new GfxStreamWaveform(pEncoder, blueprint) );

	p->_importSamples(origo, pSamples, 0, edges, 0, p->m_size.w+1, edgePitch, samplePitch);
	p->m_nbRenderSegments = edges+1;
	pEncoder->flush();
	return p;
}

GfxStreamWaveform_p GfxStreamWaveform::create( GfxStreamEncoder * pEncoder, const Waveform::Blueprint& blueprint, WaveOrigo origo, 
												const spx * pSamples, int edges, int edgePitch, int samplePitch)
{
	if( !pEncoder )
		return nullptr;

	if( !_validateBlueprint(blueprint) )
		return nullptr;

	if(edges >= blueprint.segments )
		return nullptr;
	
	auto p = GfxStreamWaveform_p( new GfxStreamWaveform(pEncoder, blueprint) );

	p->_importSamples(origo, pSamples, 0, edges, 0, p->m_size.w+1, edgePitch, samplePitch);
	p->m_nbRenderSegments = edges+1;
	pEncoder->flush();
	return p;
}

//____ constructor ___________________________________________________________

GfxStreamWaveform::GfxStreamWaveform( GfxStreamEncoder * pEncoder, const Blueprint& bp) : Waveform(bp)
{
	m_pEncoder = pEncoder;
	
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
	
	_sendCreateWaveform(m_pEncoder);
}

//____ destructor ____________________________________________________________

GfxStreamWaveform::~GfxStreamWaveform()
{
	delete [] m_pBuffer;

	* m_pEncoder << GfxStream::Header{ GfxChunkId::DeleteWaveform, GfxStream::SpxFormat::Int32_dec, 2 };
	* m_pEncoder << m_inStreamId;
}

//____ typeInfo() ____________________________________________________________

const TypeInfo& GfxStreamWaveform::typeInfo(void) const
{
	return TYPEINFO;
}

//____ importSamples() _________________________________________________________

bool GfxStreamWaveform::importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( pSource == nullptr || edgeBegin < 0 || edgeBegin > edgeEnd || edgeEnd > (m_nbSegments-1) || sampleBegin < 0 || sampleBegin > sampleEnd || sampleEnd > (m_size.w+1) )
		return false;

	_importSamples( origo, pSource, edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
	m_pEncoder->flush();
	return true;
}

bool GfxStreamWaveform::importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( pSource == nullptr || edgeBegin < 0 || edgeBegin > edgeEnd || edgeEnd > (m_nbSegments-1) || sampleBegin < 0 || sampleBegin > sampleEnd || sampleEnd > (m_size.w+1) )
		return false;

	_importSamples( origo, pSource, edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
	m_pEncoder->flush();
	return true;
}


//____ exportSamples() _________________________________________________________

bool GfxStreamWaveform::exportSamples( WaveOrigo origo, spx * pDestination, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	//TODO: Implement!!!
	
	return false;
}

bool  GfxStreamWaveform::exportSamples( WaveOrigo origo, float * pDestination, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	//TODO: Implement!!!
	
	return false;
}

//____ _importSamples() ________________________________________________________

void GfxStreamWaveform::_importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( samplePitch == 0 )
		samplePitch = 4;
	
	if( edgePitch == 0 )
		edgePitch = samplePitch * (m_size.w+1);
	
	
	spx mul = (origo == WaveOrigo::Top || origo == WaveOrigo::MiddleDown) ? 1 : -1;
	spx offset = 0;
	
	if( origo == WaveOrigo::Bottom )
		offset = m_size.h*64;
	else if( origo == WaveOrigo::MiddleDown || origo == WaveOrigo::MiddleUp )
		offset = m_size.h*32;

	for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
	{
		const spx * pSrc = (const spx*) (((const char*)pSource) + edgePitch * edge + samplePitch * sampleBegin);
		spx * pDst = m_pSamples + edge*(m_nbSegments-1) + sampleBegin;

		for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
		{
			* pDst++ = (* pSrc * mul) + offset;
			pSrc += samplePitch/4;
		}
	}
	
	_sendSamples(m_pEncoder, edgeBegin, edgeEnd, sampleBegin, sampleEnd );
}

void GfxStreamWaveform::_importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( samplePitch == 0 )
		samplePitch = 4;
	
	if( edgePitch == 0 )
		edgePitch = samplePitch * (m_size.w+1);


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
		const float * pSrc = (const float*) (((const char*)pSource) + edgePitch * edge + samplePitch * sampleBegin);
		spx * pDst = m_pSamples + edge*(m_nbSegments-1) + sampleBegin;

		for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
		{
			* pDst++ = (* pSrc * mul) + offset;
			pSrc += samplePitch/4;
		}
	}
	
	_sendSamples(m_pEncoder, edgeBegin, edgeEnd, sampleBegin, sampleEnd );
}

//____ _sendCreateWaveform() ___________________________________________________

void GfxStreamWaveform::_sendCreateWaveform( GfxStreamEncoder* pEncoder )
{
	int nbColors = ( m_pColors != nullptr ) ? m_nbSegments : 0;
	int nbGradients = ( m_pGradients != nullptr ) ? m_nbSegments : 0;


	int blockSize = 14 + nbColors * 8 + nbGradients * 34;
	
	*pEncoder << GfxStream::Header{ GfxChunkId::CreateWaveform, GfxStream::SpxFormat::Int32_dec, blockSize };
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

void GfxStreamWaveform::_sendSamples( GfxStreamEncoder* pEncoder, int edgeBegin, int edgeEnd,
											int sampleBegin, int sampleEnd )
{
	int beginBlockSize = 2 + 1 + 1 + 2 + 2;
	
	*pEncoder << GfxStream::Header{ GfxChunkId::BeginWaveformUpdate, GfxStream::SpxFormat::Int32_dec, beginBlockSize };
	*pEncoder << m_inStreamId;
	*pEncoder << (uint8_t) edgeBegin;
	*pEncoder << (uint8_t) edgeEnd;
	*pEncoder << (uint16_t) sampleBegin;
	*pEncoder << (uint16_t) sampleEnd;

	
	
	
	*pEncoder << GfxStream::Header{ GfxChunkId::WaveformSamples, GfxStream::SpxFormat::Int32_dec, beginBlockSize };

	
	
	
	
	*pEncoder << GfxStream::Header{ GfxChunkId::EndWaveformUpdate, GfxStream::SpxFormat::Int32_dec, 0 };

}


/*
CreateWaveform = 58,
BeginWaveformUpdate = 59,
WaveformSamples = 60,
EndWaveformUpdate = 61,
DeleteWaveform = 62
*/

} // namespace wg

