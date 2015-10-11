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
	
		inline static bool	cmpUTF8( const char * pChar1, uint16_t char2 );
		inline static int	sizeUTF8( uint16_t ch );
	
		inline static uint16_t readChar( const char *& pStr );
		static uint16_t	parseChar( const char * pStr );	// Should be null terminated
	
		static uint32_t	readString( const char *& pSrc, uint16_t * pDst, uint32_t maxChars = INT_MAX );
		static uint32_t	readString( const char *& pSrc, Char * pDst, uint32_t maxChars = INT_MAX );
		static uint32_t	readString( const uint16_t *& pSrc, Char * pDst, uint32_t maxChars = INT_MAX );
		static uint32_t	readString( const char *& pSrc, WgCodePage codepage, Char * pDst, uint32_t maxChars = INT_MAX );
		static uint32_t	readString( const char *& pSrc, WgCodePage codepage, uint16_t * pDst, uint32_t maxChars = INT_MAX );
	
		static uint32_t	countChars( const char * pStr, uint32_t strlen = INT_MAX );
	
		static uint32_t	countLines( const char * pStr );
		static uint32_t	countLines( const uint16_t * pStr );
		static uint32_t	countLines( const Char * pStr );
	
		static uint32_t	countWhitespaces( const char * pStr, uint32_t len = INT_MAX );
		static uint32_t	countWhitespaces( const uint16_t * pStr, uint32_t len = INT_MAX  );
		static uint32_t	countWhitespaces( const Char * pStr, uint32_t len = INT_MAX  );
	
		static uint32_t	countLineChars( const char * pStr, uint32_t len = INT_MAX );
		static uint32_t	countLineChars( const uint16_t * pStr, uint32_t len = INT_MAX );
		static uint32_t	countLineChars( const Char * pStr, uint32_t len = INT_MAX );
	
		static void		countCharsLines( const char * pStr, uint32_t& putChars, uint32_t& putLines, uint32_t strlen = INT_MAX );
		static void		countCharsLines( const uint16_t * pStr, uint32_t& putChars, uint32_t& putLines, uint32_t strlen = INT_MAX );
		static void		countCharsLines( const Char * pStr, uint32_t& putChars, uint32_t& putLines, uint32_t strlen = INT_MAX );
	
		static uint32_t	countNonFormattingChars( const char * pStr, uint32_t strlen = INT_MAX );
		static uint32_t	countNonFormattingChars( const uint16_t * pStr, uint32_t strlen = INT_MAX );
	
	
	
		static uint32_t	readFormattedString( const char * pSrc, Char * pDst, uint32_t maxChars = INT_MAX, const ResDB * pResDB = 0 );
		static uint32_t	readFormattedString( const uint16_t * pSrc, Char * pDst, uint32_t maxChars = INT_MAX, const ResDB * pResDB = 0 );
	
		static uint32_t	copyChars( const Char * pSrc, Char * pDst, uint32_t maxChars = INT_MAX );
	
		static char * nextLine( char * pStr );
	
	
		static uint32_t	writeUTF8( uint16_t charcode, char * pDest );
	
		//____
	
		static bool		uint16ToAscii( uint16_t value, uint16_t * pDest, uint32_t maxChars );
		static uint16_t	asciiTouint16_t( const uint16_t * pAscii );
		static bool		uint16ToAscii( uint16_t value, char * pDest, uint32_t maxChars );
		static uint16_t	asciiTouint16_t( const char * pAscii );
	
		static bool		uint8ToAscii( uint8_t value, uint16_t * pDest, uint32_t maxChars );
		static uint8_t	asciiTouint8_t( const uint16_t * pAscii );
		static bool		uint8ToAscii( uint8_t value, char * pDest, uint32_t maxChars );
		static uint8_t	asciiTouint8_t( const char * pAscii );
	
	//	static uint32_t	formatColor( const Color& color, uint16_t * pDest, uint32_t maxChars );
		static uint32_t	formatBeginColor( const Color& color, char * pDest );
	
		static uint32_t	uint16ToUtf8( uint16_t value, char * pDest, uint32_t maxChars );
	
	
		static uint32_t	getTextUTF8( const Char * pSrc, char * pDest, uint32_t maxBytes );
		static uint32_t	getTextUTF8( const uint16_t * pSrc, char * pDest, uint32_t maxBytes );
		static uint32_t	getTextUTF8( const char * pSrc, WgCodePage codepage, char * pDest, int maxChars = INT_MAX );
	
		static uint32_t	getTextSizeUTF8( const Char * pSrc, uint32_t maxChars = INT_MAX );
		static uint32_t	getTextSizeUTF8( const uint16_t * pSrc, uint32_t maxChars = INT_MAX );
		static uint32_t	getTextSizeUTF8( const char * pSrc, WgCodePage codepage, int maxChars = INT_MAX );
	
		static uint32_t	getTextFormatted( const Char * pSrc, uint16_t * pDest, uint32_t maxChars, const ResDB * pResDB = 0 );
		static uint32_t	getTextSizeFormatted( const Char * pSrc, uint32_t maxChars = INT_MAX, const ResDB * pResDB = 0 );
	
		static uint32_t	getTextFormattedUTF8( const Char * pSrc, char * pDest, uint32_t maxBytes, const ResDB * pResDB = 0 );
		static uint32_t	getTextSizeFormattedUTF8( const Char * pSrc, uint32_t maxChars = INT_MAX, const ResDB * pResDB = 0 );
	
		static uint32_t	getTextSizeStrippedUTF8( const char * pStr, uint32_t maxChars = INT_MAX );
		static uint32_t	getTextSizeStrippedUTF8( const uint16_t * pStr, uint32_t maxChars = INT_MAX );
		static uint32_t	getTextSizeStripped( const char * pStr, uint32_t maxChars = INT_MAX );
		static uint32_t	getTextSizeStripped( const uint16_t * pStr, uint32_t maxChars = INT_MAX );
	
	
	
		static int		stripTextCommands( const uint16_t* pSrc, uint16_t* pDest, int maxChars );
		static int		stripTextCommands( const char* pSrc, char* pDest, int maxBytes );
	
		static int		stripTextCommandsConvert( const uint16_t* pSrc, char* pDest, int maxChars );
		static int		stripTextCommandsConvert( const char* pSrc, uint16_t* pDest, int maxChars );
	
	
		static int		stripTextColorCommands( const uint16_t* pSrc, uint16_t* pDest, int maxChars );
		static int		stripTextColorCommands( const char* pSrc, char* pDest, int maxChars );
	
		static uint32_t	strlen( const uint16_t * pSrc );
		static uint32_t	strlen( const Char * pSrc );
		static int		strcmp( const uint16_t * pStr1, const uint16_t * pStr2 );
		static int		strcmp( const Char * pStr1, const Char * pStr2 );
		static int		glyphcmp( const Char * pStr1, const Char * pStr2 );
		static int		glyphcmpIgnoreCase( const Char * pStr1, const Char * pStr2 );
	
	
		static uint32_t	lineWidth( const TextAttr& attr, const char * pString );
		static uint32_t	lineWidth( const TextAttr& attr, const uint16_t * pString );
		static uint32_t	lineWidth( const TextAttr& attr, State mode, const Char * pString );
	
		static void		forwardCharacters( const char *& pChar, uint32_t nChars );
		static void		forwardEscapedCharacters( const char *& pChar, uint32_t nChars );
		static void		forwardEscapedCharacters( const uint16_t *& pChar, uint32_t nChars );
	
		static void			setGlyph( uint16_t glyph, Char * pChar, uint32_t nb );
		static void			setChars( const Char& ch, Char * pChar, uint32_t nb );
	
		static void			setProperties( const Textprop_p& pProp, Char * pChar, uint32_t nb );
		static void			setLink( const TextLink_p& pLink, Char * pChar, uint32_t nb );
		static void			setFont( const Font_p& pFont, Char * pChar, uint32_t nb );
		static void			setBreakLevel( int breakLevel, Char * pChar, uint32_t nb );
	
		static void			setColor( const Color col, Char * pChar, uint32_t nb );
		static void			setSize( int size, Char * pChar, uint32_t nb );
		static void			setStyle( FontAlt style, Char * pChar, uint32_t nb );
		static void			setUnderlined( Char * pChar, uint32_t nb );
	
		static void			setColor( const Color col, Char * pChar, uint32_t nb, State state );
		static void			setSize( int size, Char * pChar, uint32_t nb, State state );
		static void			setStyle( FontAlt style, Char * pChar, uint32_t nb, State state );
		static void			setUnderlined( Char * pChar, uint32_t nb, State state );
	
	
		inline static void	clearProperties( Char * pChar, uint32_t nb ) { setProperties( 0, pChar, nb ); }
		inline static void	clearLink( Char * pChar, uint32_t nb ) { setLink(0,pChar,nb); }
		inline static void	clearFont( Char * pChar, uint32_t nb ) { setFont(0,pChar,nb); }
		inline static void	clearBreakLevel( Char * pChar, uint32_t nb ) { setBreakLevel(-1,pChar,nb); }
	
		static void			clearColor( Char * pChar, uint32_t nb );
		static void			clearSize( Char * pChar, uint32_t nb );
		static void			clearStyle( Char * pChar, uint32_t nb );
		static void			clearUnderlined( Char * pChar, uint32_t nb );
	
		static void			clearColor( Char * pChar, uint32_t nb, State state );
		static void			clearSize( Char * pChar, uint32_t nb, State state );
		static void			clearStyle( Char * pChar, uint32_t nb, State state );
		static void			clearUnderlined( Char * pChar, uint32_t nb, State state );
	
	
	
		static void			addPropAttributes( TextAttr& attr, const Textprop_p& pProp, State state = WG_STATE_NORMAL );
	//	static void			setAttrColor( TextAttr& attr, const ColorsetPtr& pColors, State state = WG_STATE_NORMAL );
	
		static Caret_p		getCursor( const LegacyTextField * pText );
		static Textprop_p	getSelectionProperties( const LegacyTextField * pText );
		static Textprop_p	getLinkProperties( const LegacyTextField * pText );
	
	    static char *           itoa( int value, char * str, int base );
	
		static WgBreakRules		isBreakAllowed( uint16_t chr, int breakLevel );
	
		static void				clearBreakRules();
		static bool				setBreakRule( unsigned char character, int level, WgBreakRules rule );
		static void				setDefaultBreakRules();
	
		//____
	
		class TextpropEncoder
		{
		public:
			TextpropEncoder( const ResDB * pResDB );
	
			uint32_t	beginString();
			uint32_t	setProp( const Textprop_p& pNewProp );
			uint32_t	endString();
	
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
	
		static void modifyProperties( const PropModifier& modif, Char * pChar, uint32_t nb );
	
		inline static uint8_t	nibbleToAscii( uint8_t nibble );
		inline static uint8_t	asciiToNibble( uint8_t ascii );
	
		static void	refProps( Char * p, uint32_t n );
		static void	derefProps( Char * p, uint32_t n );
	};
	
	//____ readChar() _____________________________________________________________
	
	inline uint16_t TextTool::readChar( const char *& pStr )
	{
	
	#ifdef WG_USE_UTF8
	
	#ifdef WG_SAFE_UTF8	// SAFE UTF8 VERSION
	
		uint16_t g = * ((const unsigned char *&)pStr)++;
	
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
	
		uint32_t g = *((const unsigned char *&)pStr)++;
	
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
		return (uint32_t) * ((const unsigned char *&)pStr)++;
	#endif
	}
	
	//____ cmpUTF8() ______________________________________________________________
	
	inline bool TextTool::cmpUTF8( const char * pChar1, uint16_t char2 )
	{
		uint32_t g = (unsigned char) pChar1[0];
	
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
	
	inline int TextTool::sizeUTF8( uint16_t ch )
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
