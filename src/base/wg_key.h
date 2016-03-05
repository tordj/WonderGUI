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
#define WG_KEY_DOT_H

namespace wg 
{
	
	
	//____ enum ModifierKeys ____________________________________________________

	enum ModifierKeys
	{
		// Don't touch, needs to be first!
		MODKEY_NONE = 0,			// = 0

		MODKEY_SHIFT = 1,
		MODKEY_ALT = 2,
		MODKEY_ALT_SHIFT = MODKEY_ALT | MODKEY_SHIFT,
		MODKEY_CTRL = 4,
		MODKEY_CTRL_SHIFT = MODKEY_CTRL | MODKEY_SHIFT,
		MODKEY_CTRL_ALT = MODKEY_CTRL | MODKEY_ALT,
		MODKEY_CTRL_ALT_SHIFT = MODKEY_CTRL | MODKEY_ALT | MODKEY_SHIFT,
		MODKEY_SUPER = 8,

		MODKEY_SUPER_SHIFT = 9,
		MODKEY_SUPER_ALT = 10,
		MODKEY_SUPER_ALT_SHIFT = 11,
		MODKEY_SUPER_CTRL = 12,
		MODKEY_SUPER_CTRL_SHIFT = 13,
		MODKEY_SUPER_CTRL_ALT = 14,
		MODKEY_SUPER_CTRL_ALT_SHIFT = 15,
	};
	
	
	enum class Key
	{
		Left = 1,
		Right,
		Up,
		Down,
	
		Home,
		End,
		PageUp,
		PageDown,
	
		Return,
		Backspace,
		Delete,
		Tab,
		Escape,
		Space,
	
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
	
		Shift,
		Control,
		Alt,
		Super,
	
		Unmapped		// This one must always be last!
	};
	
	

} // namespace wg
#endif // WG_KEY_DOT_H
