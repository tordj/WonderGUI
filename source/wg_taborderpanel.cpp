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

static const char	c_widgetType[] = {"Taborder"};

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
	return c_widgetType;
}

//____ AddToTaborder() ________________________________________________________

bool WgTaborderPanel::AddToTaborder( WgWidget * pWidget )
{
	if( !_isValidForTaborderInsertion(pWidget) )
		return false;

	m_tabOrder.PushBack( new TaborderEntry(pWidget) );
	return true;
}

//____ InsertInTaborder() _____________________________________________________

bool WgTaborderPanel::InsertInTaborder( WgWidget * pWidget, WgWidget * _pBefore )
{
	if( !_pBefore || !_isValidForTaborderInsertion(pWidget) )
		return false;

	TaborderEntry * pBefore = _findInTaborder(_pBefore);
	if( !pBefore )
		return false;

	TaborderEntry * pEntry = new TaborderEntry(pWidget);

	pEntry->MoveBefore(pBefore);
	return true;
}

//____ RemoveFromTaborder() ___________________________________________________

bool WgTaborderPanel::RemoveFromTaborder( WgWidget * pWidget )
{
	TaborderEntry * pEntry = _findInTaborder(pWidget);
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

WgWidget * WgTaborderPanel::FirstInTaborder() const
{
	TaborderEntry * pEntry = _validateEntryForward( m_tabOrder.First() );

	if( pEntry )
		return pEntry->pWidget.GetRealPtr();

	return 0;
}

//____ NextInTaborder() _______________________________________________________

WgWidget * WgTaborderPanel::NextInTaborder( WgWidget * pCurrWidget ) const
{
	TaborderEntry * pEntry = _findInTaborder(pCurrWidget);
	if( !pEntry )
		return 0;

	pEntry = _validateEntryForward( pEntry->Next() );

	if( pEntry )
		return pEntry->pWidget.GetRealPtr();

	return 0;
}

//____ PrevInTaborder() _______________________________________________________

WgWidget * WgTaborderPanel::PrevInTaborder( WgWidget * pCurrWidget ) const
{
	TaborderEntry * pEntry = _findInTaborder(pCurrWidget);
	if( !pEntry )
		return 0;

	pEntry = _validateEntryBackward( pEntry->Prev() );

	if( pEntry )
		return pEntry->pWidget.GetRealPtr();

	return 0;
}

//____ LastInTaborder() _______________________________________________________

WgWidget * WgTaborderPanel::LastInTaborder() const
{
	TaborderEntry * pEntry = _validateEntryBackward( m_tabOrder.Last() );

	if( pEntry )
		return pEntry->pWidget.GetRealPtr();

	return 0;
}

//____ _isValidForTaborderInsertion() _________________________________________

bool WgTaborderPanel::_isValidForTaborderInsertion( WgWidget * pWidget ) const
{
	// Check so it's an ancestor to us.

	WgWidgetHolder * p = pWidget->Parent();

	while( p && p->IsWidget() && p != this )
	{
		p = p->CastToWidget()->Parent();
	}

	if( !p )
		return false;

	// Check so we don't already have this Widget in our tab-order

	if( _findInTaborder( pWidget ) )
		return false;

	// All seems ok

	return true;
}

//____ _findInTaborder() ______________________________________________________

WgTaborderPanel::TaborderEntry * WgTaborderPanel::_findInTaborder( WgWidget * pWidget ) const
{
	TaborderEntry * p = m_tabOrder.First();
	while( p )
	{
		if( p->pWidget.GetRealPtr() == pWidget )
			return p;
		p = p->Next();
	}

	return 0;
}

//____ _validateEntryForward() ________________________________________________

WgTaborderPanel::TaborderEntry * WgTaborderPanel::_validateEntryForward( WgTaborderPanel::TaborderEntry * pEntry ) const
{
	while( pEntry && !pEntry->pWidget )
		pEntry = pEntry->Next();

	return pEntry;
}

//____ _validateEntryBackward() _______________________________________________

WgTaborderPanel::TaborderEntry * WgTaborderPanel::_validateEntryBackward( WgTaborderPanel::TaborderEntry * pEntry ) const
{
	while( pEntry && !pEntry->pWidget )
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
				WgWidget * pFocused = pHandler->KeyboardFocus();
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

					pHandler->SetKeyboardFocus(pEntry->pWidget.GetRealPtr() );
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
