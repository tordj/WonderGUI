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


#ifndef WG_USERDEFINES_DOT_H
#define WG_USERDEFINES_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

//=============================================================================

//____ systemwide defines _____________________________________________________

//#define	WG_USE_SDL				// Set when Simple DirectMedia Layer is used
//								// as the base for WonderGUI for some extra
//								// speedups.


//____ font & print related defines ___________________________________________

#define	WG_USE_UTF8				// Set if you want to print UTF-8 (unicode) encoded
								// textstrings. Disable if you just want to use
								// simple, fast 8-bit (ascii + optionally 128
								// extra characters) textstrings.

//#define	WG_SAFE_UTF8		// Use slower, but more robust UTF-8 decoding
								// that better handles broken unicode strings.
								// Ignored if WG_USE_UTF8 isn't defined.

//#define WG_USE_FREETYPE			// Disable if you don't care for vector fonts and want to skip
								// dependency on freetype.


#define	WG_MAX_FONTSIZE		256	// Max size (pixels) for font.

#define WG_ESCAPE_CODE		0xA4

#define WG_DEFAULT_CODEPAGE WG_CODEPAGE_1252

//____ input related defines __________________________________________________

#define	WG_MAX_BUTTONS		8	// Maximum number of mouse buttons that wonderGUI is aware of.
#define	WG_MAX_WIDGETS		8	// Maximum number of widgets that can receive the same action.
								//(through intercept settings on parents). No overflow check!
#define	WG_MAX_KEYS_DOWN	16	// Maximum number of keys simultaneously depressed. No overflow check!

#define	WG_MAX_KEYS			128	// Number of keys that we'll let WonderGUI be aware of.
								// Keycode sent to WgInput may not equal or exceed this
								// number!


//____ enum WgModifierKeys ____________________________________________________

enum WgModifierKeys
{
	// Don't touch, needs to be first!
	WG_MODKEY_NONE = 0,			// = 0

	WG_MODKEY_SHIFT = 1,
	WG_MODKEY_ALT = 2,
	WG_MODKEY_ALT_SHIFT = WG_MODKEY_ALT | WG_MODKEY_SHIFT,
	WG_MODKEY_CTRL = 4,
	WG_MODKEY_CTRL_SHIFT = WG_MODKEY_CTRL | WG_MODKEY_SHIFT,
	WG_MODKEY_CTRL_ALT = WG_MODKEY_CTRL | WG_MODKEY_ALT,
	WG_MODKEY_CTRL_ALT_SHIFT = WG_MODKEY_CTRL | WG_MODKEY_ALT | WG_MODKEY_SHIFT,
	WG_MODKEY_SUPER = 8,

	WG_MODKEY_SUPER_SHIFT = 9,
	WG_MODKEY_SUPER_ALT = 10,
	WG_MODKEY_SUPER_ALT_SHIFT = 11,
	WG_MODKEY_SUPER_CTRL = 12,
	WG_MODKEY_SUPER_CTRL_SHIFT = 13,
	WG_MODKEY_SUPER_CTRL_ALT = 14,
	WG_MODKEY_SUPER_CTRL_ALT_SHIFT = 15,

	// Needs to be last!
	WG_MAX_MODKEYS = 16
};

//____ layer constants ____________________________________________________
#define WG_DEFAULT_LAYER 128


//=============================================================================
#endif	// WG_USERDEFINES_DOT_H
