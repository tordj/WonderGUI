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

//____ WgPen _____________________________________________________________

class WgPen
{
friend class WgFont;

public:
	WgPen();
	WgPen( WgGfxDevice * pDevice, const WgCoord& origo, const WgRect& clip = WgRect() );
	~WgPen() {}

	void					setClipRect( const WgRect& clip );
	inline void				setDevice( WgGfxDevice * pDevice ) { m_pDevice = pDevice; }

	void					setOrigo( const WgCoord& pos ) { m_origo = pos; }

	bool					setAttributes( const WgTextAttr& attr );
	bool					setSize( int size );
	void					setFont( const WgFontPtr& pFont );
	void					setStyle( WgFontAlt style );
	void					setColor( WgColor color );
//	void					setCharVisibility( int visibilityFlags );		// We need something better here...


	inline void				setPos( const WgCoord& pos ) { m_pos = pos; }
	inline void				setPosX( int x ) { m_pos.x = x; }
	inline void				setPosY( int y ) { m_pos.y = y; }

	inline void				move( const WgCoord& pos ) { m_pos += pos; }
	inline void				moveX( int x ) { m_pos.x += x; }
	inline void				moveY( int y ) { m_pos.y += y; }


	void					setTab( int width ) { m_tabWidth = width; }
	bool					setChar( Uint32 chr );
	void					flushChar() { m_pGlyph = &m_dummyGlyph; m_dummyGlyph.setAdvance(0); }
	void					applyKerning() { if( m_pPrevGlyph != &m_dummyGlyph && m_pGlyph != &m_dummyGlyph ) m_pos.x += m_pGlyphs->getKerning( m_pPrevGlyph, m_pGlyph, m_size ); }

	inline void				advancePos() { m_pos.x += m_pGlyph->advance(); }							///< Advances position past current character.
	inline void				advancePosMonospaced() { m_pos.x += m_pGlyphs->getMaxGlyphAdvance(m_size); }	///< Advances position past current character using monospace spacing.
	void					advancePosCursor( const WgCaretInstance& instance );

	inline WgGlyphPtr		getGlyph() const { return m_pGlyph; }
	inline WgCoord			getPos() const { return m_pos; }
	inline int				getPosX() const { return m_pos.x; }
	inline int				getPosY() const { return m_pos.y; }

//	inline WgCoord			getBlitPos() const { return WgCoord( m_pos.x + m_pGlyph->bearingX(), m_pos.y + m_pGlyph->bearingY() ); }
//	inline int				getBlitPosX() const { return m_pos.x + m_pGlyph->bearingX(); }
//	inline int				getBlitPosY() const { return m_pos.y + m_pGlyph->bearingY(); }

	inline const WgRect&	getClipRect() const { return m_clipRect; }
	inline bool				hasClipRect() const { return m_bClip; }

	inline WgFontPtr		getFont() const { return m_pFont; }
	inline int				getSize() const { return m_size; }
	inline WgFontAlt		getStyle() const { return m_style; }
	inline WgColor			getColor() const { return m_color; }
	inline WgGlyphsetPtr	getGlyphset() const { return m_pGlyphs; }


	inline int				getLineSpacing() const { return m_pGlyphs->getLineSpacing(m_size); }
	inline int				getLineHeight() const { return m_pGlyphs->getHeight(m_size); }
	inline int				getBaseline() const { return m_pGlyphs->getBaseline(m_size); }

	void					blitChar() const;
	bool					blitCursor( const WgCaretInstance& instance ) const;

private:
	void _init();

	void _onAttrChanged();

	class DummyGlyph : public WgGlyph
	{
	public:
		DummyGlyph() : WgGlyph( 0, 0, 0 ) {}
		const WgGlyphBitmap * getBitmap() { return 0; }

		void setAdvance( int advance ) { m_advance = advance; }
	};

	//

	//

	WgGlyphsetPtr	m_pGlyphs;			// Pointer at our glyphs.

	WgGlyphPtr		m_pPrevGlyph;	// Previous glyph, saved to allow for kerning.
	WgGlyphPtr		m_pGlyph;		// Current glyph.

	WgFontPtr		m_pFont;		// Pointer back to the font.
	int				m_wantedSize;	// Size we requested.
	int				m_size;			// Fontsize we got a glyphset for, which might be smaller than what we requested.
	WgFontAlt		m_style;		// Style of glyphset we requested.
	WgColor			m_color;		// Color this pen draws in.

	bool			m_bShowSpace;	// Set if space control character should be shown (usually a dot in the middle of the cell).
	bool			m_bShowCRLF;	// Set if the CR/LF control character should be shown.

	WgCoord			m_origo;		// Origo position, from where we start printing and count tab-positions.
	WgCoord			m_pos;			// Position of this pen in screen pixels.

	DummyGlyph		m_dummyGlyph;	// Dummy glyph used for whitespace, tab etc

	int				m_tabWidth;		// Tab width in pixels.

	WgGfxDevice *	m_pDevice;		// Device used for blitting.

	bool			m_bClip;		// Set if we have a clipping rectangle.
	WgRect			m_clipRect;		// Clipping rectangle used for clipBlit().


};

#endif //WG_PEN_DOT_H
