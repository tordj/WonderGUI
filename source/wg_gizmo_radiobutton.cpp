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

#include <wg_gizmo_radiobutton.h>
#include <wg_gizmo_container.h>
#include <wg_eventhandler.h>

using namespace WgSignal;

static const char	c_gizmoType[] = {"RadioButton"};



//____ Constructor _________________________________________________________________

WgGizmoRadiobutton::WgGizmoRadiobutton()
{
	m_bAllowUnchecking	= false;
}

//____ Destructor _____________________________________________________________

WgGizmoRadiobutton::~WgGizmoRadiobutton()
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoRadiobutton::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoRadiobutton::GetMyType( void )
{
	return c_gizmoType;
}


//____ SetState() _____________________________________________________________

bool WgGizmoRadiobutton::SetState( bool _state )
{
	if( m_bChecked != _state )
	{
		
		WgEventHandler * pHandler = EventHandler();		
		
		if( _state )
		{
			WgGizmoParent * pGroup = _findRadioGroup();
			if( pGroup )
				_unselectRecursively( pGroup );
			
			// Set and emit

			m_bChecked = true;
			Emit( Set() );
			if( pHandler )
				pHandler->QueueEvent( new WgEvent::RadiobuttonSelect(this) );
		}
		else
		{
			if( !m_bAllowUnchecking )
				return false;

			m_bChecked = false;
			Emit( Unset() );
			if( pHandler )
				pHandler->QueueEvent( new WgEvent::RadiobuttonUnselect(this) );
		}

		Emit( Flipped(), m_bChecked );
		if( pHandler )
			pHandler->QueueEvent( new WgEvent::RadiobuttonToggle(this, m_bChecked) );
		RequestRender();
	}
	return true;
}

//____ _findRadioGroup() _______________________________________________________

WgGizmoParent * WgGizmoRadiobutton::_findRadioGroup()
{
	WgGizmoParent * pRadioGroup = 0;
	WgGizmoParent * pParent = ParentX();
	while( pParent )
	{
		pRadioGroup = pParent;
		
		if( pParent->IsGizmo() )
		{
			if( pParent->CastToContainer()->IsRadioGroup() )
				break;
				
			pParent = pParent->CastToGizmo()->ParentX();
		}
		else break;
	}

	return pRadioGroup;
}

//____ _unselectRecursively() __________________________________________________

void WgGizmoRadiobutton::_unselectRecursively( WgGizmoParent * pParent )
{
	WgGizmo * pGizmo = pParent->FirstGizmo();
	
	while( pGizmo )
	{
		if( pGizmo->Type() == WgGizmoRadiobutton::GetMyType() )
		{
			WgGizmoRadiobutton * pRB = (WgGizmoRadiobutton*) pGizmo;
			if( pRB->m_bChecked )
			{
				pRB->m_bChecked = false;
				pRB->Emit( Unset() );
				pRB->Emit( Flipped(), false );

				WgEventHandler * pHandler = EventHandler();
				if( pHandler )
				{
					pHandler->QueueEvent( new WgEvent::RadiobuttonUnselect(pRB) );
					pHandler->QueueEvent( new WgEvent::RadiobuttonToggle(pRB, false) );
				}	
				pRB->RequestRender();
			}
		}
		else if( pGizmo->IsContainer() && !pGizmo->CastToContainer()->IsRadioGroup() )
			_unselectRecursively( pGizmo->CastToContainer() );
			
		pGizmo = pGizmo->NextSibling();
	}
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoRadiobutton::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoCheckbox::_onCloneContent( _pOrg );

	WgGizmoRadiobutton * pOrg = (WgGizmoRadiobutton *) _pOrg;

	m_bAllowUnchecking	= pOrg->m_bAllowUnchecking;
}
