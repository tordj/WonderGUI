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

#ifndef	WDG_TABLIST_DOT_H
#define	WDG_TABLIST_DOT_H

#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_TABLIST_DOT_H
#	include <wg_gizmo_tablist.h>
#endif

class Wdg_TabList : public Wdg_Widget<WgGizmoTablist>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_TabList,WgGizmoTablist);
	virtual ~Wdg_TabList() {};

};


#endif //WDG_TABLIST_DOT_H
