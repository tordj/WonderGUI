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
#include <wg_base.h>

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

	//____ tooltip() _______________________________________________________________

	String TextMapper::tooltip( const Text * pText ) const
	{
		return String();
	}

	//____ _chars() ___________________________________________________________

	const Char * TextMapper::_chars( const Text * pText ) const
	{
		return pText->_textBegin();
	}

	//____ _dataBlock() _______________________________________________________

	void * TextMapper::_dataBlock( Text * pText) const
	{
		return pText->m_pMapperData;
	}

	const void * TextMapper::_dataBlock( const Text * pText) const
	{
		return pText->m_pMapperData;
	}


	//____ _dataInt() _________________________________________________________

	int TextMapper::_dataInt( const Text * pText ) const
	{
		return pText->m_mapperData;
	}

	//____ _baseStyle() _________________________________________________________

	const TextStyle * TextMapper::_baseStyle( const Text * pText ) const
	{
		TextStyle * pStyle = pText->_textStyle();
		if( pStyle == 0 )
			pStyle = Base::defaultStyle().rawPtr();

		return pStyle;
	}

	//____ _state() _________________________________________________________

	State TextMapper::_state( const Text * pText ) const
	{
		return pText->_textState();
	}

	//____ _setTextDataBlock() ____________________________________________________

	void  TextMapper::_setTextDataBlock( Text * pText, void * pBlock )
	{
		pText->m_pMapperData = pBlock;
	}

	//____ _setTextDataInt() ______________________________________________________

	void  TextMapper::_setTextDataInt( Text * pText, int data )
	{
		pText->m_mapperData = data;
	}

	//____ _setTextDirty() ____________________________________________________

	void  TextMapper::_setTextDirty( Text * pText )
	{
		pText->_mapperRequestRender();
	}

	void  TextMapper::_setTextDirty( Text * pText, const RectI& rect )
	{
		pText->_mapperRequestRender( rect );
	}

	//____ _requestTextResize() _______________________________________________

	void TextMapper::_requestTextResize( Text * pText )
	{
		pText->_mapperRequestResize();
	}

	//____ _size() _____________________________________________________________

	SizeI TextMapper::_size( const Text * pText ) const
	{
		return pText->_textSize();
	}

	//____ _length() ___________________________________________________________

	int TextMapper::_length( const Text * pText ) const
	{
		return pText->_textLength();
	}


	//____ _caretVisible() _____________________________________________________

	bool TextMapper::_caretVisible( const Text * pText ) const
	{
		return pText->_caretVisible();
	}

	//____ _caretOfs() ____________________________________________________________

	int TextMapper::_caretOfs( const Text * pText ) const
	{
		return pText->_caretOffset();
	}

	//____ _selection() ________________________________________________________
	
	std::tuple<int,int>	TextMapper::_selection( const Text * pText ) const
	{
		return pText->_selectedText();
	}


} // namespace wg
