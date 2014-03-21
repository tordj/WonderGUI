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

#ifndef	WG_IWIDGETHOLDER_DOT_H
#define	WG_IWIDGETHOLDER_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class WgHook;

class WgIWidgetHolder;
typedef	WgISmartPtr<WgIWidgetHolder,WgInterfacePtr>		WgIWidgetHolderPtr;
typedef	WgWeakPtr<WgIWidgetHolder,WgInterfacePtr>		WgIWidgetHolderWeakPtr;

class WgIWidgetHolder : public WgInterface
{
	
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgIWidgetHolderPtr	Cast( const WgInterfacePtr& pInterface );

	inline WgWidgetPtr	FirstWidget() const { return WgWidgetPtr(_firstWidget()); }
	inline WgWidgetPtr	LastWidget() const { return WgWidgetPtr(_lastWidget()); }

	inline WgHookPtr	FirstHook() const { return _firstHook(); }
	inline WgHookPtr	LastHook() const { return _lastHook(); }

protected:

	virtual WgHook*		_firstHook() const = 0;
	virtual WgHook*		_lastHook() const = 0;

	WgWidget *			_firstWidget() const;
	WgWidget *			_lastWidget() const;
};


#endif //WG_IWIDGETHOLDER_DOT_H
