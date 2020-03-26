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

#include <wg3_textstyle.h>

#include <wg3_font.h>

#ifndef	WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

class WgCursor;

//____ WgPen _____________________________________________________________

class WgPen
{
friend class WgFont;

public:
	WgPen();
	WgPen( wg::GfxDevice * pDevice, const WgCoord& origo );
//	WgPen( const WgTextpropPtr& pTextprop, const WgTextpropPtr& pCharProp = 0, WgMode mode = WG_MODE_NORMAL ) { SetTextprop( pTextprop, pCharProp, mode ); }
//	WgPen( Uint16 hTextprop, Uint16 hCharProp = 0, WgMode mode = WG_MODE_NORMAL ) { SetTextprop( hTextprop, hCharProp, mode ); }
	~WgPen() {}

	inline void				SetDevice( wg::GfxDevice * pDevice ) { m_pDevice = pDevice; }
	void					SetScale( int scale );

	void					SetOrigo( const WgCoord& pos ) { m_origo = pos; }

	bool					SetAttributes( const wg::TextAttr& attr );
	bool					SetSize( int size );
	void					SetFont( wg::Font * pFont );
	void					SetColor( WgColor color );


	inline void				SetPos( const WgCoord& pos ) { m_pos = pos; }
	inline void				SetPosX( int x ) { m_pos.x = x; }
	inline void				SetPosY( int y ) { m_pos.y = y; }

	inline void				Move( const WgCoord& pos ) { m_pos += pos; }
	inline void				MoveX( int x ) { m_pos.x += x; }
	inline void				MoveY( int y ) { m_pos.y += y; }


	void					SetTab( int width ) { m_tabWidth = width; }
	bool					SetChar( Uint32 chr );
	void					FlushChar() { m_pGlyph = &m_dummyGlyph; m_dummyGlyph.SetAdvance(0); }
	void					ApplyKerning() { if( m_pPrevGlyph != &m_dummyGlyph && m_pGlyph != &m_dummyGlyph ) m_pos.x += m_pFont->kerning( m_pPrevGlyph, m_pGlyph); }

	inline void				AdvancePos() { m_pos.x += m_pGlyph->advance(); }							///< Advances position past current character.
	inline void				AdvancePosMonospaced() { m_pos.x += m_pFont->maxAdvance(); }	///< Advances position past current character using monospace spacing.
	void					AdvancePosCursor( const WgCursorInstance& instance );

	inline wg::Glyph_p		GetGlyph() const { return m_pGlyph; }
	inline WgCoord			GetPos() const { return m_pos; }
	inline int				GetPosX() const { return m_pos.x; }
	inline int				GetPosY() const { return m_pos.y; }

//	inline WgCoord			GetBlitPos() const { return WgCoord( m_pos.x + m_pGlyph->BearingX(), m_pos.y + m_pGlyph->BearingY() ); }
//	inline int				GetBlitPosX() const { return m_pos.x + m_pGlyph->BearingX(); }
//	inline int				GetBlitPosY() const { return m_pos.y + m_pGlyph->BearingY(); }

	inline wg::Font *		GetFont() const { return m_pFont; }
	inline int				GetSize() const { return m_size; }
	inline WgColor			GetColor() const { return m_color; }

	inline int				GetLineSpacing() const { return m_pFont->lineGap(); }
	inline int				GetLineHeight() const { return m_pFont->maxAscend() + m_pFont->maxDescend(); }
	inline int				GetBaseline() const { return m_pFont->maxAscend(); }

	void					BlitChar() const;
	bool					BlitCursor( const WgCursorInstance& instance ) const;

private:
	void _init();

	void _onAttrChanged();

	WgCursor * _getCursor(const WgText * pText) const;

	class DummyGlyph : public wg::Glyph
	{
	public:
		DummyGlyph() : wg::Glyph( 0, 0, 0 ) {}
		const wg::GlyphBitmap * getBitmap() { return nullptr; }

		void SetAdvance( int advance ) { m_advance = advance; }
	};

	//

	//

	wg::Font_p      m_pFont;		// Pointer at our glyphs.

	wg::Glyph_p		m_pPrevGlyph;	// Previous glyph, saved to allow for kerning.
	wg::Glyph_p		m_pGlyph;		// Current glyph.

	int				m_wantedSize;	// Size we requested.
	int				m_size;			// Fontsize we got a glyphset for, which might be smaller than what we requested.
	WgColor			m_color;		// Color this pen draws in.

	bool			m_bShowSpace;	// Set if space control character should be shown (usually a dot in the middle of the cell).
	bool			m_bShowCRLF;	// Set if the CR/LF control character should be shown.

	WgCoord			m_origo;		// Origo position, from where we start printing and count tab-positions.
	WgCoord			m_pos;			// Position of this pen in screen pixels.

	DummyGlyph		m_dummyGlyph;	// Dummy glyph used for whitespace, tab etc

	int				m_tabWidth;		// Tab width in pixels.

	wg::GfxDevice_p	m_pDevice;		// Device used for blitting.

	int				m_scale;		// Widget scale, for text sizes.

};

#endif //WG_PEN_DOT_H
