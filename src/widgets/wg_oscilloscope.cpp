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

#include <wg_oscilloscope.h>
#include <wg_gfxdevice.h>
#include <wg_base.h>

const char WgOscilloscope::CLASSNAME[] = {"Oscilloscope"};

//____ Constructor ____________________________________________________________

WgOscilloscope::WgOscilloscope()
{
	m_gridColor = WgColor::black;
	m_lineColor = WgColor::white;
	m_lineThickness = 2.f;

	m_nVGridLines = 0;
	m_pVGridLines = 0;
	m_nHGridLines = 0;
	m_pHGridLines = 0;

	m_nLinePoints = 0;
	m_pLinePoints = 0;

	m_nMarkers = 0;
	m_pMarkers = 0;
}

//____ Destructor _____________________________________________________________

WgOscilloscope::~WgOscilloscope()
{
	delete [] m_pVGridLines;
	delete [] m_pHGridLines;
	delete [] m_pLinePoints;
	delete [] m_pMarkers;
}

//____ IsInstanceOf() _________________________________________________________

bool WgOscilloscope::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgOscilloscope::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgOscilloscopePtr WgOscilloscope::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgOscilloscopePtr( static_cast<WgOscilloscope*>(pObject.RawPtr()) );

	return 0;
}


//____ PreferredSize() ________________________________________________________

WgSize WgOscilloscope::PreferredSize() const
{
	WgSize contentSize(80,64);

	if( m_pSkin )
		return m_pSkin->SizeForContent(contentSize);
	else
		return contentSize;
}


//____ SetGridColor() _________________________________________________________

void WgOscilloscope::SetGridColor( WgColor color )
{
	if( m_gridColor != color )
	{
		m_gridColor = color;
		if( m_nVGridLines != 0 || m_nHGridLines != 0 )
			_requestRender();
	}
}

//____ SetVGridLines() ________________________________________________________

void WgOscilloscope::SetVGridLines( int nLines, float pLines[] )
{
	if( nLines != m_nVGridLines )
	{
		delete [] m_pVGridLines;
		if( nLines == 0 )
			m_pVGridLines = 0;
		else
			m_pVGridLines = new float[nLines];
		m_nVGridLines = nLines;
	}

	for( int i = 0 ; i < nLines ; i++ )
		m_pVGridLines[i] = pLines[i];

	_requestRender();
}

//____ SetHGridLines() ________________________________________________________

void WgOscilloscope::SetHGridLines( int nLines, float pLines[] )
{
	if( nLines != m_nHGridLines )
	{
		delete [] m_pHGridLines;
		if( nLines == 0 )
			m_pHGridLines = 0;
		else
			m_pHGridLines = new float[nLines];
		m_nHGridLines = nLines;
	}

	for( int i = 0 ; i < nLines ; i++ )
		m_pHGridLines[i] = pLines[i];

	_requestRender();
}

//____ SetLineColor() ________________________________________________________

void WgOscilloscope::SetLineColor( WgColor color )
{
	if( color != m_lineColor )
	{
		m_lineColor = color;
		_requestRender();
	}
}

//____ SetLineThickness() _____________________________________________________

void WgOscilloscope::SetLineThickness( float thickness )
{
	WG_LIMIT( thickness, 1.f, 2.f );
	if( thickness != m_lineThickness )
	{
		m_lineThickness = thickness;
		_requestRender();
	}
}

//____ SetLinePoints() ________________________________________________________

void WgOscilloscope::SetLinePoints( int nPoints, float pPointValues[] )
{
	if( nPoints != m_nLinePoints )
	{
		delete [] m_pLinePoints;
		
		if( nPoints == 0 )
			m_pLinePoints = 0;
		else
			m_pLinePoints = new float[nPoints];
		m_nLinePoints = nPoints;
	}

	for( int i = 0 ; i < nPoints ; i++ )
		m_pLinePoints[i] = pPointValues[i];

	_requestRender();
}

//____ ClearMarkers() _________________________________________________________

void WgOscilloscope::ClearMarkers()
{
	delete [] m_pMarkers;
	m_pMarkers = 0;
	m_nMarkers = 0;
	_requestRender();
}

//____ AddMarker() ____________________________________________________________

void WgOscilloscope::AddMarker( int xOfs, float yOfs )
{
	Marker * pNew = new Marker[m_nMarkers+1];
	for( int i = 0 ; i < m_nMarkers ; i++ )
		pNew[i] = m_pMarkers[i];
	delete [] m_pMarkers;

	pNew[m_nMarkers].x = xOfs;
	pNew[m_nMarkers].y = yOfs;

	m_nMarkers++;
	m_pMarkers = pNew;
	_requestRender();
}

//____ SetMarkerSkin() _________________________________________________________

void WgOscilloscope::SetMarkerSkin( const WgSkinPtr& pSkin )
{
	if( m_pMarkerSkin != pSkin )
	{
		m_pMarkerSkin = pSkin;
		_requestRender();
	}
}



//____ _onCloneContent() ______________________________________________________

void WgOscilloscope::_onCloneContent( const WgWidget * _pOrg )
{
	const WgOscilloscope * pOrg = static_cast<const WgOscilloscope*>(_pOrg);

	m_gridColor = pOrg->m_gridColor;
	m_lineColor = pOrg->m_lineColor;
	m_lineThickness = pOrg->m_lineThickness;

	delete [] m_pVGridLines;
	delete [] m_pHGridLines;
	delete [] m_pLinePoints;
	delete [] m_pMarkers;

	m_nVGridLines = pOrg->m_nVGridLines;
	m_pVGridLines = 0;
	if( m_nVGridLines > 0 )
	{
		m_pVGridLines = new float[m_nVGridLines];
		for( int i = 0 ; i < m_nVGridLines ; i++ )
			m_pVGridLines[i] = pOrg->m_pVGridLines[i];
	}

	m_nHGridLines = pOrg->m_nHGridLines;
	m_pHGridLines = 0;
	if( m_nHGridLines > 0 )
	{
		m_pHGridLines = new float[m_nHGridLines];
		for( int i = 0 ; i < m_nHGridLines ; i++ )
			m_pHGridLines[i] = pOrg->m_pHGridLines[i];
	}

	m_nLinePoints = pOrg->m_nLinePoints;
	m_pLinePoints = 0;
	if( m_nLinePoints > 0 )
	{
		m_pLinePoints = new float[m_nLinePoints];
		for( int i = 0 ; i < m_nLinePoints ; i++ )
			m_pLinePoints[i] = pOrg->m_pLinePoints[i];
	}

	m_nMarkers = pOrg->m_nMarkers;
	m_pMarkers = 0;
	if( m_nMarkers > 0 )
	{
		m_pMarkers = new Marker[m_nMarkers];
		for( int i = 0 ; i < m_nMarkers ; i++ )
			m_pMarkers[i] = pOrg->m_pMarkers[i];
	}
	
	m_pMarkerSkin = pOrg->m_pMarkerSkin;
}

//____ _onRender() ____________________________________________________________

void WgOscilloscope::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip )
{
	WgWidget::_onRender(pDevice,_canvas,_window,_clip);

	WgRect canvas = _canvas;
	if( m_pSkin )
		canvas = m_pSkin->ContentRect(_canvas,m_state);

	//

	float centerX = canvas.x + canvas.w/2.f;
	float centerY = canvas.y + canvas.h/2.f;
	float scaleX = (canvas.w-1)/2.f;
	float scaleY = (canvas.h-1)/2.f;

	// Draw HGridLines

	for( int i = 0 ; i < m_nHGridLines ; i++ )
	{
		int ofsY = (int) (m_pHGridLines[i] * scaleY + centerY);
		pDevice->ClipDrawHorrLine( _clip, WgCoord(canvas.x,ofsY), canvas.w, m_gridColor );
	}

	// Draw VGridLines

	for( int i = 0 ; i < m_nVGridLines ; i++ )
	{
		int ofsX = (int) (m_pVGridLines[i] * scaleX + centerX);
		pDevice->ClipDrawVertLine( _clip, WgCoord(ofsX,canvas.y), canvas.h, m_gridColor );
	}

	// Draw the oscilloscope line

	int nPoints = m_nLinePoints > canvas.w ? canvas.w : m_nLinePoints;

	int allocSize = sizeof(WgCoord)*nPoints;

	WgCoord * pOut = reinterpret_cast<WgCoord*>(WgBase::MemStackAlloc(allocSize));

	for( int i = 0 ; i < nPoints ; i++ )
	{
		pOut[i] = WgCoord(canvas.x + i, (int)(centerY + scaleY*m_pLinePoints[i]));
	}

	pDevice->ClipPlotSoftPixels(_clip, nPoints, pOut, m_lineColor, m_lineThickness);

	WgBase::MemStackRelease(allocSize);

	// Render markers

	if( m_pMarkerSkin )
	{
		for( int i = 0 ; i < m_nMarkers ; i++ )
		{
			WgRect dest;

			int x = m_pMarkers[i].x;
			int y = (int) (m_pMarkers[i].y*scaleY+centerY);

			dest.SetSize( m_pMarkerSkin->PreferredSize() );
			dest.x = x - dest.w / 2;
			dest.y = y - dest.h / 2;

			m_pMarkerSkin->Render( pDevice, dest, WG_STATE_NORMAL, _clip );
		}
	}
}

