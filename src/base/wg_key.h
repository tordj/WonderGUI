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
#ifndef WG_KEY_DOT_H

namespace wg 
{
	#define WG_KEY_DOT_H
	
	
	enum WgKey
	{
		WG_KEY_LEFT = 1,
		WG_KEY_RIGHT,
		WG_KEY_UP,
		WG_KEY_DOWN,
	
		WG_KEY_HOME,
		WG_KEY_END,
		WG_KEY_PAGE_UP,
		WG_KEY_PAGE_DOWN,
	
		WG_KEY_RETURN,
		WG_KEY_BACKSPACE,
		WG_KEY_DELETE,
		WG_KEY_TAB,
		WG_KEY_ESCAPE,
		WG_KEY_SPACE,
	
		WG_KEY_F1,
		WG_KEY_F2,
		WG_KEY_F3,
		WG_KEY_F4,
		WG_KEY_F5,
		WG_KEY_F6,
		WG_KEY_F7,
		WG_KEY_F8,
		WG_KEY_F9,
		WG_KEY_F10,
		WG_KEY_F11,
		WG_KEY_F12,
	
		WG_KEY_SHIFT,
		WG_KEY_CONTROL,
		WG_KEY_ALT,
		WG_KEY_SUPER,
	
		WG_KEY_UNMAPPED		// This one must always be last!
	};
	
	#define	WG_KEY_ARRAYSIZE		WG_KEY_UNMAPPED
	
	

} // namespace wg
#endif // WG_KEY_DOT_H
