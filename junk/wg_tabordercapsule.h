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

#ifndef WG_TABORDERCAPSULE_DOT_H
#define WG_TABORDERCAPSULE_DOT_H

#ifndef WG_CAPSULE_DOT_H
#	include <wg_capsule.h>
#endif


class WgTaborderCapsule : public WgCapsule
{
public:
	WgTaborderCapsule();
	~WgTaborderCapsule();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgTaborderCapsule(); };
	

	bool	AddToTaborder( WgWidget * pWidget );
	bool	InsertInTaborder( WgWidget * pWidget, WgWidget * pBefore );

	bool	RemoveFromTaborder( WgWidget * pWidget );
	void	ClearTaborder();


	WgWidget * FirstInTaborder() const;
	WgWidget * NextInTaborder( WgWidget * pCurrWidget ) const;
	WgWidget * PrevInTaborder( WgWidget * pCurrWidget ) const;
	WgWidget * LastInTaborder() const;

protected:

	virtual void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );

private:

	class TaborderEntry : public WgLink
	{
	public:
		LINK_METHODS( TaborderEntry );

		TaborderEntry(WgWidget* p) { pWidget = p; }
		WgWidgetWeakPtr 	pWidget;
	};

	bool 								_isValidForTaborderInsertion( WgWidget * pWidget ) const;
	WgTaborderCapsule::TaborderEntry * 	_findInTaborder( WgWidget * pWidget ) const;
	WgTaborderCapsule::TaborderEntry * 	_validateEntryForward( WgTaborderCapsule::TaborderEntry * pEntry ) const;
	WgTaborderCapsule::TaborderEntry * 	_validateEntryBackward( WgTaborderCapsule::TaborderEntry * pEntry ) const;



	WgChain<TaborderEntry> 	m_tabOrder;
};

#endif //WG_TABORDERCAPSULE_DOT_H
