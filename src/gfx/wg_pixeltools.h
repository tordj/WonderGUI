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
#ifndef	WG_PIXELTOOLS_DOT_H
#define	WG_PIXELTOOLS_DOT_H
#pragma once

#include <wg_gfxtypes.h>
#include <wg_color.h>

namespace wg
{
	namespace PixelTools
	{
		/* CURRENT LIMITATIONS
		
			Not more than 8 bits per channel.
			Bitplanes not supported.		
		*/


		bool copyPixels(int width, int height, uint8_t* pSrc, PixelFormat srcFmt, int srcPitchAdd,
			uint8_t* pDst, PixelFormat dstFmt, int dstPitchAdd, const Color8* pSrcPalette,
			Color8* pDstPalette, int srcPaletteEntries, int& dstPaletteEntries, int maxDstPaletteEntries);

		bool copyPixels(int width, int height, uint8_t* pSrc, const PixelDescription2& srcFmt, int srcPitchAdd,
			uint8_t* pDst, PixelFormat dstFmt, int dstPitchAdd, const Color8* pSrcPalette,
			Color8* pDstPalette, int srcPaletteEntries, int& dstPaletteEntries, int maxDstPaletteEntries);

	}
}

#endif //WG_PIXELTOOLS_DOT_H
