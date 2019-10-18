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
#include <wg_ctext.h>

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

	String TextMapper::tooltip( const CText * pText ) const
	{
		return String();
	}

	//____ _charBuffer() ___________________________________________________________

	CharBuffer * TextMapper::_charBuffer( CText * pText ) const
	{
		return &(pText->m_charBuffer);
	}

	const CharBuffer * TextMapper::_charBuffer( const CText * pText ) const
	{
		return &(pText->m_charBuffer);
	}

	//____ _dataBlock() _______________________________________________________

	void * TextMapper::_dataBlock( CText * pText) const
	{
		return pText->m_pTextMapperData;
	}

	const void * TextMapper::_dataBlock( const CText * pText) const
	{
		return pText->m_pTextMapperData;
	}


	//____ _dataInt() _________________________________________________________

	int TextMapper::_dataInt( const CText * pText ) const
	{
		return pText->m_textMapperData;
	}

	//____ _baseStyle() _________________________________________________________

	const TextStyle * TextMapper::_baseStyle( const CText * pText ) const
	{
		TextStyle * pStyle = pText->m_pStyle.rawPtr();
		if( pStyle == 0 )
			pStyle = Base::defaultStyle().rawPtr();

		return pStyle;
	}

	//____ _state() _________________________________________________________

	State TextMapper::_state( const CText * pText ) const
	{
		return pText->m_state;
	}

	//____ _editState() _________________________________________________________

	const EditState * TextMapper::_editState( const CText * pText ) const
	{
		return pText->_editState();
	}


	//____ _setComponentDataBlock() ____________________________________________________

	void  TextMapper::_setComponentDataBlock( CText * pText, void * pBlock )
	{
		pText->m_pTextMapperData = pBlock;
	}

	//____ _setComponentDataInt() ______________________________________________________

	void  TextMapper::_setComponentDataInt( CText * pText, int data )
	{
		pText->m_textMapperData = data;
	}

	//____ _setComponentDirty() ____________________________________________________

	void  TextMapper::_setComponentDirty( CText * pText )
	{
		pText->_mapperRequestRender();
	}

	void  TextMapper::_setComponentDirty( CText * pText, const RectI& rect )
	{
		pText->_mapperRequestRender( rect );
	}

	//____ _requestComponentResize() _______________________________________________

	void TextMapper::_requestComponentResize( CText * pText )
	{
		pText->_mapperRequestResize();
	}


} // namespace wg
