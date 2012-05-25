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

#include <wg_gizmo_taborder.h>
#include <wg_eventhandler.h>

static const char	c_gizmoType[] = {"TabOrder"};

//____ Constructor ____________________________________________________________

WgGizmoTabOrder::WgGizmoTabOrder()
{
}

//____ Destructor _____________________________________________________________

WgGizmoTabOrder::~WgGizmoTabOrder()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoTabOrder::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoTabOrder::GetMyType()
{
	return c_gizmoType;
}

//____ AddToTabOrder() ________________________________________________________

bool WgGizmoTabOrder::AddToTabOrder( WgGizmo * pGizmo )
{
	if( !_isValidForTabOrderInsertion(pGizmo) )
		return false;

	m_tabOrder.PushBack( new TabOrderEntry(pGizmo) );
	return true;
}

//____ InsertInTabOrder() _____________________________________________________

bool WgGizmoTabOrder::InsertInTabOrder( WgGizmo * pGizmo, WgGizmo * _pBefore )
{
	if( !_pBefore || !_isValidForTabOrderInsertion(pGizmo) )
		return false;

	TabOrderEntry * pBefore = _findInTabOrder(_pBefore);
	if( !pBefore )
		return false;

	TabOrderEntry * pEntry = new TabOrderEntry(pGizmo);

	pEntry->MoveBefore(pBefore);
	return true;
}

//____ RemoveFromTabOrder() ___________________________________________________

bool WgGizmoTabOrder::RemoveFromTabOrder( WgGizmo * pGizmo )
{
	TabOrderEntry * pEntry = _findInTabOrder(pGizmo);
	if( !pEntry )
		return false;

	delete pEntry;
	return true;
}

//____ ClearTabOrder() ________________________________________________________

void WgGizmoTabOrder::ClearTabOrder()
{
	m_tabOrder.Clear();
}

//____ FirstInTabOrder() ______________________________________________________

WgGizmo * WgGizmoTabOrder::FirstInTabOrder() const
{
	TabOrderEntry * pEntry = _validateEntryForward( m_tabOrder.First() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ NextInTabOrder() _______________________________________________________

WgGizmo * WgGizmoTabOrder::NextInTabOrder( WgGizmo * pCurrGizmo ) const
{
	TabOrderEntry * pEntry = _findInTabOrder(pCurrGizmo);
	if( !pEntry )
		return 0;

	pEntry = _validateEntryForward( pEntry->Next() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ PrevInTabOrder() _______________________________________________________

WgGizmo * WgGizmoTabOrder::PrevInTabOrder( WgGizmo * pCurrGizmo ) const
{
	TabOrderEntry * pEntry = _findInTabOrder(pCurrGizmo);
	if( !pEntry )
		return 0;

	pEntry = _validateEntryBackward( pEntry->Prev() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ LastInTabOrder() _______________________________________________________

WgGizmo * WgGizmoTabOrder::LastInTabOrder() const
{
	TabOrderEntry * pEntry = _validateEntryBackward( m_tabOrder.Last() );

	if( pEntry )
		return pEntry->pGizmo.GetRealPtr();

	return 0;
}

//____ _isValidForTabOrderInsertion() _________________________________________

bool WgGizmoTabOrder::_isValidForTabOrderInsertion( WgGizmo * pGizmo ) const
{
	// Check so it's an ancestor to us.

	WgGizmoParent * p = pGizmo->ParentX();

	while( p && p->IsGizmo() && p != this )
	{
		p = p->CastToGizmo()->ParentX();
	}

	if( !p )
		return false;

	// Check so we don't already have this Gizmo in our tab-order

	if( _findInTabOrder( pGizmo ) )
		return false;

	// All seems ok

	return true;
}

//____ _findInTabOrder() ______________________________________________________

WgGizmoTabOrder::TabOrderEntry * WgGizmoTabOrder::_findInTabOrder( WgGizmo * pGizmo ) const
{
	TabOrderEntry * p = m_tabOrder.First();
	while( p )
	{
		if( p->pGizmo.GetRealPtr() == pGizmo )
			return p;
		p = p->Next();
	}

	return 0;
}

//____ _validateEntryForward() ________________________________________________

WgGizmoTabOrder::TabOrderEntry * WgGizmoTabOrder::_validateEntryForward( WgGizmoTabOrder::TabOrderEntry * pEntry ) const
{
	while( pEntry && !pEntry->pGizmo )
		pEntry = pEntry->Next();

	return pEntry;
}

//____ _validateEntryBackward() _______________________________________________

WgGizmoTabOrder::TabOrderEntry * WgGizmoTabOrder::_validateEntryBackward( WgGizmoTabOrder::TabOrderEntry * pEntry ) const
{
	while( pEntry && !pEntry->pGizmo )
		pEntry = pEntry->Prev();

	return pEntry;
}

//____ _onEvent() _____________________________________________________________

void WgGizmoTabOrder::_onEvent( const WgEvent::Event * _pEvent, WgEventHandler * pHandler )
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

				TabOrderEntry * pEntry = _findInTabOrder( pFocused );

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
