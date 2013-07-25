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

#include <wg_radiobutton.h>
#include <wg_panel.h>

const char WgRadioButton::CLASSNAME[] = {"RadioButton"};



//____ Constructor _________________________________________________________________

WgRadioButton::WgRadioButton()
{
	m_bAllowUnselecting	= false;
}

//____ Destructor _____________________________________________________________

WgRadioButton::~WgRadioButton()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgRadioButton::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgCheckBox::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgRadioButton::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgRadioButtonPtr WgRadioButton::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgRadioButtonPtr( static_cast<WgRadioButton*>(pObject.GetRealPtr()) );

	return 0;
}


//____ _onStateChanged() ______________________________________________________

void WgRadioButton::_onStateChanged( WgState oldState, WgState newState )
{
	if( newState.IsSelected() && !oldState.IsSelected() )
	{
			WgContainer * pGroup = _findRadioGroup();
			if( pGroup )
				_unselectRecursively( pGroup );
	}

	WgCheckBox::_onStateChanged(oldState,newState);
}

//____ _findRadioGroup() _______________________________________________________

WgContainer * WgRadioButton::_findRadioGroup()
{
	WgContainer * pParent = Parent();
	while( pParent )
	{
		if( pParent->_isPanel() && static_cast<WgPanel*>(pParent)->IsRadioGroup() )
			return pParent;

		pParent = pParent->Parent();
	}

	return 0;
}

//____ _unselectRecursively() __________________________________________________

void WgRadioButton::_unselectRecursively( WgContainer * pParent )
{
	WgWidget * pWidget = pParent->_firstWidget();

	while( pWidget )
	{
		if( pWidget->IsInstanceOf(WgRadioButton::CLASSNAME) )
		{
			WgRadioButton * pRB = (WgRadioButton*) pWidget;
			if( pRB->m_state.IsSelected() && pRB != this )
				pRB->SetSelected(false);
		}
		else if( pWidget->IsContainer() && (!static_cast<WgContainer*>(pWidget)->_isPanel() || !static_cast<WgPanel*>(pWidget)->IsRadioGroup()) )
			_unselectRecursively( static_cast<WgContainer*>(pWidget) );

		pWidget = pWidget->NextSibling();
	}
}

//____ _onCloneContent() _______________________________________________________

void WgRadioButton::_onCloneContent( const WgWidget * _pOrg )
{
	WgCheckBox::_onCloneContent( _pOrg );

	WgRadioButton * pOrg = (WgRadioButton *) _pOrg;

	m_bAllowUnselecting	= pOrg->m_bAllowUnselecting;
}
