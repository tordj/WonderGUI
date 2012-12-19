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

#ifndef WG_WIDGET_COLLECTION_DOT_H
#define WG_WIDGET_COLLECTION_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_HOOK_DOT_H
#	include <wg_hook.h>
#endif


class WgWidget;
class WgEmitter;


class WgWidgetCollection
{
	friend class WgWidget;

public:
	WgWidgetCollection() {};
	virtual ~WgWidgetCollection() {};

	inline WgWidget *	FirstWidget() const { WgHook * p = FirstHook(); if( p ) return p->Widget(); else return 0; }
	inline WgWidget *	LastWidget() const { WgHook * p = LastHook(); if( p ) return p->Widget(); else return 0; }

	inline WgHook *	FirstHook() const { return _firstHook(); }
	inline WgHook *	LastHook() const { return _lastHook(); }

protected:
	virtual WgHook*	_firstHook() const = 0;
	virtual WgHook*	_lastHook() const = 0;
};




#endif //WG_WIDGET_COLLECTION_DOT_H

