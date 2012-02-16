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

#include <wg_font.h>
#include <wg_bitmapglyphs.h>
#include <wg_vectorglyphs.h>


//____ Constructor ____________________________________________________________

WgFont::WgFont()
{

	_init();
}

#ifdef WG_USE_FREETYPE
WgFont::WgFont( WgVectorGlyphs * pNormal )
{
	_init();
	SetVectorGlyphs( pNormal, WG_STYLE_NORMAL );
}
#endif

WgFont::WgFont( WgBitmapGlyphs * pNormal, int size )
{
	_init();
	SetBitmapGlyphs( pNormal, WG_STYLE_NORMAL, size );
}

//____ _init() _________________________________________________________________

void WgFont::_init()
{
#ifdef	WG_USE_FREETYPE
	m_pDefaultVectorGlyphs = 0;

	for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
		m_aVectorGlyphs[i] = 0;
#endif

	for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
	{
		m_aBitmapGlyphs[i] = 0;
		m_aDefaultBitmapGlyphs[i] = 0;
	}
}



//____ Destructor _____________________________________________________________

WgFont::~WgFont()
{
#ifdef	WG_USE_FREETYPE
	for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
	{
		if( m_aVectorGlyphs[i] != 0 )
			delete [] m_aVectorGlyphs[i];
	}
#endif

	for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
	{
		if( m_aBitmapGlyphs[i] != 0 )
			delete [] m_aBitmapGlyphs[i];
	}
}


//____ GetGlyphSet() _______________________________________________________________

WgGlyphSet * WgFont::GetGlyphSet( WgFontStyle style, int size ) const
{
	// Find the right glyphset to the following priorities:
	//
	// 1. BitmapGlyphs of the right style and size.
	// 2. Default BitmapGlyphs of the right size.
	// 3. VectorGlyphs of the right style.
	// 4. Default VectorGlyphs.
	// 5. BitmapGlyphs of the closest smaller size (preferably same style, otherwise default).

	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] != 0 )
		return m_aBitmapGlyphs[size][style];
	else if( m_aDefaultBitmapGlyphs[size] != 0 )
		return m_aDefaultBitmapGlyphs[size];
#ifdef WG_USE_FREETYPE
	else if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] != 0 )
		return m_aVectorGlyphs[style][size];
	else if( m_pDefaultVectorGlyphs )
		return m_pDefaultVectorGlyphs;
#endif

	for( int i = size-1 ; i >= 0 ; i-- )
	{
		if( m_aBitmapGlyphs[i] != 0 && m_aBitmapGlyphs[i][style] != 0 )
			return m_aBitmapGlyphs[i][style];

		if( m_aDefaultBitmapGlyphs[i] != 0 )
			return m_aDefaultBitmapGlyphs[i];
	}

	return 0;			// Could not get any glyphset of same or smaller size, regardless of style.
}

//____ GetUnderline() _________________________________________________________

const WgUnderline * WgFont::GetUnderline( int size )
{
	// Create an underline specification from the '_' character as default.
	// It should be possible to specify something different in the spec file later on...

	WgGlyphPtr pUnder = GetGlyph('_', WG_STYLE_NORMAL, size);

	const WgGlyphBitmap * pSrc = pUnder->GetBitmap();

	m_tempUnderline.pSurf = pSrc->pSurface;
	m_tempUnderline.rect = pSrc->rect;
	m_tempUnderline.bearingX = pSrc->bearingX;
	m_tempUnderline.bearingY = pSrc->bearingY;

	if( pSrc->rect.w > 2 )
	{
		m_tempUnderline.leftBorder = 1;
		m_tempUnderline.rightBorder = 1;
	}
	else
	{
		m_tempUnderline.leftBorder = 0;
		m_tempUnderline.rightBorder = 0;
	}

	return &m_tempUnderline;
}


//____ GetGlyph() _____________________________________________________________

WgGlyphPtr WgFont::GetGlyph( Uint32 chr, WgFontStyle style, int size ) const
{
	WgGlyphPtr p;

	// Special case: For whitespace we give vector glyphs top priority

#ifdef WG_USE_FREETYPE

	if( chr == 32 || chr == 0xA0 )
	{
		// VectorGlyphs whitespace spec of the right style

		if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] != 0 )
		{
			p = m_aVectorGlyphs[style][size]->GetGlyph( chr, size );
			if( p )
				return p;
		}

		// Default VectorGlyphs whitespace spec

		if( m_pDefaultVectorGlyphs )
		{
			p = m_pDefaultVectorGlyphs->GetGlyph( chr, size );
			if( p )
				return p;
		}
	}
#endif

	// Find the right glyph to the following priorities:

	// 1. BitmapGlyphs of the right style and size.


	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] != 0 )
	{
		p = m_aBitmapGlyphs[size][style]->GetGlyph( chr, size );
		if( p )
			return p;
	}

	// 2. Default BitmapGlyphs of the right size.

	if( m_aDefaultBitmapGlyphs[size] != 0 )
	{
		p = m_aDefaultBitmapGlyphs[size]->GetGlyph( chr, size );
		if( p )
			return p;
	}

#ifdef WG_USE_FREETYPE
	// 3. VectorGlyphs of the right style.

	if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] != 0 )
	{
		p = m_aVectorGlyphs[style][size]->GetGlyph( chr, size );
		if( p )
			return p;
	}

	// 4. Default VectorGlyphs.

	if( m_pDefaultVectorGlyphs )
	{
		p = m_pDefaultVectorGlyphs->GetGlyph( chr, size );
		if( p )
			return p;
	}
#endif

	// 5. BitmapGlyphs of the closest smaller size (preferably same style, otherwise default).


	for( int i = size-1 ; i >= 0 ; i-- )
	{
		if( m_aBitmapGlyphs[i] != 0 && m_aBitmapGlyphs[i][style] != 0 )
		{
			p = m_aBitmapGlyphs[i][style]->GetGlyph( chr, i );
			if( p )
				return p;
		}

		if( m_aDefaultBitmapGlyphs[i] != 0 )
		{
			p = m_aDefaultBitmapGlyphs[i]->GetGlyph( chr, size );
			if( p )
				return p;
		}
	}

	// No representation for the glyph found, we failed.

	return 0;
}

//____ IsGlyphProvided() ______________________________________________________

WgFont::GlyphProvided WgFont::IsGlyphProvided( Uint32 chr, WgFontStyle style, int size ) const
{
	// Find the right glyph to the following priorities:

	//TODO: Vector glyphs generate the bitmap on this call, that is totally unnecessary!

	// 1. BitmapGlyphs of the right style and size.

	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] != 0 )
	{
		if( m_aBitmapGlyphs[size][style]->HasGlyph( chr ) )
			return EXACT_MATCH_PROVIDED;
	}

	// 2. Default BitmapGlyphs of the right size.

	if( m_aDefaultBitmapGlyphs[size] != 0 )
	{
		if( m_aDefaultBitmapGlyphs[size]->HasGlyph( chr ) )
			return DEFAULT_PROVIDED;
	}

#ifdef WG_USE_FREETYPE
	// 3. VectorGlyphs of the right style.

	if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] != 0 )
	{
		if( m_aVectorGlyphs[style][size]->HasGlyph( chr ) )
			return EXACT_MATCH_PROVIDED;
	}

	// 4. Default VectorGlyphs.

	if( m_pDefaultVectorGlyphs )
	{
		if( m_pDefaultVectorGlyphs->HasGlyph( chr ) )
			return DEFAULT_PROVIDED;
	}
#endif

	// 5. BitmapGlyphs of the closest smaller size (preferably same style, otherwise default).


	for( int i = size-1 ; i >= 0 ; i-- )
	{
		if( m_aBitmapGlyphs[i] != 0 && m_aBitmapGlyphs[i][style] != 0 )
		{
			if( m_aBitmapGlyphs[i][style]->HasGlyph( chr ) )
				return SMALLER_MATCH_PROVIDED;
		}

		if( m_aDefaultBitmapGlyphs[i] != 0 )
		{
			if( m_aDefaultBitmapGlyphs[size]->HasGlyph( chr ) )
				return SMALLER_DEFAULT_PROVIDED;
		}
	}

	// No representation for the glyph found, we failed.

	return NOT_PROVIDED;
}

#ifdef WG_USE_FREETYPE
//____ SetVectorGlyphs() ______________________________________________________

bool WgFont::SetVectorGlyphs( WgVectorGlyphs * pGlyph, WgFontStyle style )
{
	if( m_aVectorGlyphs[style] == 0 )
		m_aVectorGlyphs[style] = new WgVectorGlyphs*[WG_MAX_FONTSIZE+1];

	for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
		m_aVectorGlyphs[style][i] = pGlyph;

	return true;
}

bool WgFont::SetVectorGlyphs( WgVectorGlyphs * pGlyph, WgFontStyle style, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return false;

	if( m_aVectorGlyphs[style] == 0 )
	{
		m_aVectorGlyphs[style] = new WgVectorGlyphs*[WG_MAX_FONTSIZE+1];

		for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
			m_aVectorGlyphs[style][i] = 0;
	}

	m_aVectorGlyphs[style][size] = pGlyph;

	return true;
}



//____ SetDefaultVectorGlyphs() _______________________________________________

bool WgFont::SetDefaultVectorGlyphs( WgVectorGlyphs * pGlyphs )
{
	m_pDefaultVectorGlyphs = pGlyphs;
	return true;
}

//____ ReplaceVectorGlyphs() __________________________________________________

int WgFont::ReplaceVectorGlyphs( WgVectorGlyphs * pOld, WgVectorGlyphs * pNew )
{
	int nbReplaced = 0;

	if( m_pDefaultVectorGlyphs == pOld )
	{
		m_pDefaultVectorGlyphs = pNew;
		nbReplaced++;
	}

	for( int style = 0 ; style  < WG_NB_FONTSTYLES ; style++ )
	{
		if( m_aVectorGlyphs[style] != 0 )
		{
			for( int size = 0 ; size <= WG_MAX_FONTSIZE ; size++ )
				if( m_aVectorGlyphs[style][size] == pOld )
				{
					m_aVectorGlyphs[style][size] = pNew;
					nbReplaced++;
				}
		}
	}

	return nbReplaced;
}

#endif

//____ ReplaceBitmapGlyphs() __________________________________________________

int WgFont::ReplaceBitmapGlyphs( WgBitmapGlyphs * pOld, WgBitmapGlyphs * pNew )
{
	if( pOld == 0 )
		return 0;

	int nbReplaced = 0;

	for( int size = 0 ; size <= WG_MAX_FONTSIZE ; size++ )
	{
		if( m_aDefaultBitmapGlyphs[size] == pOld )
		{
			m_aDefaultBitmapGlyphs[size] = pNew;
			nbReplaced++;
		}

		if( m_aBitmapGlyphs[size] != 0 )
		{
			for( int style = 0 ; style  < WG_NB_FONTSTYLES ; style++ )
			{
				if( m_aBitmapGlyphs[size][style] == pOld )
				{
					m_aBitmapGlyphs[size][style] = pNew;
					nbReplaced++;
				}

			}
		}
	}

	return nbReplaced;
}


//____ SetBitmapGlyphs() ______________________________________________________

bool WgFont::SetBitmapGlyphs( WgBitmapGlyphs * pGlyph, WgFontStyle style, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return false;

	if( m_aBitmapGlyphs[size] == 0 )
	{
		WgBitmapGlyphs** p = new WgBitmapGlyphs*[WG_NB_FONTSTYLES];
		for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
			p[i] = 0;

		m_aBitmapGlyphs[size] = p;
	}

	m_aBitmapGlyphs[size][style] = pGlyph;
	return true;
}


//____ SetDefaultBitmapGlyphs() _______________________________________________

bool WgFont::SetDefaultBitmapGlyphs( WgBitmapGlyphs * pGlyphs, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return false;

	m_aDefaultBitmapGlyphs[size] = pGlyphs;
	return true;
}

//____ GetBitmapGlyphs() ______________________________________________________

WgBitmapGlyphs * WgFont::GetBitmapGlyphs( WgFontStyle style, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return false;

	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] != 0 )
		return m_aBitmapGlyphs[size][style];

	return 0;
}

