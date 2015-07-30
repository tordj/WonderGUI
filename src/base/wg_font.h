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

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_GLYPHSET_DOT_H
#	include <wg_glyphset.h>
#endif



#include <assert.h>

#ifdef	WG_USE_FREETYPE
#	ifndef WG_VECTORGLYPHS_DOT_H
#		include <wg_vectorglyphs.h>
#	endif
#endif

#ifndef WG_BITMAPGLYPHS_DOT_H
#	include <wg_bitmapglyphs.h>
#endif


class WgFont;
typedef	WgStrongPtr<WgFont,WgObject_p>		WgFont_p;
typedef	WgWeakPtr<WgFont,WgObject_wp>	WgFont_wp;

//____ WgFont ______________________________________________________________

class WgFont : public WgObject
{
public:
	static WgFont_p	create() { return WgFont_p(new WgFont()); }
#ifdef WG_USE_FREETYPE
	static WgFont_p	create( const WgVectorGlyphs_p& pNormal ) { return WgFont_p(new WgFont(pNormal)); }
#endif
	static WgFont_p	create( const WgBitmapGlyphs_p& pNormal, int size ) { return WgFont_p(new WgFont(pNormal,size)); }

	bool				isInstanceOf( const char * pClassName ) const;
	const char *		className( void ) const;
	static const char	CLASSNAME[];
	static WgFont_p	cast( const WgObject_p& pObject );

	enum GlyphProvided
	{
		NOT_PROVIDED			= 0,
		EXACT_MATCH_PROVIDED	= 1,
		DEFAULT_PROVIDED		= 2,
		SMALLER_MATCH_PROVIDED	= 3,
		SMALLER_DEFAULT_PROVIDED= 4
	};



	WgGlyphset_p		getGlyphset( WgFontAlt style, int size ) const;
	WgGlyph_p			getGlyph( Uint32 chr, WgFontAlt style, int size ) const;
	GlyphProvided		isGlyphProvided( Uint32 chr, WgFontAlt style, int size ) const;

#ifdef	WG_USE_FREETYPE
	bool				setVectorGlyphs( const WgVectorGlyphs_p& pGlyphs, WgFontAlt style  );
	bool				setVectorGlyphs( const WgVectorGlyphs_p& pGlyphs, WgFontAlt style, int size );
	bool				setDefaultVectorGlyphs( const WgVectorGlyphs_p& pGlyphs );

	inline WgVectorGlyphs_p	getVectorGlyphs( WgFontAlt style, int size  ) const { if( size <= WG_MAX_FONTSIZE && m_aVectorGlyphs[style] != 0 ) return m_aVectorGlyphs[style][size]; else return 0; }
	inline WgVectorGlyphs_p	getDefaultVectorGlyphs( ) const { return m_pDefaultVectorGlyphs; }

	int					replaceVectorGlyphs( const WgVectorGlyphs_p& pOld, const WgVectorGlyphs_p& pNew );
#endif

	int					replaceBitmapGlyphs( const WgBitmapGlyphs_p& pOld, const WgBitmapGlyphs_p& pNew );


	bool				setBitmapGlyphs( const WgBitmapGlyphs_p& pGlyph, WgFontAlt style, int size );
	bool				setDefaultBitmapGlyphs( const WgBitmapGlyphs_p& pGlyphs, int size = 0 );

	WgBitmapGlyphs_p			getBitmapGlyphs( WgFontAlt style, int size );
	inline WgBitmapGlyphs_p	getDefaultBitmapGlyphs( int size = 0 ) const { if( size <= WG_MAX_FONTSIZE ) return m_aDefaultBitmapGlyphs[size]; else return 0; }

	const WgUnderline *	getUnderline( int size );


protected:
	WgFont();
#ifdef WG_USE_FREETYPE
	WgFont( const WgVectorGlyphs_p& pNormal );
#endif
	WgFont( const WgBitmapGlyphs_p& pNormal, int size );
	~WgFont();

	void	_init();

#ifdef	WG_USE_FREETYPE
	WgVectorGlyphs_p	m_pDefaultVectorGlyphs;
	WgVectorGlyphs_p*	m_aVectorGlyphs[WG_NB_FONTSTYLES];			// Pointer at an array of WG_MAX_FONTSIZE+1 WgVectorGlyhphs.
#endif

	WgBitmapGlyphs_p *	m_aBitmapGlyphs[WG_MAX_FONTSIZE+1];			// Pointer at an array of WG_NB_FONTSTYLES WgBitmapGlyphs.
	WgBitmapGlyphs_p	m_aDefaultBitmapGlyphs[WG_MAX_FONTSIZE+1];

	WgUnderline			m_tempUnderline;							// Holds the last requested underline.

};




#endif	// WG_FONT_DOT_H
