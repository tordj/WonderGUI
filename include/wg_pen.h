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
	WgPen( WgGfxDevice * pDevice, const WgCord& origo, const WgRect& clip = WgRect() );
//	WgPen( const WgTextPropPtr& pTextProp, const WgTextPropPtr& pCharProp = 0, WgMode mode = WG_MODE_NORMAL ) { SetTextProp( pTextProp, pCharProp, mode ); } 
//	WgPen( Uint16 hTextProp, Uint16 hCharProp = 0, WgMode mode = WG_MODE_NORMAL ) { SetTextProp( hTextProp, hCharProp, mode ); } 
	~WgPen() {}

	void					SetClipRect( const WgRect& clip );
	inline void				SetDevice( WgGfxDevice * pDevice ) { m_pDevice = pDevice; }

	void					SetOrigo( const WgCord& pos ) { m_origo = pos; }

	inline bool				SetTextProp( const WgTextPropPtr& pTextProp, const WgTextPropPtr& pCharProp = 0, WgMode mode = WG_MODE_NORMAL ) { return SetTextProp( pTextProp.m_hProp, pCharProp.m_hProp, mode ); }
	bool					SetTextProp( Uint16 hTextProp, Uint16 hCharProp = 0, WgMode mode = WG_MODE_NORMAL );


	inline void				SetPos( const WgCord& pos ) { m_pos = pos; }
	inline void				SetPosX( int x ) { m_pos.x = x; }
	inline void				SetPosY( int y ) { m_pos.y = y; }

	inline void				Move( const WgCord& pos ) { m_pos += pos; }
	inline void				MoveX( int x ) { m_pos.x += x; }
	inline void				MoveY( int y ) { m_pos.y += y; }


	void					SetTab( int width ) { m_tabWidth = width; }
	bool					SetChar( Uint32 chr );
	void					FlushChar() { m_pGlyph = &m_dummyGlyph; m_dummyGlyph.advance = 0; }
	void					ApplyKerning() { if( m_pPrevGlyph != &m_dummyGlyph && m_pGlyph != &m_dummyGlyph ) m_pos.x += m_pGlyphs->GetKerning( m_pPrevGlyph, m_pGlyph, m_size ); }

	inline void				AdvancePos() { m_pos.x += m_pGlyph->advance; }							///< Advances position past current character.
	inline void				AdvancePosMonospaced() { m_pos.x += m_pGlyphs->GetMaxGlyphAdvance(m_size); }	///< Advances position past current character using monospace spacing.
	void					AdvancePosCursor( const WgCursorInstance& instance );

	inline const WgGlyph *	GetGlyph() const { return m_pGlyph; }
	inline WgCord			GetPos() const { return m_pos; }
	inline int				GetPosX() const { return m_pos.x; }
	inline int				GetPosY() const { return m_pos.y; }

	inline WgCord			GetBlitPos() const { return WgCord( m_pos.x + m_pGlyph->bearingX, m_pos.y + m_pGlyph->bearingY ); }
	inline int				GetBlitPosX() const { return m_pos.x + m_pGlyph->bearingX; }
	inline int				GetBlitPosY() const { return m_pos.y + m_pGlyph->bearingY; }
	


	inline WgFont *			GetFont() const { return m_pFont; }
	inline int				GetSize() const { return m_size; }
	inline int				GetStyle() const { return m_style; }
	inline WgColor			GetColor() const { return m_color; }
	inline const WgGlyphSet *GetGlyphSet() const { return m_pGlyphs; }


	inline int				GetLineSpacing() const { return m_pGlyphs->GetLineSpacing(m_size); }
	inline int				GetLineHeight() const { return m_pGlyphs->GetHeight(m_size); }
	inline int				GetBaseline() const { return m_pGlyphs->GetBaseline(m_size); }

	inline void				BlitChar() const { if( m_bClip ) m_pDevice->ClipBlit( m_clipRect, m_pGlyph->pSurf, m_pGlyph->rect, GetBlitPosX(), GetBlitPosY() ); else m_pDevice->Blit( m_pGlyph->pSurf, m_pGlyph->rect, GetBlitPosX(), GetBlitPosY() ); }
	
	bool					BlitCursor( const WgCursorInstance& instance ) const;

private:
	void Init();

	WgGlyphSet *m_pGlyphs;			// Pointer at our glyphs.

	const WgGlyph *	m_pPrevGlyph;	// Previous glyph, saved to allow for kerning.
	const WgGlyph *	m_pGlyph;		// Current glyph.

	WgFont *		m_pFont;		// Pointer back to the font.
	int				m_size;			// Fontsize we got a glyphset for, which might be smaller than what we requested.
	WgFontStyle		m_style;		// Style of glyphset we requested.
	WgColor			m_color;		// Color this pen draws in.

	bool			m_bShowSpace;	// Set if space control character should be shown (usually a dot in the middle of the cell).
	bool			m_bShowCRLF;	// Set if the CR/LF control character should be shown.

	WgCord			m_origo;		// Origo position, from where we start printing and count tab-positions.
	WgCord			m_pos;			// Position of this pen in screen pixels.

	WgGlyph			m_dummyGlyph;	// Dummy glyph used for whitespace, tab etc

	int				m_tabWidth;		// Tab width in pixels.

	WgGfxDevice *	m_pDevice;		// Device used for blitting.

	bool			m_bClip;		// Set if we have a clipping rectangle.
	WgRect			m_clipRect;		// Clipping rectangle used for ClipBlit().
};

#endif //WG_PEN_DOT_H