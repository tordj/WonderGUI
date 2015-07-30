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

const char WgFont::CLASSNAME[] = {"Font"};

//____ Constructor ____________________________________________________________

WgFont::WgFont()
{

	_init();
}

#ifdef WG_USE_FREETYPE
WgFont::WgFont( const WgVectorGlyphsPtr& pNormal )
{
	_init();
	setVectorGlyphs( pNormal, WG_FONT_NORMAL );
}
#endif

WgFont::WgFont( const WgBitmapGlyphsPtr& pNormal, int size )
{
	_init();
	setBitmapGlyphs( pNormal, WG_FONT_NORMAL, size );
}

//____ _init() _________________________________________________________________

void WgFont::_init()
{
#ifdef	WG_USE_FREETYPE
	for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
		m_aVectorGlyphs[i] = 0;
#endif

	for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
		m_aBitmapGlyphs[i] = 0;
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

//____ isInstanceOf() _________________________________________________________

bool WgFont::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgFont::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgFontPtr WgFont::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgFontPtr( static_cast<WgFont*>(pObject.rawPtr()) );

	return 0;
}


//____ getGlyphset() _______________________________________________________________

WgGlyphsetPtr WgFont::getGlyphset( WgFontAlt style, int size ) const
{
	// Find the right glyphset to the following priorities:
	//
	// 1. BitmapGlyphs of the right style and size.
	// 2. Default BitmapGlyphs of the right size.
	// 3. VectorGlyphs of the right style.
	// 4. Default VectorGlyphs.
	// 5. BitmapGlyphs of the closest smaller size (preferably same style, otherwise default).

	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] )
		return m_aBitmapGlyphs[size][style];
	else if( m_aDefaultBitmapGlyphs[size] )
		return m_aDefaultBitmapGlyphs[size];
#ifdef WG_USE_FREETYPE
	else if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] )
		return m_aVectorGlyphs[style][size];
	else if( m_pDefaultVectorGlyphs )
		return m_pDefaultVectorGlyphs;
#endif

	for( int i = size-1 ; i >= 0 ; i-- )
	{
		if( m_aBitmapGlyphs[i] != 0 && m_aBitmapGlyphs[i][style] )
			return m_aBitmapGlyphs[i][style];

		if( m_aDefaultBitmapGlyphs[i] )
			return m_aDefaultBitmapGlyphs[i];
	}

	return 0;			// Could not get any glyphset of same or smaller size, regardless of style.
}

//____ getUnderline() _________________________________________________________

const WgUnderline * WgFont::getUnderline( int size )
{
	// Create an underline specification from the '_' character as default.
	// It should be possible to specify something different in the spec file later on...

	WgGlyphPtr pUnder = getGlyph('_', WG_FONT_NORMAL, size);

	const WgGlyphBitmap * pSrc = pUnder->getBitmap();

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


//____ getGlyph() _____________________________________________________________

WgGlyphPtr WgFont::getGlyph( Uint32 chr, WgFontAlt style, int size ) const
{
	WgGlyphPtr p;

	// Special case: For whitespace we give vector glyphs top priority

#ifdef WG_USE_FREETYPE

	if( chr == 32 || chr == 0xA0 )
	{
		// VectorGlyphs whitespace spec of the right style

		if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] )
		{
			p = m_aVectorGlyphs[style][size]->getGlyph( chr, size );
			if( p )
				return p;
		}

		// Default VectorGlyphs whitespace spec

		if( m_pDefaultVectorGlyphs )
		{
			p = m_pDefaultVectorGlyphs->getGlyph( chr, size );
			if( p )
				return p;
		}
	}
#endif

	// Find the right glyph to the following priorities:

	// 1. BitmapGlyphs of the right style and size.


	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] )
	{
		p = m_aBitmapGlyphs[size][style]->getGlyph( chr, size );
		if( p )
			return p;
	}

	// 2. Default BitmapGlyphs of the right size.

	if( m_aDefaultBitmapGlyphs[size] )
	{
		p = m_aDefaultBitmapGlyphs[size]->getGlyph( chr, size );
		if( p )
			return p;
	}

#ifdef WG_USE_FREETYPE
	// 3. VectorGlyphs of the right style.

	if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] )
	{
		p = m_aVectorGlyphs[style][size]->getGlyph( chr, size );
		if( p )
			return p;
	}

	// 4. Default VectorGlyphs.

	if( m_pDefaultVectorGlyphs )
	{
		p = m_pDefaultVectorGlyphs->getGlyph( chr, size );
		if( p )
			return p;
	}
#endif

	// 5. BitmapGlyphs of the closest smaller size (preferably same style, otherwise default).


	for( int i = size-1 ; i >= 0 ; i-- )
	{
		if( m_aBitmapGlyphs[i] != 0 && m_aBitmapGlyphs[i][style] )
		{
			p = m_aBitmapGlyphs[i][style]->getGlyph( chr, i );
			if( p )
				return p;
		}

		if( m_aDefaultBitmapGlyphs[i] )
		{
			p = m_aDefaultBitmapGlyphs[i]->getGlyph( chr, size );
			if( p )
				return p;
		}
	}

	// No representation for the glyph found, we failed.

	return 0;
}

//____ isGlyphProvided() ______________________________________________________

WgFont::GlyphProvided WgFont::isGlyphProvided( Uint32 chr, WgFontAlt style, int size ) const
{
	// Find the right glyph to the following priorities:

	//TODO: Vector glyphs generate the bitmap on this call, that is totally unnecessary!

	// 1. BitmapGlyphs of the right style and size.

	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] )
	{
		if( m_aBitmapGlyphs[size][style]->hasGlyph( chr ) )
			return EXACT_MATCH_PROVIDED;
	}

	// 2. Default BitmapGlyphs of the right size.

	if( m_aDefaultBitmapGlyphs[size] )
	{
		if( m_aDefaultBitmapGlyphs[size]->hasGlyph( chr ) )
			return DEFAULT_PROVIDED;
	}

#ifdef WG_USE_FREETYPE
	// 3. VectorGlyphs of the right style.

	if( m_aVectorGlyphs[style] != 0 && m_aVectorGlyphs[style][size] )
	{
		if( m_aVectorGlyphs[style][size]->hasGlyph( chr ) )
			return EXACT_MATCH_PROVIDED;
	}

	// 4. Default VectorGlyphs.

	if( m_pDefaultVectorGlyphs )
	{
		if( m_pDefaultVectorGlyphs->hasGlyph( chr ) )
			return DEFAULT_PROVIDED;
	}
#endif

	// 5. BitmapGlyphs of the closest smaller size (preferably same style, otherwise default).


	for( int i = size-1 ; i >= 0 ; i-- )
	{
		if( m_aBitmapGlyphs[i] != 0 && m_aBitmapGlyphs[i][style] )
		{
			if( m_aBitmapGlyphs[i][style]->hasGlyph( chr ) )
				return SMALLER_MATCH_PROVIDED;
		}

		if( m_aDefaultBitmapGlyphs[i] )
		{
			if( m_aDefaultBitmapGlyphs[size]->hasGlyph( chr ) )
				return SMALLER_DEFAULT_PROVIDED;
		}
	}

	// No representation for the glyph found, we failed.

	return NOT_PROVIDED;
}

#ifdef WG_USE_FREETYPE
//____ setVectorGlyphs() ______________________________________________________

bool WgFont::setVectorGlyphs( const WgVectorGlyphsPtr& pGlyph, WgFontAlt style )
{
	if( m_aVectorGlyphs[style] == 0 )
		m_aVectorGlyphs[style] = new WgVectorGlyphsPtr[WG_MAX_FONTSIZE+1];

	for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
		m_aVectorGlyphs[style][i] = pGlyph;

	return true;
}

bool WgFont::setVectorGlyphs( const WgVectorGlyphsPtr& pGlyph, WgFontAlt style, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return false;

	if( m_aVectorGlyphs[style] == 0 )
	{
		m_aVectorGlyphs[style] = new WgVectorGlyphsPtr[WG_MAX_FONTSIZE+1];

		for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
			m_aVectorGlyphs[style][i] = 0;
	}

	m_aVectorGlyphs[style][size] = pGlyph;

	return true;
}



//____ setDefaultVectorGlyphs() _______________________________________________

bool WgFont::setDefaultVectorGlyphs( const WgVectorGlyphsPtr& pGlyphs )
{
	m_pDefaultVectorGlyphs = pGlyphs;
	return true;
}

//____ replaceVectorGlyphs() __________________________________________________

int WgFont::replaceVectorGlyphs( const WgVectorGlyphsPtr& pOld, const WgVectorGlyphsPtr& pNew )
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

//____ replaceBitmapGlyphs() __________________________________________________

int WgFont::replaceBitmapGlyphs( const WgBitmapGlyphsPtr& pOld, const WgBitmapGlyphsPtr& pNew )
{
	if( !pOld )
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


//____ setBitmapGlyphs() ______________________________________________________

bool WgFont::setBitmapGlyphs( const WgBitmapGlyphsPtr& pGlyph, WgFontAlt style, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return false;

	if( m_aBitmapGlyphs[size] == 0 )
	{
		WgBitmapGlyphsPtr* p = new WgBitmapGlyphsPtr[WG_NB_FONTSTYLES];
		for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
			p[i] = 0;

		m_aBitmapGlyphs[size] = p;
	}

	m_aBitmapGlyphs[size][style] = pGlyph;
	return true;
}


//____ setDefaultBitmapGlyphs() _______________________________________________

bool WgFont::setDefaultBitmapGlyphs( const WgBitmapGlyphsPtr& pGlyphs, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return false;

	m_aDefaultBitmapGlyphs[size] = pGlyphs;
	return true;
}

//____ getBitmapGlyphs() ______________________________________________________

WgBitmapGlyphsPtr WgFont::getBitmapGlyphs( WgFontAlt style, int size )
{
	if( size < 0 || size > WG_MAX_FONTSIZE )
		return 0;

	if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] )
		return m_aBitmapGlyphs[size][style];

	return 0;
}

