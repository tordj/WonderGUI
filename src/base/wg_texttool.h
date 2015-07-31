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

#ifndef	WG_TEXTTOOL_DOT_H
#define WG_TEXTTOOL_DOT_H

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_GLYPHSET_DOT_H
#	include <wg_glyphset.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

#ifndef WG_CHARSEQ_DOT_H
#	include <wg_charseq.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

namespace wg 
{
	
	class Font;
	class Glyphset;
	class LegacyTextField;
	class Char;
	class CaretInstance;
	class ResDB;
	class TextNode;
	
	//____ TextTool _____________________________________________________________
	
	class TextTool
	{
		friend class CharBuffer;
		friend class String;
		friend class CharSeq;
		friend class CharSeq::CharBasket;
	
	public:
	
		inline static bool	cmpUTF8( const char * pChar1, Uint16 char2 );
		inline static int	sizeUTF8( Uint16 ch );
	
		inline static Uint16 readChar( const char *& pStr );
		static Uint16	parseChar( const char * pStr );	// Should be null terminated
	
		static Uint32	readString( const char *& pSrc, Uint16 * pDst, Uint32 maxChars = INT_MAX );
		static Uint32	readString( const char *& pSrc, Char * pDst, Uint32 maxChars = INT_MAX );
		static Uint32	readString( const Uint16 *& pSrc, Char * pDst, Uint32 maxChars = INT_MAX );
		static Uint32	readString( const char *& pSrc, WgCodePage codepage, Char * pDst, Uint32 maxChars = INT_MAX );
		static Uint32	readString( const char *& pSrc, WgCodePage codepage, Uint16 * pDst, Uint32 maxChars = INT_MAX );
	
		static Uint32	countChars( const char * pStr, Uint32 strlen = INT_MAX );
	
		static Uint32	countLines( const char * pStr );
		static Uint32	countLines( const Uint16 * pStr );
		static Uint32	countLines( const Char * pStr );
	
		static Uint32	countWhitespaces( const char * pStr, Uint32 len = INT_MAX );
		static Uint32	countWhitespaces( const Uint16 * pStr, Uint32 len = INT_MAX  );
		static Uint32	countWhitespaces( const Char * pStr, Uint32 len = INT_MAX  );
	
		static Uint32	countLineChars( const char * pStr, Uint32 len = INT_MAX );
		static Uint32	countLineChars( const Uint16 * pStr, Uint32 len = INT_MAX );
		static Uint32	countLineChars( const Char * pStr, Uint32 len = INT_MAX );
	
		static void		countCharsLines( const char * pStr, Uint32& putChars, Uint32& putLines, Uint32 strlen = INT_MAX );
		static void		countCharsLines( const Uint16 * pStr, Uint32& putChars, Uint32& putLines, Uint32 strlen = INT_MAX );
		static void		countCharsLines( const Char * pStr, Uint32& putChars, Uint32& putLines, Uint32 strlen = INT_MAX );
	
		static Uint32	countNonFormattingChars( const char * pStr, Uint32 strlen = INT_MAX );
		static Uint32	countNonFormattingChars( const Uint16 * pStr, Uint32 strlen = INT_MAX );
	
	
	
		static Uint32	readFormattedString( const char * pSrc, Char * pDst, Uint32 maxChars = INT_MAX, const ResDB * pResDB = 0 );
		static Uint32	readFormattedString( const Uint16 * pSrc, Char * pDst, Uint32 maxChars = INT_MAX, const ResDB * pResDB = 0 );
	
		static Uint32	copyChars( const Char * pSrc, Char * pDst, Uint32 maxChars = INT_MAX );
	
		static char * nextLine( char * pStr );
	
	
		static Uint32	writeUTF8( Uint16 charcode, char * pDest );
	
		//____
	
		static bool		uint16ToAscii( Uint16 value, Uint16 * pDest, Uint32 maxChars );
		static Uint16	asciiToUint16( const Uint16 * pAscii );
		static bool		uint16ToAscii( Uint16 value, char * pDest, Uint32 maxChars );
		static Uint16	asciiToUint16( const char * pAscii );
	
		static bool		uint8ToAscii( Uint8 value, Uint16 * pDest, Uint32 maxChars );
		static Uint8	asciiToUint8( const Uint16 * pAscii );
		static bool		uint8ToAscii( Uint8 value, char * pDest, Uint32 maxChars );
		static Uint8	asciiToUint8( const char * pAscii );
	
	//	static Uint32	formatColor( const Color& color, Uint16 * pDest, Uint32 maxChars );
		static Uint32	formatBeginColor( const Color& color, char * pDest );
	
		static Uint32	uint16ToUtf8( Uint16 value, char * pDest, Uint32 maxChars );
	
	
		static Uint32	getTextUTF8( const Char * pSrc, char * pDest, Uint32 maxBytes );
		static Uint32	getTextUTF8( const Uint16 * pSrc, char * pDest, Uint32 maxBytes );
		static Uint32	getTextUTF8( const char * pSrc, WgCodePage codepage, char * pDest, int maxChars = INT_MAX );
	
		static Uint32	getTextSizeUTF8( const Char * pSrc, Uint32 maxChars = INT_MAX );
		static Uint32	getTextSizeUTF8( const Uint16 * pSrc, Uint32 maxChars = INT_MAX );
		static Uint32	getTextSizeUTF8( const char * pSrc, WgCodePage codepage, int maxChars = INT_MAX );
	
		static Uint32	getTextFormatted( const Char * pSrc, Uint16 * pDest, Uint32 maxChars, const ResDB * pResDB = 0 );
		static Uint32	getTextSizeFormatted( const Char * pSrc, Uint32 maxChars = INT_MAX, const ResDB * pResDB = 0 );
	
		static Uint32	getTextFormattedUTF8( const Char * pSrc, char * pDest, Uint32 maxBytes, const ResDB * pResDB = 0 );
		static Uint32	getTextSizeFormattedUTF8( const Char * pSrc, Uint32 maxChars = INT_MAX, const ResDB * pResDB = 0 );
	
		static Uint32	getTextSizeStrippedUTF8( const char * pStr, Uint32 maxChars = INT_MAX );
		static Uint32	getTextSizeStrippedUTF8( const Uint16 * pStr, Uint32 maxChars = INT_MAX );
		static Uint32	getTextSizeStripped( const char * pStr, Uint32 maxChars = INT_MAX );
		static Uint32	getTextSizeStripped( const Uint16 * pStr, Uint32 maxChars = INT_MAX );
	
	
	
		static int		stripTextCommands( const Uint16* pSrc, Uint16* pDest, int maxChars );
		static int		stripTextCommands( const char* pSrc, char* pDest, int maxBytes );
	
		static int		stripTextCommandsConvert( const Uint16* pSrc, char* pDest, int maxChars );
		static int		stripTextCommandsConvert( const char* pSrc, Uint16* pDest, int maxChars );
	
	
		static int		stripTextColorCommands( const Uint16* pSrc, Uint16* pDest, int maxChars );
		static int		stripTextColorCommands( const char* pSrc, char* pDest, int maxChars );
	
		static Uint32	strlen( const Uint16 * pSrc );
		static Uint32	strlen( const Char * pSrc );
		static int		strcmp( const Uint16 * pStr1, const Uint16 * pStr2 );
		static int		strcmp( const Char * pStr1, const Char * pStr2 );
		static int		glyphcmp( const Char * pStr1, const Char * pStr2 );
		static int		glyphcmpIgnoreCase( const Char * pStr1, const Char * pStr2 );
	
	
		static Uint32	lineWidth( const TextAttr& attr, const char * pString );
		static Uint32	lineWidth( const TextAttr& attr, const Uint16 * pString );
		static Uint32	lineWidth( const TextAttr& attr, State mode, const Char * pString );
	
		static void		forwardCharacters( const char *& pChar, Uint32 nChars );
		static void		forwardEscapedCharacters( const char *& pChar, Uint32 nChars );
		static void		forwardEscapedCharacters( const Uint16 *& pChar, Uint32 nChars );
	
		static void			setGlyph( Uint16 glyph, Char * pChar, Uint32 nb );
		static void			setChars( const Char& ch, Char * pChar, Uint32 nb );
	
		static void			setProperties( const Textprop_p& pProp, Char * pChar, Uint32 nb );
		static void			setLink( const TextLink_p& pLink, Char * pChar, Uint32 nb );
		static void			setFont( const Font_p& pFont, Char * pChar, Uint32 nb );
		static void			setBreakLevel( int breakLevel, Char * pChar, Uint32 nb );
	
		static void			setColor( const Color col, Char * pChar, Uint32 nb );
		static void			setSize( int size, Char * pChar, Uint32 nb );
		static void			setStyle( FontAlt style, Char * pChar, Uint32 nb );
		static void			setUnderlined( Char * pChar, Uint32 nb );
	
		static void			setColor( const Color col, Char * pChar, Uint32 nb, State state );
		static void			setSize( int size, Char * pChar, Uint32 nb, State state );
		static void			setStyle( FontAlt style, Char * pChar, Uint32 nb, State state );
		static void			setUnderlined( Char * pChar, Uint32 nb, State state );
	
	
		inline static void	clearProperties( Char * pChar, Uint32 nb ) { setProperties( 0, pChar, nb ); }
		inline static void	clearLink( Char * pChar, Uint32 nb ) { setLink(0,pChar,nb); }
		inline static void	clearFont( Char * pChar, Uint32 nb ) { setFont(0,pChar,nb); }
		inline static void	clearBreakLevel( Char * pChar, Uint32 nb ) { setBreakLevel(-1,pChar,nb); }
	
		static void			clearColor( Char * pChar, Uint32 nb );
		static void			clearSize( Char * pChar, Uint32 nb );
		static void			clearStyle( Char * pChar, Uint32 nb );
		static void			clearUnderlined( Char * pChar, Uint32 nb );
	
		static void			clearColor( Char * pChar, Uint32 nb, State state );
		static void			clearSize( Char * pChar, Uint32 nb, State state );
		static void			clearStyle( Char * pChar, Uint32 nb, State state );
		static void			clearUnderlined( Char * pChar, Uint32 nb, State state );
	
	
	
		static void			addPropAttributes( TextAttr& attr, const Textprop_p& pProp, State state = WG_STATE_NORMAL );
	//	static void			setAttrColor( TextAttr& attr, const ColorsetPtr& pColors, State state = WG_STATE_NORMAL );
	
		static Caret_p		getCursor( const LegacyTextField * pText );
		static Textprop_p	getSelectionProperties( const LegacyTextField * pText );
		static Textprop_p	getLinkProperties( const LegacyTextField * pText );
	
	    static char *           itoa( int value, char * str, int base );
	
		static WgBreakRules		isBreakAllowed( Uint16 chr, int breakLevel );
	
		static void				clearBreakRules();
		static bool				setBreakRule( unsigned char character, int level, WgBreakRules rule );
		static void				setDefaultBreakRules();
	
		//____
	
		class TextpropEncoder
		{
		public:
			TextpropEncoder( const ResDB * pResDB );
	
			Uint32	beginString();
			Uint32	setProp( const Textprop_p& pNewProp );
			Uint32	endString();
	
			inline const char * getCodes() const { return m_temp; }
	
		private:
			char			m_temp[128];
	
			bool			m_bColorTagOpen;
			bool			m_bStyleTagOpen;
			bool			m_bSizeTagOpen;
			bool			m_bUnderTagOpen;
	
			const ResDB *	m_pResDB;
	
			Textprop_p	m_pBaseProp;
			Textprop_p	m_pActiveProp;
		};
	
	
	private:
		//____
	
		class PropModifier
		{
		public:
			virtual void modify( Textprop& prop ) const = 0;
	
		protected:
			PropModifier() {};
			virtual ~PropModifier() {}
		};
	
		class PropSizeModifier : public PropModifier
		{
		public:
			PropSizeModifier( int size ) { m_size = size; }
			void modify( Textprop& prop ) const { prop.setSize(m_size); }
		private:
			int		m_size;
		};
	
		class PropSizeClearer : public PropModifier
		{
		public:
			void modify( Textprop& prop ) const { prop.clearSize(); }
		};
	
	
		class PropColorModifier : public PropModifier
		{
		public:
			PropColorModifier( Color col ) { m_col = col; }
			void modify( Textprop& prop ) const { prop.setColor(m_col); }
		private:
			Color m_col;
		};
	
		class PropColorClearer : public PropModifier
		{
		public:
			void modify( Textprop& prop ) const { prop.clearColor(); }
		};
	
	
		class PropStyleModifier : public PropModifier
		{
		public:
			PropStyleModifier( FontAlt style ) { m_style = style; }
			void modify( Textprop& prop ) const { prop.setStyle(m_style); }
		private:
			FontAlt m_style;
		};
	
		class PropUnderlinedModifier : public PropModifier
		{
		public:
			PropUnderlinedModifier( bool bUnderlined ) { m_bUnderlined = bUnderlined; }
			void modify( Textprop& prop ) const { if( m_bUnderlined) prop.setUnderlined(); else prop.clearUnderlined(); }
		private:
			bool	m_bUnderlined;
		};
	
		class PropBreakLevelModifier : public PropModifier
		{
		public:
			PropBreakLevelModifier( int level ) { m_level = level; }
			void modify( Textprop& prop ) const { prop.setBreakLevel(m_level); }
		private:
			int		m_level;
		};
	
		class PropLinkModifier : public PropModifier
		{
		public:
			PropLinkModifier( const TextLink_p& pLink ) { m_pLink = pLink; }
			void modify( Textprop& prop ) const { prop.setLink( m_pLink ); }
		private:
			TextLink_p	m_pLink;
		};
	
		class PropFontModifier : public PropModifier
		{
		public:
			PropFontModifier( const Font_p& pFont ) { m_pFont = pFont; }
			void modify( Textprop& prop ) const { prop.setFont( m_pFont ); }
		private:
			Font_p	m_pFont;
		};
	
		class PropStateSizeModifier : public PropModifier
		{
		public:
			PropStateSizeModifier( int size, State state ) { m_size = size; m_state = state; }
			void modify( Textprop& prop ) const { prop.setSize(m_size,m_state); }
		private:
			int		m_size;
			State	m_state;
		};
	
		class PropStateSizeClearer : public PropModifier
		{
		public:
			PropStateSizeClearer( State state ) { m_state = state; }
			void modify( Textprop& prop ) const { prop.clearSize(m_state); }
		private:
			State	m_state;
		};
	
	
		class PropStateColorModifier : public PropModifier
		{
		public:
			PropStateColorModifier( Color col, State state ) { m_col = col; m_state = state; }
			void modify( Textprop& prop ) const { prop.setColor(m_col,m_state); }
		private:
			Color m_col;
			State	m_state;
		};
	
		class PropStateColorClearer : public PropModifier
		{
		public:
			PropStateColorClearer( State state ) { m_state = state; }
			void modify( Textprop& prop ) const { prop.clearColor(m_state); }
		private:
			State	m_state;
		};
	
	
		class PropStateStyleModifier : public PropModifier
		{
		public:
			PropStateStyleModifier( FontAlt style, State state ) { m_style = style; m_state = state; }
			void modify( Textprop& prop ) const { prop.setStyle(m_style,m_state); }
		private:
			FontAlt m_style;
			State	m_state;
		};
	
		class PropStateUnderlinedModifier : public PropModifier
		{
		public:
			PropStateUnderlinedModifier( bool bUnderlined, State state ) { m_bUnderlined = bUnderlined; m_state = state; }
			void modify( Textprop& prop ) const { if( m_bUnderlined) prop.setUnderlined(m_state); else prop.clearUnderlined(m_state); }
		private:
			bool	m_bUnderlined;
			State	m_state;
		};
	
	
	
		//____
	
		static void modifyProperties( const PropModifier& modif, Char * pChar, Uint32 nb );
	
		inline static Uint8	nibbleToAscii( Uint8 nibble );
		inline static Uint8	asciiToNibble( Uint8 ascii );
	
		static void	refProps( Char * p, Uint32 n );
		static void	derefProps( Char * p, Uint32 n );
	};
	
	//____ readChar() _____________________________________________________________
	
	inline Uint16 TextTool::readChar( const char *& pStr )
	{
	
	#ifdef WG_USE_UTF8
	
	#ifdef WG_SAFE_UTF8	// SAFE UTF8 VERSION
	
		Uint16 g = * ((const unsigned char *&)pStr)++;
	
		if( g < 128 )
			return g;											// 1 byte character (7 bits data).
	
		// Skip incomplete character if we started in the middle of one.
	
		while( g & 0x40 == 0 )
		{
			if( g == 0 )
				return 0;
			g = * ((const unsigned char *&)pStr)++;
		}
	
		// Decode character the safer way.
	
		if( g & 0xE0 == 0xC0 )					// 2 bytes character (11 bits data).
		{
			g &= 0x1F;
			g <<= 6;
			if( * ((const unsigned char *&)pStr) < 128 ) return 0xFFFFFFFF; // Broken character.
			g += ((*pStr++) & 0x3F);
		}
		else if( g & 0xF0 == 0xE0 )			// 3 bytes character (16 bits data).
		{
			g &= 0x0F;
			g <<= 6;
			if( * ((const unsigned char *&)pStr) < 128 ) return 0xFFFFFFFF; // Broken character.
			g += ((*pStr++) & 0x3F);
			g <<= 6;
			if( * ((const unsigned char *&)pStr) < 128 ) return 0xFFFFFFFF; // Broken character.
			g += ((*pStr++) & 0x3F);
		}
		else if( g & 0xF8 == 0xF0 )			// 4 bytes character (21 bits data).
		{
			g &= 0x07;
			g <<= 6;
			if( * ((const unsigned char *&)pStr) < 128 ) return 0xFFFFFFFF; // Broken character.
			g += ((*pStr++) & 0x3F);
			g <<= 6;
			if( * ((const unsigned char *&)pStr) < 128 ) return 0xFFFFFFFF; // Broken character.
			g += ((*pStr++) & 0x3F);
			g <<= 6;
			if( * ((const unsigned char *&)pStr) < 128 ) return 0xFFFFFFFF; // Broken character.
			g += ((*pStr++) & 0x3F);
		}
		else
			return 0xFFFFFFFF;					// Broken character (or more than 4 bytes which we don't support)
	
	#else // FAST UTF8 VERSION
	
		Uint32 g = *((const unsigned char *&)pStr)++;
	
		if( g < 128 )
			return g;											// 1 byte character (7 bits data).
	
		g &= 0x3F;
		g <<= 6;
		g += ((*pStr++) & 0x3F);
	
		if( g >= (1<<11) )								// 3 bytes character (16 bits data).
		{
			g &= (1<<11)-1;
			g <<= 6;
			g += ((*pStr++) & 0x3F);
		}
	#endif
		return g;
	
	#else // 8 BIT CHARACTER VERSION
		return (Uint32) * ((const unsigned char *&)pStr)++;
	#endif
	}
	
	//____ cmpUTF8() ______________________________________________________________
	
	inline bool TextTool::cmpUTF8( const char * pChar1, Uint16 char2 )
	{
		Uint32 g = (unsigned char) pChar1[0];
	
		if( g >= 128 )
		{
			g &= 0x3F;
			g <<= 6;
			g += ((pChar1[1]) & 0x3F);
	
			if( g >= (1<<11) )								// 3 bytes character (16 bits data).
			{
				g &= (1<<11)-1;
				g <<= 6;
				g += ((pChar1[2]) & 0x3F);
			}
		}
	
		return (g == char2);
	}
	
	//____ sizeUTF8() _____________________________________________________________
	
	inline int TextTool::sizeUTF8( Uint16 ch )
	{
		if( ch < 128 )
			return 1;
	
		if( ch < (1<<11) )
			return 2;
	
		return 3;
	}
	
	
	
	typedef TextTool TextParser;
	

} // namespace wg
#endif //WG_TEXTTOOL_DOT_H
