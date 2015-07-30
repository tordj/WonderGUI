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

#include <wg_extendedskin.h>
#include <wg_util.h>

using namespace WgUtil;

const char WgExtendedSkin::CLASSNAME[] = {"ExtendedSkin"};

//____ isInstanceOf() _________________________________________________________

bool WgExtendedSkin::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgSkin::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgExtendedSkin::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgExtendedSkinPtr WgExtendedSkin::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgExtendedSkinPtr( static_cast<WgExtendedSkin*>(pObject.rawPtr()) );

	return 0;
}


//____ setContentPadding() ____________________________________________________

void WgExtendedSkin::setContentPadding( WgBorder padding )
{
	m_contentPadding = padding;
}

//____ setContentShift() ______________________________________________________

void WgExtendedSkin::setContentShift( WgStateEnum state, WgCoord shift )
{
	int index = _stateToIndex(state);
	m_contentShift[index] = shift;
}

//____ setHoveredContentShift() _______________________________________________

void WgExtendedSkin::setHoveredContentShift( WgCoord shift )
{
	m_contentShift[_stateToIndex(WG_STATE_HOVERED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_HOVERED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)] = shift;

	m_contentShift[_stateToIndex(WG_STATE_PRESSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)] = shift;
}

//____ setPressedContentShift() _______________________________________________

void WgExtendedSkin::setPressedContentShift( WgCoord shift )
{
	m_contentShift[_stateToIndex(WG_STATE_PRESSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)] = shift;
}

//____ setSelectedContentShift() ______________________________________________

void WgExtendedSkin::setSelectedContentShift( WgCoord shift )
{
	m_contentShift[_stateToIndex(WG_STATE_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_FOCUSED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_HOVERED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)] = shift;
}

//____ setFocusedContentShift() _______________________________________________

void WgExtendedSkin::setFocusedContentShift( WgCoord shift )
{
	m_contentShift[_stateToIndex(WG_STATE_FOCUSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_FOCUSED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED)] = shift;
	m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)] = shift;
}

//____ minSize() ______________________________________________________________

WgSize WgExtendedSkin::minSize() const
{
	return WgSize(m_contentPadding.width(), m_contentPadding.height() );
}

//____ minSize() ______________________________________________________________

WgSize WgExtendedSkin::preferredSize() const
{
	return WgSize(m_contentPadding.width(), m_contentPadding.height() );
}

//____ sizeForContent() _______________________________________________________

WgSize WgExtendedSkin::sizeForContent( const WgSize contentSize ) const
{
	return contentSize + m_contentPadding;
}

//____ contentPadding() _______________________________________________________

WgSize WgExtendedSkin::contentPadding() const
{
	return m_contentPadding.size();
}


//____ contentRect() __________________________________________________________

WgRect WgExtendedSkin::contentRect( const WgRect& canvas, WgState state ) const
{
	return (canvas - m_contentPadding) + m_contentShift[_stateToIndex(state)];
}

//____ isStateIdentical() ______________________________________________________

bool WgExtendedSkin::isStateIdentical( WgState state, WgState comparedTo ) const
{
	return ( m_contentShift[_stateToIndex(state)] == m_contentShift[_stateToIndex(comparedTo)] );
}
