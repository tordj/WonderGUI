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


#ifndef WG_CABIFREETYPEFONT_DOT_H
#define WG_CABIFREETYPEFONT_DOT_H
#pragma once


#include <wg_cabi_font.h>
#include <wg_cabi_bitmapcache.h>

namespace wg
{
	class	CABIFreeTypeFont;
	typedef	StrongPtr<CABIFreeTypeFont>	CABIFreeTypeFont_p;
	typedef	WeakPtr<CABIFreeTypeFont>		CABIFreeTypeFont_wp;

	class CABIFreeTypeFont : public CABIFont
	{
	public:


		//.____ Creation __________________________________________

		static CABIFreeTypeFont_p	create(wg_obj cFreeTypeFont, CABIBitmapCache* pCache);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ______________________________________________________

		void		getGlyphWithBitmap(uint16_t chr, Glyph& glyph) override;


	private:
		CABIFreeTypeFont( wg_obj cFont, CABIBitmapCache * pCache );
		~CABIFreeTypeFont();
		
		CABIBitmapCache_p m_pCache;
		
	};



} // namespace wg

#endif //WG_CABIFREETYPEFONT_DOT_H
