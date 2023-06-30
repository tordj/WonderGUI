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
		m_topOutlineThickness(bp.topOutlineThickness),
		m_bottomOutlineThickness(bp.bottomOutlineThickness),
		m_origo(bp.origo),
		m_nbSamples(bp.size.w+1)
	{

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
		
		m_pSamples = new spx[m_nbSamples];
		
		m_dirtBegin	= 0;
		m_dirtEnd 	= m_nbSamples;
	}

	//____ destructor ____________________________________________________________

	Waveform::~Waveform()
	{
		delete [] m_pSamples;		
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
			EdgemapTools::convertSamples(m_pSamples + sampleBegin*2, pTopSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 2);

		if( pBottomSamples )
			EdgemapTools::convertSamples(m_pSamples + sampleBegin*2+1, pBottomSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 2);
	}

	void Waveform::setSamples( int sampleBegin, int sampleEnd, float * pTopSamples, float * pBottomSamples )
	{
		limit(sampleBegin, 0, m_nbSamples);
		limit(sampleEnd, sampleBegin, m_nbSamples);

		if( pTopSamples )
			EdgemapTools::convertSamples(m_pSamples + sampleBegin*2, pTopSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 2);

		if( pBottomSamples )
			EdgemapTools::convertSamples(m_pSamples + sampleBegin*2+1, pBottomSamples, m_origo, m_pEdgemap->pixelSize().h*64, 1, sampleEnd-sampleBegin, 0, 1, 0, 2);

	}

	//____ setFlatTopLine() ______________________________________________________

	void Waveform::setFlatTopLine( int sampleBegin, int sampleEnd, spx sample )
	{
		limit(sampleBegin, 0, m_nbSamples);
		limit(sampleEnd, sampleBegin, m_nbSamples);
		
		spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

		spx * p = m_pSamples + sampleBegin*2;
		for( int i = 0 ; i < sampleEnd - sampleBegin ; i++ )
		{
			* p = out;
			p += 2;
		}

		if( sampleBegin < m_dirtBegin )
			m_dirtBegin = sampleBegin;

		if( sampleEnd > m_dirtEnd )
			m_dirtEnd = sampleEnd;
	}

	void Waveform::setFlatTopLine( int sampleBegin, int sampleEnd, float sample )
	{
		limit(sampleBegin, 0, m_nbSamples);
		limit(sampleEnd, sampleBegin, m_nbSamples);
		
		spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

		spx * p = m_pSamples + sampleBegin*2;
		for( int i = 0 ; i < sampleEnd - sampleBegin ; i++ )
		{
			* p = out;
			p += 2;
		}

		if( sampleBegin < m_dirtBegin )
			m_dirtBegin = sampleBegin;

		if( sampleEnd > m_dirtEnd )
			m_dirtEnd = sampleEnd;
	}

	//____ setFlatBottomLine() ___________________________________________________

void Waveform::setFlatBottomLine( int sampleBegin, int sampleEnd, spx sample )
{
	limit(sampleBegin, 0, m_nbSamples);
	limit(sampleEnd, sampleBegin, m_nbSamples);
	
	spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

	spx * p = m_pSamples + sampleBegin*2+1;
	for( int i = 0 ; i < sampleEnd - sampleBegin ; i++ )
	{
		* p = out;
		p += 2;
	}

	if( sampleBegin < m_dirtBegin )
		m_dirtBegin = sampleBegin;

	if( sampleEnd > m_dirtEnd )
		m_dirtEnd = sampleEnd;
}

void Waveform::setFlatBottomLine( int sampleBegin, int sampleEnd, float sample )
{
	limit(sampleBegin, 0, m_nbSamples);
	limit(sampleEnd, sampleBegin, m_nbSamples);
	
	spx out = EdgemapTools::convertSample(sample, m_origo, m_pEdgemap->pixelSize().h*64);

	spx * p = m_pSamples + sampleBegin*2+1;
	for( int i = 0 ; i < sampleEnd - sampleBegin ; i++ )
	{
		* p = out;
		p += 2;
	}

	if( sampleBegin < m_dirtBegin )
		m_dirtBegin = sampleBegin;

	if( sampleEnd > m_dirtEnd )
		m_dirtEnd = sampleEnd;
}

	//____ refresh() _____________________________________________________________

	Edgemap_p Waveform::refresh()
	{
		if( m_dirtEnd > m_dirtBegin )
		{
			int allocated = (m_dirtEnd - m_dirtBegin)*4*sizeof(spx);
			
			spx * pBuffer = (spx*) GfxBase::memStackAlloc(allocated);
			
			
			// Continue here...
			
			GfxBase::memStackFree(allocated);
		}
		
		m_dirtBegin = m_nbSamples;
		m_dirtEnd = 0;
	}


}
