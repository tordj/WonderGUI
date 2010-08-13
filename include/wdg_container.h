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

#ifndef	WDG_CONTAINER_DOT_H
#define	WDG_CONTAINER_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif

#ifndef WG_GIZMO_DUMMY_DOT_H
#	include <wg_gizmo_dummy.h>
#endif


class Wdg_Container : public Wdg_Widget<WgGizmoDummy>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_Container,WgGizmoDummy);
	virtual ~Wdg_Container() {};

};

#endif //	WDG_CONTAINER_DOT_H
