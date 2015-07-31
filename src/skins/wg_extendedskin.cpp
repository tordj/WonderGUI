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

namespace wg 
{
	
	using namespace WgUtil;
	
	const char ExtendedSkin::CLASSNAME[] = {"ExtendedSkin"};
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ExtendedSkin::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Skin::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ExtendedSkin::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ExtendedSkin_p ExtendedSkin::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ExtendedSkin_p( static_cast<ExtendedSkin*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setContentPadding() ____________________________________________________
	
	void ExtendedSkin::setContentPadding( Border padding )
	{
		m_contentPadding = padding;
	}
	
	//____ setContentShift() ______________________________________________________
	
	void ExtendedSkin::setContentShift( StateEnum state, Coord shift )
	{
		int index = _stateToIndex(state);
		m_contentShift[index] = shift;
	}
	
	//____ setHoveredContentShift() _______________________________________________
	
	void ExtendedSkin::setHoveredContentShift( Coord shift )
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
	
	void ExtendedSkin::setPressedContentShift( Coord shift )
	{
		m_contentShift[_stateToIndex(WG_STATE_PRESSED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_PRESSED_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)] = shift;
	}
	
	//____ setSelectedContentShift() ______________________________________________
	
	void ExtendedSkin::setSelectedContentShift( Coord shift )
	{
		m_contentShift[_stateToIndex(WG_STATE_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_FOCUSED_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_HOVERED_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_PRESSED_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)] = shift;
	}
	
	//____ setFocusedContentShift() _______________________________________________
	
	void ExtendedSkin::setFocusedContentShift( Coord shift )
	{
		m_contentShift[_stateToIndex(WG_STATE_FOCUSED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_FOCUSED_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_HOVERED_FOCUSED_SELECTED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED)] = shift;
		m_contentShift[_stateToIndex(WG_STATE_PRESSED_FOCUSED_SELECTED)] = shift;
	}
	
	//____ minSize() ______________________________________________________________
	
	Size ExtendedSkin::minSize() const
	{
		return Size(m_contentPadding.width(), m_contentPadding.height() );
	}
	
	//____ minSize() ______________________________________________________________
	
	Size ExtendedSkin::preferredSize() const
	{
		return Size(m_contentPadding.width(), m_contentPadding.height() );
	}
	
	//____ sizeForContent() _______________________________________________________
	
	Size ExtendedSkin::sizeForContent( const Size contentSize ) const
	{
		return contentSize + m_contentPadding;
	}
	
	//____ contentPadding() _______________________________________________________
	
	Size ExtendedSkin::contentPadding() const
	{
		return m_contentPadding.size();
	}
	
	
	//____ contentRect() __________________________________________________________
	
	Rect ExtendedSkin::contentRect( const Rect& canvas, State state ) const
	{
		return (canvas - m_contentPadding) + m_contentShift[_stateToIndex(state)];
	}
	
	//____ isStateIdentical() ______________________________________________________
	
	bool ExtendedSkin::isStateIdentical( State state, State comparedTo ) const
	{
		return ( m_contentShift[_stateToIndex(state)] == m_contentShift[_stateToIndex(comparedTo)] );
	}

} // namespace wg
