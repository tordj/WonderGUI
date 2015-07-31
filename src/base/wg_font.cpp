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

namespace wg 
{
	
	const char Font::CLASSNAME[] = {"Font"};
	
	//____ Constructor ____________________________________________________________
	
	Font::Font()
	{
	
		_init();
	}
	
	#ifdef WG_USE_FREETYPE
	Font::Font( const VectorGlyphs_p& pNormal )
	{
		_init();
		setVectorGlyphs( pNormal, WG_FONT_NORMAL );
	}
	#endif
	
	Font::Font( const BitmapGlyphs_p& pNormal, int size )
	{
		_init();
		setBitmapGlyphs( pNormal, WG_FONT_NORMAL, size );
	}
	
	//____ _init() _________________________________________________________________
	
	void Font::_init()
	{
	#ifdef	WG_USE_FREETYPE
		for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
			m_aVectorGlyphs[i] = 0;
	#endif
	
		for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
			m_aBitmapGlyphs[i] = 0;
	}
	
	
	
	//____ Destructor _____________________________________________________________
	
	Font::~Font()
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
	
	bool Font::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Font::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Font_p Font::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Font_p( static_cast<Font*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ getGlyphset() _______________________________________________________________
	
	Glyphset_p Font::getGlyphset( FontAlt style, int size ) const
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
	
	const WgUnderline * Font::getUnderline( int size )
	{
		// Create an underline specification from the '_' character as default.
		// It should be possible to specify something different in the spec file later on...
	
		Glyph_p pUnder = getGlyph('_', WG_FONT_NORMAL, size);
	
		const GlyphBitmap * pSrc = pUnder->getBitmap();
	
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
	
	Glyph_p Font::getGlyph( Uint32 chr, FontAlt style, int size ) const
	{
		Glyph_p p;
	
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
	
	Font::GlyphProvided Font::isGlyphProvided( Uint32 chr, FontAlt style, int size ) const
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
	
	bool Font::setVectorGlyphs( const VectorGlyphs_p& pGlyph, FontAlt style )
	{
		if( m_aVectorGlyphs[style] == 0 )
			m_aVectorGlyphs[style] = new VectorGlyphs_p[WG_MAX_FONTSIZE+1];
	
		for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
			m_aVectorGlyphs[style][i] = pGlyph;
	
		return true;
	}
	
	bool Font::setVectorGlyphs( const VectorGlyphs_p& pGlyph, FontAlt style, int size )
	{
		if( size < 0 || size > WG_MAX_FONTSIZE )
			return false;
	
		if( m_aVectorGlyphs[style] == 0 )
		{
			m_aVectorGlyphs[style] = new VectorGlyphs_p[WG_MAX_FONTSIZE+1];
	
			for( int i = 0 ; i <= WG_MAX_FONTSIZE ; i++ )
				m_aVectorGlyphs[style][i] = 0;
		}
	
		m_aVectorGlyphs[style][size] = pGlyph;
	
		return true;
	}
	
	
	
	//____ setDefaultVectorGlyphs() _______________________________________________
	
	bool Font::setDefaultVectorGlyphs( const VectorGlyphs_p& pGlyphs )
	{
		m_pDefaultVectorGlyphs = pGlyphs;
		return true;
	}
	
	//____ replaceVectorGlyphs() __________________________________________________
	
	int Font::replaceVectorGlyphs( const VectorGlyphs_p& pOld, const VectorGlyphs_p& pNew )
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
	
	int Font::replaceBitmapGlyphs( const BitmapGlyphs_p& pOld, const BitmapGlyphs_p& pNew )
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
	
	bool Font::setBitmapGlyphs( const BitmapGlyphs_p& pGlyph, FontAlt style, int size )
	{
		if( size < 0 || size > WG_MAX_FONTSIZE )
			return false;
	
		if( m_aBitmapGlyphs[size] == 0 )
		{
			BitmapGlyphs_p* p = new BitmapGlyphs_p[WG_NB_FONTSTYLES];
			for( int i = 0 ; i < WG_NB_FONTSTYLES ; i++ )
				p[i] = 0;
	
			m_aBitmapGlyphs[size] = p;
		}
	
		m_aBitmapGlyphs[size][style] = pGlyph;
		return true;
	}
	
	
	//____ setDefaultBitmapGlyphs() _______________________________________________
	
	bool Font::setDefaultBitmapGlyphs( const BitmapGlyphs_p& pGlyphs, int size )
	{
		if( size < 0 || size > WG_MAX_FONTSIZE )
			return false;
	
		m_aDefaultBitmapGlyphs[size] = pGlyphs;
		return true;
	}
	
	//____ getBitmapGlyphs() ______________________________________________________
	
	BitmapGlyphs_p Font::getBitmapGlyphs( FontAlt style, int size )
	{
		if( size < 0 || size > WG_MAX_FONTSIZE )
			return 0;
	
		if( m_aBitmapGlyphs[size] != 0 && m_aBitmapGlyphs[size][style] )
			return m_aBitmapGlyphs[size][style];
	
		return 0;
	}
	

} // namespace wg
