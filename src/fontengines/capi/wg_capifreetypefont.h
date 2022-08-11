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


#ifndef WG_CAPIFREETYPEFONT_DOT_H
#define WG_CAPIFREETYPEFONT_DOT_H
#pragma once


#include <wg_capifont.h>

namespace wg
{
	class	CAPIFreeTypeFont;
	typedef	StrongPtr<CAPIFreeTypeFont>	CAPIFreeTypeFont_p;
	typedef	WeakPtr<CAPIFreeTypeFont>		CAPIFreeTypeFont_wp;

	class CAPIFreeTypeFont : public CAPIFont
	{
	public:


		//.____ Creation __________________________________________

		static CAPIFreeTypeFont_p	create( wg_obj cFreeTypeFont ) { return CAPIFreeTypeFont_p(new CAPIFreeTypeFont(cFreeTypeFont)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ______________________________________________________

		void		getGlyphWithBitmap(uint16_t chr, Glyph& glyph) override;


	private:
		CAPIFreeTypeFont( wg_obj cFont );
		~CAPIFreeTypeFont();
		
		
		
	};



} // namespace wg

#endif //WG_CAPIFREETYPEFONT_DOT_H
