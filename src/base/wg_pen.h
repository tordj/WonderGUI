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
#ifndef WG_PEN_DOT_H
#define WG_PEN_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_GLYPHSET_DOT_H
#	include <wg_glyphset.h>
#endif

#ifndef	WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

namespace wg 
{
	
	//____ Pen _____________________________________________________________
	
	class Pen
	{
	friend class Font;
	
	public:
		Pen();
		Pen( GfxDevice * pDevice, const Coord& origo, const Rect& clip = Rect() );
		~Pen() {}
	
		void					setClipRect( const Rect& clip );
		inline void				setDevice( GfxDevice * pDevice ) { m_pDevice = pDevice; }
	
		void					setOrigo( const Coord& pos ) { m_origo = pos; }
	
		bool					setAttributes( const TextAttr& attr );
		bool					setSize( int size );
		void					setFont( const Font_p& pFont );
		void					setStyle( FontAlt style );
		void					setColor( Color color );
	//	void					setCharVisibility( int visibilityFlags );		// We need something better here...
	
	
		inline void				setPos( const Coord& pos ) { m_pos = pos; }
		inline void				setPosX( int x ) { m_pos.x = x; }
		inline void				setPosY( int y ) { m_pos.y = y; }
	
		inline void				move( const Coord& pos ) { m_pos += pos; }
		inline void				moveX( int x ) { m_pos.x += x; }
		inline void				moveY( int y ) { m_pos.y += y; }
	
	
		void					setTab( int width ) { m_tabWidth = width; }
		bool					setChar( uint32_t chr );
		void					flushChar() { m_pGlyph = &m_dummyGlyph; m_dummyGlyph.setAdvance(0); }
		void					applyKerning() { if( m_pPrevGlyph != &m_dummyGlyph && m_pGlyph != &m_dummyGlyph ) m_pos.x += m_pGlyphs->getKerning( m_pPrevGlyph, m_pGlyph, m_size ); }
	
		inline void				advancePos() { m_pos.x += m_pGlyph->advance(); }							///< Advances position past current character.
		inline void				advancePosMonospaced() { m_pos.x += m_pGlyphs->getMaxGlyphAdvance(m_size); }	///< Advances position past current character using monospace spacing.
		void					advancePosCursor( const CaretInstance& instance );
	
		inline Glyph_p		getGlyph() const { return m_pGlyph; }
		inline Coord			getPos() const { return m_pos; }
		inline int				getPosX() const { return m_pos.x; }
		inline int				getPosY() const { return m_pos.y; }
	
	//	inline Coord			getBlitPos() const { return Coord( m_pos.x + m_pGlyph->bearingX(), m_pos.y + m_pGlyph->bearingY() ); }
	//	inline int				getBlitPosX() const { return m_pos.x + m_pGlyph->bearingX(); }
	//	inline int				getBlitPosY() const { return m_pos.y + m_pGlyph->bearingY(); }
	
		inline const Rect&	getClipRect() const { return m_clipRect; }
		inline bool				hasClipRect() const { return m_bClip; }
	
		inline Font_p		getFont() const { return m_pFont; }
		inline int				getSize() const { return m_size; }
		inline FontAlt		getStyle() const { return m_style; }
		inline Color			getColor() const { return m_color; }
		inline Glyphset_p	getGlyphset() const { return m_pGlyphs; }
	
	
		inline int				getLineSpacing() const { return m_pGlyphs->getLineSpacing(m_size); }
		inline int				getLineHeight() const { return m_pGlyphs->getHeight(m_size); }
		inline int				getBaseline() const { return m_pGlyphs->getBaseline(m_size); }
	
		void					blitChar() const;
		bool					blitCursor( const CaretInstance& instance ) const;
	
	private:
		void _init();
	
		void _onAttrChanged();
	
		class DummyGlyph : public Glyph
		{
		public:
			DummyGlyph() : Glyph( 0, 0, 0 ) {}
			const GlyphBitmap * getBitmap() { return 0; }
	
			void setAdvance( int advance ) { m_advance = advance; }
		};
	
		//
	
		//
	
		Glyphset_p	m_pGlyphs;			// Pointer at our glyphs.
	
		Glyph_p		m_pPrevGlyph;	// Previous glyph, saved to allow for kerning.
		Glyph_p		m_pGlyph;		// Current glyph.
	
		Font_p		m_pFont;		// Pointer back to the font.
		int				m_wantedSize;	// Size we requested.
		int				m_size;			// Fontsize we got a glyphset for, which might be smaller than what we requested.
		FontAlt		m_style;		// Style of glyphset we requested.
		Color			m_color;		// Color this pen draws in.
	
		bool			m_bShowSpace;	// Set if space control character should be shown (usually a dot in the middle of the cell).
		bool			m_bShowCRLF;	// Set if the CR/LF control character should be shown.
	
		Coord			m_origo;		// Origo position, from where we start printing and count tab-positions.
		Coord			m_pos;			// Position of this pen in screen pixels.
	
		DummyGlyph		m_dummyGlyph;	// Dummy glyph used for whitespace, tab etc
	
		int				m_tabWidth;		// Tab width in pixels.
	
		GfxDevice *	m_pDevice;		// Device used for blitting.
	
		bool			m_bClip;		// Set if we have a clipping rectangle.
		Rect			m_clipRect;		// Clipping rectangle used for clipBlit().
	
	
	};
	

} // namespace wg
#endif //WG_PEN_DOT_H
