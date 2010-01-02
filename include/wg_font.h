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

#ifndef	WG_FONTSET_DOT_H
#define	WG_FONTSET_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_FONT_DOT_H
#	include <wg_glyphset.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#include <assert.h>

class WgCursor;
class WgCursorInstance;


//____ WgFont ______________________________________________________________

class WgFont
{
public:
	WgFont();
	WgFont( WgGlyphSet * pNormal, WgCursor * pCursor );
	~WgFont();


	inline WgGlyphSet*	GetGlyphSet( WgFontStyle style ) const
									{ assert( style<WG_NB_FONTSTYLES ); return m_aGlyphSets[style]; }
	inline WgGlyphSet*	GetDefaultGlyphSet( ) const { return m_pDefault; }

	bool				SetGlyphSet( WgFontStyle style, WgGlyphSet * pGlyph );
	bool				SetDefaultGlyphSet( WgGlyphSet * pGlyph );

	bool				SetCursor( WgCursor * pCursor );
	inline WgCursor * 	GetCursor() const { return m_pCursor; }

	inline static void		SetDefaultFont(WgFont * pFont) { g_pDefaultFont = pFont; }
	inline static WgFont *	GetDefaultFont() { return g_pDefaultFont; }

protected:

	WgGlyphSet *		m_aGlyphSets[WG_NB_FONTSTYLES];
	WgCursor *			m_pCursor;
	WgGlyphSet *		m_pDefault;

	static WgFont *	g_pDefaultFont;
};


#endif	// WG_FONT_DOT_H
