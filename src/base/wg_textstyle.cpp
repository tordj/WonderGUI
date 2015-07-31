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

#include <wg_textstyle.h>

namespace wg 
{
	
	const char TextStyle::CLASSNAME[] = {"TextStyle"};
	
	
	
	//____ Constructor _____________________________________________________________
	
	TextStyle::TextStyle()
	{
		m_pFirstChild = 0;
		m_pNextSibling = 0;
		m_pPrevSibling = 0;
	
		_clearSet( &m_specAttr );
		_clearSet( &m_combAttr );
	}
	
	
	//____ Destructor ______________________________________________________________
	
	TextStyle::~TextStyle()
	{
		if( m_pNextSibling )
			m_pNextSibling->m_pPrevSibling = m_pPrevSibling;
	
		if( m_pPrevSibling )
			m_pPrevSibling->m_pNextSibling = m_pNextSibling;
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool TextStyle::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * TextStyle::className( void ) const
	{
		return CLASSNAME;
	}
	
	//____ cast() _________________________________________________________________
	
	TextStyle_p TextStyle::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextStyle_p( static_cast<TextStyle*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setParent() _____________________________________________________________
	
	bool TextStyle::setParent( const TextStyle_p& pParent )
	{
		// Check so we don't get circular references.
	
		if( pParent )
		{
			TextStyle * p = pParent.rawPtr();
			while( p != 0 && p != this )
				p = p->m_pParent.rawPtr();
	
			if( p == this )
				return false;
		}
	
		//
	
		if( m_pParent )
		{
			if( m_pNextSibling )
				m_pNextSibling->m_pPrevSibling = m_pPrevSibling;
	
			if( m_pPrevSibling )
				m_pPrevSibling->m_pNextSibling = m_pNextSibling;
			else
				m_pParent->m_pFirstChild = m_pNextSibling;
		}
	
		m_pParent = pParent;
		if( pParent )
		{
			m_pNextSibling = m_pParent->m_pFirstChild;
			m_pPrevSibling = 0;
			m_pParent->m_pFirstChild = this;
		}
	
		// Update combined values
	
		_refreshComb();
		return true;
	}
	
	
	//____ cascade() _______________________________________________________________
	
	void TextStyle::cascade()
	{
		TextStyle * pChild = m_pFirstChild;
		while( pChild )
		{
			if( pChild->_refreshComb() )
				pChild->cascade();
	
			pChild = pChild->m_pNextSibling;
		}
	}
	
	//____ setFont() _______________________________________________________________
	
	void TextStyle::setFont( const Font_p& pFont )
	{
		if( pFont != m_specAttr.pFont )
		{
			m_specAttr.pFont = pFont;
			if( !pFont && m_pParent )
				m_combAttr.pFont = m_pParent->m_combAttr.pFont;
			else
				m_combAttr.pFont = pFont;
		}
	}
	
	//____ setLink() _______________________________________________________________
	
	void TextStyle::setLink( const TextLink_p& pLink )
	{
		if( pLink != m_specAttr.pLink )
		{
			m_specAttr.pLink = pLink;
			if( !pLink && m_pParent )
				m_combAttr.pLink = m_pParent->m_combAttr.pLink;
			else
				m_combAttr.pLink = pLink;
		}
	}
	
	//____ setColor() ______________________________________________________________
	
	void TextStyle::setColor( Color color )
	{
		if( color == Color::none )
			clearColor();
		else
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.color[i] = color;
				m_combAttr.color[i] = color;
			}
		}}
	
	void TextStyle::setColor( Color color, State state )
	{
		if( color == Color::none )
			clearColor(state);
		else
		{
			int idx = WgUtil::_stateToIndex(state);
			m_specAttr.color[idx] = color;
			m_combAttr.color[idx] = color;
		}
	}
	
	//____ setBgColor() ______________________________________________________________
	
	void TextStyle::setBgColor( Color color )
	{
		if( color == Color::none )
			clearBgColor();
		else
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.bgColor[i] = color;
				m_combAttr.bgColor[i] = color;
			}
		}
	}
	
	void TextStyle::setBgColor( Color color, State state )
	{
		if( color == Color::none )
			clearBgColor(state);
		else
		{
			int idx = WgUtil::_stateToIndex(state);
			m_specAttr.bgColor[idx] = color;
			m_combAttr.bgColor[idx] = color;
		}
	}
	
	//____ setSize() _______________________________________________________________
	
	void TextStyle::setSize( int size )
	{
		if( size == WG_FONTSIZE_INHERIT )
			clearSize();
		else
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.size[i] = size;
				m_combAttr.size[i] = size;
			}
		}
	}
	
	void TextStyle::setSize( int size, State state )
	{
		if( size == WG_FONTSIZE_INHERIT )
			clearSize(state);
		else
		{
			int idx = WgUtil::_stateToIndex(state);
			m_specAttr.size[idx] = size;
			m_combAttr.size[idx] = size;
		}
	}
	
	
	//____ setDecoration() _________________________________________________________
	
	void TextStyle::setDecoration( TextDecoration decoration )
	{
		if( decoration == WG_DECORATION_INHERIT )
			clearDecoration();
		else
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.decoration[i] = decoration;
				m_combAttr.decoration[i] = decoration;
			}
		}
	}
	
	void TextStyle::setDecoration( TextDecoration decoration, State state )
	{
		if( decoration == WG_DECORATION_INHERIT )
			clearDecoration(state);
		else
		{
			int idx = WgUtil::_stateToIndex(state);
			m_specAttr.decoration[idx] = decoration;
			m_combAttr.decoration[idx] = decoration;
		}
	}
	
	
	//____ clearFont() _____________________________________________________________
	
	void TextStyle::clearFont()
	{
		m_specAttr.pFont = 0;
		if( m_pParent )
			m_combAttr.pFont = m_pParent->m_combAttr.pFont;
		else
			m_combAttr.pFont = 0;
	}
	
	//____ clearLink() _____________________________________________________________
	
	void TextStyle::clearLink()
	{
		m_specAttr.pLink = 0;
		if( m_pParent )
			m_combAttr.pLink = m_pParent->m_combAttr.pLink;
		else
			m_combAttr.pLink = 0;
	}
	
	//____ clearColor() ____________________________________________________________
	
	void TextStyle::clearColor()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.color[i] = Color::none;
				m_combAttr.color[i] = m_pParent->m_combAttr.color[i];
			}
		}
		else
		{		for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.color[i] = Color::none;
				m_combAttr.color[i] = Color::none;
			}
		}
	}
	
	void TextStyle::clearColor( State state )
	{
		int idx = WgUtil::_stateToIndex(state);
	
		m_specAttr.color[idx] = Color::none;
		m_combAttr.color[idx] = m_pParent ? m_pParent->m_combAttr.color[idx] : Color::none;
	}
	
	
	//____ clearBgColor() ____________________________________________________________
	
	void TextStyle::clearBgColor()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.bgColor[i] = Color::none;
				m_combAttr.bgColor[i] = m_pParent->m_combAttr.bgColor[i];
			}
		}
		else
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.bgColor[i] = Color::none;
				m_combAttr.bgColor[i] = Color::none;
			}
		}
	}
	
	void TextStyle::clearBgColor( State state )
	{
		int idx = WgUtil::_stateToIndex(state);
	
		m_specAttr.bgColor[idx] = Color::none;
		m_combAttr.bgColor[idx] = m_pParent ? m_pParent->m_combAttr.bgColor[idx] : Color::none;
	}
	
	//____ clearSize() ____________________________________________________________
	
	void TextStyle::clearSize()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.size[i] = WG_FONTSIZE_INHERIT;
				m_combAttr.size[i] = m_pParent->m_combAttr.size[i];
			}
		}
		else
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.size[i] = WG_FONTSIZE_INHERIT;
				m_combAttr.size[i] = WG_FONTSIZE_INHERIT;
			}
		}
	}
	
	void TextStyle::clearSize( State state )
	{
		int idx = WgUtil::_stateToIndex(state);
	
		m_specAttr.size[idx] = WG_FONTSIZE_INHERIT;
		m_combAttr.size[idx] = m_pParent ? m_pParent->m_combAttr.size[idx] : WG_FONTSIZE_INHERIT;
	}
	
	//____ clearDecoration() ____________________________________________________________
	
	void TextStyle::clearDecoration()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.decoration[i] = WG_DECORATION_INHERIT;
				m_combAttr.decoration[i] = m_pParent->m_combAttr.decoration[i];
			}
		}
		else
		{
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				m_specAttr.decoration[i] = WG_DECORATION_INHERIT;
				m_combAttr.decoration[i] = WG_DECORATION_INHERIT;
			}
		}
	}
	
	void TextStyle::clearDecoration( State state )
	{
		int idx = WgUtil::_stateToIndex(state);
	
		m_specAttr.decoration[idx] = WG_DECORATION_INHERIT;
		m_combAttr.decoration[idx] = m_pParent ? m_pParent->m_combAttr.decoration[idx] : WG_DECORATION_INHERIT;
	}
	
	//____ exportAttr() ____________________________________________________________
	
	void TextStyle::exportAttr( State state, TextAttr2 * pDest ) const
	{
		int idx = WgUtil::_stateToIndex(state);
	
		pDest->pFont 		= m_combAttr.pFont;
		pDest->pLink 		= m_combAttr.pLink;
		pDest->size 		= m_combAttr.size[idx];
		pDest->color		= m_combAttr.color[idx];
		pDest->bgColor		= m_combAttr.bgColor[idx];
		pDest->decoration	= m_combAttr.decoration[idx];
				
		if( pDest->size == WG_FONTSIZE_INHERIT )
			pDest->size = 12;								// Default to size 12.
	}
	
	//____ addToAttr() _____________________________________________________________
	
	void TextStyle::addToAttr( State state, TextAttr2 * pDest ) const 
	{
		int idx = WgUtil::_stateToIndex(state);
	
		if( m_combAttr.pFont )
			pDest->pFont = m_combAttr.pFont;
		if( m_combAttr.pLink )
			pDest->pLink = m_combAttr.pLink;
		if( m_combAttr.size[idx] != WG_FONTSIZE_INHERIT )
			pDest->size	= m_combAttr.size[idx];
		if( m_combAttr.color[idx] != Color::none )
			pDest->color = m_combAttr.color[idx];
		if( m_combAttr.bgColor[idx] != Color::none )
			pDest->bgColor = m_combAttr.bgColor[idx];
		if( m_combAttr.decoration[idx] != WG_DECORATION_INHERIT )
			pDest->decoration = m_combAttr.decoration[idx];
	}
	
	
	//____ refreshComb() ___________________________________________________________
	
	bool TextStyle::_refreshComb()
	{
		if( m_pParent )
		{
			AttrSet		newComb;
	
			newComb.pFont = m_specAttr.pFont ? m_specAttr.pFont : m_pParent->m_combAttr.pFont;
			newComb.pLink = m_specAttr.pLink ? m_specAttr.pLink : m_pParent->m_combAttr.pLink;
	
	
			for( int i = 0 ; i < WG_NB_STATES ; i++ )
			{
				newComb.size[i] = m_specAttr.size[i] != WG_FONTSIZE_INHERIT ? m_specAttr.size[i] : m_pParent->m_combAttr.size[i];
				newComb.color[i] = m_specAttr.color[i] != Color::none ? m_specAttr.color[i] : m_pParent->m_combAttr.color[i];
				newComb.bgColor[i] = m_specAttr.bgColor[i] != Color::none ? m_specAttr.bgColor[i] : m_pParent->m_combAttr.bgColor[i];
				newComb.decoration[i] = m_specAttr.decoration[i] != WG_DECORATION_INHERIT ? m_specAttr.decoration[i] : m_pParent->m_combAttr.decoration[i];
			}
	
			if( _compareSets( &newComb, &m_combAttr ) == false )
			{
				m_combAttr = newComb;
				return true;
			}
			return false;
		}
		else
		{
			bool retVal = !_compareSets( &m_specAttr, &m_combAttr );
			m_combAttr = m_specAttr;
			return retVal;
		}
	}
	
	//____ _clearSet() _____________________________________________________________
	
	void TextStyle::_clearSet( TextStyle::AttrSet * pSet )
	{
		pSet->pFont = 0;
		pSet->pLink = 0;
	
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			pSet->size[i] 		= WG_FONTSIZE_INHERIT;
			pSet->color[i] 		= Color::none;
			pSet->bgColor[i] 	= Color::none;
			pSet->decoration[i] = WG_DECORATION_INHERIT;
		}
	}
	
	//____ _compareSets() __________________________________________________________
	
	bool TextStyle::_compareSets( TextStyle::AttrSet * p1, TextStyle::AttrSet * p2 )
	{
		if( p1->pFont != p2->pFont || p1->pLink != p2->pLink )
			return false;
	
		for( int i = 0 ; i < WG_NB_STATES ; i++ )
		{
			if( p1->size[i] 		!= p2->size[i] ||
				p1->color[i] 		!= p2->color[i] ||
				p1->bgColor[i] 		!= p2->bgColor[i] ||
				p1->decoration[i] 	!= p2->decoration[i] )
				return false;
		}
	
		return true;
	}

} // namespace wg
