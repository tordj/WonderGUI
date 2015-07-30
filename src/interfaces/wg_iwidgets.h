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

#ifndef	WG_IWIDGETS_DOT_H
#define	WG_IWIDGETS_DOT_H

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class WgHook;

class WgIWidgets;
typedef	WgIStrongPtr<WgIWidgets,WgInterface_p>		WgIWidgets_p;
typedef	WgIWeakPtr<WgIWidgets,WgInterface_wp>	WgIWidgets_wp;

class WgIWidgets : public WgInterface
{
	
public:
	virtual bool			isInstanceOf( const char * pClassName ) const;
	virtual const char *	className( void ) const;
	static const char		CLASSNAME[];
	static WgIWidgets_p	cast( const WgInterface_p& pInterface );

	inline WgWidget_p	firstWidget() const { return WgWidget_p(_firstWidget()); }
	inline WgWidget_p	lastWidget() const { return WgWidget_p(_lastWidget()); }

protected:

	virtual WgWidget *		_firstWidget() const = 0;
	virtual WgWidget *		_lastWidget() const = 0;
};


#endif //WG_IWIDGETS_DOT_H
