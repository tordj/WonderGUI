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
#include <wg_gizmo_collection.h>

using namespace WgSignal;

static const char	c_gizmoType[] = {"TordJ/RadioButton2"};



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
		if( _state )
		{

			// Go trough siblings and uncheck any checked RadioButton2

			if( m_pHook )
			{
				WgGizmo * pGizmo = m_pHook->Collection()->FirstGizmo();
				while( pGizmo )
				{
					if( pGizmo->Type() == s_type )
					{
						WgGizmoRadiobutton * pRB = (WgGizmoRadiobutton*) pGizmo;
						if( pRB->m_bChecked )
						{
							pRB->m_bChecked = false;
							pRB->m_pHook->GetEmitter()->Emit( Unset() );
							pRB->m_pHook->GetEmitter()->Emit( Flipped(), false );
							pRB->RequestRender();
						}
					}
					pGizmo = pGizmo->NextSibling();
				}
			}

			// Set and emit

			m_bChecked = true;
			if( m_pHook )
				m_pHook->GetEmitter()->Emit( Set() );
		}
		else
		{
			if( !m_bAllowUnchecking )
				return false;

			m_bChecked = false;
			if( m_pHook )
				m_pHook->GetEmitter()->Emit( Unset() );
		}

		if( m_pHook )
			m_pHook->GetEmitter()->Emit( Flipped(), m_bChecked );
		RequestRender();
	}
	return true;
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoRadiobutton::OnCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoCheckbox::OnCloneContent( _pOrg );
	
	WgGizmoRadiobutton * pOrg = (WgGizmoRadiobutton *) _pOrg;

	m_bAllowUnchecking	= pOrg->m_bAllowUnchecking;
}
