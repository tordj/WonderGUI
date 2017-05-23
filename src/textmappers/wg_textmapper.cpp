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

#include <wg_textmapper.h>
#include <wg_textbaseitem.h>

namespace wg 
{
	
	const char TextMapper::CLASSNAME[] = {"TextMapper"};
	
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool TextMapper::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * TextMapper::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	TextMapper_p TextMapper::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextMapper_p( static_cast<TextMapper*>(pObject) );
	
		return 0;
	}
	
	//____ tooltip() _______________________________________________________________
	
	String TextMapper::tooltip( const TextBaseItem * pItem ) const
	{
		return String();
	}
		
	//____ _charBuffer() ___________________________________________________________
	
	CharBuffer * TextMapper::_charBuffer( TextBaseItem * pItem ) const
	{
		return &(pItem->m_charBuffer);
	}
	
	const CharBuffer * TextMapper::_charBuffer( const TextBaseItem * pItem ) const
	{
		return &(pItem->m_charBuffer);
	}
	
	//____ _itemDataBlock() _______________________________________________________
	
	void * TextMapper::_itemDataBlock( TextBaseItem * pItem) const
	{
		return pItem->m_pTextMapperData;
	}
	
	const void * TextMapper::_itemDataBlock( const TextBaseItem * pItem) const
	{
		return pItem->m_pTextMapperData;
	}
	
	
	//____ _itemDataInt() _________________________________________________________
	
	int TextMapper::_itemDataInt( const TextBaseItem * pItem ) const
	{
		return pItem->m_textMapperData;
	}
	
	//____ _baseStyle() _________________________________________________________
	
	const TextStyle * TextMapper::_baseStyle( const TextBaseItem * pItem ) const
	{
		TextStyle * pStyle = pItem->m_pStyle.rawPtr();
		if( pStyle == 0 )
			pStyle = Base::defaultStyle().rawPtr();
		
		return pStyle;
	}
	
	//____ _state() _________________________________________________________
	
	State TextMapper::_state( const TextBaseItem * pItem ) const
	{
		return pItem->m_state;
	}

	//____ _editState() _________________________________________________________
	
	const EditState * TextMapper::_editState( const TextBaseItem * pItem ) const
	{
		return pItem->_editState();
	}

	
	//____ _setItemDataBlock() ____________________________________________________
	
	void  TextMapper::_setItemDataBlock( TextBaseItem * pItem, void * pBlock )
	{
		pItem->m_pTextMapperData = pBlock;
	}
	
	//____ _setItemDataInt() ______________________________________________________
	
	void  TextMapper::_setItemDataInt( TextBaseItem * pItem, int data )
	{
		pItem->m_textMapperData = data;
	}

	//____ _setItemDirty() ____________________________________________________

	void  TextMapper::_setItemDirty( TextBaseItem * pItem )
	{
		pItem->_requestRender();
	}
	
	void  TextMapper::_setItemDirty( TextBaseItem * pItem, const Rect& rect )
	{
		pItem->_requestRender( rect );
	}

	//____ _requestItemResize() _______________________________________________

	void TextMapper::_requestItemResize( TextBaseItem * pItem )
	{
		pItem->_requestResize();
	}
	

} // namespace wg
