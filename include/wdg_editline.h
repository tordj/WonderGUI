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

#ifndef	WDG_EDITLINE_DOT_H
#define	WDG_EDITLINE_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif

#ifndef WG_GIZMO_EDITLINE_DOT_H
#	include <wg_gizmo_editline.h>
#endif

class Wdg_EditLine : public Wdg_Widget<WgGizmoEditline>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_EditLine,WgGizmoEditline);
	virtual ~Wdg_EditLine() {};
	virtual bool		IsInputField() const	{ return (TextObj()->nbChars() > 0); }
};

#endif // WDG_EDITLINE_DOT_H
