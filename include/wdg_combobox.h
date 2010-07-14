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

#ifndef	WDG_COMBOBOX_DOT_H
#define WDG_COMBOBOX_DOT_H

#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif

#ifndef WG_GIZMO_COMBOBOX_DOT_H
#	include <wg_gizmo_combobox.h>
#endif

	// Press in textfield:
	//		Editable - Grab input focus.
	//		Non-editable - open menu.

	//	+ Option för att fälla ut meny när man börjar editera?
	//	+ Option för att öppna menyn över/under/over? (hur gör vi med button om vi tillåter over?).
	//	+ Sätta bredd och maxhöjd på meny? Eller funktion för detta i menyn?
	//	+ Funktion för att sätta default text (antingen entry eller meddelande eller specificerad text?)
	//	+ Pil upp/ner för att stega igenom menyalternativen, både på öppen och stängd meny.

class Wdg_ComboBox : public Wdg_Widget<WgGizmoCombobox>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_ComboBox,WgGizmoCombobox);
	virtual ~Wdg_ComboBox() {};

};

#endif // WDG_COMBOBOX_DOT_H
