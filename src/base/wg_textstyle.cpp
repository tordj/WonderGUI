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
#include <wg_textstylemanager.h>

namespace wg 
{
	
	const char TextStyle::CLASSNAME[] = {"TextStyle"};
	
	
	
	//____ Constructor _____________________________________________________________
	
	TextStyle::TextStyle()
	{
		m_handle = TextStyleManager::_reserveHandle(this);
		
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
			
		TextStyleManager::_releaseHandle(m_handle);
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
	
	TextStyle_p TextStyle::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextStyle_p( static_cast<TextStyle*>(pObject) );
	
		return 0;
	}
	
	//____ setParent() _____________________________________________________________
	
	bool TextStyle::setParent( TextStyle * pParent )
	{
		// Check so we don't get circular references.
	
		if( pParent )
		{
			TextStyle * p = pParent;
			while( p != 0 && p != this )
				p = p->m_pParent;
	
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
	
	void TextStyle::setFont( Font * pFont )
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
	
	void TextStyle::setLink( TextLink * pLink )
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
	
	void TextStyle::setColor( Color color, BlendMode operation )
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.colorBlendMode[i] = operation;
				m_specAttr.color[i] = color;

				m_combAttr.color[i] = Color::blend( m_pParent->m_combAttr.color[i], color, operation );

				// This doesn't always blends colors as logically expected when adding together attribute structures,
				// but covers most common and important cases.
				//
				// It always gets right if attr-structures that gets added either:
				//
				// 1. Only has one level where color is set in its TextStyle hierarchy.
				// 2. Has a BlendMode::Replace somewhere in its TextStyle hierarchy.

				BlendMode parentOp = m_pParent->m_combAttr.colorBlendMode[i];
				if( parentOp == BlendMode::Undefined || parentOp == BlendMode::Ignore || operation == BlendMode::Replace )
					m_combAttr.colorBlendMode[i] = operation;
			}			
		}
		else
		{
		
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.colorBlendMode[i] = operation;
				m_specAttr.color[i] = color;

				m_combAttr.color[i] = Color::blend( Color::White, color, operation );
				m_combAttr.colorBlendMode[i] = operation;
			}			
		}
	}
	
	void TextStyle::setColor( Color color, State state, BlendMode operation )
	{
		int i = Util::_stateToIndex(state);

		m_specAttr.colorBlendMode[i] = operation;
		m_specAttr.color[i] = color;

		if( m_pParent )
		{
			m_combAttr.color[i] = Color::blend( m_pParent->m_combAttr.color[i], color, operation );

			BlendMode parentOp = m_pParent->m_combAttr.colorBlendMode[i];
			if( parentOp == BlendMode::Undefined || parentOp == BlendMode::Ignore || operation == BlendMode::Replace )
				m_combAttr.colorBlendMode[i] = operation;
		}
		else
		{		
			m_combAttr.color[i] = Color::blend( Color::White, color, operation );
			m_combAttr.colorBlendMode[i] = operation;
		}
	}
	
	//____ setBgColor() ______________________________________________________________
	
	void TextStyle::setBgColor( Color color, BlendMode operation )
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.bgColorBlendMode[i] = operation;
				m_specAttr.bgColor[i] = color;

				m_combAttr.bgColor[i] = Color::blend( m_pParent->m_combAttr.bgColor[i], color, operation );

				// This doesn't always blends colors as logically expected when adding together attribute structures,
				// but covers most common and important cases.
				//
				// It always gets right if attr-structures that gets added either:
				//
				// 1. Only has one level where color is set in its TextStyle hierarchy.
				// 2. Has a BlendMode::Replace somewhere in its TextStyle hierarchy.

				BlendMode parentOp = m_pParent->m_combAttr.bgColorBlendMode[i];
				if( parentOp == BlendMode::Undefined || parentOp == BlendMode::Ignore || operation == BlendMode::Replace )
					m_combAttr.bgColorBlendMode[i] = operation;
			}			
		}
		else
		{		
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.bgColorBlendMode[i] = operation;
				m_specAttr.bgColor[i] = color;

				m_combAttr.bgColor[i] = Color::blend( Color::White, color, operation );
				m_combAttr.bgColorBlendMode[i] = operation;
			}			
		}
	}
	
	void TextStyle::setBgColor( Color color, State state, BlendMode operation )
	{
		int i = Util::_stateToIndex(state);

		m_specAttr.bgColorBlendMode[i] = operation;
		m_specAttr.bgColor[i] = color;

		if( m_pParent )
		{
			m_combAttr.bgColor[i] = Color::blend( m_pParent->m_combAttr.bgColor[i], color, operation );

			BlendMode parentOp = m_pParent->m_combAttr.bgColorBlendMode[i];
			if( parentOp == BlendMode::Undefined || parentOp == BlendMode::Ignore || operation == BlendMode::Replace )
				m_combAttr.bgColorBlendMode[i] = operation;
		}
		else
		{		
			m_combAttr.bgColor[i] = Color::blend( Color::White, color, operation );
			m_combAttr.bgColorBlendMode[i] = operation;
		}
	}

	
	//____ setSize() _______________________________________________________________
	
	void TextStyle::setSize( int size )
	{
		if( size == 0 )
			clearSize();
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.size[i] = size;
				m_combAttr.size[i] = size;
			}
		}
	}
	
	void TextStyle::setSize( int size, State state )
	{
		if( size == 0 )
			clearSize(state);
		else
		{
			int idx = Util::_stateToIndex(state);
			m_specAttr.size[idx] = size;
			m_combAttr.size[idx] = size;
		}
	}
	
	
	//____ setDecoration() _________________________________________________________
	
	void TextStyle::setDecoration( TextDecoration decoration )
	{
		if( decoration == TextDecoration::Undefined )
			clearDecoration();
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.decoration[i] = decoration;
				m_combAttr.decoration[i] = decoration;
			}
		}
	}
	
	void TextStyle::setDecoration( TextDecoration decoration, State state )
	{
		if( decoration == TextDecoration::Undefined )
			clearDecoration(state);
		else
		{
			int idx = Util::_stateToIndex(state);
			m_specAttr.decoration[idx] = decoration;
			m_combAttr.decoration[idx] = decoration;
		}
	}
	
	//____ setRenderMode() _________________________________________________________
	
	void TextStyle::setRenderMode( BlendMode mode )
	{
		if( mode == BlendMode::Undefined )
			clearRenderMode();
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.renderMode[i] = mode;
				m_combAttr.renderMode[i] = mode;
			}
		}
	}
	
	void TextStyle::setRenderMode( BlendMode mode, State state )
	{
		if( mode == BlendMode::Undefined )
			clearRenderMode(state);
		else
		{
			int idx = Util::_stateToIndex(state);
			m_specAttr.renderMode[idx] = mode;
			m_combAttr.renderMode[idx] = mode;
		}
	}

	//____ setBgRenderMode() _________________________________________________________
	
	void TextStyle::setBgRenderMode( BlendMode mode )
	{
		if( mode == BlendMode::Undefined )
			clearBgRenderMode();
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.bgRenderMode[i] = mode;
				m_combAttr.bgRenderMode[i] = mode;
			}
		}
	}
	
	void TextStyle::setBgRenderMode( BlendMode mode, State state )
	{
		if( mode == BlendMode::Undefined )
			clearBgRenderMode(state);
		else
		{
			int idx = Util::_stateToIndex(state);
			m_specAttr.bgRenderMode[idx] = mode;
			m_combAttr.bgRenderMode[idx] = mode;
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
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.colorBlendMode[i] = BlendMode::Undefined;
				m_specAttr.color[i] = Color::White;
				m_combAttr.color[i] = m_pParent->m_combAttr.color[i];
				m_combAttr.colorBlendMode[i] = m_pParent->m_combAttr.colorBlendMode[i];
			}
		}
		else
		{			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.colorBlendMode[i] = BlendMode::Undefined;
				m_specAttr.color[i] = Color::White;
				m_combAttr.color[i] = Color::White;
				m_combAttr.colorBlendMode[i] = BlendMode::Undefined;
			}
		}
	}
	
	void TextStyle::clearColor( State state )
	{
		int idx = Util::_stateToIndex(state);
	
		m_specAttr.colorBlendMode[idx] = BlendMode::Undefined;
		m_specAttr.color[idx] = Color::White;

		if( m_pParent )
		{
			m_combAttr.color[idx] = m_pParent->m_combAttr.color[idx];
			m_combAttr.colorBlendMode[idx] = m_pParent->m_combAttr.colorBlendMode[idx];			
		}
		else
		{
			m_combAttr.color[idx] = Color::White;
			m_combAttr.colorBlendMode[idx] = BlendMode::Undefined;
		}
	}
	
	
	//____ clearBgColor() ____________________________________________________________
	
	void TextStyle::clearBgColor()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.bgColorBlendMode[i] = BlendMode::Undefined;
				m_specAttr.bgColor[i] = Color::White;
				m_combAttr.bgColor[i] = m_pParent->m_combAttr.bgColor[i];
				m_combAttr.bgColorBlendMode[i] = m_pParent->m_combAttr.bgColorBlendMode[i];
			}
		}
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.bgColorBlendMode[i] = BlendMode::Undefined;
				m_specAttr.bgColor[i] = Color::White;
				m_combAttr.bgColor[i] = Color::White;
				m_combAttr.bgColorBlendMode[i] = BlendMode::Undefined;
			}
		}
	}
	
	void TextStyle::clearBgColor( State state )
	{
		int idx = Util::_stateToIndex(state);
	
		m_specAttr.bgColorBlendMode[idx] = BlendMode::Ignore;
		m_specAttr.bgColor[idx] = Color::White;

		if( m_pParent )
		{
			m_combAttr.bgColor[idx] = m_pParent->m_combAttr.bgColor[idx];
			m_combAttr.bgColorBlendMode[idx] = m_pParent->m_combAttr.bgColorBlendMode[idx];			
		}
		else
		{
			m_combAttr.bgColor[idx] = Color::White;			
			m_combAttr.bgColorBlendMode[idx] = BlendMode::Undefined;
		}
	}
	
	//____ clearSize() ____________________________________________________________
	
	void TextStyle::clearSize()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.size[i] = 0;
				m_combAttr.size[i] = m_pParent->m_combAttr.size[i];
			}
		}
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.size[i] = 0;
				m_combAttr.size[i] = 0;
			}
		}
	}
	
	void TextStyle::clearSize( State state )
	{
		int idx = Util::_stateToIndex(state);
	
		m_specAttr.size[idx] = 0;
		m_combAttr.size[idx] = m_pParent ? m_pParent->m_combAttr.size[idx] : 0;
	}
	
	//____ clearDecoration() ____________________________________________________________
	
	void TextStyle::clearDecoration()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.decoration[i] = TextDecoration::Undefined;
				m_combAttr.decoration[i] = m_pParent->m_combAttr.decoration[i];
			}
		}
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.decoration[i] = TextDecoration::Undefined;
				m_combAttr.decoration[i] = TextDecoration::Undefined;
			}
		}
	}
	
	void TextStyle::clearDecoration( State state )
	{
		int idx = Util::_stateToIndex(state);
	
		m_specAttr.decoration[idx] = TextDecoration::Undefined;
		m_combAttr.decoration[idx] = m_pParent ? m_pParent->m_combAttr.decoration[idx] : TextDecoration::Undefined;
	}

	//____ clearRenderMode() ____________________________________________________________
	
	void TextStyle::clearRenderMode()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.renderMode[i] = BlendMode::Undefined;
				m_combAttr.renderMode[i] = m_pParent->m_combAttr.renderMode[i];
			}
		}
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.renderMode[i] = BlendMode::Undefined;
				m_combAttr.renderMode[i] = BlendMode::Undefined;
			}
		}
	}
	
	void TextStyle::clearRenderMode( State state )
	{
		int idx = Util::_stateToIndex(state);
	
		m_specAttr.renderMode[idx] = BlendMode::Undefined;
		m_combAttr.renderMode[idx] = m_pParent ? m_pParent->m_combAttr.renderMode[idx] : BlendMode::Undefined;
	}


	//____ clearBgRenderMode() ____________________________________________________________
	
	void TextStyle::clearBgRenderMode()
	{
		if( m_pParent )
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.bgRenderMode[i] = BlendMode::Undefined;
				m_combAttr.bgRenderMode[i] = m_pParent->m_combAttr.bgRenderMode[i];
			}
		}
		else
		{
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				m_specAttr.bgRenderMode[i] = BlendMode::Undefined;
				m_combAttr.bgRenderMode[i] = BlendMode::Undefined;
			}
		}
	}
	
	void TextStyle::clearBgRenderMode( State state )
	{
		int idx = Util::_stateToIndex(state);
	
		m_specAttr.bgRenderMode[idx] = BlendMode::Undefined;
		m_combAttr.bgRenderMode[idx] = m_pParent ? m_pParent->m_combAttr.bgRenderMode[idx] : BlendMode::Undefined;
	}

	
	//____ exportAttr() ____________________________________________________________
	
	void TextStyle::exportAttr( State state, TextAttr * pDest ) const
	{
		int idx = Util::_stateToIndex(state);
	
		pDest->pFont 		= m_combAttr.pFont;
		pDest->pLink 		= m_combAttr.pLink;
		pDest->size 		= m_combAttr.size[idx];
		pDest->color		= m_combAttr.color[idx];
		pDest->bgColor		= m_combAttr.bgColor[idx];
		pDest->decoration	= m_combAttr.decoration[idx];
		pDest->renderMode		= m_combAttr.renderMode[idx];
		pDest->bgRenderMode		= m_combAttr.bgRenderMode[idx];
				
		if( pDest->size == 0 )
			pDest->size = 12;								// Default to size 12.
			
		if( pDest->decoration == TextDecoration::Undefined )
			pDest->decoration = TextDecoration::None;
	}
	
	//____ addToAttr() _____________________________________________________________
	
	void TextStyle::addToAttr( State state, TextAttr * pDest ) const 
	{
		int idx = Util::_stateToIndex(state);
	
		if( m_combAttr.pFont )
			pDest->pFont = m_combAttr.pFont;
		if( m_combAttr.pLink )
			pDest->pLink = m_combAttr.pLink;
		if( m_combAttr.size[idx] != 0 )
			pDest->size	= m_combAttr.size[idx];
		if( m_combAttr.decoration[idx] != TextDecoration::Undefined )
			pDest->decoration = m_combAttr.decoration[idx];
		if( m_combAttr.renderMode[idx] != BlendMode::Undefined )
			pDest->renderMode = m_combAttr.renderMode[idx];
		if( m_combAttr.bgRenderMode[idx] != BlendMode::Undefined )
			pDest->bgRenderMode = m_combAttr.bgRenderMode[idx];

		pDest->color = Color::blend( pDest->color, m_combAttr.color[idx], m_combAttr.colorBlendMode[idx] );
		pDest->bgColor = Color::blend( pDest->bgColor, m_combAttr.bgColor[idx], m_combAttr.bgColorBlendMode[idx] );
	}
	
	//____ isIdentical() _____________________________________________________

	bool TextStyle::isIdentical( TextStyle * pOther )
	{
		return _compareSets( &m_combAttr, &(pOther->m_combAttr) );
	}
	
	//____ isIdenticalForState() _____________________________________________
	
	bool TextStyle::isIdenticalForState( TextStyle * pOther, State state )
	{
		return _compareSetsForState( &m_combAttr, &(pOther->m_combAttr), state );		
	}

	
	//____ refreshComb() ___________________________________________________________
	
	bool TextStyle::_refreshComb()
	{
		if( m_pParent )
		{
			AttrSet		newComb;
	
			newComb.pFont = m_specAttr.pFont ? m_specAttr.pFont : m_pParent->m_combAttr.pFont;
			newComb.pLink = m_specAttr.pLink ? m_specAttr.pLink : m_pParent->m_combAttr.pLink;
	
	
			for( int i = 0 ; i < StateEnum_Nb ; i++ )
			{
				newComb.size[i] = m_specAttr.size[i] != 0 ? m_specAttr.size[i] : m_pParent->m_combAttr.size[i];
				newComb.color[i] = Color::blend( m_pParent->m_combAttr.color[i], m_specAttr.color[i], m_specAttr.colorBlendMode[i] );
				newComb.bgColor[i] = Color::blend( m_pParent->m_combAttr.bgColor[i], m_specAttr.bgColor[i], m_specAttr.bgColorBlendMode[i] );
				newComb.decoration[i] = m_specAttr.decoration[i] != TextDecoration::Undefined ? m_specAttr.decoration[i] : m_pParent->m_combAttr.decoration[i];
				newComb.renderMode[i] = m_specAttr.renderMode[i] == BlendMode::Undefined ? m_pParent->m_combAttr.renderMode[i] : m_specAttr.renderMode[i];
				newComb.bgRenderMode[i] = m_specAttr.bgRenderMode[i] == BlendMode::Undefined ? m_pParent->m_combAttr.bgRenderMode[i] : m_specAttr.bgRenderMode[i];

				BlendMode parentOp = m_pParent->m_combAttr.colorBlendMode[i];
				if( parentOp == BlendMode::Undefined || parentOp == BlendMode::Ignore || m_specAttr.colorBlendMode[i] == BlendMode::Replace )
					m_combAttr.colorBlendMode[i] = m_specAttr.colorBlendMode[i];

				parentOp = m_pParent->m_combAttr.bgColorBlendMode[i];
				if( parentOp == BlendMode::Undefined || parentOp == BlendMode::Ignore || m_specAttr.bgColorBlendMode[i] == BlendMode::Replace )
					m_combAttr.bgColorBlendMode[i] = m_specAttr.bgColorBlendMode[i];
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
	
		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			pSet->size[i] 			= 0;
			pSet->color[i] 			= Color::White;
			pSet->bgColor[i] 		= Color::Transparent;
			pSet->colorBlendMode[i]	= BlendMode::Undefined;
			pSet->bgColorBlendMode[i]	= BlendMode::Undefined;
			pSet->decoration[i] 	= TextDecoration::Undefined;
			pSet->renderMode[i] 		= BlendMode::Undefined;
			pSet->bgRenderMode[i] 		= BlendMode::Undefined;
		}
	}
	
	//____ _compareSets() __________________________________________________________
	
	bool TextStyle::_compareSets( TextStyle::AttrSet * p1, TextStyle::AttrSet * p2 )
	{
		if( p1->pFont != p2->pFont || p1->pLink != p2->pLink )
			return false;
	
		for( int i = 0 ; i < StateEnum_Nb ; i++ )
		{
			if( p1->size[i] 			!= p2->size[i] ||
				p1->colorBlendMode[i] 	!= p2->colorBlendMode[i] ||
				p1->bgColorBlendMode[i] 	!= p2->bgColorBlendMode[i] ||
				p1->decoration[i] 		!= p2->decoration[i] ||
				p1->renderMode[i]			!= p2->renderMode[i] ||
				p1->bgRenderMode[i]			!= p2->bgRenderMode[i] ||
				p1->color[i] 			!= p2->color[i] ||
				p1->bgColor[i] 			!= p2->bgColor[i] )
				return false;				
		}
	
		return true;
	}

	//____ _compareSetsForState() __________________________________________________________
	
	bool TextStyle::_compareSetsForState( TextStyle::AttrSet * p1, TextStyle::AttrSet * p2, State state )
	{
		if( p1->pFont != p2->pFont || p1->pLink != p2->pLink )
			return false;
	
		int i = Util::_stateToIndex(state);
	
		if( p1->size[i] 			!= p2->size[i] ||
			p1->colorBlendMode[i] 	!= p2->colorBlendMode[i] ||
			p1->bgColorBlendMode[i] 	!= p2->bgColorBlendMode[i] ||
			p1->decoration[i] 		!= p2->decoration[i] ||
			p1->renderMode[i]			!= p2->renderMode[i] ||
			p1->bgRenderMode[i]			!= p2->bgRenderMode[i] ||
			p1->color[i] 			!= p2->color[i] ||
			p1->bgColor[i] 			!= p2->bgColor[i] )
			return false;				
	
		return true;
	}


} // namespace wg
