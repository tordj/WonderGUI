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

using namespace WgUtil;

const char WgBoxSkin::CLASSNAME[] = {"BoxSkin"};

//____ create() _______________________________________________________________

WgBoxSkin_p WgBoxSkin::create()
{
	return WgBoxSkin_p(new WgBoxSkin());
}

WgBoxSkin_p WgBoxSkin::create( WgColor color, WgBorder frame, WgColor frameColor )
{
	return WgBoxSkin_p(new WgBoxSkin(color, frame, frameColor));
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

WgBoxSkin::WgBoxSkin( WgColor color, WgBorder frame, WgColor frameColor )
{
    m_frame = frame;

	for( int i = 0 ; i < WG_NB_STATES ; i++ )
	{
		m_color[i] = color;
		m_frameColor[i] = frameColor;
	}

	bool hasFrame = (frame.width() + frame.height() > 0 );
	if( color.a == 255 && (!hasFrame || frameColor.a == 255) )
		m_bOpaque = true;
	else
		m_bOpaque = false;
}

//____ isInstanceOf() _________________________________________________________

bool WgBoxSkin::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgExtendedSkin::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgBoxSkin::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgBoxSkin_p WgBoxSkin::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgBoxSkin_p( static_cast<WgBoxSkin*>(pObject.rawPtr()) );

	return 0;
}


//____ setColor() _____________________________________________________________

void WgBoxSkin::setColor( WgColor color )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_color[i] = color;

	if( m_frame.width() + m_frame.height() == 0 )
		m_bOpaque = color.a == 255;
	else if( (color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque) )
		_updateOpaqueFlag();
}

//____ setFrameColor() ________________________________________________________

void WgBoxSkin::setFrameColor( WgColor color )
{
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_frameColor[i] = color;

	bool hasFrame = (m_frame.width() + m_frame.height() > 0 );
	if( hasFrame && ((color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque)) )
		_updateOpaqueFlag();
}

//____ setFrameThickness() ____________________________________________________

void WgBoxSkin::setFrameThickness( WgBorder frame )
{
	bool hadFrame = (m_frame.width() + m_frame.height() > 0 );
	bool hasFrame = (frame.width() + frame.height() > 0);

	m_frame = frame;

	if( hadFrame != hasFrame )
		_updateOpaqueFlag();
}

//____ setFrame() _____________________________________________________________

void WgBoxSkin::setFrame( WgBorder frame, WgColor color )
{
	m_frame = frame;
	for( int i = 0 ; i < WG_NB_STATES ; i++ )
		m_frameColor[i] = color;

	if( (color.a == 255 && !m_bOpaque) || (color.a < 255 && m_bOpaque) )
		_updateOpaqueFlag();
}

//____ setStateColor() _________________________________________________________

void WgBoxSkin::setStateColor( WgStateEnum state, WgColor color )
{
	int i = _stateToIndex(state);

	Uint8	oldAlpha = m_color[i].a;
	m_color[i] = color;
	if( (color.a < 255 && m_bOpaque) || (color.a == 255 && oldAlpha < 255 ) )
		_updateOpaqueFlag();
}

void WgBoxSkin::setStateColor( WgStateEnum state, WgColor color, WgColor frameColor )
{
	int i = _stateToIndex(state);

	int		oldCombAlpha = ((int)m_color[i].a) + ((int)m_frameColor[i].a);
	int		newCombAlpha = ((int)color.a) + ((int)frameColor.a);

	m_color[i]		= color;
	m_frameColor[i] = frameColor;

	if( (newCombAlpha < 510 && m_bOpaque) || (newCombAlpha == 510 && oldCombAlpha < 510 ) )
		_updateOpaqueFlag();
}

//____ render() _______________________________________________________________
	
void WgBoxSkin::render( WgGfxDevice * pDevice, const WgRect& _canvas, WgState state, const WgRect& _clip ) const
{
	int i = _stateToIndex(state);
	if( m_frame.width() + m_frame.height() == 0 )
	{
		pDevice->fill( _clip, m_color[i] );
	}
	else
	{
		WgRect top( WgRect(_canvas.x, _canvas.y, _canvas.w, m_frame.top), _clip );
		WgRect left( WgRect(_canvas.x, _canvas.y+m_frame.top, m_frame.left, _canvas.h - m_frame.height()), _clip );
		WgRect right( WgRect(_canvas.x + _canvas.w - m_frame.right, _canvas.y+m_frame.top, m_frame.right, _canvas.h - m_frame.height()), _clip );
		WgRect bottom( WgRect(_canvas.x, _canvas.y + _canvas.h - m_frame.bottom, _canvas.w, m_frame.bottom), _clip );
		WgRect center( _canvas - m_frame, _clip );

		pDevice->fill( top, m_frameColor[i] );
		pDevice->fill( left, m_frameColor[i] );
		pDevice->fill( right, m_frameColor[i] );
		pDevice->fill( bottom, m_frameColor[i] );

		if( center.w > 0 || center.h > 0 )
			pDevice->fill( center, m_color[i] );
	}
}

//____ minSize() ______________________________________________________________

WgSize WgBoxSkin::minSize() const
{
	WgSize content = WgExtendedSkin::minSize();
	WgSize frame = m_frame.size();

	return WgSize( WgMax(content.w,frame.w), WgMax(content.h,frame.h) );
}

//____ preferredSize() ________________________________________________________

WgSize WgBoxSkin::preferredSize() const
{
	WgSize content = WgExtendedSkin::preferredSize();
	WgSize frame = m_frame.size();

	return WgSize( WgMax(content.w,frame.w), WgMax(content.h,frame.h) );
}

//____ sizeForContent() _______________________________________________________

WgSize WgBoxSkin::sizeForContent( const WgSize contentSize ) const
{
	WgSize content = WgExtendedSkin::sizeForContent(contentSize);
	WgSize frame = m_frame.size();

	return WgSize( WgMax(content.w,frame.w), WgMax(content.h,frame.h) );
}

//____ markTest() _____________________________________________________________

bool WgBoxSkin::markTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const
{
	if( !canvas.contains(ofs) )
		return false;

	int opacity;

	if( m_bOpaque )
		opacity = 255;
	else
	{
		int i = _stateToIndex(state);

		WgRect center = canvas - m_frame;
		if( center.contains(ofs) )
			opacity = m_color[i].a;
		else
			opacity = m_frameColor[i].a;
	}

	return ( opacity >= opacityTreshold );
}

//____ isOpaque() _____________________________________________________________

bool WgBoxSkin::isOpaque() const
{
	return m_bOpaque;
}

bool WgBoxSkin::isOpaque( WgState state ) const
{
	int i = _stateToIndex(state);
	if( m_bOpaque || (m_color[i].a == 255 && (m_frameColor[i] == 255 || (m_frame.width() + m_frame.height() == 0))) )
		return true;

	return false;
}

bool WgBoxSkin::isOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const
{
	if( m_bOpaque )
		return true;

	WgRect center = WgRect(canvasSize) - m_frame;
	int i = _stateToIndex(state);
	if( center.contains(rect) )
		return m_color[i].a == 255;
	else if( !center.intersectsWith(rect) )
		return m_frameColor[i].a == 255;

	return m_color[i].a == 255 && m_frameColor[i].a == 255;
}

//____ isStateIdentical() ____________________________________________________

bool WgBoxSkin::isStateIdentical( WgState state, WgState comparedTo ) const
{
	int i1 = _stateToIndex(state);
	int i2 = _stateToIndex(comparedTo);

	if( m_color[i1] == m_color[i2] && (m_frame.isEmpty() || m_frameColor[i1] == m_frameColor[i2]) && 
		WgExtendedSkin::isStateIdentical(state,comparedTo) )
		return true;
	else
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

	bool hasFrame = (m_frame.width() + m_frame.height() > 0);
	
	if( alpha == 255*WG_NB_STATES && (!hasFrame || frameAlpha == 255*WG_NB_STATES) )
		m_bOpaque = true;
	else
		m_bOpaque = false;
}
