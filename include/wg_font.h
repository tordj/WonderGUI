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

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef WG_GLYPHSET_DOT_H
#	include <wg_glyphset.h>
#endif



#include <assert.h>

class WgCursor;

#ifdef	WG_USE_FREETYPE
	class WgVectorGlyphs;
#endif

class WgBitmapGlyphs;


//____ WgFont ______________________________________________________________

class WgFont
{
public:
	WgFont();
#ifdef WG_USE_FREETYPE
	WgFont( WgVectorGlyphs * pNormal, WgCursor * pCursor );
#endif
	WgFont( WgBitmapGlyphs * pNormal, int size, WgCursor * pCursor );
	~WgFont();

	enum GlyphProvided
	{
		NOT_PROVIDED			= 0,
		EXACT_MATCH_PROVIDED	= 1,
		DEFAULT_PROVIDED		= 2,
		SMALLER_MATCH_PROVIDED	= 3,
		SMALLER_DEFAULT_PROVIDED= 4
	};



	WgGlyphSet *		GetGlyphSet( WgFontStyle style, int size ) const;
	const WgGlyph *		GetGlyph( Uint32 chr, WgFontStyle style, int size ) const;					
	GlyphProvided		IsGlyphProvided( Uint32 chr, WgFontStyle style, int size ) const;					

#ifdef	WG_USE_FREETYPE
	bool				SetVectorGlyphs( WgVectorGlyphs * pGlyph, WgFontStyle style  );
	bool				SetDefaultVectorGlyphs( WgVectorGlyphs * pGlyphs );

	inline WgVectorGlyphs *	GetVectorGlyphs( WgFontStyle style  ) { return m_aVectorGlyphs[style]; }
	inline WgVectorGlyphs *	GetDefaultVectorGlyphs( ) const { return m_pDefaultVectorGlyphs; }

	int					ReplaceVectorGlyphs( WgVectorGlyphs * pOld, WgVectorGlyphs * pNew );
#endif

	int					ReplaceBitmapGlyphs( WgBitmapGlyphs * pOld, WgBitmapGlyphs * pNew );


	bool				SetBitmapGlyphs( WgBitmapGlyphs * pGlyph, WgFontStyle style, int size );
	bool				SetDefaultBitmapGlyphs( WgBitmapGlyphs * pGlyphs, int size = 0 );

	WgBitmapGlyphs *		GetBitmapGlyphs( WgFontStyle style, int size );
	inline WgBitmapGlyphs *	GetDefaultBitmapGlyphs( int size = 0 ) const { if( size <= WG_MAX_FONTSIZE ) return m_aDefaultBitmapGlyphs[size]; else return false; }

	bool				SetCursor( WgCursor * pCursor );
	inline WgCursor * 	GetCursor() const { return m_pCursor; }

	const WgUnderline *	GetUnderline( int size );


protected:
	void	Init();

	WgCursor *			m_pCursor;

#ifdef	WG_USE_FREETYPE
	WgVectorGlyphs *	m_pDefaultVectorGlyphs;
	WgVectorGlyphs *	m_aVectorGlyphs[WG_NB_FONTSTYLES];
#endif

	WgBitmapGlyphs **	m_aBitmapGlyphs[WG_MAX_FONTSIZE+1];			// Pointer at an array of WG_NB_FONTSTYLES WgBitmapGlyphs.
	WgBitmapGlyphs *	m_aDefaultBitmapGlyphs[WG_MAX_FONTSIZE+1];

	WgUnderline			m_tempUnderline;							// Holds the last requested underline.

};




#endif	// WG_FONT_DOT_H
