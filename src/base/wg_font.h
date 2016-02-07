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

#ifdef	USE_FREETYPE
#	ifndef WG_VECTORGLYPHS_DOT_H
#		include <wg_vectorglyphs.h>
#	endif
#endif

#ifndef WG_BITMAPGLYPHS_DOT_H
#	include <wg_bitmapglyphs.h>
#endif

namespace wg 
{
	
	
	class Font;
	typedef	StrongPtr<Font,Object_p>		Font_p;
	typedef	WeakPtr<Font,Object_wp>	Font_wp;
	
	//____ Font ______________________________________________________________
	
	class Font : public Object
	{
	public:
		static Font_p	create() { return Font_p(new Font()); }
	#ifdef USE_FREETYPE
		static Font_p	create( const VectorGlyphs_p& pNormal ) { return Font_p(new Font(pNormal)); }
	#endif
		static Font_p	create( const BitmapGlyphs_p& pNormal, int size ) { return Font_p(new Font(pNormal,size)); }
	
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Font_p	cast( const Object_p& pObject );
	
		enum GlyphProvided
		{
			NOT_PROVIDED			= 0,
			EXACT_MATCH_PROVIDED	= 1,
			DEFAULT_PROVIDED		= 2,
			SMALLER_MATCH_PROVIDED	= 3,
			SMALLER_DEFAULT_PROVIDED= 4
		};
	
	
	
		Glyphset_p		getGlyphset( FontAlt style, int size ) const;
		Glyph_p			getGlyph( uint32_t chr, FontAlt style, int size ) const;
		GlyphProvided		isGlyphProvided( uint32_t chr, FontAlt style, int size ) const;
	
	#ifdef	USE_FREETYPE
		bool				setVectorGlyphs( const VectorGlyphs_p& pGlyphs, FontAlt style  );
		bool				setVectorGlyphs( const VectorGlyphs_p& pGlyphs, FontAlt style, int size );
		bool				setDefaultVectorGlyphs( const VectorGlyphs_p& pGlyphs );
	
		inline VectorGlyphs_p	getVectorGlyphs( FontAlt style, int size  ) const { if( size <= WG_MAX_FONTSIZE && m_aVectorGlyphs[(int)style] != 0 ) return m_aVectorGlyphs[(int)style][size]; else return 0; }
		inline VectorGlyphs_p	getDefaultVectorGlyphs( ) const { return m_pDefaultVectorGlyphs; }
	
		int					replaceVectorGlyphs( const VectorGlyphs_p& pOld, const VectorGlyphs_p& pNew );
	#endif
	
		int					replaceBitmapGlyphs( const BitmapGlyphs_p& pOld, const BitmapGlyphs_p& pNew );
	
	
		bool				setBitmapGlyphs( const BitmapGlyphs_p& pGlyph, FontAlt style, int size );
		bool				setDefaultBitmapGlyphs( const BitmapGlyphs_p& pGlyphs, int size = 0 );
	
		BitmapGlyphs_p			getBitmapGlyphs( FontAlt style, int size );
		inline BitmapGlyphs_p	getDefaultBitmapGlyphs( int size = 0 ) const { if( size <= WG_MAX_FONTSIZE ) return m_aDefaultBitmapGlyphs[size]; else return 0; }
	
		const Underline *	getUnderline( int size );
	
	
	protected:
		Font();
	#ifdef USE_FREETYPE
		Font( const VectorGlyphs_p& pNormal );
	#endif
		Font( const BitmapGlyphs_p& pNormal, int size );
		~Font();
	
		void	_init();
	
	#ifdef	USE_FREETYPE
		VectorGlyphs_p	m_pDefaultVectorGlyphs;
		VectorGlyphs_p*	m_aVectorGlyphs[WG_NB_FONTSTYLES];			// Pointer at an array of WG_MAX_FONTSIZE+1 WgVectorGlyhphs.
	#endif
	
		BitmapGlyphs_p *	m_aBitmapGlyphs[WG_MAX_FONTSIZE+1];			// Pointer at an array of WG_NB_FONTSTYLES BitmapGlyphs.
		BitmapGlyphs_p	m_aDefaultBitmapGlyphs[WG_MAX_FONTSIZE+1];
	
		Underline			m_tempUnderline;							// Holds the last requested underline.
	
	};
	
	
	
	

} // namespace wg
#endif	// WG_FONT_DOT_H
