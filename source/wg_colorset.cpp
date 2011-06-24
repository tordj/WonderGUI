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

#include <wg_colorset.h>


WgColorSetPtr WgColorSet::Create()
{
	return WgColorSetPtr(new WgColorSet());
}

WgColorSetPtr WgColorSet::Create( WgColor allModes )
{
	WgColorSet * p = new WgColorSet();

	p->m_color[WG_MODE_NORMAL] = allModes;
	p->m_color[WG_MODE_MARKED] = allModes;
	p->m_color[WG_MODE_SELECTED] = allModes;
	p->m_color[WG_MODE_DISABLED] = allModes;
	p->m_color[WG_MODE_SPECIAL] = allModes;

	return WgColorSetPtr(p);
}

WgColorSetPtr  WgColorSet::Create( WgColor normal, WgColor over, WgColor selected, WgColor disabled, WgColor special )
{
	WgColorSet * p = new WgColorSet();

	p->m_color[WG_MODE_NORMAL] = normal;
	p->m_color[WG_MODE_MARKED] = over;
	p->m_color[WG_MODE_SELECTED] = selected;
	p->m_color[WG_MODE_DISABLED] = disabled;
	p->m_color[WG_MODE_SPECIAL] = special;

	return WgColorSetPtr(p);
}

void WgColorSet::SetColor( WgColor color, WgMode mode )
{ 
	if( mode == WG_MODE_ALL )
	{
		m_color[WG_MODE_NORMAL] = color;
		m_color[WG_MODE_MARKED] = color;
		m_color[WG_MODE_SELECTED] = color;
		m_color[WG_MODE_DISABLED] = color;
		m_color[WG_MODE_SPECIAL] = color;
	}
	else
		m_color[mode] = color; 
}

