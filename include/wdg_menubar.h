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

#ifndef	WDG_MENUBAR_DOT_H
#define	WDG_MENUBAR_DOT_H


#ifndef	WG_GIZMO_WIDGET_WRAPPER_DOT_H
#	include <wg_gizmo_widget_wrapper.h>
#endif


#ifndef WG_GIZMO_MENUBAR_DOT_H
#	include <wg_gizmo_menubar.h>
#endif


/*
	Att stödja:

	- Ikon/checkbox till vänster.
	- Accelerator till höger (innan pilen).
	- Separator.
	- Cascading menus med pil till höger.
	- Disablade entries.
	- Navigera menyerna med tangenter + keybindings.

	- Drop down/Pull down.

	- "End of menu"-section with info.
	- Baloon help.
	- Automatiskt öppna uppåt eller nedåt beroende på plats.

	* Press on head drar ner menyn, release på entry aktiverar entry (så fungerar alla fallen).
	* Click på öppen header samt Release/press utanför menyn stänger menyn.
	* Mouse over på annan header byter meny.
*/


class Wdg_MenuBar : public Wdg_Widget<WgGizmoMenubar>
{
public:
	WIDGET_GIZMO_CONSTRUCTORS(Wdg_MenuBar,WgGizmoMenubar);
	virtual ~Wdg_MenuBar() {};

};


#endif //	WDG_MENUBAR_DOT_H
