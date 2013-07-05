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

static const char	c_widgetType[] = {"RadioButton"};



//____ Constructor _________________________________________________________________

WgRadioButton::WgRadioButton()
{
	m_bAllowUnselecting	= false;
}

//____ Destructor _____________________________________________________________

WgRadioButton::~WgRadioButton()
{
}

//____ Type() _________________________________________________________________

const char * WgRadioButton::Type( void ) const
{
	return GetClass();
}

//____ GetClass() _____________________________________________________________

const char * WgRadioButton::GetClass( void )
{
	return c_widgetType;
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
		if( pParent->IsPanel() && pParent->CastToPanel()->IsRadioGroup() )
			return pParent;

		pParent = pParent->Parent();
	}

	return 0;
}

//____ _unselectRecursively() __________________________________________________

void WgRadioButton::_unselectRecursively( WgContainer * pParent )
{
	WgWidget * pWidget = pParent->FirstWidget();

	while( pWidget )
	{
		if( pWidget->Type() == WgRadioButton::GetClass() )
		{
			WgRadioButton * pRB = (WgRadioButton*) pWidget;
			if( pRB->m_state.IsSelected() && pRB != this )
				pRB->SetSelected(false);
		}
		else if( pWidget->IsContainer() && (!pWidget->IsPanel() || !pWidget->CastToPanel()->IsRadioGroup()) )
			_unselectRecursively( pWidget->CastToContainer() );

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
