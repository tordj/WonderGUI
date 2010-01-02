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

#ifndef WG_RESOURCE_VISITOR_DOT_H
#define WG_RESOURCE_VISITOR_DOT_H

class WgResourceVisitor
{
public:
	virtual void Visit(class WgXmlRoot*) {}
	virtual void Visit(class WgInterceptRes*) {}
	virtual void Visit(class WgModePropRes*) {}
	virtual void Visit(class WgTextPropRes*) {}
	virtual void Visit(class WgGeometryRes*) {}
	virtual void Visit(class WgColorRes*) {}
	virtual void Visit(class WgSurfaceRes*) {}
	virtual void Visit(class WgGlyphSetRes*) {}
	virtual void Visit(class WgFontRes*) {}
	virtual void Visit(class WgAnimRes*) {}
	virtual void Visit(class WgBlockSetRes*) {}
	virtual void Visit(class WgWidgetRes*) {}
	virtual void Visit(class WgItemRes*) {}
	virtual void Visit(class WgMenuItemRes*) {}
	virtual void Visit(class WgMenuSubMenuRes*) {}
	virtual void Visit(class WgBorderRes*) {}
	virtual void Visit(class WgTileRes*) {}
	virtual void Visit(class WgBlockRes*) {}
	virtual void Visit(class WgTextHolderRes*) {}
	virtual void Visit(class WgItemHolderRes*) {}
	virtual void Visit(class WgValueHolderRes*) {}
	virtual void Visit(class Wdg_TabList_Res*) {}
	virtual void Visit(class Wdg_TableView_Res*) {}
	virtual void Visit(class Wdg_ComboBox_Res*) {}
	virtual void Visit(class Wdg_Menu_Res*) {}
	virtual void Visit(class Wdg_MenuBar_Res*) {}
	virtual void Visit(class WgMenuBarItemRes*) {}
	virtual void Visit(class WgBaseViewRes*) { }
};

#endif