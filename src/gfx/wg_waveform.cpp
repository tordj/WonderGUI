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

#include <wg_waveform.h>
#include <wg_gfxbase.h>

#include <algorithm>

namespace wg
{
	const TypeInfo Waveform::TYPEINFO = { "Waveform", &Object::TYPEINFO };


	//.____ create() _____________________________________________________________

	Waveform_p Waveform::create( const Blueprint& blueprint, EdgemapFactory * pFactory )
	{
		return Waveform_p( new Waveform(blueprint, pFactory) );
	}

	//____ constructor ___________________________________________________________

	Waveform::Waveform( const Blueprint& bp , EdgemapFactory * pFactory )
	: 	m_color(bp.color),
		m_outlineColor(bp.outlineColor),
		m_origo(bp.origo),
		m_nbSamples(bp.size.w+1)
	{

		m_topBrush.thickness = bp.topOutlineThickness;
		m_bottomBrush.thickness = bp.bottomOutlineThickness;

		// Generate brush slope

		_generateInfluenceSlope( m_topBrush );
		_generateInfluenceSlope( m_bottomBrush );
		
		if( pFactory == nullptr )
			pFactory = GfxBase::defaultEdgemapFactory();

		if( bp.gradient.isValid || bp.outlineGradient.isValid )
		{
			Gradient transparent(Color::Transparent);
			Gradient outline = bp.outlineGradient.isValid ? bp.outlineGradient : Gradient(bp.color);
			Gradient fill = bp.gradient.isValid ? bp.gradient : Gradient(bp.color);

			Gradient	gradients[5] = { transparent, outline, fill, outline, transparent };

			m_pEdgemap = pFactory->createEdgemap( WGBP(Edgemap, _.gradients = gradients, _.size = bp.size, _.segments = 5 ) );
		}
		else
		{
			HiColor	colors[5] = { Color::Transparent, bp.outlineColor, bp.color, bp.outlineColor, Color::Transparent };
			m_pEdgemap = pFactory->createEdgemap( WGBP(Edgemap, _.colors = colors, _.size = bp.size, _.segments = 5 ) );
		}
		
		m_topSamplesPadding = (bp.topOutlineThickness/2 / 64) + 1;
		m_bottomSamplesPadding = (bp.bottomOutlineThickness/2 / 64) + 1;

		
		m_pSampleBuffer = new spx[(m_nbSamples+m_topSamplesPadding*2)+(m_nbSamples+m_bottomSamplesPadding*2)];
		
		m_pTopSamples = m_pSampleBuffer + m_topSamplesPadding;
		m_pBottomSamples = m_pSampleBuffer + (m_nbSamples + m_topSamplesPadding*2) + m_bottomSamplesPadding;
		
		m_dirtBegin	= 0;
		m_dirtEnd 	= m_nbSamples;
	}

	//____ destructor ____________________________________________________________

	Waveform::~Waveform()
	{
		delete [] m_pSampleBuffer;
	}

	//____ typeInfo() ____________________________________________________________

	const TypeInfo& Waveform::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSamples() __________________________________________________________

	void Waveform::setSamples( int sampleBegin, int sampleEnd, spx * pTopSamples, spx * pBottomSamples )
	{
		limit(sampleBegin, 0, m_nbSamples);
		limit(sampleEnd, sampleBegin, m_nbSamples);

		if( pTopSamples )
			EdgemapTools::convertSamples(m_pTopSamples + sampleBegin, pTopSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 1);

		if( pBottomSamples )
			EdgemapTools::convertSamples(m_pBottomSamples + sampleBegin, pBottomSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 1);
	}

	void Waveform::setSamples( int sampleBegin, int sampleEnd, float * pTopSamples, float * pBottomSamples )
	{
		limit(sampleBegin, 0, m_nbSamples);
		limit(sampleEnd, sampleBegin, m_nbSamples);

		if( pTopSamples )
			EdgemapTools::convertSamples(m_pTopSamples + sampleBegin, pTopSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 1);

		if( pBottomSamples )
			EdgemapTools::convertSamples(m_pBottomSamples + sampleBegin, pBottomSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 1);

	}

	//____ setFlatTopLine() ______________________________________________________

	void Waveform::setFlatTopLine( int sampleBegin, int sampleEnd, spx sample )
	{
		spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

		_setFlatLine(m_pTopSamples, sampleBegin, sampleEnd, out);
	}

	void Waveform::setFlatTopLine( int sampleBegin, int sampleEnd, float sample )
	{
		spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

		_setFlatLine(m_pTopSamples, sampleBegin, sampleEnd, out);
	}

	//____ setFlatBottomLine() ___________________________________________________

void Waveform::setFlatBottomLine( int sampleBegin, int sampleEnd, spx sample )
{
	spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

	_setFlatLine(m_pBottomSamples, sampleBegin, sampleEnd, out);
}

void Waveform::setFlatBottomLine( int sampleBegin, int sampleEnd, float sample )
{
	spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

	_setFlatLine(m_pBottomSamples, sampleBegin, sampleEnd, out);
}

	//____ refresh() _____________________________________________________________

	Edgemap_p Waveform::refresh()
	{
		if( m_dirtEnd - m_dirtBegin <= 0  )
			return m_pEdgemap;
			
		// Copy first/last samples to padding if changed.
		
		if( m_dirtBegin == 0 )
		{
			spx value = m_pTopSamples[0];
			spx * p = m_pTopSamples - m_topSamplesPadding;
			while( p != m_pTopSamples )
				* p++ = value;

			value = m_pBottomSamples[0];
			p = m_pBottomSamples - m_bottomSamplesPadding;
			while( p != m_pBottomSamples )
				* p++ = value;
		}
		
		if( m_dirtEnd == m_nbSamples )
		{
			spx value = m_pTopSamples[m_nbSamples-1];
			spx * p = m_pTopSamples + m_nbSamples + m_topSamplesPadding -1;
			while( p >= m_pTopSamples + m_nbSamples )
				* p-- = value;

			value = m_pBottomSamples[m_nbSamples-1];
			p = m_pBottomSamples + m_nbSamples + m_bottomSamplesPadding -1;
			while( p >= m_pBottomSamples + m_nbSamples )
				* p-- = value;
		}

		int padding = std::max(m_topSamplesPadding,m_bottomSamplesPadding);
		
		int dirtBegin = std::max(0,m_dirtBegin-padding);
		int dirtEnd = std::min(m_nbSamples,m_dirtEnd+padding);
		int dirtSize = dirtEnd - dirtBegin;

		
		int allocated = (dirtSize)*4*sizeof(spx);
		
		spx * pBuffer = (spx*) GfxBase::memStackAlloc(allocated);
		
		_drawLine(m_pTopSamples + m_dirtBegin, pBuffer, pBuffer + dirtSize, dirtSize, m_topBrush );
		_drawLine(m_pBottomSamples + m_dirtBegin, pBuffer+dirtSize*2, pBuffer + dirtSize*3, dirtSize, m_bottomBrush );

		// Loop through new edges and fix possible issues.
		
		for (int i = 0; i < dirtSize; i++)
		{
			// Flip lines if they cross
			
			if( pBuffer[i] > pBuffer[i+dirtSize*2])
			{
				std::swap( pBuffer[i], pBuffer[i+dirtSize*2]);
				std::swap( pBuffer[i+dirtSize], pBuffer[i+dirtSize*3]);
			}
			
			// Handle outline overlapping
			
			if (pBuffer[i+dirtSize*2] < pBuffer[i+dirtSize] )
			{
				pBuffer[i+dirtSize*2] = pBuffer[i+dirtSize];
				if (pBuffer[i+dirtSize*3] < pBuffer[i+dirtSize*2] )
					pBuffer[i+dirtSize*3] = pBuffer[i+dirtSize*2];
			}
		}
		
		// Update edgemap.
		
		m_pEdgemap->importSamples(WaveOrigo::Top, pBuffer, 0, 2, dirtBegin, dirtEnd);
		m_pEdgemap->importSamples(WaveOrigo::Top, pBuffer + dirtSize*2, 2, 4, dirtBegin, dirtEnd);
		
		GfxBase::memStackFree(allocated);
		
		m_dirtBegin = m_nbSamples;
		m_dirtEnd = 0;

		return m_pEdgemap;
	}

	//____ _setFlatLine() _______________________________________________________

	void Waveform::_setFlatLine( spx * pSamples, int sampleBegin, int sampleEnd, spx sample )
	{
		limit(sampleBegin, 0, m_nbSamples);
		limit(sampleEnd, sampleBegin, m_nbSamples);
		
		spx * p = pSamples + sampleBegin;
		for( int i = 0 ; i < sampleEnd - sampleBegin ; i++ )
			* p++ = sample;

		if( sampleBegin < m_dirtBegin )
			m_dirtBegin = sampleBegin;

		if( sampleEnd > m_dirtEnd )
			m_dirtEnd = sampleEnd;
	}


	//____ _drawLine() __________________________________________________________

	void Waveform::_drawLine(spx * pSrc, spx * pDestTop, spx * pDestBottom, int nPoints, Brush& brush)
	{

		for (int i = 0; i < nPoints; i++)
		{
			
			
			// Start with top and bottom for current point

			int top = pSrc[i] - brush.influenceSlope[0];
			int bottom = pSrc[i] + brush.influenceSlope[0];

			// Check brush's coverage from previous points

			spx * pInfluence = brush.influenceSlope;
			spx * _pSrc = pSrc + i;
			
			
			while( * pInfluence != 0 )
			{
				spx inc = (_pSrc[-1] - _pSrc[0])/4;
				spx sample = * _pSrc--;
				
				for( int j = 0 ; j < 4 ; j++ )
				{
					sample += inc;

					spx topCover = sample - *pInfluence;
					spx bottomCover = sample + *pInfluence;

					if (topCover < top)
						top = topCover;
					else if (bottomCover > bottom)
						bottom = bottomCover;
					
					pInfluence++;
				}
			}

			// Check brush's coverage from following points

			pInfluence = brush.influenceSlope;
			_pSrc = pSrc + i;

			while( * pInfluence != 0 )
			{
				spx inc = (_pSrc[1] - _pSrc[0])/4;
				spx sample = * _pSrc++;
				
				for( int j = 0 ; j < 4 ; j++ )
				{
					sample += inc;

					spx topCover = sample - *pInfluence;
					spx bottomCover = sample + *pInfluence;

					if (topCover < top)
						top = topCover;
					else if (bottomCover > bottom)
						bottom = bottomCover;
					
					pInfluence++;
				}
			}

			// Save traced values

			*pDestTop++ = top;
			*pDestBottom++ = bottom;
		}
	}

	//____ _generateInfluenceSlope() ____________________________________________

	void Waveform::_generateInfluenceSlope(Brush& brush)
	{
		auto pCurveTab = GfxBase::curveTab();
		int curveTabSize = GfxBase::curveTabSize();
		
		int radius = brush.thickness / 2;
		
		brush.influenceSlope[0] = radius;

		int nSteps = radius * 4 / 64;
		
		for( int i = 0 ; i < nSteps ; i++ )
			brush.influenceSlope[i] = (pCurveTab[curveTabSize -1 -(curveTabSize * i*(64/4) / radius)] * radius) >> 16;

		brush.influenceSlope[nSteps] 	= 0;
		brush.influenceSlope[nSteps+1] 	= 0;
		brush.influenceSlope[nSteps+2] 	= 0;
		brush.influenceSlope[nSteps+3] 	= 0;
	}


}
