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

#ifndef	WG_DUMMYFONT_DOT_H
#define	WG_DUMMYFONT_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_font.h>



namespace wg
{
	class DummyFont;
	typedef	StrongPtr<DummyFont,Font_p>	DummyFont_p;
	typedef	WeakPtr<DummyFont,Font_wp>		DummyFont_wp;

	class DummyFont : public Font
	{
	public:
		static DummyFont_p	create() { return DummyFont_p(new DummyFont()); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static DummyFont_p	cast( const Object_p& pObject );

		int				nbGlyphs();
		bool			hasGlyphs();
		bool			isMonospace();
		bool			isMonochrome();
		bool			hasGlyph( uint16_t chr );


		bool			setSize( int size );
		int				size();


		// Methods dealing with the font in the specified size.
	
		int				kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph );
		Glyph_p			getGlyph( uint16_t chr );
	
		int				lineGap();
		int				whitespaceAdvance();
		int				maxAdvance();
		int				maxAscend();
		int				maxDescend();


	protected:
		DummyFont();
		~DummyFont();

		class DummyGlyph : public Glyph
		{
		public:
			DummyGlyph( Font * pFont );
	
			const GlyphBitmap * getBitmap() { return &m_src; }	
			GlyphBitmap	m_src;
		};

		DummyGlyph	m_glyph;

	};

}

#endif // WG_DUMMYFONT_DOT_H
