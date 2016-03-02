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

#ifndef	WG_BITMAPFONT_DOT_H
#define	WG_BITMAPFONT_DOT_H

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_FONT_DOT_H
#	include <wg_font.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

namespace wg 
{
	
	class BitmapFont;
	typedef	StrongPtr<BitmapFont,Font_p>	BitmapFont_p;
	typedef	WeakPtr<BitmapFont,Font_wp>		BitmapFont_wp;
	
	//____ BitmapFont _____________________________________________________________
	
	class BitmapFont : public Font
	{
	public:
		static BitmapFont_p	create( const Surface_p& pSurf, char * pGlyphSpec, bool binaryFile=false ) { return BitmapFont_p(new BitmapFont(pSurf,pGlyphSpec,binaryFile)); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static BitmapFont_p	cast( const Object_p& pObject );
	
	
		inline Type			getType() const { return BITMAP; }
	
		void				insertGlyphs( const Surface_p& pSurf, char* pGlyphSpec, bool binaryFile=false );
		void				copyGlyphs( BitmapFont* pOtherFont );
	
	
		Glyph_p				getGlyph( uint16_t chr, int size = 0 );						// Size is just a dummy for BitmapFont...
		bool				hasGlyph( uint16_t chr );
		int					getKerning( Glyph_p pLeftGlyph, Glyph_p pRightGlyph, int size );
	
		inline int			getHeight( int size ) { return m_height; }
		inline int			getLineSpacing( int size ) { return m_height; }
		inline int			getBaseline( int size ) { return m_baseline; }
		inline int			getNbGlyphs() { return m_nGlyphs; }
		inline bool			hasGlyphs() { return m_nGlyphs?true:false; }
		inline bool			isMonospace() { return m_bMonospace; }
		inline int			getWhitespaceAdvance( int size ) { return m_spaceSpacing; }
		inline int			getMaxGlyphAdvance( int size ) { return m_maxSpacing; }
	
	protected:
		BitmapFont( const Surface_p& pSurf, char * pGlyphSpec, bool binaryFile=false );
		~BitmapFont();
	
		class MyGlyph : public Glyph
		{
		public:
			MyGlyph();
			MyGlyph( int advance, int8_t bearingX, int8_t bearingY, uint32_t kerningIndex, Font * pFont, const Surface_p& pSurf, const Rect& rect );
	
			const GlyphBitmap * getBitmap() { return &m_src; }
	
			void setAdvance( short advance ) { m_advance = advance; }
	
			GlyphBitmap	m_src;
		};
	
	
		MyGlyph * 	m_glyphTab[256];
	
		int			m_nKerningGlyphs;
		int8_t*		m_pKerningTable;
	
		bool		m_bMonospace;
		float		m_avgSpacing;
		int			m_maxSpacing;
		uint32_t	m_spaceSpacing;
		uint32_t	m_nGlyphs;
		uint32_t	m_height;
		int			m_baseline;
	};
	
	

} // namespace wg
#endif // WG_BITMAPFONT_DOT_H
