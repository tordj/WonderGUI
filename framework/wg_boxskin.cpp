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
#include <wg_util.h>

#include <algorithm>

#ifdef SOFTUBE_USE_PACE_FUSION
#include "PaceFusion.h"
#endif

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
		m_color[i] = WgColor::Black;
		m_frameColor[i] = WgColor::Black;
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

	bool hasFrame = frame.width() + frame.height();
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

	if( m_frame.width() + m_frame.height() == 0 )
		m_bOpaque = color.a == 255;
	else if( (color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque) )
		_updateOpaqueFlag();
}

//____ SetFrameColor() ________________________________________________________

void WgBoxSkin::SetFrameColor( WgColor color )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_frameColor[i] = color;

	bool hasFrame = m_frame.width() + m_frame.height();
	if( hasFrame && ((color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque)) )
		_updateOpaqueFlag();
}

//____ SetFrameThickness() ____________________________________________________

void WgBoxSkin::SetFrameThickness( WgBorders frame )
{
	bool hadFrame = m_frame.width() + m_frame.height();
	bool hasFrame = frame.width() + frame.height();

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
	int i = WgUtil::_stateToIndex(state);

	Uint8	oldAlpha = m_color[i].a;
	m_color[i] = color;
	if( (color.a < 255 && m_bOpaque) || (color.a == 255 && oldAlpha < 255 ) )
		_updateOpaqueFlag();
}

void WgBoxSkin::SetStateColor( WgStateEnum state, WgColor color, WgColor frameColor )
{
	int i = WgUtil::_stateToIndex(state);

	int		oldCombAlpha = ((int)m_color[i].a) + ((int)m_frameColor[i].a);
	int		newCombAlpha = ((int)color.a) + ((int)frameColor.a);

	m_color[i]		= color;
	m_frameColor[i] = frameColor;

	if( (newCombAlpha < 510 && m_bOpaque) || (newCombAlpha == 510 && oldCombAlpha < 510 ) )
		_updateOpaqueFlag();
}

//____ Render() _______________________________________________________________
	
#ifdef SOFTUBE_USE_PACE_FUSION
PACE_FUSION_NO_USER_CALLBACK
#endif
void WgBoxSkin::Render( wg::GfxDevice * pDevice, WgState state, const WgRect& _canvas, int scale ) const
{
	int i = WgUtil::_stateToIndex(state);
	if( m_frame.width() + m_frame.height() == 0 )
	{
		pDevice->fill( _canvas, m_color[i] );
	}
	else
	{
		WgBorders frame(m_frame.top *scale >> WG_SCALE_BINALS, m_frame.right *scale >> WG_SCALE_BINALS, m_frame.bottom *scale >> WG_SCALE_BINALS, m_frame.left *scale >> WG_SCALE_BINALS);

		WgRect top(_canvas.x, _canvas.y, _canvas.w, frame.top);
		WgRect left(_canvas.x, _canvas.y+frame.top, frame.left, _canvas.h - frame.height());
		WgRect right( _canvas.x + _canvas.w - frame.right, _canvas.y+frame.top, frame.right, _canvas.h - frame.height());
		WgRect bottom( _canvas.x, _canvas.y + _canvas.h - frame.bottom, _canvas.w, frame.bottom);
		WgRect center( _canvas - frame );

		pDevice->fill( top, m_frameColor[i] );
		pDevice->fill( left, m_frameColor[i] );
		pDevice->fill( right, m_frameColor[i] );
		pDevice->fill( bottom, m_frameColor[i] );

		if( center.w > 0 || center.h > 0 )
			pDevice->fill( center, m_color[i] );
	}
}

//____ MinSize() ______________________________________________________________

WgSize WgBoxSkin::MinSize(int scale) const
{
	WgSize content = WgExtendedSkin::MinSize(scale);
	WgSize frame = _scaledFrame(scale);

	return WgSize( std::max(content.w,frame.w), std::max(content.h,frame.h) );
}

//____ PreferredSize() ________________________________________________________

WgSize WgBoxSkin::PreferredSize(int scale) const
{
	WgSize content = WgExtendedSkin::PreferredSize(scale);
	WgSize frame = _scaledFrame(scale);

	return WgSize( std::max(content.w,frame.w), std::max(content.h,frame.h) );
}

//____ SizeForContent() _______________________________________________________

WgSize WgBoxSkin::SizeForContent( const WgSize contentSize, int scale ) const
{
	WgSize content = WgExtendedSkin::SizeForContent(contentSize, scale);
	WgSize frame = _scaledFrame(scale);

	return WgSize( std::max(content.w,frame.w), std::max(content.h,frame.h) );
}

//____ MarkTest() _____________________________________________________________

bool WgBoxSkin::MarkTest( const WgCoord& ofs, const WgSize& canvasSize, WgState state, int opacityTreshold, int scale ) const
{
    if( ofs.x < 0 || ofs.y < 0 || ofs.x > canvasSize.w || ofs.y > canvasSize.h )
        return false;
        
	if( m_bOpaque )
		return true;
    
	int i = WgUtil::_stateToIndex(state);

	WgRect center = ContentRect(WgRect(canvasSize), state, scale);
	if( center.contains(ofs) )
		return m_color[i].a >= opacityTreshold;
	else
		return m_frameColor[i].a >= opacityTreshold;
}


//____ IsOpaque() _____________________________________________________________

#ifdef SOFTUBE_USE_PACE_FUSION
PACE_FUSION_NO_USER_CALLBACK
#endif
bool WgBoxSkin::IsOpaque() const
{
	return m_bOpaque;
}

bool WgBoxSkin::IsOpaque( WgState state ) const
{
	int i = WgUtil::_stateToIndex(state);
	if( m_bOpaque || (m_color[i].a == 255 && (m_frameColor[i] == 255 || (m_frame.width() + m_frame.height() == 0))) )
		return true;

	return false;
}

bool WgBoxSkin::IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state, int scale ) const
{
	if( m_bOpaque )
		return true;

	WgRect center = ContentRect(WgRect(canvasSize), state, scale);
	int i = WgUtil::_stateToIndex(state);
	if( center.contains(rect) )
		return m_color[i].a == 255;
	else if( !center.intersectsWith(rect) )
		return m_frameColor[i].a == 255;

	return m_color[i].a == 255 && m_frameColor[i].a == 255;
}

//____ IsStateIdentical() _____________________________________________________

bool WgBoxSkin::IsStateIdentical(WgState state, WgState comparedTo) const
{
	int i1 = WgUtil::_stateToIndex(state);
	int i2 = WgUtil::_stateToIndex(comparedTo);

	if (m_color[i1] == m_color[i2] && m_frameColor[i1] == m_frameColor[i2] && WgExtendedSkin::IsStateIdentical(state, comparedTo))
		return true;

	return false;
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

	bool hasFrame = m_frame.width() + m_frame.height();
	
	if( alpha == 255*WG_NB_STATES && (!hasFrame || frameAlpha == 255*WG_NB_STATES) )
		m_bOpaque = true;
	else
		m_bOpaque = false;
}
