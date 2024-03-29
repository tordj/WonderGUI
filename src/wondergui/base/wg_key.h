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
#pragma once

namespace wg
{


	//____ ModKeys ____________________________________________________

	namespace ModifierKeysNS
	{
		enum ModKeys
		{
			// Don't touch, needs to be first!
			None = 0,			// = 0

			Shift = 1,
			Alt = 2,
			AltShift = 3,
			Ctrl = 4,
			CtrlShift = 5,
			CtrlAlt = 6,
			CtrlAltShift = 7,
			Super = 8,

			SuperShift = 9,
			SuperAlt = 10,
			SuperAltShift = 11,
			SuperCtrl = 12,
			SuperCtrlShift = 13,
			SuperCtrlAlt = 14,
			SuperCtrlAltShift = 15,
		};

	}

	using ModKeys = ModifierKeysNS::ModKeys;

	//____ Key ________________________________________________________________

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
