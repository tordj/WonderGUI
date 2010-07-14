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

#ifndef	WDG_EDITVALUE_DOT_H
#define	WDG_EDITVALUE_DOT_H

#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif

#ifndef WG_GIZMO_EDITVALUE_DOT_H
#	include <wg_gizmo_editvalue.h>
#endif

class Wdg_EditValue : public Wdg_Widget<WgGizmoEditvalue>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_EditValue,WgGizmoEditvalue);
	virtual ~Wdg_EditValue() {};

	//____ Callbacks ______________________________________
	
	static void cbIncValue			(void * pWdg)				{ ((Wdg_EditValue*)pWdg)->IncValue(); }
	static void cbDecValue			(void * pWdg)				{ ((Wdg_EditValue*)pWdg)->DecValue(); }

	static void cbIncValue			(void * pWdg, Sint32 inc)	{ ((Wdg_EditValue*)pWdg)->IncValue(inc); }
	static void cbDecValue			(void * pWdg, Sint32 dec)	{ ((Wdg_EditValue*)pWdg)->DecValue(dec); }


	static void cbSetValue			(void * pWdg, Sint64 value)	{ ((Wdg_EditValue*)pWdg)->SetValue(value); }
	static void cbSetFractionalValue(void * pWdg, float frac)	{ ((Wdg_EditValue*)pWdg)->SetFractionalValue(frac); }


};


#endif // WDG_VALUE_DOT_H
