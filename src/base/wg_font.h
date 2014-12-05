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
typedef	WgStrongPtr<WgFont,WgObjectPtr>		WgFontPtr;
typedef	WgWeakPtr<WgFont,WgObjectWeakPtr>	WgFontWeakPtr;

//____ WgFont ______________________________________________________________

class WgFont : public WgObject
{
public:
	static WgFontPtr	Create() { return WgFontPtr(new WgFont()); }
#ifdef WG_USE_FREETYPE
	static WgFontPtr	Create( const WgVectorGlyphsPtr& pNormal ) { return WgFontPtr(new WgFont(pNormal)); }
#endif
	static WgFontPtr	Create( const WgBitmapGlyphsPtr& pNormal, int size ) { return WgFontPtr(new WgFont(pNormal,size)); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFontPtr	Cast( const WgObjectPtr& pObject );

	enum GlyphProvided
	{
		NOT_PROVIDED			= 0,
		EXACT_MATCH_PROVIDED	= 1,
		DEFAULT_PROVIDED		= 2,
		SMALLER_MATCH_PROVIDED	= 3,
		SMALLER_DEFAULT_PROVIDED= 4
	};



	WgGlyphsetPtr		GetGlyphset( WgFontStyle style, int size ) const;
	WgGlyphPtr			GetGlyph( Uint32 chr, WgFontStyle style, int size ) const;
	GlyphProvided		IsGlyphProvided( Uint32 chr, WgFontStyle style, int size ) const;

#ifdef	WG_USE_FREETYPE
	bool				SetVectorGlyphs( const WgVectorGlyphsPtr& pGlyphs, WgFontStyle style  );
	bool				SetVectorGlyphs( const WgVectorGlyphsPtr& pGlyphs, WgFontStyle style, int size );
	bool				SetDefaultVectorGlyphs( const WgVectorGlyphsPtr& pGlyphs );

	inline WgVectorGlyphsPtr	GetVectorGlyphs( WgFontStyle style, int size  ) const { if( size <= WG_MAX_FONTSIZE && m_aVectorGlyphs[style] != 0 ) return m_aVectorGlyphs[style][size]; else return 0; }
	inline WgVectorGlyphsPtr	GetDefaultVectorGlyphs( ) const { return m_pDefaultVectorGlyphs; }

	int					ReplaceVectorGlyphs( const WgVectorGlyphsPtr& pOld, const WgVectorGlyphsPtr& pNew );
#endif

	int					ReplaceBitmapGlyphs( const WgBitmapGlyphsPtr& pOld, const WgBitmapGlyphsPtr& pNew );


	bool				SetBitmapGlyphs( const WgBitmapGlyphsPtr& pGlyph, WgFontStyle style, int size );
	bool				SetDefaultBitmapGlyphs( const WgBitmapGlyphsPtr& pGlyphs, int size = 0 );

	WgBitmapGlyphsPtr			GetBitmapGlyphs( WgFontStyle style, int size );
	inline WgBitmapGlyphsPtr	GetDefaultBitmapGlyphs( int size = 0 ) const { if( size <= WG_MAX_FONTSIZE ) return m_aDefaultBitmapGlyphs[size]; else return 0; }

	const WgUnderline *	GetUnderline( int size );


protected:
	WgFont();
#ifdef WG_USE_FREETYPE
	WgFont( const WgVectorGlyphsPtr& pNormal );
#endif
	WgFont( const WgBitmapGlyphsPtr& pNormal, int size );
	~WgFont();

	void	_init();

#ifdef	WG_USE_FREETYPE
	WgVectorGlyphsPtr	m_pDefaultVectorGlyphs;
	WgVectorGlyphsPtr*	m_aVectorGlyphs[WG_NB_FONTSTYLES];			// Pointer at an array of WG_MAX_FONTSIZE+1 WgVectorGlyhphs.
#endif

	WgBitmapGlyphsPtr *	m_aBitmapGlyphs[WG_MAX_FONTSIZE+1];			// Pointer at an array of WG_NB_FONTSTYLES WgBitmapGlyphs.
	WgBitmapGlyphsPtr	m_aDefaultBitmapGlyphs[WG_MAX_FONTSIZE+1];

	WgUnderline			m_tempUnderline;							// Holds the last requested underline.

};




#endif	// WG_FONT_DOT_H
