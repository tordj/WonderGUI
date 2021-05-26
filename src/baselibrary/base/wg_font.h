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

#ifndef	WG_FONT_DOT_H
#define	WG_FONT_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_pointers.h>
#include <wg_surface.h>

namespace wg
{

	class	Font;




	//____ GlyphBitmap _____________________________________________________________

	struct GlyphBitmap
	{
	public:

		//.____ Properties _________________________________

		Surface_p 	pSurface;
		RectSPX		rect;
		spx			bearingX;		// x offset when rendering the glyph (negated offset to glyph origo)
		spx			bearingY;		// y offset when rendering the glyph (negated offset to glyph origo)
	};


	//____ Glyph ________________________________________________________________

	class Glyph
	{
	friend class Font;

	public:

		//.____ Rendering ___________________________________________________________

		virtual const GlyphBitmap * getBitmap() = 0;

		//.____ Misc ___________________________________________________________

		inline spx		advance() { return m_advance; }
		inline int		kerningIndex() { return m_kerningIndex; }

        //.____ Internal _______________________________________________________

        inline Font *   _font() { return m_pFont; }

	protected:
		Glyph();
		Glyph( spx advance, int _kerningIndex, Font * pFont );
		virtual ~Glyph() {}

		Font *			m_pFont;		// glyphset that this glyph belongs to
		spx				m_advance;		// spacing to next glyph
		int				m_kerningIndex;	// index into kerning table (BitmapFont) or glyph_index (FreeTypeFont)
	};

	typedef Glyph*	Glyph_p;

	class Font;
	typedef	StrongPtr<Font>		Font_p;
	typedef	WeakPtr<Font>		Font_wp;

	//____ Font _____________________________________________________________
	/**
	 * @class Font
	 * @file wg_font.h
	 * @brief Base class for various types of fonts.
	 *
	 * The Font class is the base class from which all fonts are subclassed,
	 * providing the functionality to get metrics for the font and metrics
	 * and bitmaps of all its character glyphs.
	 */


	class Font : public Object
	{
	public:
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ____________________________________________

		virtual bool			setSize( spx size ) = 0;	///@brief Set the font size for subsequent calls.
		virtual spx				size() = 0;
		virtual spx				kerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph ) = 0;
		virtual Glyph_p			getGlyph( uint16_t chr ) = 0;

		virtual spx				lineGap() = 0;					///@brief Returns distance between two lines of this font and current size.
		virtual spx				whitespaceAdvance() = 0;		///@brief Returns width of a whitespace for current size.
		virtual spx				maxAdvance() = 0;				///@brief Returns largets occupied width of a character for current size.
		virtual spx				maxAscend() = 0;				///@brief Returns largets height from baseline of a character for current size.
		virtual spx 			maxDescend() = 0;				///@brief Returns largets depth from baseline of a character for current size.

		//.____ Misc ___________________________________________________________

		virtual int				nbGlyphs() = 0;					///@brief Return number of distinct glyphs in the font.
		virtual bool			hasGlyphs() = 0;				///@brief Check if font has any glyphs at all.
		virtual bool			hasGlyph( uint16_t chr ) = 0;	///@brief Check if font provides a glyph for specified character.
		virtual bool			isMonospace() = 0;				///@brief Check if font is monospaced.
		virtual bool			isMonochrome();					///@brief Check if font is monochrome or multi-colored.

        bool                    setBackupFont(Font * pFont);    ///@brief Font used when glyph can't be found in our font.
        inline Font_p           backupFont() const;             ///@brief Font used when glyph can't be found in our font.


	protected:
		Font() {}
		virtual ~Font() {}

        Font_p         m_pBackupFont;
	};


    //____ backupFont() _____________________________________________________

    inline Font_p Font::backupFont() const
    {
        return  m_pBackupFont;
    }


} // namespace wg
#endif // WG_FONT_DOT_H