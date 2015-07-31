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

#include <wg_textpresenter.h>
#include <wg_presentablefield.h>

namespace wg 
{
	
	const char TextPresenter::CLASSNAME[] = {"TextPresenter"};
	
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool TextPresenter::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * TextPresenter::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	TextPresenter_p TextPresenter::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return TextPresenter_p( static_cast<TextPresenter*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ tooltip() _______________________________________________________________
	
	String TextPresenter::tooltip( const PresentableField * pField ) const
	{
		return String();
	}
	
	//____ _charBuffer() ___________________________________________________________
	
	CharBuffer * TextPresenter::_charBuffer( PresentableField * pField ) const
	{
		return &(pField->m_charBuffer);
	}
	
	const CharBuffer * TextPresenter::_charBuffer( const PresentableField * pField ) const
	{
		return &(pField->m_charBuffer);
	}
	
	//____ _fieldDataBlock() _______________________________________________________
	
	void * TextPresenter::_fieldDataBlock( PresentableField * pField) const
	{
		return pField->m_pPresenterData;
	}
	
	const void * TextPresenter::_fieldDataBlock( const PresentableField * pField) const
	{
		return pField->m_pPresenterData;
	}
	
	
	//____ _fieldDataInt() _________________________________________________________
	
	int TextPresenter::_fieldDataInt( PresentableField * pField ) const
	{
		return pField->m_presenterData;
	}
	
	//____ _baseStyle() _________________________________________________________
	
	TextStyle * TextPresenter::_baseStyle( PresentableField * pField ) const
	{
		TextStyle * pStyle = pField->m_pStyle.rawPtr();
		if( pStyle == 0 )
			pStyle = Base::defaultStyle().rawPtr();
		
		return pStyle;
	}
	
	//____ _state() _________________________________________________________
	
	State TextPresenter::_state( PresentableField * pField ) const
	{
		return pField->m_state;
	}
	
	//____ _setFieldDataBlock() ____________________________________________________
	
	void  TextPresenter::_setFieldDataBlock( PresentableField * pField, void * pBlock )
	{
		pField->m_pPresenterData = pBlock;
	}
	
	//____ _setFieldDataInt() ______________________________________________________
	
	void  TextPresenter::_setFieldDataInt( PresentableField * pField, int data )
	{
		pField->m_presenterData = data;
	}

} // namespace wg
