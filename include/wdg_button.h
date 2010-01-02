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

#ifndef	WDG_BUTTON_DOT_H
#define	WDG_BUTTON_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_BUTTON_DOT_H
#	include <wg_gizmo_button.h>
#endif

class Wdg_Button : public Wdg_Widget<WgGizmoButton>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_Button,WgGizmoButton);
	virtual ~Wdg_Button() {};

};

#endif //	WDG_BUTTON_DOT_H
