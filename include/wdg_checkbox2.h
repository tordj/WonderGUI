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

#ifndef	WDG_CHECKBOX2_DOT_H
#define	WDG_CHECKBOX2_DOT_H

#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_CHECKBOX_DOT_H
#	include <wg_gizmo_checkbox.h>
#endif

class Wdg_CheckBox2 : public Wdg_Widget<WgGizmoCheckbox>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_CheckBox2,WgGizmoCheckbox);
	virtual ~Wdg_CheckBox2() {};

	static void cbCheck( void * pObj ) {((Wdg_CheckBox2*)pObj)->Check();};
	static void cbUncheck( void * pObj ) {((Wdg_CheckBox2*)pObj)->Uncheck();};
	static void cbChangeState( void * pObj ) {((Wdg_CheckBox2*)pObj)->ChangeState();};
	static void cbSetState( void * pObj, bool state ) {((Wdg_CheckBox2*)pObj)->SetState(state);};

};


#endif //	TEXT_WDG_DOT_H
