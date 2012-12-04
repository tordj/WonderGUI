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

#include <wg_taborderpanel.h>
#include <wg_eventhandler.h>

static const char	c_gizmoType[] = {"Taborder"};

//____ Constructor ____________________________________________________________

WgTaborderPanel::WgTaborderPanel()
{
}

//____ Destructor _____________________________________________________________

WgTaborderPanel::~WgTaborderPanel()
{
}

//____ Type() _________________________________________________________________

const char * WgTaborderPanel::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgTaborderPanel::GetClass()
{
	return c_gizmoType;
}

//____ AddToTaborder() ________________________________________________________

bool WgTaborderPanel::AddToTaborder( WgGizmo * pGizmo )
{
	if( !_isValidForTaborderInsertion(pGizmo) )
		return false;

	m_tabOrder.PushBack( new TaborderEntry(pGizmo) );
	return true;
}

//____ InsertInTaborder() _____________________________________________________

bool WgTaborderPanel::InsertInTaborder( WgGizmo * pGizmo, WgGizmo * _pBefore )
{
	if( !_pBefore || !_isValidForTaborderInsertion(pGizmo) )
		return false;

	TaborderEntry * pBefore = _findInTaborder(_pBefore);
	if( !pBefore )
		return false;

	TaborderEntry * pEntry = new TaborderEntry(pGizmo);

	pEntry->MoveBefore(pBefore);
	return true;
}

//____ RemoveFromTaborder() ___________________________________________________

bool WgTaborderPanel::RemoveFromTaborder( WgGizmo * pGizmo )
{
	TaborderEntry * pEntry = _findInTaborder(pGizmo);
	if( !pEntry )
		return false;

	delete pEntry;
	return true;
}

//____ ClearTaborder() ________________________________________________________

void WgTaborderPanel::ClearTaborder()
{
	m_tabOrder.Clear();
}

//____ FirstInTaborder() ______________________________________________________

WgGizmo * WgTaborderPanel::FirstInTaborder() const
{
	TaborderEntry * pEntry = _validateEntryForward( m_tabOrder.First() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ NextInTaborder() _______________________________________________________

WgGizmo * WgTaborderPanel::NextInTaborder( WgGizmo * pCurrGizmo ) const
{
	TaborderEntry * pEntry = _findInTaborder(pCurrGizmo);
	if( !pEntry )
		return 0;

	pEntry = _validateEntryForward( pEntry->Next() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ PrevInTaborder() _______________________________________________________

WgGizmo * WgTaborderPanel::PrevInTaborder( WgGizmo * pCurrGizmo ) const
{
	TaborderEntry * pEntry = _findInTaborder(pCurrGizmo);
	if( !pEntry )
		return 0;

	pEntry = _validateEntryBackward( pEntry->Prev() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ LastInTaborder() _______________________________________________________

WgGizmo * WgTaborderPanel::LastInTaborder() const
{
	TaborderEntry * pEntry = _validateEntryBackward( m_tabOrder.Last() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ _isValidForTaborderInsertion() _________________________________________

bool WgTaborderPanel::_isValidForTaborderInsertion( WgGizmo * pGizmo ) const
{
	// Check so it's an ancestor to us.

	WgGizmoContainer * p = pGizmo->Parent();

	while( p && p->IsGizmo() && p != this )
	{
		p = p->CastToGizmo()->Parent();
	}

	if( !p )
		return false;

	// Check so we don't already have this Gizmo in our tab-order

	if( _findInTaborder( pGizmo ) )
		return false;

	// All seems ok

	return true;
}

//____ _findInTaborder() ______________________________________________________

WgTaborderPanel::TaborderEntry * WgTaborderPanel::_findInTaborder( WgGizmo * pGizmo ) const
{
	TaborderEntry * p = m_tabOrder.First();
	while( p )
	{
		if( p->pGizmo.GetRealPtr() == pGizmo )
			return p;
		p = p->Next();
	}

	return 0;
}

//____ _validateEntryForward() ________________________________________________

WgTaborderPanel::TaborderEntry * WgTaborderPanel::_validateEntryForward( WgTaborderPanel::TaborderEntry * pEntry ) const
{
	while( pEntry && !pEntry->pGizmo )
		pEntry = pEntry->Next();

	return pEntry;
}

//____ _validateEntryBackward() _______________________________________________

WgTaborderPanel::TaborderEntry * WgTaborderPanel::_validateEntryBackward( WgTaborderPanel::TaborderEntry * pEntry ) const
{
	while( pEntry && !pEntry->pGizmo )
		pEntry = pEntry->Prev();

	return pEntry;
}

//____ _onEvent() _____________________________________________________________

void WgTaborderPanel::_onEvent( const WgEvent::Event * _pEvent, WgEventHandler * pHandler )
{
	switch( _pEvent->Type() )
	{
		case WG_EVENT_KEY_PRESS:
		case WG_EVENT_KEY_REPEAT:
		{
			const WgEvent::KeyEvent * pEvent = static_cast<const WgEvent::KeyEvent*>(_pEvent);
			if( pEvent->TranslatedKeyCode() == WG_KEY_TAB )
			{
				WgGizmo * pFocused = pHandler->KeyboardFocus();
				if( pFocused->IsTabLocked() )
					break;

				TaborderEntry * pEntry = _findInTaborder( pFocused );

				if( pEntry )
				{
					if( pEvent->ModKeys() == WG_MODKEY_NONE )					// Forward
					{
						pEntry = _validateEntryForward( pEntry->Next() );
						if( !pEntry )
							pEntry = _validateEntryForward( m_tabOrder.First() );

					}
					else if( pEvent->ModKeys() == WG_MODKEY_SHIFT )				// Backward
					{
						pEntry = _validateEntryBackward( pEntry->Prev() );
						if( !pEntry )
							pEntry = _validateEntryBackward( m_tabOrder.Last() );
					}

					pHandler->SetKeyboardFocus(pEntry->pGizmo.GetRealPtr() );
				}
			}
			else
				pHandler->ForwardEvent(_pEvent);
		}
		break;

		default:
			pHandler->ForwardEvent(_pEvent);
			break;
	}


}
