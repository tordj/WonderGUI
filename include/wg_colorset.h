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

#ifndef WG_COLORSET_DOT_H
#define WG_COLORSET_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

class WgColorSet;

typedef	WgSmartPtr<WgColorSet>	WgColorSetPtr;

class WgColorSet : public WgRefCounted
{
public:
	static WgColorSetPtr	Create();
	static WgColorSetPtr	Create( WgColor allModes );
	static WgColorSetPtr	Create( WgColor normal, WgColor over, WgColor selected, WgColor disabled, WgColor special );

	inline WgColor	Color(WgMode mode) const { return m_color[mode]; }
	void			SetColor(WgColor color, WgMode mode = WG_MODE_ALL);

private:
	WgColorSet() {};
	virtual ~WgColorSet() {};

	WgColor		m_color[WG_NB_MODES];
};



#endif //WG_COLORSET_DOT_H
