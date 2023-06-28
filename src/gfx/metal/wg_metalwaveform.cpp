/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI Graphics Toolkit is free Glware; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Glware Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_metalwaveform.h>

namespace wg
{

const TypeInfo MetalWaveform::TYPEINFO = { "MetalWaveform", &Waveform::TYPEINFO };


//____ create() ______________________________________________________________

MetalWaveform_p	MetalWaveform::create( const Blueprint& blueprint )
{
	if( !_validateBlueprint(blueprint) )
		return nullptr;
	
	return MetalWaveform_p( new MetalWaveform(blueprint) );
}

MetalWaveform_p MetalWaveform::create( const Waveform::Blueprint& blueprint, WaveOrigo origo, const float * pSamples, int edges, int edgePitch, int samplePitch)
{
	if( !_validateBlueprint(blueprint) )
		return nullptr;

	if(edges >= blueprint.segments )
		return nullptr;

	auto p = MetalWaveform_p( new MetalWaveform(blueprint) );

	p->_importSamples(origo, pSamples, 0, edges, 0, p->m_size.w+1, edgePitch, samplePitch);
	p->m_nbRenderSegments = edges+1;
	
	return p;
}

MetalWaveform_p MetalWaveform::create( const Waveform::Blueprint& blueprint, WaveOrigo origo, const spx * pSamples, int edges, int edgePitch, int samplePitch)
{
	if( !_validateBlueprint(blueprint) )
		return nullptr;

	if(edges >= blueprint.segments )
		return nullptr;
	
	auto p = MetalWaveform_p( new MetalWaveform(blueprint) );

	p->_importSamples(origo, pSamples, 0, edges, 0, p->m_size.w+1, edgePitch, samplePitch);
	p->m_nbRenderSegments = edges+1;
	
	return p;
}

//____ constructor ___________________________________________________________

MetalWaveform::MetalWaveform(const Blueprint& bp) : Waveform(bp)
{
	// Analyze gradients to figure out our tint mode and amount of render colors.
	
	int renderColorsExtraBytes;
	
	if( bp.gradients == nullptr )
	{
		m_tintMode = TintMode::Flat;
		renderColorsExtraBytes = 0;
	}
	else
	{
		bool bHorizontal = false;
		bool bVertical = false;
		
		for( int i = 0 ; i < bp.segments ; i++ )
		{
			const Gradient& grad = bp.gradients[i];
			
			if(grad.topLeft != grad.topRight)
				bHorizontal = true;

	 		if(grad.topLeft != grad.bottomLeft)
				bVertical = true;
		}
		
		if( bHorizontal && bVertical )
		{
			m_tintMode = TintMode::GradientXY;
			renderColorsExtraBytes = bp.segments * sizeof(HiColor) * 4;
		}
		else if( bVertical )
		{
			m_tintMode = TintMode::GradientY;
			renderColorsExtraBytes = bp.segments * sizeof(HiColor) * 2;
		}
		else										// Defaults to GradientX even if all colors were identical since that is the fastest one.
		{
			m_tintMode = TintMode::GradientX;
			renderColorsExtraBytes = bp.segments * sizeof(HiColor) * 2;
		}
	}
	
	// Setup buffers
	
	int sampleArraySize = (bp.size.w+1) * bp.segments * sizeof(spx);
	int colorArraySize = bp.colors ? bp.segments*sizeof(HiColor) : 0;
	int gradientArraySize = bp.gradients ? bp.segments*sizeof(Gradient) : 0;

	int bytes = sampleArraySize + colorArraySize + gradientArraySize + renderColorsExtraBytes;
	
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

	switch( m_tintMode )
	{
		default:
		case TintMode::Flat:
			m_pRenderColors = m_pColors;
			break;
			
		case TintMode::GradientX:
		{
			m_pRenderColors = (HiColor*) pBuffer;

			for( int i = 0 ; i < bp.segments ; i++ )
			{
				const Gradient& grad = bp.gradients[i];

				m_pRenderColors[i*2] = grad.topLeft;
				m_pRenderColors[i*2+1] = grad.topRight;
			}
			
			pBuffer += renderColorsExtraBytes;
		}
		case TintMode::GradientY:
		{
			m_pRenderColors = (HiColor*) pBuffer;

			for( int i = 0 ; i < bp.segments ; i++ )
			{
				const Gradient& grad = bp.gradients[i];

				m_pRenderColors[i*2] = grad.topLeft;
				m_pRenderColors[i*2+1] = grad.bottomLeft;
			}
			
			pBuffer += renderColorsExtraBytes;
		}
		case TintMode::GradientXY:
		{
			m_pRenderColors = (HiColor*) pBuffer;

			for( int i = 0 ; i < bp.segments ; i++ )
			{
				const Gradient& grad = bp.gradients[i];

				m_pRenderColors[i*4]   = grad.topLeft;
				m_pRenderColors[i*4+1] = grad.topRight;
				m_pRenderColors[i*4+2] = grad.bottomRight;
				m_pRenderColors[i*4+3] = grad.bottomLeft;
			}
			
			pBuffer += renderColorsExtraBytes;
		}
			break;
	}
	
	m_pSamples = (spx*) pBuffer;
}

//____ destructor ____________________________________________________________

MetalWaveform::~MetalWaveform()
{
	delete [] m_pBuffer;
}

//____ typeInfo() ____________________________________________________________

const TypeInfo& MetalWaveform::typeInfo(void) const
{
	return TYPEINFO;
}

//____ importSamples() _________________________________________________________

bool MetalWaveform::importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( pSource == nullptr || edgeBegin < 0 || edgeBegin > edgeEnd || edgeEnd > (m_nbSegments-1) || sampleBegin < 0 || sampleBegin > sampleEnd || sampleEnd > (m_size.w+1) )
		return false;

	_importSamples( origo, pSource, edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
	return true;
}

bool MetalWaveform::importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( pSource == nullptr || edgeBegin < 0 || edgeBegin > edgeEnd || edgeEnd > (m_nbSegments-1) || sampleBegin < 0 || sampleBegin > sampleEnd || sampleEnd > (m_size.w+1) )
		return false;

	_importSamples( origo, pSource, edgeBegin, edgeEnd, sampleBegin, sampleEnd, edgePitch, samplePitch);
	return true;
}


//____ exportSamples() _________________________________________________________

bool MetalWaveform::exportSamples( WaveOrigo origo, spx * pDestination, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	//TODO: Implement!!!
	
	return false;
}

bool  MetalWaveform::exportSamples( WaveOrigo origo, float * pDestination, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	//TODO: Implement!!!
	
	return false;
}

//____ _importSamples() ________________________________________________________

void MetalWaveform::_importSamples( WaveOrigo origo, const spx * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( samplePitch == 0 )
		samplePitch = 1;
	
	
	if( edgePitch == 0 )
		edgePitch = samplePitch * (sampleEnd - sampleBegin);
	
	int destSamplePitch = m_nbSegments-1;
	int destEdgePitch = 1;


	spx mul = (origo == WaveOrigo::Top || origo == WaveOrigo::MiddleDown) ? 1 : -1;
	spx offset = 0;
	
	if( origo == WaveOrigo::Bottom )
		offset = m_size.h*64;
	else if( origo == WaveOrigo::MiddleDown || origo == WaveOrigo::MiddleUp )
		offset = m_size.h*32;

	for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
	{
		const spx * pSrc = pSource + edgePitch * (edge-edgeBegin);
		spx * pDst = m_pSamples + edge + sampleBegin*(m_nbSegments-1);

		for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
		{
			* pDst = (* pSrc * mul) + offset;
			pDst += destSamplePitch;
			pSrc += samplePitch;
		}
	}
}

void MetalWaveform::_importSamples( WaveOrigo origo, const float * pSource, int edgeBegin, int edgeEnd,
							  int sampleBegin, int sampleEnd, int edgePitch, int samplePitch )
{
	if( samplePitch == 0 )
		samplePitch = 1;
	
	
	if( edgePitch == 0 )
		edgePitch = samplePitch * (sampleEnd - sampleBegin);
	
	int destSamplePitch = m_nbSegments-1;
	int destEdgePitch = 1;


	spx mul = (origo == WaveOrigo::Top || origo == WaveOrigo::MiddleDown) ? 1 : -1;
	spx offset = 0;

	if( origo == WaveOrigo::Bottom )
		offset = m_size.h*64;
	else if( origo == WaveOrigo::MiddleDown || origo == WaveOrigo::MiddleUp )
		offset = m_size.h*32;

	if( origo == WaveOrigo::MiddleDown || origo == WaveOrigo::MiddleUp )
		mul *= m_size.h*32;
	else.
		mul *= m_size.h*64;
	
	for( int edge = edgeBegin ; edge < edgeEnd ; edge++ )
	{
		const float * pSrc = pSource + edgePitch * (edge-edgeBegin);
		spx * pDst = m_pSamples + edge + sampleBegin*(m_nbSegments-1);

		for( int sample = sampleBegin ; sample < sampleEnd ; sample++ )
		{
			* pDst = (* pSrc * mul) + offset;
			pDst += destSamplePitch;
			pSrc += samplePitch;
		}
	}
}



} // namespace wg

