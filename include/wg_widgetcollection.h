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

#ifndef WG_WIDGETCOLLECTION_DOT_H
#define WG_WIDGETCOLLECTION_DOT_H

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

class WgWidget;
class WgWidgetCollection;

typedef	WgSmartPtr<WgWidgetCollection>	WgWidgetCollectionPtr;

class WgWidgetCollection : public WgObject
{
public:
	virtual int			Size() const = 0;
	virtual bool		IsEmpty() const = 0;

	virtual WgWidget *	First() const = 0;
	virtual WgWidget *	Last() const = 0;

	virtual WgWidget *	Get( int index ) const = 0;
	virtual	int			Find( WgWidget * pWidget ) const = 0;
	virtual WgWidget *	Next( WgWidget * pWidget ) const = 0;
	virtual WgWidget *	Prev( WgWidget * pWidget ) const = 0;
protected:
	WgWidgetCollection() {};
	virtual ~WgWidgetCollection() {};


};




#endif //WG_WIDGETCOLLECTION_DOT_H

