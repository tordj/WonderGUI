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

#include <wg_boxskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>

//____ Create() _______________________________________________________________

WgBoxSkinPtr WgBoxSkin::Create()
{
	return WgBoxSkinPtr(new WgBoxSkin());
}

WgBoxSkinPtr WgBoxSkin::Create( WgColor color, WgBorders frame, WgColor frameColor )
{
	return WgBoxSkinPtr(new WgBoxSkin(color, frame, frameColor));
}

//____ Constructor ____________________________________________________________

WgBoxSkin::WgBoxSkin()
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_color[i] = WgColor::black;
		m_frameColor[i] = WgColor::black;
	}

	m_bOpaque = true;
}

WgBoxSkin::WgBoxSkin( WgColor color, WgBorders frame, WgColor frameColor )
{
    m_frame = frame;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_color[i] = color;
		m_frameColor[i] = frameColor;
	}

	bool hasFrame = frame.Width() + frame.Height();
	if( color.a == 255 && (!hasFrame || frameColor.a == 255) )
		m_bOpaque = true;
	else
		m_bOpaque = false;
}

//____ SetColor() _____________________________________________________________

void WgBoxSkin::SetColor( WgColor color )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_color[i] = color;

	if( m_frame.Width() + m_frame.Height() == 0 )
		m_bOpaque = color.a == 255;
	else if( (color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque) )
		_updateOpaqueFlag();
}

//____ SetFrameColor() ________________________________________________________

void WgBoxSkin::SetFrameColor( WgColor color )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_frameColor[i] = color;

	bool hasFrame = m_frame.Width() + m_frame.Height();
	if( hasFrame && ((color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque)) )
		_updateOpaqueFlag();
}

//____ SetFrameThickness() ____________________________________________________

void WgBoxSkin::SetFrameThickness( WgBorders frame )
{
	bool hadFrame = m_frame.Width() + m_frame.Height();
	bool hasFrame = frame.Width() + frame.Height();

	m_frame = frame;

	if( hadFrame != hasFrame )
		_updateOpaqueFlag();
}

//____ SetFrame() _____________________________________________________________

void WgBoxSkin::SetFrame( WgBorders frame, WgColor color )
{
	m_frame = frame;
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_frameColor[i] = color;

	if( (color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque) )
		_updateOpaqueFlag();
}

//____ SetSateColor() _________________________________________________________

void WgBoxSkin::SetStateColor( WgStateEnum state, WgColor color )
{
	int i = _stateToIndex(state);

	Uint8	oldAlpha = m_color[i].a;
	m_color[i] = color;
	if( (color.a < 255 && m_bOpaque) || (color.a == 255 && oldAlpha < 255 ) )
		_updateOpaqueFlag();
}

void WgBoxSkin::SetStateColor( WgStateEnum state, WgColor color, WgColor frameColor )
{
	int i = _stateToIndex(state);

	int		oldCombAlpha = ((int)m_color[i].a) + ((int)m_frameColor[i].a);
	int		newCombAlpha = ((int)color.a) + ((int)frameColor.a);

	m_color[i]		= color;
	m_frameColor[i] = frameColor;

	if( (newCombAlpha < 510 && m_bOpaque) || (newCombAlpha == 510 && oldCombAlpha < 510 ) )
		_updateOpaqueFlag();
}

//____ Render() _______________________________________________________________
	
void WgBoxSkin::Render( WgGfxDevice * pDevice, WgState state, const WgRect& _canvas, const WgRect& _clip ) const
{
	int i = _stateToIndex(state);
	if( m_frame.Width() + m_frame.Height() == 0 )
	{
		pDevice->Fill( _canvas, m_color[i] );
	}
	else
	{
		WgRect top( WgRect(_canvas.x, _canvas.y, _canvas.w, m_frame.top), _clip );
		WgRect left( WgRect(_canvas.x, _canvas.y+m_frame.top, m_frame.left, _canvas.h - m_frame.Height()), _clip );
		WgRect right( WgRect(_canvas.x + _canvas.w - m_frame.right, _canvas.y+m_frame.top, m_frame.right, _canvas.h - m_frame.Height()), _clip );
		WgRect bottom( WgRect(_canvas.x, _canvas.y + _canvas.h - m_frame.bottom, _canvas.w, m_frame.bottom), _clip );
		WgRect center( _canvas - m_frame, _clip );

		pDevice->Fill( top, m_frameColor[i] );
		pDevice->Fill( left, m_frameColor[i] );
		pDevice->Fill( right, m_frameColor[i] );
		pDevice->Fill( bottom, m_frameColor[i] );

		if( center.w > 0 || center.h > 0 )
			pDevice->Fill( center, m_color[i] );
	}
}

//____ MinSize() ______________________________________________________________

WgSize WgBoxSkin::MinSize() const
{
	WgSize content = WgExtendedSkin::MinSize();
	WgSize frame = m_frame.Size();

	return WgSize( WgMax(content.w,frame.w), WgMax(content.h,frame.h) );
}

//____ PreferredSize() ________________________________________________________

WgSize WgBoxSkin::PreferredSize() const
{
	WgSize content = WgExtendedSkin::PreferredSize();
	WgSize frame = m_frame.Size();

	return WgSize( WgMax(content.w,frame.w), WgMax(content.h,frame.h) );
}

//____ SizeForContent() _______________________________________________________

WgSize WgBoxSkin::SizeForContent( const WgSize contentSize ) const
{
	WgSize content = WgExtendedSkin::SizeForContent(contentSize);
	WgSize frame = m_frame.Size();

	return WgSize( WgMax(content.w,frame.w), WgMax(content.h,frame.h) );
}

//____ MarkTest() _____________________________________________________________

bool WgBoxSkin::MarkTest( const WgCoord& ofs, const WgSize& canvasSize, WgState state ) const
{
	if( m_bOpaque )
		return true;

	int i = _stateToIndex(state);

	WgRect center = WgRect(canvasSize) - m_frame;
	if( center.Contains(ofs) )
		return m_color[i].a > 0;
	else
		return m_frameColor[i].a > 0;
}

//____ IsOpaque() _____________________________________________________________

bool WgBoxSkin::IsOpaque() const
{
	return m_bOpaque;
}

bool WgBoxSkin::IsOpaque( WgState state ) const
{
	int i = _stateToIndex(state);
	if( m_bOpaque || (m_color[i].a == 255 && (m_frameColor[i] == 255 || (m_frame.Width() + m_frame.Height() == 0))) )
		return true;

	return false;
}

bool WgBoxSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const
{
	if( m_bOpaque )
		return true;

	WgRect center = WgRect(canvasSize) - m_frame;
	int i = _stateToIndex(state);
	if( center.Contains(rect) )
		return m_color[i].a == 255;
	else if( !center.IntersectsWith(rect) )
		return m_frameColor[i].a == 255;

	return m_color[i].a == 255 && m_frameColor[i].a == 255;
}

//____ _updateOpaqueFlag() ____________________________________________________

void WgBoxSkin::_updateOpaqueFlag()
{
	int alpha = 0;
	int frameAlpha = 0;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		alpha += (int) m_color[i].a;
		frameAlpha += (int) m_frameColor[i].a;
	}

	bool hasFrame = m_frame.Width() + m_frame.Height();
	
	if( alpha == 255*WG_NB_STATES && (!hasFrame || frameAlpha == 255*WG_NB_STATES) )
		m_bOpaque = true;
	else
		m_bOpaque = false;
}
