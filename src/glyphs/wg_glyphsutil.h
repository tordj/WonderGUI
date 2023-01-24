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

#ifndef WG_GLYPHSUTIL_DOT_H
#define WG_GLYPHSUTIL_DOT_H
#pragma once

#include <cstdint>
#include <wg_gfxutil.h>

namespace wg
{

	//____ Util _________________________________________________________________

	namespace Util		/** @private */
	{
		inline int readUTF8Char( const char *& pStr );
	}

	int Util::readUTF8Char( const char *& _pStr )
	{
		const uint8_t *& pStr = (const uint8_t *&) _pStr;

		int g = * pStr++;
		if( g < 128 )
			return g;											// 1 byte character (7 bits data).

		if( (g & 0xE0) == 0xC0 )					// 2 bytes character (11 bits data).
		{
			uint8_t c1 = pStr[0];
			if( (c1 & 0xC0) != 0x80 )
				return 0xFFFD;
			
			pStr += 1;
			return ((g & 0x1F) << 6) | int(c1 & 0x3F);
		}
		else if( (g & 0xF0) == 0xE0 )			// 3 bytes character (16 bits data).
		{
			uint8_t c1 = pStr[0];
			if( (c1 & 0xC0) != 0x80 )
				return 0xFFFD;

			uint8_t c2 = pStr[1];
			if( (c2 & 0xC0) != 0x80 )
				return 0xFFFD;

			pStr += 2;
			return ((g & 0x0F) << 12) | (int(c1 & 0x3F) << 6) | int(c2 & 0x3F) ;
		}
		else if( (g & 0xF8) == 0xF0 )			// 4 bytes character (21 bits data).
		{
			uint8_t c1 = pStr[0];
			if( (c1 & 0xC0) != 0x80 )
				return 0xFFFD;

			uint8_t c2 = pStr[1];
			if( (c2 & 0xC0) != 0x80 )
				return 0xFFFD;

			uint8_t c3 = pStr[2];
			if( (c3 & 0xC0) != 0x80 )
				return 0xFFFD;

			pStr += 3;
			return ((g & 0x07) << 18) | (int(c1 & 0x3F) << 12) | (int(c2 & 0x3F) << 6) | int(c3 & 0x3F);
		}
		else
			return 0xFFFD;					// Broken character (or more than 4 bytes which we don't support)

		return g;
	}

} // namespace wg
#endif // WG_GLYPHSUTIL_DOT_H
