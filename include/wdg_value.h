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

#ifndef	WDG_VALUE_DOT_H
#define	WDG_VALUE_DOT_H

#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_VALUE_DOT_H
#	include <wg_gizmo_value.h>
#endif

class Wdg_Value : public Wdg_Widget<WgGizmoValue>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_Value,WgGizmoValue);
	virtual ~Wdg_Value() {};

	//____ Callbacks ______________________________________
	
	static void cbIncValue			(void * pWdg)				{ ((Wdg_Value*)pWdg)->IncValue(); }
	static void cbDecValue			(void * pWdg)				{ ((Wdg_Value*)pWdg)->DecValue(); }

	static void cbSetValue			(void * pWdg, Sint32 value)	{ ((Wdg_Value*)pWdg)->SetValue(value); }
	static void cbSetFractionalValue(void * pWdg, float frac)	{ ((Wdg_Value*)pWdg)->SetFractionalValue(frac); }

};


#endif // WDG_VALUE_DOT_H
