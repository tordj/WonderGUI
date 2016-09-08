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

	//#define USE_FREETYPE			// Disable if you don't care for vector fonts and want to skip
									// dependency on freetype.

	//#define	SAFE_UTF8		// Use slower, but more robust UTF-8 decoding
									// that better handles broken unicode strings.
									// Ignored if USE_UTF8 isn't defined.


namespace wg
{
	//____ font & print related defines ___________________________________________




	static const int MaxFontSize = 256;	// Max size (pixels) for font.

	static const int EscapeCode = 0xA4;

	static const CodePage DefaultCodePage = CodePage::_1252;
}

//=============================================================================
#endif	// WG_USERDEFINES_DOT_H
