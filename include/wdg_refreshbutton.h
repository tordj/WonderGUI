/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@swipnet.se].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------
	
  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@swipnet.se] for details.

=========================================================================*/

#ifndef	WDG_REFRESHBUTTON_DOT_H
#define	WDG_REFRESHBUTTON_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_REFRESHBUTTON_DOT_H
#	include <wg_gizmo_refreshbutton.h>
#endif

class Wdg_RefreshButton : public Wdg_Widget<WgGizmoRefreshButton>
{
public:
	Wdg_RefreshButton( void ) {};
	Wdg_RefreshButton ( const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoRefreshButton>(geometry, pParent) {};
	Wdg_RefreshButton( WgOrigo origo, const WgRect& geometry, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoRefreshButton>( origo, geometry, pParent ) {};
	Wdg_RefreshButton( WgOrigo upperLeft, Sint32 x1, Sint32 y1, WgOrigo lowerRight, Sint32 x2, Sint32 y2, WgWidget * pParent = 0 ) : Wdg_Widget<WgGizmoRefreshButton>(upperLeft,x1, y1, lowerRight, x2, y2, pParent) {}	
	virtual ~Wdg_RefreshButton() {};

};

#endif //	WDG_REFRESHBUTTON_DOT_H
