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


#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <wg_glyphset.h>
#include <wg_texttool.h>


// TEMP PROFILING INCLUDES
//#include "Utilities/EProfiler.h"
//#include "Misc/Util/Util.hpp"






//____ WgGlyph::WgGlyph() _______________________________________________________

WgGlyph::WgGlyph()
{
	rect.x = 0; rect.y = 0; rect.w = 0; rect.h = 0;
	advance = 0;
	bearingX = 0;
	bearingY = 0;
	kerningIndex = 0;
	pSurf = 0;
}

//____ WgGlyph::WgGlyph() _______________________________________________________

WgGlyph::WgGlyph(Sint32 _rectX, Sint32 _rectY, Sint32 _rectW, Sint32 _rectH, Uint8 _advance, Sint8 _bearingX, Sint8 _bearingY, Uint16 _kerningIndex, WgSurface* _pSurface)
{
	rect.x = _rectX; rect.y = _rectY; rect.w = _rectW; rect.h = _rectH;
	advance = _advance;
	bearingX = _bearingX;
	bearingY = _bearingY;
	kerningIndex = _kerningIndex;
	pSurf = _pSurface;
}


