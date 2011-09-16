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

#include <vector>

#ifndef WG_GIZMO_TABORDER_DOT_H
#define WG_GIZMO_TABORDER_DOT_H

#ifndef WG_MONOTAINER_DOT_H
#	include <wg_monotainer.h>
#endif


class WgGizmoTabOrder : public WgMonotainer
{
public:
	WgGizmoTabOrder();
	~WgGizmoTabOrder();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	bool	AddToTabOrder( WgGizmo * pGizmo );
	bool	InsertInTabOrder( WgGizmo * pGizmo, WgGizmo * pBefore );

	bool	RemoveFromTabOrder( WgGizmo * pGizmo );
	void	ClearTabOrder();


	WgGizmo * FirstInTabOrder() const;
	WgGizmo * NextInTabOrder( WgGizmo * pCurrGizmo ) const;
	WgGizmo * PrevInTabOrder( WgGizmo * pCurrGizmo ) const;
	WgGizmo * LastInTabOrder() const;

protected:

	virtual void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );

private:

	class TabOrderEntry : public WgLink
	{
	public:
		LINK_METHODS( TabOrderEntry );

		TabOrderEntry(WgGizmo* p) { pGizmo = p; }
		WgGizmoWeakPtr 	pGizmo;
	};

	bool 								_isValidForTabOrderInsertion( WgGizmo * pGizmo ) const;
	WgGizmoTabOrder::TabOrderEntry * 	_findInTabOrder( WgGizmo * pGizmo ) const;
	WgGizmoTabOrder::TabOrderEntry * 	_validateEntryForward( WgGizmoTabOrder::TabOrderEntry * pEntry ) const;
	WgGizmoTabOrder::TabOrderEntry * 	_validateEntryBackward( WgGizmoTabOrder::TabOrderEntry * pEntry ) const;



	WgChain<TabOrderEntry> 	m_tabOrder;
};

#endif //WG_GIZMO_TABORDER_DOT_H
