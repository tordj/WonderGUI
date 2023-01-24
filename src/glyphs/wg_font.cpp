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
#include <wg_font.h>
#include <wg_gfxbase.h>

namespace wg
{

	const TypeInfo Font::TYPEINFO = { "Font", &Object::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Font::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isMonochrome() ______________________________________________________

	bool Font::isMonochrome()
	{
		return true;
	}

	//____ getGlyphAsSurface() ___________________________________________________

	Surface_p Font::getGlyphAsSurface(uint16_t chr, const Surface::Blueprint& _bp, SurfaceFactory * pFactory )
	{
		Glyph glyph;
		getGlyphWithBitmap(chr, glyph);
		
		if( glyph.pSurface == nullptr )
			return nullptr;
		
		if( pFactory == nullptr )
			pFactory = GfxBase::defaultSurfaceFactory();
		
		if( !pFactory )
			return nullptr;
		
		RectI pixelRect = glyph.rect/64;
		
		Surface::Blueprint bp = _bp;
		if( bp.size.isEmpty() )
			bp.size = pixelRect.size();
		
		auto pSurface = pFactory->createSurface(bp);
		pSurface->copy({ 0,0 }, glyph.pSurface, pixelRect );
		return pSurface;
	}

} // namespace wg
