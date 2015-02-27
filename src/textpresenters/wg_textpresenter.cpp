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

const char WgTextPresenter::CLASSNAME[] = {"TextPresenter"};



//____ IsInstanceOf() _________________________________________________________

bool WgTextPresenter::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgTextPresenter::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgTextPresenterPtr WgTextPresenter::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgTextPresenterPtr( static_cast<WgTextPresenter*>(pObject.RawPtr()) );

	return 0;
}

//____ Tooltip() _______________________________________________________________

WgString WgTextPresenter::Tooltip( const WgPresentableField * pField ) const
{
	return WgString();
}

//____ _charBuffer() ___________________________________________________________

WgCharBuffer * WgTextPresenter::_charBuffer( WgPresentableField * pField ) const
{
	return &(pField->m_charBuffer);
}

const WgCharBuffer * WgTextPresenter::_charBuffer( const WgPresentableField * pField ) const
{
	return &(pField->m_charBuffer);
}

//____ _fieldDataBlock() _______________________________________________________

void * WgTextPresenter::_fieldDataBlock( WgPresentableField * pField) const
{
	return pField->m_pPresenterData;
}

const void * WgTextPresenter::_fieldDataBlock( const WgPresentableField * pField) const
{
	return pField->m_pPresenterData;
}


//____ _fieldDataInt() _________________________________________________________

int WgTextPresenter::_fieldDataInt( WgPresentableField * pField ) const
{
	return pField->m_presenterData;
}

//____ _baseStyle() _________________________________________________________

WgTextStyle * WgTextPresenter::_baseStyle( WgPresentableField * pField ) const
{
	WgTextStyle * pStyle = pField->m_pStyle.RawPtr();
	if( pStyle == 0 )
		pStyle = WgBase::DefaultStyle().RawPtr();
	
	return pStyle;
}

//____ _state() _________________________________________________________

WgState WgTextPresenter::_state( WgPresentableField * pField ) const
{
	return pField->m_state;
}

//____ _setFieldDataBlock() ____________________________________________________

void  WgTextPresenter::_setFieldDataBlock( WgPresentableField * pField, void * pBlock )
{
	pField->m_pPresenterData = pBlock;
}

//____ _setFieldDataInt() ______________________________________________________

void  WgTextPresenter::_setFieldDataInt( WgPresentableField * pField, int data )
{
	pField->m_presenterData = data;
}
