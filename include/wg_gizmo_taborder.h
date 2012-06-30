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


class WgGizmoTaborder : public WgMonotainer
{
public:
	WgGizmoTaborder();
	~WgGizmoTaborder();

	virtual const char *Type( void ) const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoTaborder(); };
	

	bool	AddToTaborder( WgGizmo * pGizmo );
	bool	InsertInTaborder( WgGizmo * pGizmo, WgGizmo * pBefore );

	bool	RemoveFromTaborder( WgGizmo * pGizmo );
	void	ClearTaborder();


	WgGizmo * FirstInTaborder() const;
	WgGizmo * NextInTaborder( WgGizmo * pCurrGizmo ) const;
	WgGizmo * PrevInTaborder( WgGizmo * pCurrGizmo ) const;
	WgGizmo * LastInTaborder() const;

protected:

	virtual void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );

private:

	class TaborderEntry : public WgLink
	{
	public:
		LINK_METHODS( TaborderEntry );

		TaborderEntry(WgGizmo* p) { pGizmo = p; }
		WgGizmoWeakPtr 	pGizmo;
	};

	bool 								_isValidForTaborderInsertion( WgGizmo * pGizmo ) const;
	WgGizmoTaborder::TaborderEntry * 	_findInTaborder( WgGizmo * pGizmo ) const;
	WgGizmoTaborder::TaborderEntry * 	_validateEntryForward( WgGizmoTaborder::TaborderEntry * pEntry ) const;
	WgGizmoTaborder::TaborderEntry * 	_validateEntryBackward( WgGizmoTaborder::TaborderEntry * pEntry ) const;



	WgChain<TaborderEntry> 	m_tabOrder;
};

#endif //WG_GIZMO_TABORDER_DOT_H
