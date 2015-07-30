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

class WgFont;
class WgGlyphset;
class WgLegacyTextField;
class WgChar;
class WgCaretInstance;
class WgResDB;
class WgTextNode;

//____ WgTextTool _____________________________________________________________

class WgTextTool
{
	friend class WgCharBuffer;
	friend class WgString;
	friend class WgCharSeq;
	friend class WgCharSeq::WgCharBasket;

public:

	inline static bool	cmpUTF8( const char * pChar1, Uint16 char2 );
	inline static int	sizeUTF8( Uint16 ch );

	inline static Uint16 readChar( const char *& pStr );
	static Uint16	parseChar( const char * pStr );	// Should be null terminated

	static Uint32	readString( const char *& pSrc, Uint16 * pDst, Uint32 maxChars = INT_MAX );
	static Uint32	readString( const char *& pSrc, WgChar * pDst, Uint32 maxChars = INT_MAX );
	static Uint32	readString( const Uint16 *& pSrc, WgChar * pDst, Uint32 maxChars = INT_MAX );
	static Uint32	readString( const char *& pSrc, WgCodePage codepage, WgChar * pDst, Uint32 maxChars = INT_MAX );
	static Uint32	readString( const char *& pSrc, WgCodePage codepage, Uint16 * pDst, Uint32 maxChars = INT_MAX );

	static Uint32	countChars( const char * pStr, Uint32 strlen = INT_MAX );

	static Uint32	countLines( const char * pStr );
	static Uint32	countLines( const Uint16 * pStr );
	static Uint32	countLines( const WgChar * pStr );

	static Uint32	countWhitespaces( const char * pStr, Uint32 len = INT_MAX );
	static Uint32	countWhitespaces( const Uint16 * pStr, Uint32 len = INT_MAX  );
	static Uint32	countWhitespaces( const WgChar * pStr, Uint32 len = INT_MAX  );

	static Uint32	countLineChars( const char * pStr, Uint32 len = INT_MAX );
	static Uint32	countLineChars( const Uint16 * pStr, Uint32 len = INT_MAX );
	static Uint32	countLineChars( const WgChar * pStr, Uint32 len = INT_MAX );

	static void		countCharsLines( const char * pStr, Uint32& putChars, Uint32& putLines, Uint32 strlen = INT_MAX );
	static void		countCharsLines( const Uint16 * pStr, Uint32& putChars, Uint32& putLines, Uint32 strlen = INT_MAX );
	static void		countCharsLines( const WgChar * pStr, Uint32& putChars, Uint32& putLines, Uint32 strlen = INT_MAX );

	static Uint32	countNonFormattingChars( const char * pStr, Uint32 strlen = INT_MAX );
	static Uint32	countNonFormattingChars( const Uint16 * pStr, Uint32 strlen = INT_MAX );



	static Uint32	readFormattedString( const char * pSrc, WgChar * pDst, Uint32 maxChars = INT_MAX, const WgResDB * pResDB = 0 );
	static Uint32	readFormattedString( const Uint16 * pSrc, WgChar * pDst, Uint32 maxChars = INT_MAX, const WgResDB * pResDB = 0 );

	static Uint32	copyChars( const WgChar * pSrc, WgChar * pDst, Uint32 maxChars = INT_MAX );

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

//	static Uint32	formatColor( const WgColor& color, Uint16 * pDest, Uint32 maxChars );
	static Uint32	formatBeginColor( const WgColor& color, char * pDest );

	static Uint32	uint16ToUtf8( Uint16 value, char * pDest, Uint32 maxChars );


	static Uint32	getTextUTF8( const WgChar * pSrc, char * pDest, Uint32 maxBytes );
	static Uint32	getTextUTF8( const Uint16 * pSrc, char * pDest, Uint32 maxBytes );
	static Uint32	getTextUTF8( const char * pSrc, WgCodePage codepage, char * pDest, int maxChars = INT_MAX );

	static Uint32	getTextSizeUTF8( const WgChar * pSrc, Uint32 maxChars = INT_MAX );
	static Uint32	getTextSizeUTF8( const Uint16 * pSrc, Uint32 maxChars = INT_MAX );
	static Uint32	getTextSizeUTF8( const char * pSrc, WgCodePage codepage, int maxChars = INT_MAX );

	static Uint32	getTextFormatted( const WgChar * pSrc, Uint16 * pDest, Uint32 maxChars, const WgResDB * pResDB = 0 );
	static Uint32	getTextSizeFormatted( const WgChar * pSrc, Uint32 maxChars = INT_MAX, const WgResDB * pResDB = 0 );

	static Uint32	getTextFormattedUTF8( const WgChar * pSrc, char * pDest, Uint32 maxBytes, const WgResDB * pResDB = 0 );
	static Uint32	getTextSizeFormattedUTF8( const WgChar * pSrc, Uint32 maxChars = INT_MAX, const WgResDB * pResDB = 0 );

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
	static Uint32	strlen( const WgChar * pSrc );
	static int		strcmp( const Uint16 * pStr1, const Uint16 * pStr2 );
	static int		strcmp( const WgChar * pStr1, const WgChar * pStr2 );
	static int		glyphcmp( const WgChar * pStr1, const WgChar * pStr2 );
	static int		glyphcmpIgnoreCase( const WgChar * pStr1, const WgChar * pStr2 );


	static Uint32	lineWidth( const WgTextAttr& attr, const char * pString );
	static Uint32	lineWidth( const WgTextAttr& attr, const Uint16 * pString );
	static Uint32	lineWidth( const WgTextAttr& attr, WgState mode, const WgChar * pString );

	static void		forwardCharacters( const char *& pChar, Uint32 nChars );
	static void		forwardEscapedCharacters( const char *& pChar, Uint32 nChars );
	static void		forwardEscapedCharacters( const Uint16 *& pChar, Uint32 nChars );

	static void			setGlyph( Uint16 glyph, WgChar * pChar, Uint32 nb );
	static void			setChars( const WgChar& ch, WgChar * pChar, Uint32 nb );

	static void			setProperties( const WgTextpropPtr& pProp, WgChar * pChar, Uint32 nb );
	static void			setLink( const WgTextLinkPtr& pLink, WgChar * pChar, Uint32 nb );
	static void			setFont( const WgFontPtr& pFont, WgChar * pChar, Uint32 nb );
	static void			setBreakLevel( int breakLevel, WgChar * pChar, Uint32 nb );

	static void			setColor( const WgColor col, WgChar * pChar, Uint32 nb );
	static void			setSize( int size, WgChar * pChar, Uint32 nb );
	static void			setStyle( WgFontAlt style, WgChar * pChar, Uint32 nb );
	static void			setUnderlined( WgChar * pChar, Uint32 nb );

	static void			setColor( const WgColor col, WgChar * pChar, Uint32 nb, WgState state );
	static void			setSize( int size, WgChar * pChar, Uint32 nb, WgState state );
	static void			setStyle( WgFontAlt style, WgChar * pChar, Uint32 nb, WgState state );
	static void			setUnderlined( WgChar * pChar, Uint32 nb, WgState state );


	inline static void	clearProperties( WgChar * pChar, Uint32 nb ) { setProperties( 0, pChar, nb ); }
	inline static void	clearLink( WgChar * pChar, Uint32 nb ) { setLink(0,pChar,nb); }
	inline static void	clearFont( WgChar * pChar, Uint32 nb ) { setFont(0,pChar,nb); }
	inline static void	clearBreakLevel( WgChar * pChar, Uint32 nb ) { setBreakLevel(-1,pChar,nb); }

	static void			clearColor( WgChar * pChar, Uint32 nb );
	static void			clearSize( WgChar * pChar, Uint32 nb );
	static void			clearStyle( WgChar * pChar, Uint32 nb );
	static void			clearUnderlined( WgChar * pChar, Uint32 nb );

	static void			clearColor( WgChar * pChar, Uint32 nb, WgState state );
	static void			clearSize( WgChar * pChar, Uint32 nb, WgState state );
	static void			clearStyle( WgChar * pChar, Uint32 nb, WgState state );
	static void			clearUnderlined( WgChar * pChar, Uint32 nb, WgState state );



	static void			addPropAttributes( WgTextAttr& attr, const WgTextpropPtr& pProp, WgState state = WG_STATE_NORMAL );
//	static void			setAttrColor( WgTextAttr& attr, const WgColorsetPtr& pColors, WgState state = WG_STATE_NORMAL );

	static WgCaretPtr		getCursor( const WgLegacyTextField * pText );
	static WgTextpropPtr	getSelectionProperties( const WgLegacyTextField * pText );
	static WgTextpropPtr	getLinkProperties( const WgLegacyTextField * pText );

    static char *           itoa( int value, char * str, int base );

	static WgBreakRules		isBreakAllowed( Uint16 chr, int breakLevel );

	static void				clearBreakRules();
	static bool				setBreakRule( unsigned char character, int level, WgBreakRules rule );
	static void				setDefaultBreakRules();

	//____

	class TextpropEncoder
	{
	public:
		TextpropEncoder( const WgResDB * pResDB );

		Uint32	beginString();
		Uint32	setProp( const WgTextpropPtr& pNewProp );
		Uint32	endString();

		inline const char * getCodes() const { return m_temp; }

	private:
		char			m_temp[128];

		bool			m_bColorTagOpen;
		bool			m_bStyleTagOpen;
		bool			m_bSizeTagOpen;
		bool			m_bUnderTagOpen;

		const WgResDB *	m_pResDB;

		WgTextpropPtr	m_pBaseProp;
		WgTextpropPtr	m_pActiveProp;
	};


private:
	//____

	class PropModifier
	{
	public:
		virtual void modify( WgTextprop& prop ) const = 0;

	protected:
		PropModifier() {};
		virtual ~PropModifier() {}
	};

	class PropSizeModifier : public PropModifier
	{
	public:
		PropSizeModifier( int size ) { m_size = size; }
		void modify( WgTextprop& prop ) const { prop.setSize(m_size); }
	private:
		int		m_size;
	};

	class PropSizeClearer : public PropModifier
	{
	public:
		void modify( WgTextprop& prop ) const { prop.clearSize(); }
	};


	class PropColorModifier : public PropModifier
	{
	public:
		PropColorModifier( WgColor col ) { m_col = col; }
		void modify( WgTextprop& prop ) const { prop.setColor(m_col); }
	private:
		WgColor m_col;
	};

	class PropColorClearer : public PropModifier
	{
	public:
		void modify( WgTextprop& prop ) const { prop.clearColor(); }
	};


	class PropStyleModifier : public PropModifier
	{
	public:
		PropStyleModifier( WgFontAlt style ) { m_style = style; }
		void modify( WgTextprop& prop ) const { prop.setStyle(m_style); }
	private:
		WgFontAlt m_style;
	};

	class PropUnderlinedModifier : public PropModifier
	{
	public:
		PropUnderlinedModifier( bool bUnderlined ) { m_bUnderlined = bUnderlined; }
		void modify( WgTextprop& prop ) const { if( m_bUnderlined) prop.setUnderlined(); else prop.clearUnderlined(); }
	private:
		bool	m_bUnderlined;
	};

	class PropBreakLevelModifier : public PropModifier
	{
	public:
		PropBreakLevelModifier( int level ) { m_level = level; }
		void modify( WgTextprop& prop ) const { prop.setBreakLevel(m_level); }
	private:
		int		m_level;
	};

	class PropLinkModifier : public PropModifier
	{
	public:
		PropLinkModifier( const WgTextLinkPtr& pLink ) { m_pLink = pLink; }
		void modify( WgTextprop& prop ) const { prop.setLink( m_pLink ); }
	private:
		WgTextLinkPtr	m_pLink;
	};

	class PropFontModifier : public PropModifier
	{
	public:
		PropFontModifier( const WgFontPtr& pFont ) { m_pFont = pFont; }
		void modify( WgTextprop& prop ) const { prop.setFont( m_pFont ); }
	private:
		WgFontPtr	m_pFont;
	};

	class PropStateSizeModifier : public PropModifier
	{
	public:
		PropStateSizeModifier( int size, WgState state ) { m_size = size; m_state = state; }
		void modify( WgTextprop& prop ) const { prop.setSize(m_size,m_state); }
	private:
		int		m_size;
		WgState	m_state;
	};

	class PropStateSizeClearer : public PropModifier
	{
	public:
		PropStateSizeClearer( WgState state ) { m_state = state; }
		void modify( WgTextprop& prop ) const { prop.clearSize(m_state); }
	private:
		WgState	m_state;
	};


	class PropStateColorModifier : public PropModifier
	{
	public:
		PropStateColorModifier( WgColor col, WgState state ) { m_col = col; m_state = state; }
		void modify( WgTextprop& prop ) const { prop.setColor(m_col,m_state); }
	private:
		WgColor m_col;
		WgState	m_state;
	};

	class PropStateColorClearer : public PropModifier
	{
	public:
		PropStateColorClearer( WgState state ) { m_state = state; }
		void modify( WgTextprop& prop ) const { prop.clearColor(m_state); }
	private:
		WgState	m_state;
	};


	class PropStateStyleModifier : public PropModifier
	{
	public:
		PropStateStyleModifier( WgFontAlt style, WgState state ) { m_style = style; m_state = state; }
		void modify( WgTextprop& prop ) const { prop.setStyle(m_style,m_state); }
	private:
		WgFontAlt m_style;
		WgState	m_state;
	};

	class PropStateUnderlinedModifier : public PropModifier
	{
	public:
		PropStateUnderlinedModifier( bool bUnderlined, WgState state ) { m_bUnderlined = bUnderlined; m_state = state; }
		void modify( WgTextprop& prop ) const { if( m_bUnderlined) prop.setUnderlined(m_state); else prop.clearUnderlined(m_state); }
	private:
		bool	m_bUnderlined;
		WgState	m_state;
	};



	//____

	static void modifyProperties( const PropModifier& modif, WgChar * pChar, Uint32 nb );

	inline static Uint8	nibbleToAscii( Uint8 nibble );
	inline static Uint8	asciiToNibble( Uint8 ascii );

	static void	refProps( WgChar * p, Uint32 n );
	static void	derefProps( WgChar * p, Uint32 n );
};

//____ readChar() _____________________________________________________________

inline Uint16 WgTextTool::readChar( const char *& pStr )
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

inline bool WgTextTool::cmpUTF8( const char * pChar1, Uint16 char2 )
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

inline int WgTextTool::sizeUTF8( Uint16 ch )
{
	if( ch < 128 )
		return 1;

	if( ch < (1<<11) )
		return 2;

	return 3;
}



typedef WgTextTool WgTextParser;

#endif //WG_TEXTTOOL_DOT_H
