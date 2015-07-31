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

namespace wg 
{
	
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
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgOscilloscope::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgWidget::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgOscilloscope::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgOscilloscope_p WgOscilloscope::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgOscilloscope_p( static_cast<WgOscilloscope*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ preferredSize() ________________________________________________________
	
	WgSize WgOscilloscope::preferredSize() const
	{
		WgSize contentSize(80,64);
	
		if( m_pSkin )
			return m_pSkin->sizeForContent(contentSize);
		else
			return contentSize;
	}
	
	
	//____ setGridColor() _________________________________________________________
	
	void WgOscilloscope::setGridColor( WgColor color )
	{
		if( m_gridColor != color )
		{
			m_gridColor = color;
			if( m_nVGridLines != 0 || m_nHGridLines != 0 )
				_requestRender();
		}
	}
	
	//____ setVGridLines() ________________________________________________________
	
	void WgOscilloscope::setVGridLines( int nLines, float pLines[] )
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
	
	//____ setHGridLines() ________________________________________________________
	
	void WgOscilloscope::setHGridLines( int nLines, float pLines[] )
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
	
	//____ setLineColor() ________________________________________________________
	
	void WgOscilloscope::setLineColor( WgColor color )
	{
		if( color != m_lineColor )
		{
			m_lineColor = color;
			_requestRender();
		}
	}
	
	//____ setLineThickness() _____________________________________________________
	
	void WgOscilloscope::setLineThickness( float thickness )
	{
		LIMIT( thickness, 1.f, 2.f );
		if( thickness != m_lineThickness )
		{
			m_lineThickness = thickness;
			_requestRender();
		}
	}
	
	//____ setLinePoints() ________________________________________________________
	
	void WgOscilloscope::setLinePoints( int nPoints, float pPointValues[] )
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
	
	//____ clearMarkers() _________________________________________________________
	
	void WgOscilloscope::clearMarkers()
	{
		delete [] m_pMarkers;
		m_pMarkers = 0;
		m_nMarkers = 0;
		_requestRender();
	}
	
	//____ addMarker() ____________________________________________________________
	
	void WgOscilloscope::addMarker( int xOfs, float yOfs )
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
	
	//____ setMarkerSkin() _________________________________________________________
	
	void WgOscilloscope::setMarkerSkin( const WgSkin_p& pSkin )
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
			canvas = m_pSkin->contentRect(_canvas,m_state);
	
		//
	
		float centerX = canvas.x + canvas.w/2.f;
		float centerY = canvas.y + canvas.h/2.f;
		float scaleX = (canvas.w-1)/2.f;
		float scaleY = (canvas.h-1)/2.f;
	
		// Draw HGridLines
	
		for( int i = 0 ; i < m_nHGridLines ; i++ )
		{
			int ofsY = (int) (m_pHGridLines[i] * scaleY + centerY);
			pDevice->clipDrawHorrLine( _clip, WgCoord(canvas.x,ofsY), canvas.w, m_gridColor );
		}
	
		// Draw VGridLines
	
		for( int i = 0 ; i < m_nVGridLines ; i++ )
		{
			int ofsX = (int) (m_pVGridLines[i] * scaleX + centerX);
			pDevice->clipDrawVertLine( _clip, WgCoord(ofsX,canvas.y), canvas.h, m_gridColor );
		}
	
		// Draw the oscilloscope line
	
		int nPoints = m_nLinePoints > canvas.w ? canvas.w : m_nLinePoints;
	
		int allocSize = sizeof(WgCoord)*nPoints;
	
		WgCoord * pOut = reinterpret_cast<WgCoord*>(WgBase::memStackAlloc(allocSize));
	
		for( int i = 0 ; i < nPoints ; i++ )
		{
			pOut[i] = WgCoord(canvas.x + i, (int)(centerY + scaleY*m_pLinePoints[i]));
		}
	
		pDevice->clipPlotSoftPixels(_clip, nPoints, pOut, m_lineColor, m_lineThickness);
	
		WgBase::memStackRelease(allocSize);
	
		// Render markers
	
		if( m_pMarkerSkin )
		{
			for( int i = 0 ; i < m_nMarkers ; i++ )
			{
				WgRect dest;
	
				int x = m_pMarkers[i].x;
				int y = (int) (m_pMarkers[i].y*scaleY+centerY);
	
				dest.setSize( m_pMarkerSkin->preferredSize() );
				dest.x = x - dest.w / 2;
				dest.y = y - dest.h / 2;
	
				m_pMarkerSkin->render( pDevice, dest, WG_STATE_NORMAL, _clip );
			}
		}
	}
	

} // namespace wg
