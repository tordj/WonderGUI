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

#include <assert.h>
#include <memory.h>
#include <wchar.h>
#include <wctype.h>
#include <wg_codepages.h>

#include <string>

#include <wg_texttool.h>
#include <wg_font.h>
#include <wg_text.h>
#include <wg_textpropmanager.h>
#include <wg_charseq.h>
#include <wg_cursorinstance.h>
#include <wg_resdb.h>
#include <wg_base.h>
#include <wg_pen.h>

const static char itoa_table [35+36+1]= { "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" };

static char	breakRulesTab[256];


//____ parseChar() ____________________________________________________________

Uint16 WgTextTool::parseChar( const char * pChar )
{
	if( pChar[0] == 0 )
		return 0;

	if( pChar[0] == '0' && pChar[1] == 'x' )
	{
		Uint16 value = 0;
		pChar += 2;

		for( char in = * pChar++ ; in != 0 ; in = * pChar++ )
		{
			if( in >= '0' && in <= '9' )
				value = (value << 4) + (in - '0' );
			else if( in >= 'a' && in <= 'f' )
				value = (value << 4 )+ (in - 'a') + 10;
			else if( in >= 'A' && in <= 'F' )
				value = (value << 4 )+ (in - 'A') + 10;
			else
				break;
		}

		return value;
	}
	else
	{
		return readChar( pChar );
	}

}



//____ clearBreakRules() ______________________________________________________

void WgTextTool::clearBreakRules()
{
	for( int i = 0 ; i < 256 ; i++ )
		breakRulesTab[i] = WG_NO_BREAK | 0xF;
}


//____ addBreakRule() _________________________________________________________

bool WgTextTool::setBreakRule( unsigned char character, int level, WgBreakRules rule )
{
	if( level < 0 || level > 15 )
		return false;

	breakRulesTab[character] = rule | level;
	return true;
}

//____ setDefaultBreakRules() _________________________________________________

void WgTextTool::setDefaultBreakRules()
{
	clearBreakRules();

	// Set level 1 breaks

	breakRulesTab[WG_BREAK_PERMITTED] = WG_BREAK_ON | 0x1;
	breakRulesTab[WG_HYPHEN_BREAK_PERMITTED] = WG_BREAK_ON | 0x1;

	// Set level 2 breaks

	breakRulesTab[0x20] = WG_BREAK_ON | 0x2;			// Whitespace
	breakRulesTab[0x09] = WG_BREAK_ON | 0x2;			// Tab

	// Set level 3 breaks

	breakRulesTab['\\']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['/']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['-']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['+']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['*']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['~']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['=']	= WG_BREAK_AFTER | 0x3;

	breakRulesTab[')']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['}']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab[']']	= WG_BREAK_AFTER | 0x3;
	breakRulesTab['>']	= WG_BREAK_AFTER | 0x3;

	breakRulesTab['(']	= WG_BREAK_BEFORE | 0x3;
	breakRulesTab['{']	= WG_BREAK_BEFORE | 0x3;
	breakRulesTab['[']	= WG_BREAK_BEFORE | 0x3;
	breakRulesTab['<']	= WG_BREAK_BEFORE | 0x3;

	// Set level 4 breaks

	breakRulesTab[0xbf]	= WG_BREAK_BEFORE | 0x5;	// inverted question mark (beginning of question).
	breakRulesTab['?']	= WG_BREAK_AFTER | 0x5;
	breakRulesTab[':']	= WG_BREAK_AFTER | 0x4;
	breakRulesTab[';']	= WG_BREAK_AFTER | 0x4;
	breakRulesTab['.']	= WG_BREAK_AFTER | 0x5;
	breakRulesTab[',']	= WG_BREAK_AFTER | 0x5;
	breakRulesTab['!']	= WG_BREAK_AFTER | 0x5;
	breakRulesTab['_']	= WG_BREAK_AFTER | 0x4;
	breakRulesTab['|']	= WG_BREAK_AFTER | 0x4;
}


//____ isBreakAllowed() _______________________________________________________

WgBreakRules WgTextTool::isBreakAllowed( Uint16 chr, int breakLevel )
{
	if( chr > 255 )
		return WG_NO_BREAK;

	if( breakLevel >= (breakRulesTab[chr] & 0xF) )
		return (WgBreakRules) (breakRulesTab[chr] & 0xF0);

	return WG_NO_BREAK;
}


//____ itoa() _________________________________________________________________

char * WgTextTool::itoa( int value, char * str, int base )
{

    char * p1 = str+33;
    char * p2 = str;

    if( value < 0 )
        * p2++ = '-';

    while( value != 0 )
    {
        * --p1 = itoa_table[(value%base)+35];
        value /= base;
    }

    while( p1 < str+33 )
        * p2++ = * p1++;

    * p2++ = 0;

    return str;
}


//____ readString() ___________________________________________________________

Uint32 WgTextTool::readString( const char *& pSrc, Uint16 * pDst, Uint32 maxChars )
{
	if( !pSrc )
	{
	    if( maxChars > 0 )
			pDst[0] = 0;
		return 0;
	}
	Uint32 n = 0;

	while( * pSrc != 0 && n < maxChars )
		pDst[n++] = readChar(pSrc);

	if( n != maxChars )
		pDst[n] = 0;
	return n;
}


Uint32 WgTextTool::readString( const char *& pSrc, WgChar * pDst, Uint32 maxChars )
{
	if( !pSrc )
	{
	    if( maxChars > 0 )
            pDst[0].SetGlyph(0);
		return 0;
	}
	Uint32 n = 0;

	while( * pSrc != 0 && n < maxChars )
	{
		pDst[n++].SetGlyph(readChar(pSrc));
	}
	if( n != maxChars )
		pDst[n].SetGlyph(0);
	return n;
}

Uint32 WgTextTool::readString( const Uint16 *& pSrc, WgChar * pDst, Uint32 maxChars )
{
	if( !pSrc )
	{
	    if( maxChars > 0 )
            pDst[0].SetGlyph(0);
		return 0;
	}
	Uint32 n = 0;

	while( * pSrc != 0 && n < maxChars )
		pDst[n++].SetGlyph( * pSrc++);

	if( n != maxChars )
		pDst[n].SetGlyph(0);
	return n;
}


Uint32 WgTextTool::readString( const char *& pSrc, WgCodePage codepage, WgChar * pDst, Uint32 maxChars )
{
	Uint16 * pCP = WgCodePages::GetCodePage( codepage );
	if( !pCP || !pSrc )
	{
	    if( maxChars > 0 )
            pDst[0].SetGlyph(0);
		return 0;
	}

	Uint32 n = 0;
	for( unsigned char * p = (unsigned char *) pDst ; p[n] != 0 && n < maxChars ; n++ )
		pDst[n].SetGlyph( pCP[p[n]] );

	if( n != maxChars )
		pDst[n].SetGlyph(0);
	return n;
}

Uint32 WgTextTool::readString( const char *& pSrc, WgCodePage codepage, Uint16 * pDst, Uint32 maxChars )
{
	Uint16 * pCP = WgCodePages::GetCodePage( codepage );
	if( !pCP || !pSrc )
	{
	    if( maxChars > 0 )
            pDst[0] = 0;
		return 0;
	}

	Uint32 n = 0;
	for( unsigned char * p = (unsigned char *) pDst ; p[n] != 0 && n < maxChars ; n++ )
		pDst[n] = pCP[p[n]];

	if( n != maxChars )
		pDst[n] = 0;
	return n;
}


//____ CountWhitespaces() ______________________________________________________

Uint32 WgTextTool::countWhitespaces( const char * pStr, Uint32 len )
{
	if( !pStr )
		return 0;

	Uint32 n = 0;
	for( unsigned int i = 0 ; i < len && pStr[i] != 0 ; i++ )
	{
		if( pStr[i] == ' ' || pStr[i] == WG_NO_BREAK_SPACE )
			n++;
	}
	return n;
}

Uint32 WgTextTool::countWhitespaces( const Uint16 * pStr, Uint32 len  )
{
	if( !pStr )
		return 0;

	Uint32 n = 0;
	for( unsigned int i = 0 ; i < len && pStr[i] != 0 ; i++ )
	{
		if( pStr[i] == ' ' || pStr[i] == WG_NO_BREAK_SPACE )
			n++;
	}
	return n;
}

Uint32 WgTextTool::countWhitespaces( const WgChar * pStr, Uint32 len  )
{
	if( !pStr )
		return 0;

	Uint32 n = 0;
	for( unsigned int i = 0 ; i < len && !pStr[i].IsEndOfText() ; i++ )
	{
		if( pStr[i].IsWhitespace() )
			n++;
	}
	return n;
}



//____ countNonFormattingChars() ______________________________________________

Uint32 WgTextTool::countNonFormattingChars( const char * pStr, Uint32 strlen )
{
	if( !pStr )
		return 0;

	const char * pEnd = pStr + strlen;
	Uint32 n = 0;
	while( * pStr != 0 && pStr != pEnd )
	{
		Uint16 c = readChar(pStr);

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					n++;				// Newline should be counted
					break;
				case '{':				// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
			n++;
	}
	return n;
}


Uint32 WgTextTool::countNonFormattingChars( const Uint16 * pStr, Uint32 strlen )
{
	if( !pStr )
		return 0;

	const Uint16 * pEnd = pStr + strlen;
	Uint32 n = 0;
	while( * pStr != 0 && pStr != pEnd )
	{
		Uint16 c = * pStr++;

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					n++;				// Newline should be counted
					break;
				case '{':			// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
			n++;
	}
	return n;
}


//____ getTextSizeStripped() ______________________________________________

Uint32 WgTextTool::getTextSizeStripped( const char * pStr, Uint32 maxChars )
{
	Uint32 nChar = 0;

	while( * pStr != 0 && nChar < maxChars )
	{
		char c = * pStr++;

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					nChar++;				// Newline should be counted
					break;
				case '{':			// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
			nChar++;
}
	return nChar;


}

Uint32 WgTextTool::getTextSizeStripped( const Uint16 * pStr, Uint32 maxChars )
{
	Uint32 nChar = 0;

	while( * pStr != 0 && nChar < maxChars )
	{
		Uint16 c = * pStr++;

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					nChar++;				// Newline should be counted
					break;
				case '{':			// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
			nChar++;
	}
	return nChar;
}


//____ getTextSizeStrippedUTF8() ______________________________________________

Uint32 WgTextTool::getTextSizeStrippedUTF8( const char * pStr, Uint32 maxChars )
{
	Uint32 nChar = 0;
	Uint32 nBytes = 0;

	while( * pStr != 0 && nChar < maxChars )
	{
		char c = * pStr++;

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					nChar++;				// Newline should be counted
					nBytes++;
					break;
				case '{':			// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
		{
			nChar++;
			if( (c & 0x80) == 0 )
				nBytes++;
			else if( (c & 0xE0) == 0xC0 )
				nBytes += 2;
			else
				nBytes += 3;
		}
	}
	return nBytes;


}

Uint32 WgTextTool::getTextSizeStrippedUTF8( const Uint16 * pStr, Uint32 maxChars )
{
	Uint32 nChar = 0;
	Uint32 nBytes = 0;

	while( * pStr != 0 && nChar < maxChars )
	{
		Uint16 c = * pStr++;

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					nChar++;				// Newline should be counted
					nBytes++;
					break;
				case '{':			// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
		{
			nChar++;
			if( c < 128 )
				nBytes++;
			else if( c < (1<<11) )
				nBytes += 2;
			else
				nBytes += 3;
		}
	}
	return nBytes;
}


//____ readFormattedString() __________________________________________________
/**
	@brief	Reads a formatted UTF-8 string into WgChars.

	Reads a UTF-8 string equipped with escape codes for text properties into
	an array of WgChar.

	@param pSrc		Pointer at source to read from. This must be a null-terminated
					string which may include WG escape codes.
	@param pDst		Pointer at an array of WgChar where to write the WgChar converted
					string. The string will be null-terminated if the null-termination
					will fit into maxChars.
	@param maxChars	Maximum number of characters to write to the destination.

	@return			Number of WgChar written into the array, excluding null-terminator.
					This value is <= maxChars. If return value == maxChars, the
					destination is not null-terminated.
*/

Uint32 WgTextTool::readFormattedString( const char * _pSrc, WgChar * pDst, Uint32 maxChars, const WgResDB * pResDB )
{
	if( maxChars == 0 )
		return 0;

	if( !_pSrc )
	{
		pDst[0].SetGlyph(0);
		return 0;
	}

	WgChar *	pBeginUnderlined = 0;
	Uint32		nUnderlinedRecursions = 0;

	WgChar *	pBeginStyle = 0;
	WgFontStyle	styleStack[16];
	Uint32		nStyleRecursions = 0;

	WgChar *	pBeginColor = 0;
	WgColor		colorStack[16];
	Uint32		nColorRecursions = 0;

	WgChar		myChar;

	const char * pSrc = _pSrc;		// We need a pointer that can be increased.

	Uint32 n = 0;
	while( * pSrc != 0 )
	{
		Uint16 c = readChar(pSrc);

		if( c == WG_ESCAPE_CODE )
		{
			Uint8 cmd = * pSrc++;

			switch( cmd )
			{
				case 'n':					// Encoded newline
					if( n < maxChars )
					{
						myChar.SetGlyph( '\n' );
						pDst[n++] = myChar;
					}
				break;

				case '{':					// BEGIN COLOR
					if( nColorRecursions != 0 )
						WgTextTool::SetColor( colorStack[nColorRecursions-1], pBeginColor, pDst + n - pBeginColor );

					if( nColorRecursions < 16 )
					{
						pBeginColor = &pDst[n];

						WgColor col;

/*						if( pSrc[0] == '#' )
						{

						}
						else
*/						{
							col.a = WgTextTool::AsciiToUint8( &pSrc[0] );
							col.r = WgTextTool::AsciiToUint8( &pSrc[2] );
							col.g = WgTextTool::AsciiToUint8( &pSrc[4] );
							col.b = WgTextTool::AsciiToUint8( &pSrc[6] );
							pSrc += 8;
						}

						colorStack[nColorRecursions++] = col;
					}
				break;

				case '}':					// END COLOR
					if( nColorRecursions > 0 )
					{
						nColorRecursions--;
						WgTextTool::SetColor( colorStack[nColorRecursions], pBeginColor, pDst + n - pBeginColor );
						pBeginColor = &pDst[n];
					}
				break;

				case '_':					// BEGIN UNDERLINED
					if( nUnderlinedRecursions == 0 )
						pBeginUnderlined = &pDst[n];

					nUnderlinedRecursions++;
				break;

				case '|':					// END UNDERLINED
					nUnderlinedRecursions--;
					if( nUnderlinedRecursions == 0 )
						WgTextTool::SetUnderlined( pBeginUnderlined, pDst + n - pBeginUnderlined );
				break;

				case '#':					// END STYLE
					if( nStyleRecursions > 0 )
					{
						nStyleRecursions--;
						WgTextTool::SetStyle( styleStack[nStyleRecursions], pBeginStyle, pDst + n - pBeginStyle );
						pBeginStyle = &pDst[n];
					}
				break;

				case '(':					// SET PROP
				{
					if( pResDB )
					{
						// Read the prop id (as normal 7-bit ascii).

						std::string id;
						while( * pSrc != ')' )
						{
							assert( * pSrc != 0 );
							id += * pSrc++;
						}
						pSrc++;

						if( id == "null" )
							myChar.SetProperties( WgTextPropPtr() );
						else
							myChar.SetProperties( pResDB->GetTextProp(id) );
					}
				}
				break;

				default:					// Either setting a font style or this is an invalid command
				{
					bool bOk = true;
					WgFontStyle style;

					// Determine style or that this is an invalid command

					switch( cmd )
					{
						case 'd':
							style = WG_STYLE_NORMAL;
							break;
						case 'b':
							style = WG_STYLE_BOLD;
							break;
						case 'i':
							style = WG_STYLE_ITALIC;
							break;
						case 'I':
							style = WG_STYLE_BOLD_ITALIC;
							break;
						case 'S':
							style = WG_STYLE_SUPERSCRIPT;
							break;
						case 's':
							style = WG_STYLE_SUBSCRIPT;
							break;
						case 'm':
							style = WG_STYLE_MONOSPACE;
							break;
						case 'h':
						{
							Uint8 c = * pSrc++;
							if( c < '1' || c > '5' )
								bOk = false;
							else
							{
								style = (WgFontStyle) (WG_STYLE_HEADING_1 + c - '1');
								if((Uint32)style >= WG_NB_FONTSTYLES)
									bOk = false;
							}
							break;
						}
						case 'u':
						{
							Uint8 c = * pSrc++;
							if( c < '1' || c > '5' )
								bOk = false;
							else
							{
								style = (WgFontStyle) (WG_STYLE_USER_1 + c - '1');
								if((Uint32)style >= WG_NB_FONTSTYLES)
									bOk = false;
							}
							break;
						}
						default:
							bOk = false;
					}

					// Set style unless it was an invalid command

					if( bOk )
					{
						if( nStyleRecursions != 0 )
							WgTextTool::SetStyle( styleStack[nStyleRecursions-1], pBeginStyle, pDst + n - pBeginStyle );

						if( nStyleRecursions < 16 )
						{
							pBeginStyle = &pDst[n];
							styleStack[nStyleRecursions++] = style;
						}
					}
				}
				break;
			}
		}
		else						// Just a normal character, copy it.
		{
			if( n < maxChars )
			{
				myChar.SetGlyph( c );
				pDst[n++] = myChar;
			}
		}
	}

	// Terminate the string if possible.

	if( n != maxChars )
		pDst[n].SetGlyph(0);

	return n;
}

/**
	@brief	Reads a formatted Uint16 string into WgChars.

	Reads a Uint16 string equipped with escape codes for text properties into
	an array of WgChar.

	@param pSrc		Pointer at source to read from. This must be a null-terminated
					string which may include WG escape codes.
	@param pDst		Pointer at an array of WgChar where to write the WgChar converted
					string. The string will be null-terminated if the null-termination
					will fit into maxChars.
	@param maxChars	Maximum number of characters to write to the destination.

	@return			Number of WgChar written into the array, excluding null-terminator.
					This value is <= maxChars. If return value == maxChars, the
					destination is not null-terminated.
*/

Uint32 WgTextTool::readFormattedString( const Uint16 * _pSrc, WgChar * pDst, Uint32 maxChars, const WgResDB * pResDB )
{
	if( maxChars == 0 )
		return 0;

	if( !_pSrc )
	{
		pDst[0].SetGlyph(0);
		return 0;
	}

	WgChar *	pBeginUnderlined = 0;
	Uint32		nUnderlinedRecursions = 0;

	WgChar *	pBeginStyle = 0;
	WgFontStyle	styleStack[16];
	Uint32		nStyleRecursions = 0;

	WgChar *	pBeginColor = 0;
	WgColor		colorStack[16];
	Uint32		nColorRecursions = 0;

	WgChar		myChar;

	const Uint16 * pSrc = _pSrc;		// We need a pointer that can be increased.

	Uint32 n = 0;
	while( * pSrc != 0  )
	{
		Uint16 c = * pSrc++;

		if( c == WG_ESCAPE_CODE )
		{
			Uint16 cmd = * pSrc++;

			switch( cmd )
			{
				case 'n':					// Encoded newline
					if( n < maxChars )
					{
						myChar.SetGlyph( '\n' );
						pDst[n++] = myChar;
					}
				break;

				case '{':					// BEGIN COLOR
					if( nColorRecursions != 0 )
						WgTextTool::SetColor( colorStack[nColorRecursions-1], pBeginColor, pDst + n - pBeginColor );

					if( nColorRecursions < 16 )
					{
						pBeginColor = &pDst[n];

						WgColor col;
						col.a = WgTextTool::AsciiToUint8( &pSrc[0] );
						col.r = WgTextTool::AsciiToUint8( &pSrc[2] );
						col.g = WgTextTool::AsciiToUint8( &pSrc[4] );
						col.b = WgTextTool::AsciiToUint8( &pSrc[6] );
						pSrc += 8;

						colorStack[nColorRecursions++] = col;
					}
				break;

				case '}':					// END COLOR
					if( nColorRecursions > 0 )
					{
						nColorRecursions--;
						WgTextTool::SetColor( colorStack[nColorRecursions], pBeginColor, pDst + n - pBeginColor );
						pBeginColor = &pDst[n];
					}
				break;

				case '_':					// BEGIN UNDERLINED
					if( nUnderlinedRecursions == 0 )
						pBeginUnderlined = &pDst[n];

					nUnderlinedRecursions++;
				break;

				case '|':					// END UNDERLINED
					nUnderlinedRecursions--;
					if( nUnderlinedRecursions == 0 )
						WgTextTool::SetUnderlined( pBeginUnderlined, pDst + n - pBeginUnderlined );
				break;

				case '#':					// END STYLE
					if( nStyleRecursions > 0 )
					{
						nStyleRecursions--;
						WgTextTool::SetStyle( styleStack[nStyleRecursions], pBeginStyle, pDst + n - pBeginStyle );
						pBeginStyle = &pDst[n];
					}
				break;

				case '(':					// SET PROP
				{
					if( pResDB )
					{
						// Read the prop id (as normal 7-bit ascii).

						std::string id;
						while( * pSrc != ')' )
						{
							assert( * pSrc != 0 );
							id += (char) (* pSrc++);
						}
						pSrc++;

						if( id == "null" )
							myChar.SetProperties( WgTextPropPtr() );
						else
							myChar.SetProperties( pResDB->GetTextProp(id) );
					}
				}
				break;


				default:					// Either setting a font style or this is an invalid command
				{
					bool bOk = true;
					WgFontStyle style;

					// Determine style or that this is an invalid command

					switch( cmd )
					{
						case 'd':
							style = WG_STYLE_NORMAL;
							break;
						case 'b':
							style = WG_STYLE_BOLD;
							break;
						case 'i':
							style = WG_STYLE_ITALIC;
							break;
						case 'I':
							style = WG_STYLE_BOLD_ITALIC;
							break;
						case 'S':
							style = WG_STYLE_SUPERSCRIPT;
							break;
						case 's':
							style = WG_STYLE_SUBSCRIPT;
							break;
						case 'm':
							style = WG_STYLE_MONOSPACE;
							break;
						case 'h':
						{
							Uint16 c = * pSrc++;
							if( c < '1' || c > '5' )
								bOk = false;
							else
								style = (WgFontStyle) (WG_STYLE_HEADING_1 + c - '1');
							break;
						}
						case 'u':
						{
							Uint16 c = * pSrc++;
							if( c < '1' || c > '5' )
								bOk = false;
							else
								style = (WgFontStyle) (WG_STYLE_USER_1 + c - '1');
							break;
						}
						default:
							bOk = false;
					}

					// Set style unless it was an invalid command

					if( bOk )
					{
						if( nStyleRecursions != 0 )
							WgTextTool::SetStyle( styleStack[nStyleRecursions-1], pBeginStyle, pDst + n - pBeginStyle );

						if( nStyleRecursions < 16 )
						{
							pBeginStyle = &pDst[n];
							styleStack[nStyleRecursions++] = style;
						}
					}
				}
				break;
			}
		}
		else								// Just a normal character, copy it.
		{
			if( n < maxChars )
			{
				myChar.SetGlyph( c );
				pDst[n++] = myChar;
			}
		}
	}

	// Terminate the string if possible.

	if( n != maxChars )
		pDst[n].SetGlyph(0);

	return n;
}





//____ getTextFormattedUTF8() __________________________________________________

Uint32 WgTextTool::getTextFormattedUTF8( const WgChar * pSrc, char * pDst, Uint32 maxBytes, const WgResDB * pResDB )
{
	Uint32	ofs			= 0;
	Uint16	hActiveProp = 0;

	TextPropEncoder	enc(pResDB);

	Uint32 n = enc.BeginString();
	assert( n == 0 );						// If this has changed we need to add some code here...

	while( !pSrc->IsEndOfText() )
	{

		if( hActiveProp != pSrc->PropHandle() )
		{
			// Handle changed properties

			n = enc.SetProp( pSrc->Properties() );

			if( n > maxBytes - ofs )
				break;								// Can't fit this encoding in, just quit.

			strcpy( pDst+ofs, enc.GetCodes() );
			ofs += n;

			hActiveProp = pSrc->PropHandle();
		}


		// Copy the glyph.

		Uint16 glyph = pSrc->Glyph();

		if( glyph < 128 )
		{
			if( ofs >= maxBytes )
				break;					// Can't fit character, so we just terminate.

			pDst[ofs++] = (char) glyph;
		}
		else if( glyph < 0x800 )
		{
			if( ofs+1 >= maxBytes )
				break;					// Can't fit character, so we just terminate.

			pDst[ofs++] =  0xC0 | ((char) ((glyph & 0x07C0) >> 6));
			pDst[ofs++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		}
		else
		{
			if( ofs+2 > maxBytes )
				break;					// Can't fit character, so we just terminate.

			pDst[ofs++] =  0xE0 | ((char) ((glyph & 0xF000) >> 12));
			pDst[ofs++] =  0x80 | ((char) ((glyph & 0x0FC0) >> 6));
			pDst[ofs++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		}

		pSrc++;
	}

	// Finalize string by adding any last needed encoding.

	n = enc.EndString();
	if( n <= maxBytes - ofs )
	{
		strcpy( pDst+ofs, enc.GetCodes() );
		ofs += n;
	}

	//

	if( ofs != maxBytes )
		pDst[ofs] = 0;

	return ofs;
}

//____ getTextFormatted() _____________________________________________________

Uint32 WgTextTool::getTextFormatted( const WgChar * pSrc, Uint16 * pDst, Uint32 maxBytes, const WgResDB * pResDB )
{
	Uint32	ofs			= 0;
	Uint16	hActiveProp = 0;

	TextPropEncoder	enc(pResDB);
	Uint32 n = enc.BeginString();
	assert( n == 0 );						// If this has changed we need to add some code here...

	while( !pSrc->IsEndOfText() )
	{
		if( hActiveProp != pSrc->PropHandle() )
		{
			// Handle changed properties

			n = enc.SetProp( pSrc->Properties() );

			if( n > maxBytes - ofs )
				break;								// Can't fit this encoding in, just quit.

			const char * p = enc.GetCodes();
			for( unsigned int i = 0 ; i < n ; i++ )
				pDst[ofs++] = * p++;

			hActiveProp = pSrc->PropHandle();
		}

		// Copy the glyph.

		if( ofs >= maxBytes )
			break;					// Can't fit character, so we just terminate.

		pDst[ofs++] = pSrc->Glyph();

		pSrc++;
	}

	// Finalize string by adding any last needed encoding.

	n = enc.EndString();
	if( n <= maxBytes - ofs )
	{
		const char * p = enc.GetCodes();
		for( unsigned int i = 0 ; i < n ; i++ )
			pDst[ofs++] = * p++;
	}

	//

	if( ofs != maxBytes )
		pDst[ofs] = 0;

	return ofs;
}

//____ getTextSizeFormattedUTF8() _____________________________________________

Uint32 WgTextTool::getTextSizeFormattedUTF8( const WgChar * pSrc, Uint32 maxChars, const WgResDB * pResDB )
{
	Uint32 ofs = 0;
	Uint32 charsRead = 0;

	WgTextPropPtr	pActiveProp;
	Uint16			hActiveProp = 0;

	TextPropEncoder	enc(pResDB);
	ofs += enc.BeginString();


	while( charsRead < maxChars && !pSrc->IsEndOfText() )
	{
		// Handle any changes to properties

		if( hActiveProp != pSrc->PropHandle() )
		{
			ofs += enc.SetProp( pSrc->Properties() );
			hActiveProp = pSrc->PropHandle();
		}

		// Add the glyph

		Uint16 glyph = pSrc->Glyph();

		if( glyph < 128 )
			ofs++;
		else if( glyph < 0x800 )
			ofs+=2;
		else
			ofs+=3;

		pSrc++;
		charsRead++;
	}

	//

	ofs += enc.EndString();


	return ofs;
}


//____ getTextSizeFormatted() _________________________________________________

Uint32 WgTextTool::getTextSizeFormatted( const WgChar * pSrc, Uint32 maxChars, const WgResDB * pResDB )
{
	Uint32 ofs = 0;
	Uint32 charsRead = 0;

	WgTextPropPtr	pActiveProp;
	Uint16			hActiveProp = 0;

	TextPropEncoder	enc(pResDB);
	ofs += enc.BeginString();


	while( charsRead < maxChars && !pSrc->IsEndOfText() )
	{
		// Handle any changes to properties

		if( hActiveProp != pSrc->PropHandle() )
		{
			ofs += enc.SetProp( pSrc->Properties() );
			hActiveProp = pSrc->PropHandle();
		}

		// Add the glyph

		ofs++;

		pSrc++;
		charsRead++;
	}

	//

	ofs += enc.EndString();

	return ofs;
}


//____ CopyChars() ____________________________________________________________

Uint32 WgTextTool::CopyChars( const WgChar * pSrc, WgChar * pDst, Uint32 maxChars )
{
	// Special RefProps() equivalent which also counts characters and stops at NULL.

	Uint16	hProp = 0;
	Uint16	nProp = 0;

	unsigned int n = 0;					// Also number of characters after the for-loop...
	for( ; n < maxChars ; n++ )
	{

		Uint16 h = pSrc[n].PropHandle();
		if( h == hProp )
			nProp++;
		else
		{
			if( hProp )
				WgTextPropManager::IncRef(hProp, nProp );

			hProp = h;
			nProp = 1;
		}
		if( pSrc[n].IsEndOfText() )
		{
			n++;
			break;
		}
	}

	if( hProp )
		WgTextPropManager::IncRef(hProp, nProp );

	// Dereference the same amount of characters from destination.

	DerefProps( pDst, n );

	// Copy chars in a fast and straight way...

	memcpy( pDst, pSrc, n*sizeof(WgChar) );

	return n;
}



//____ DerefProps() ____________________________________________________________

void WgTextTool::DerefProps( WgChar * p, Uint32 n )
{
	Uint16	hProp = 0;
	Uint16	nProp = 0;

	for( int i = 0 ; i < (int) n ; i++ )
	{

		Uint16 h = p[i].PropHandle();
		if( h == hProp )
			nProp++;
		else
		{
			if( hProp )
				WgTextPropManager::DecRef(hProp, nProp );

			hProp = h;
			nProp = 1;
		}
	}

	if( hProp )
		WgTextPropManager::DecRef(hProp, nProp );
}


//____ RefProps() ______________________________________________________________

void WgTextTool::RefProps( WgChar * p, Uint32 n )
{
	Uint16	hProp = 0;
	Uint16	nProp = 0;

	for( unsigned int i = 0 ; i < n ; i++ )
	{

		Uint16 h = p[i].PropHandle();
		if( h == hProp )
			nProp++;
		else
		{
			if( hProp )
				WgTextPropManager::IncRef(hProp, nProp );

			hProp = h;
			nProp = 1;
		}
	}

	if( hProp )
		WgTextPropManager::IncRef(hProp, nProp );
}


//____ countChars() ___________________________________________________________

Uint32	WgTextTool::countChars( const char * pStr, Uint32 strlen )
{
	if( !pStr )
		return 0;

	Uint32 n = 0;
	for( const char * p = pStr ; (Uint32) (p - pStr) < strlen ; n++ )
		if( readChar(p) == 0 )
			break;

	return n;
}


//____ countLines() ___________________________________________________________

Uint32	WgTextTool::countLines( const char * pStr )
{
	if( !pStr )
		return 0;

	Uint32 n = 1;
	while( * pStr != 0 )
	{
		if( * pStr++ == '\n' )
			n++;
	}

	return n;
}

Uint32	WgTextTool::countLines( const Uint16 * pStr )
{
	if( !pStr )
		return 0;

	Uint32 n = 1;
	while( * pStr != 0 )
	{
		if( * pStr++ == '\n' )
			n++;
	}

	return n;
}

Uint32	WgTextTool::countLines( const WgChar * pStr )
{
	if( !pStr )
		return 0;

	Uint32 n = 1;
	while( !pStr->IsEndOfText() )
	{
		if( pStr->IsEndOfLine() )
			n++;
		pStr++;
	}

	return n;
}



//____ countLineChars() _______________________________________________________
/**
	Count characters up until (but not including) the end of the string
	or the first line termination character (CR or LF) found.
*/
Uint32	WgTextTool::countLineChars( const char * pStr, Uint32 len )
{
	if( !pStr || len == 0 )
		return 0;

	Uint32 n = 0;
	const char * pEnd = pStr + len;

	Uint16 ch = readChar(pStr);
	while( ch != 0 && ch != 10 && ch != 13 && pStr < pEnd )
	{
		n++;
		ch = readChar(pStr);
	}
	return n;
}

//____ countLineChars() _______________________________________________________
/**
	Count characters up until (but not including) the end of the string
	or the first line termination character (CR or LF) found.
*/
Uint32	WgTextTool::countLineChars( const Uint16 * pStr, Uint32 len )
{
	if( !pStr || len == 0 )
		return 0;

	Uint32 n = 0;

	Uint16 ch = * pStr++;
	while( ch != 0 && ch != 10 && ch != 13 && n < len )
	{
		n++;
		ch = * pStr++;
	}
	return n;
}

//____ countLineChars() _______________________________________________________
/**
	Count characters up until (but not including) the end of the string
	or the first line termination character (CR or LF) found.
*/
Uint32	WgTextTool::countLineChars( const WgChar * pStr, Uint32 len )
{
	if( !pStr || len == 0 )
		return 0;

	Uint32 n = 0;

	while( !pStr->IsEndOfLine() && n < len )
		n++;

	return n;
}


//____ countCharsLines() ______________________________________________________

void	WgTextTool::countCharsLines( const char * pStr, Uint32& nChars, Uint32& nLines, Uint32 strlen )
{
	if( !pStr )
		return;

	const char * pEnd = pStr + strlen;

	nChars = 0;
	nLines = 1;

	while( pStr != pEnd )
	{
		Uint16 c = readChar(pStr);
		if( c == 0 )
			break;

		if( c == (Uint16) '\n' )
			nLines++;

		nChars++;
	}
}

void	WgTextTool::countCharsLines( const Uint16 * pStr, Uint32& nChars, Uint32& nLines, Uint32 strlen )
{
	if( !pStr )
		return;

	const Uint16 * pEnd = pStr + strlen;

	nChars = 0;
	nLines = 1;

	while( pStr != pEnd )
	{
		Uint16 c = * pStr++;
		if( c == 0 )
			break;

		if( c == (Uint16) '\n' )
			nLines++;

		nChars++;
	}
}


void	WgTextTool::countCharsLines( const WgChar * pStr, Uint32& nChars, Uint32& nLines, Uint32 strlen )
{
	if( !pStr )
		return;

	const WgChar * pEnd = pStr + strlen;

	nChars = 0;
	nLines = 1;

	while( pStr + nChars != pEnd )
	{
		if( pStr[nChars].IsEndOfText() )
			break;

		if( pStr[nChars].IsEndOfLine() )
			nLines++;

		nChars++;
	}
}


//____ nextLine() _____________________________________________________________

char * WgTextTool::nextLine( char * p )
{
	while( * p != 10 )
	{
		if( * p == 0 )
			return p;
		p++;
	}
	p++;

	if( * p == 13 )
		p++;

	return p;
}

//____ writeUTF8() ____________________________________________________________

Uint32 WgTextTool::writeUTF8( Uint16 glyph, char * pDest )
{
	if( glyph < 128 )
	{
		pDest[0] = (char) glyph;
		return 1;
	}
	else if( glyph < 0x800 )
	{
		pDest[0] =  0xC0 | ((char) ((glyph & 0x07C0) >> 6));
		pDest[1] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		return 2;
	}
	else
	{
		pDest[0] =  0xE0 | ((char) ((glyph & 0xF000) >> 12));
		pDest[1] =  0x80 | ((char) ((glyph & 0x0FC0) >> 6));
		pDest[2] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		return 3;
	}
}


//____ strlen() ____________________________________________________________
Uint32 WgTextTool::strlen( const Uint16 * pSrc )
{
	Uint32 nChars = 0;
	while( * pSrc != 0 )
	{
		++nChars;
		++pSrc;
	}
	return nChars;
}


//____ strlen() ____________________________________________________________
Uint32 WgTextTool::strlen( const WgChar * pSrc )
{
	Uint32 nChars = 0;
	while( !pSrc->IsEndOfText() )
	{
		++nChars;
		++pSrc;
	}
	return nChars;
}

//____ strcmp() ____________________________________________________________
int WgTextTool::strcmp( const Uint16* pStr1, const Uint16* pStr2 )
{
	while( *pStr1 != L'\0' && *pStr1 == *pStr2 )
	{
		pStr1++;
		pStr2++;
	}
	return *pStr1 - *pStr2;
}

//____ strcmp() ____________________________________________________________
int WgTextTool::strcmp( const WgChar * pStr1, const WgChar * pStr2 )
{
	while( !pStr1->IsEndOfText() && pStr1->Equals(*pStr2) )
	{
		pStr1++;
		pStr2++;
	}

	if( pStr1->IsEndOfText() && pStr2->IsEndOfText() )
		return 0;

	return pStr1->all - pStr2->all;
}


//____ glyphcmp() ____________________________________________________________
int WgTextTool::glyphcmp( const WgChar * pStr1, const WgChar * pStr2 )
{
	while( !pStr1->IsEndOfText() && pStr1->glyph == pStr2->glyph )
	{
		pStr1++;
		pStr2++;
	}

	if( pStr1->IsEndOfText() && pStr2->IsEndOfText() )
		return 0;

	return pStr1->glyph - pStr2->glyph;
}

//____ glyphcmpIgnoreCase() _______________________________________________________
int WgTextTool::glyphcmpIgnoreCase( const WgChar * pStr1, const WgChar * pStr2 )
{
	while( !pStr1->IsEndOfText() && towlower(pStr1->glyph) == towlower(pStr2->glyph) )
	{
		pStr1++;
		pStr2++;
	}

	if( pStr1->IsEndOfText() && pStr2->IsEndOfText() )
		return 0;

	return towlower(pStr1->glyph) - towlower(pStr2->glyph);
}



//____ NibbleToAscii() ____________________________________________________________
inline Uint8 WgTextTool::NibbleToAscii( Uint8 nibble )
{
	if( nibble <= 9 )
		return '0' + nibble;
	else
		return 'A' + ( nibble - 0x0A );
}

//____ AsciiToNibble() ____________________________________________________________
inline Uint8 WgTextTool::AsciiToNibble( Uint8 ascii )
{
	if( ascii >= '0' && ascii <= '9' )
		return 0x00 + ascii - '0';
	else if( ascii >= 'A' && ascii <= 'F' )
		return 0x0A + (ascii - 'A');
	else if( ascii >= 'a' && ascii <= 'f' )
		return 0x0A + (ascii - 'a');

	return 0;
}

//____ Uint16ToAscii() ____________________________________________________________
bool WgTextTool::Uint16ToAscii( Uint16 value, Uint16 * pDest, Uint32 maxChars )
{
	if( 0 == pDest || maxChars < 4 )
		return false;

	Uint8ToAscii( value >> 8, (Uint16*)&pDest[0], 2 );
	Uint8ToAscii( value & 0xFF, (Uint16*)&pDest[2], 2 );

	return true;
}

//____ AsciiToUint16() ____________________________________________________________
Uint16 WgTextTool::AsciiToUint16( const Uint16 * pAscii )
{
	Uint16 high = AsciiToUint8( (Uint16*)&pAscii[0] );
	Uint16 low = AsciiToUint8( (Uint16*)&pAscii[2] );

	Uint16 value = high << 8 | low;

	return value;
}

//____ Uint16ToAscii() ____________________________________________________________
bool WgTextTool::Uint16ToAscii( Uint16 value, char * pDest, Uint32 maxChars )
{
	if( 0 == pDest || maxChars < 4 )
		return false;

	Uint8ToAscii( value >> 8, (char*)&pDest[0], 2 );
	Uint8ToAscii( value & 0xFF, (char*)&pDest[2], 2 );

	return true;
}

//____ AsciiToUint16() ____________________________________________________________
Uint16 WgTextTool::AsciiToUint16( const char * pAscii )
{
	Uint16 high = AsciiToUint8( (char*)&pAscii[0] );
	Uint16 low = AsciiToUint8( (char*)&pAscii[2] );

	Uint16 value = high << 8 | low;

	return value;
}

//____ Uint8ToAscii( Uint16* ) ____________________________________________________________
bool WgTextTool::Uint8ToAscii( Uint8 value, Uint16 * pDest, Uint32 maxChars )
{
	if( 0 == pDest || maxChars < 2 )
		return false;

	Uint8 high = NibbleToAscii( value >> 4 );
	Uint8 low = NibbleToAscii( value & 0x0F );

	pDest[ 0 ] = high;
	pDest[ 1 ] = low;

	return true;
}

//____ AsciiToUint8() ____________________________________________________________
Uint8 WgTextTool::AsciiToUint8( const Uint16 * pAscii )
{
	Uint8 high = AsciiToNibble( (Uint8)pAscii[ 0 ] );
	Uint8 low = AsciiToNibble( (Uint8)pAscii[ 1 ] );

	Uint8 value = high << 4 | low;

	return value;
}

//____ Uint8ToAscii( char* ) ____________________________________________________________
bool WgTextTool::Uint8ToAscii( Uint8 value, char * pDest, Uint32 maxChars )
{
	if( 0 == pDest || maxChars < 2 )
		return false;

	Uint8 high = NibbleToAscii( value >> 4 );
	Uint8 low = NibbleToAscii( value & 0x0F );

	pDest[ 0 ] = high;
	pDest[ 1 ] = low;

	return true;
}

//____ AsciiToUint8() ____________________________________________________________
Uint8 WgTextTool::AsciiToUint8( const char * pAscii )
{
	Uint8 high = AsciiToNibble( pAscii[ 0 ] );
	Uint8 low = AsciiToNibble( pAscii[ 1 ] );

	Uint8 value = high << 4 | low;

	return value;
}

//____ Uint16ToUTF8() ____________________________________________________________
Uint32 WgTextTool::Uint16ToUTF8( Uint16 value, char * pDest, Uint32 maxChars )
{
	if( maxChars < 1 )
		return 0;

	unsigned int nChars = 0;

	Uint16 glyph = value;
	if( glyph < 128 )
	{
		pDest[nChars++] = (char) glyph;
		if( nChars == maxChars )
			return nChars;
	}
	else if( glyph < 0x800 )
	{
		pDest[nChars++] =  0xC0 | ((char) ((glyph & 0x07C0) >> 6));
		if( nChars == maxChars )
			return nChars;

		pDest[nChars++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		if( nChars == maxChars )
			return nChars;
	}
	else
	{
		pDest[nChars++] =  0xE0 | ((char) ((glyph & 0xF000) >> 12));
		if( nChars == maxChars )
			return nChars;

		pDest[nChars++] =  0x80 | ((char) ((glyph & 0x0FC0) >> 6));
		if( nChars == maxChars )
			return nChars;

		pDest[nChars++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		if( nChars == maxChars )
			return nChars;
	}

	return nChars;
}

//____ formatBeginColor() _____________________________________________________

Uint32 WgTextTool::formatBeginColor( const WgColor& color, char * pDest )
{
	pDest += writeUTF8( WG_ESCAPE_CODE, pDest );
	* pDest++ = '{';

	Uint8ToAscii( color.a, pDest, 2 );
	Uint8ToAscii( color.r, pDest+2, 2 );
	Uint8ToAscii( color.g, pDest+4, 2 );
	Uint8ToAscii( color.b, pDest+6, 2 );

	return sizeUTF8( WG_ESCAPE_CODE ) + 9;
}


//____ getTextUTF8() __________________________________________________________

/**
	Writes a UTF8 representation of the WgChar-string (or first part thereof) into
	the specified area. WgChar effects such as underline, font changes etc are
	ignored.


	@param	maxBytes		Maximum number of bytes for the line, including
							termination character.



	@returns		Length of UTF8-string written to the area (excluding
					termination character). No incomplete multibyte characters
					are written, so number of bytes written can be less
					than maxChars-1 even when whole string didn't fit.

*/

Uint32 WgTextTool::getTextUTF8( const WgChar * pSrc, char * pDest, Uint32 maxBytes )
{
	if( maxBytes < 1 || pSrc == 0 || pDest == 0 )
		return 0;

	Uint32 nChars = 0;
	Uint16 glyph;

	while( (glyph = pSrc->Glyph()) != 0 )
	{
		if( glyph < 128 )
		{
			if( nChars +2 > maxBytes )
				break;					// Can't fit character + termination, so we just terminate.

			pDest[nChars++] = (char) glyph;
		}
		else if( glyph < 0x800 )
		{
			if( nChars +3 > maxBytes )
				break;					// Can't fit character + termination, so we just terminate.

			pDest[nChars++] =  0xC0 | ((char) ((glyph & 0x07C0) >> 6));
			pDest[nChars++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		}
		else
		{
			if( nChars +4 > maxBytes )
				break;					// Can't fit character + termination, so we just terminate.

			pDest[nChars++] =  0xE0 | ((char) ((glyph & 0xF000) >> 12));
			pDest[nChars++] =  0x80 | ((char) ((glyph & 0x0FC0) >> 6));
			pDest[nChars++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		}

		pSrc++;
	}

	pDest[nChars] = 0;
	return nChars;
}

Uint32 WgTextTool::getTextUTF8( const Uint16 * pSrc, char * pDest, Uint32 maxBytes )
{
	if( maxBytes < 1 || pSrc == 0 || pDest == 0 )
		return 0;

	Uint32 nChars = 0;
	Uint16 glyph;

	while( (glyph = * pSrc) != 0 )
	{
		if( glyph < 128 )
		{
			if( nChars +2 > maxBytes )
				break;					// Can't fit character + termination, so we just terminate.

			pDest[nChars++] = (char) glyph;
		}
		else if( glyph < 0x800 )
		{
			if( nChars +3 > maxBytes )
				break;					// Can't fit character + termination, so we just terminate.

			pDest[nChars++] =  0xC0 | ((char) ((glyph & 0x07C0) >> 6));
			pDest[nChars++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		}
		else
		{
			if( nChars +4 > maxBytes )
				break;					// Can't fit character + termination, so we just terminate.

			pDest[nChars++] =  0xE0 | ((char) ((glyph & 0xF000) >> 12));
			pDest[nChars++] =  0x80 | ((char) ((glyph & 0x0FC0) >> 6));
			pDest[nChars++] =  0x80 | ((char) ((glyph & 0x003F) >> 0));
		}

		pSrc++;
	}

	pDest[nChars] = 0;
	return nChars;
}

//____ getTextUTF8() __________________________________________________________

Uint32 WgTextTool::getTextUTF8( const char * pSrc, WgCodePage codepage, char * pDest, int maxChars )
{
	Uint16 * pCP = WgCodePages::GetCodePage( codepage );
	if( !pCP )
	{
		pDest[0] = 0;
		return 0;
	}

	unsigned char * p = (unsigned char *) pSrc;

	int nBytes = 0;
	for( int i = 0 ; i < maxChars && p[i] != 0 ; i++ )
		nBytes += writeUTF8( pCP[p[i]], pDest + nBytes );
	
	pDest[nBytes] = 0;					// Yes, we do terminate destination!
	return nBytes;
}



//____ getTextSizeUTF8() ______________________________________________________

Uint32 WgTextTool::getTextSizeUTF8( const WgChar* pSrc, Uint32 len )
{
	Uint32 size = 0;
	Uint16 glyph = 0;
	for( Uint32 i = 0 ; i < len && (0 != ( glyph = pSrc->Glyph()) ) ; i++ )
	{
		size++;
		if( glyph > 127 )
		{
			size++;
			if( glyph > 0x7FF )
				size++;
		}
		pSrc++;
	}

	return size;
}

//____ getTextSizeUTF8() ____________________________________________________________

Uint32 WgTextTool::getTextSizeUTF8( const Uint16* pSrc, Uint32 len )
{
	Uint32 size = 0;
	Uint16 glyph = 0;
	for( Uint32 i = 0 ; i < len && (0 != ( glyph = * pSrc) ) ; i++ )
	{
		size++;
		if( glyph > 127 )
		{
			size++;
			if( glyph > 0x7FF )
				size++;
		}
		pSrc++;
	}

	return size;
}

//____ getTextSizeUTF8() ____________________________________________________________

Uint32 WgTextTool::getTextSizeUTF8( const char * pSrc, WgCodePage codepage, int maxChars )
{
	Uint16 * pCP = WgCodePages::GetCodePage( codepage );
	if( !pCP )
		return 0;

	unsigned char * p = (unsigned char *) pSrc;

	int nBytes = 0;
	for( int i = 0 ; i < maxChars && p[i] != 0 ; i++ )
		nBytes += sizeUTF8( p[i] );

	return nBytes;
}



//____ textWidth() ____________________________________________________________
Uint32 WgTextTool::textWidth( const WgText& kTextObj )
{
	Uint32 nLines	= kTextObj.nbLines();
	Uint32 maxWidth = 0;

	for( Uint32 i = 0 ; i < nLines ; i++ )
	{
		Uint32 w = lineWidth( kTextObj.getNode(), kTextObj.getProperties(), kTextObj.mode(), kTextObj.getLineText( i ) );

		if( w > maxWidth )
			maxWidth = w;
	}
	return maxWidth;
}


//____ lineWidth() ____________________________________________________________

Uint32 WgTextTool::lineWidth( WgTextNode * pNode, const WgTextPropPtr& pProp, const char * pString )
{
	WgPen pen;
	pen.SetTextNode( pNode );
	pen.SetTextProp( pProp );

	while( * pString != 0 && * pString != '\n' )
	{
		pen.SetChar( * pString++ );
		pen.ApplyKerning();
		pen.AdvancePos();
	}

	// We include the terminator in case it is set to be visible.

	pen.SetChar( * pString );
	pen.ApplyKerning();
	pen.AdvancePos();

	return pen.GetPosX();
}

Uint32 WgTextTool::lineWidth( WgTextNode * pNode, const WgTextPropPtr& pProp, const Uint16 * pString )
{
	WgPen pen;
	pen.SetTextNode( pNode );
	pen.SetTextProp( pProp );

	while( * pString != 0 && * pString != '\n' )
	{
		pen.SetChar( * pString++ );
		pen.ApplyKerning();
		pen.AdvancePos();
	}

	// We include the terminator in case it is set to be visible.

	pen.SetChar( * pString );
	pen.ApplyKerning();
	pen.AdvancePos();

	return pen.GetPosX();
}


Uint32 WgTextTool::lineWidth( WgTextNode * pNode, const WgTextPropPtr& pDefProp, WgMode mode, const WgChar * pString )
{
	WgPen pen;
	Uint16 hProp = pString->PropHandle();

	pen.SetTextNode( pNode );
	pen.SetTextProp( pDefProp.GetHandle(), hProp, mode );

	while( !pString->IsEndOfLine() )
	{
		if( pString->PropHandle() != hProp )
			pen.SetTextProp( pDefProp.GetHandle(), hProp, mode );

		pen.SetChar( pString->Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
		pString++;
	}

	// We include the terminator in case it is set to be visible.

	pen.SetChar( pString->Glyph() );
	pen.ApplyKerning();
	pen.AdvancePos();

	return pen.GetPosX();
}

//____ lineWidthPart() ________________________________________________________

Uint32 WgTextTool::lineWidthPart( WgTextNode * pNode, const WgTextPropPtr& pDefProp, WgMode mode, const WgChar * pString, int nCol )
{
	WgPen pen;
	Uint16 hProp = pString->PropHandle();

	pen.SetTextNode( pNode );
	pen.SetTextProp( pDefProp.GetHandle(), hProp, mode );

	for( int i = 0 ; i < nCol ; i++ )
	{
		if( pString->IsEndOfLine() )
			break;

		if( pString->PropHandle() != hProp )
			pen.SetTextProp( pDefProp.GetHandle(), hProp, mode );

		pen.SetChar( pString->Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
		pString++;
	}

	return pen.GetPosX();
}

//____ forwardCharacters() ____________________________________________________

void WgTextTool::forwardCharacters( const char *& pChar, Uint32 nChars )
{
	for( unsigned int i = 0 ; i < nChars && * pChar != 0 ; i++ )
	{
		char c = * pChar++;

		if( (c & 0xC0) == 0xC0 )			// Beginning of multibyte character
		{
			pChar++;						// skip one byte

			if( (c & 0xE0) == 0xE0 )		// Beginning of 3+ byte character?
			{
				pChar++;					// skip one byte

				if( (c & 0xF0) == 0xF0 )	// Beginning of 4 byte character? (not really supported, but we don't want a crash)
					pChar++;
			}
		}
	}

}

//____ forwardEscapedCharacters() ____________________________________________________

void WgTextTool::forwardEscapedCharacters( const char *& pStr, Uint32 nChars )
{
	Uint32 n = 0;
	while( * pStr != 0 && n < nChars )
	{
		Uint16 c = readChar(pStr);

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					n++;				// Newline should be counted
					break;
				case '{':				// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
			n++;
	}
}


//____ forwardEscapedCharacters() ____________________________________________________

void WgTextTool::forwardEscapedCharacters( const Uint16 *& pStr, Uint32 nChars )
{
	Uint32 n = 0;
	while( * pStr != 0 && n < nChars )
	{
		Uint16 c = * pStr++;

		if( c == WG_ESCAPE_CODE )
		{
			switch( * pStr )
			{
				case 'n':				// Newline
					pStr++;
					n++;				// Newline should be counted
					break;
				case '{':				// Begin color
					pStr += 7;
				break;

				case 'h':			// Start heading
				case 'u':			// Start user style
					pStr += 2;
				break;

				case '(':			// Set textprop
					while( * pStr != 0 && * pStr != ')' )
						pStr++;
					if( pStr != 0 )
						pStr++;
				break;

				default:
					pStr++;
			}
		}
		else
			n++;
	}
}




//____ forwardColumns() _______________________________________________________

void  WgTextTool::forwardColumns( const WgChar *& pPos, Uint32 nColumns )
{
	Uint32 len = 0;

	while( pPos->Glyph() != 0 && len < nColumns  )
	{
		len++;
		pPos++;
	}
}


//____ forwardPixels() ________________________________________________________
/**
	Forwards pPos the number of characters needed to as closely as possible
	reach (but not pass) the specified number of pixels.

	@returns	The exact number of pixels pPos was moved, which can and typically
				will be less than the amount requested.

*/


int  WgTextTool::forwardPixels( WgTextNode * pNode, const WgTextPropPtr& pDefProp, WgMode mode, const WgChar *& pPos, Uint32 nPixels )
{
	WgPen	pen;

	Uint16	hProp = 0xFFFF;
	int		length = 0;

	pen.SetTextNode( pNode );

	while( !pPos->IsEndOfLine() )
	{
		if( hProp != pPos->PropHandle() )
		{
			hProp = pPos->PropHandle();
			pen.SetTextProp( pDefProp.GetHandle(), hProp, mode );
		}

		pen.SetChar( pPos->Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();

		if( (Uint32) pen.GetPosX() > nPixels )
			return length;

		length = pen.GetPosX();
		pPos++;
	}

	pen.SetChar( pPos->Glyph() );	// End the line to get correct length. Pointer at EOL.
	return pen.GetPosX();
}

//____ ofsX2column() ___________________________________________________________
/**
	Finds the character column in a line of text whose leftmost pixel is closest
	to the specified X-offset. Optionally returns a remainder as well (which might
	be negative).

	This method is mainly made for translating mouse positions to text columns
	when re-positioning cursor or selecting text sections. Therefore we want to
	find what gap between two columns is being pointed at, not a specific column.
*/

Uint32	WgTextTool::ofsX2column( WgTextNode * pNode, const WgTextPropPtr& pDefProp, WgMode mode, int ofsX, const WgChar * pString, WgCursorInstance * pCursor, int * wpRemainder )
{
	if(!pString)
		return 0;
	const WgChar * pPos = pString;


	WgPen	pen;
	int		begX = 0;							// Beginning cordinate of character
	int		endX = 0;							// End cordinate of character
	Uint16	hProp = 0xFFFF;

	int		cursColumn = -1;
	if( pCursor )
		cursColumn = pCursor->column();

	pen.SetTextNode( pNode );

	// Find beginning and end cordinates of character that ofs is within.

	while( !pPos->IsEndOfLine() )
	{
		if( pPos->PropHandle()!= hProp )
		{
			hProp = pPos->PropHandle();
			pen.SetTextProp( pDefProp.GetHandle(), hProp, mode );
		}

		if( cursColumn == 0 )
			pen.AdvancePosCursor( * pCursor );
		cursColumn--;

		pen.SetChar( pPos->Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
		endX = pen.GetPosX();

		if( endX > ofsX )
			break;

		begX = endX;
		pPos++;
	}

	// Choose column depending on what half of the character ofs is within
	// and calculate remainder accordingly.

	int remainder, column;

	if( pPos->IsEndOfLine() || ( (ofsX - begX) < (endX - begX)/2 ) )
	{
		remainder = ofsX - begX;
		column = pPos - pString;
	}
	else
	{
		remainder = ofsX - endX;
		column = pPos + 1 - pString;
	}

	// Return values.

	if( wpRemainder )
		* wpRemainder = remainder;

	return column;
}


//____ stripTextCommands() ____________________________________________________

int WgTextTool::stripTextCommands( const Uint16* pSrc, Uint16* pDest, int maxChars )
{
	int n = 0;
	while( n < maxChars )
	{
		Uint16 c = * pSrc++;

		if( c == 0 )
		{
			pDest[n] = 0;
			break;
		}

		if( c == WG_ESCAPE_CODE )
		{
			if( * pSrc == '(' )
			{
				while( * pSrc != ')' )
					pSrc++;
				pSrc++;
			}
			else if( * pSrc == 'h' || * pSrc == 'u' )
				pSrc += 2;
			else if( * pSrc == '{' )
				pSrc += 9;
			else
				pSrc++;
		}
		else
			pDest[n++] = c;
	}

	return n;
}


int WgTextTool::stripTextCommands( const char* pSrc, char* pDest, int maxChars )
{
	int n = 0;
	while( n < maxChars )
	{
		Uint16 c = readChar( pSrc );

		if( c == 0 )
		{
			pDest[n] = 0;
			break;
		}

		if( c == WG_ESCAPE_CODE )
		{
			if( * pSrc == '(' )
			{
				while( * pSrc != ')' )
					pSrc++;
				pSrc++;
			}
			if( * pSrc == 'h' || * pSrc == 'u' )
				pSrc += 2;
			else if( * pSrc == '{' )
				pSrc += 9;
			else
				pSrc++;
		}
		else
			n += writeUTF8( c, pDest + n );
	}

	return n;
}


int WgTextTool::stripTextColorCommands( const Uint16* pSrc, Uint16* pDest, int maxChars )
{
	int n = 0;
	while( n < maxChars )
	{
		Uint16 c = * pSrc++;

		if( c == 0 )
		{
			pDest[n] = 0;
			break;
		}

		if( c == WG_ESCAPE_CODE && *pSrc == '{')
			pSrc += 9;
		else if( c == WG_ESCAPE_CODE && *pSrc == '}' )
			pSrc++;
		else
			pDest[n++] = c;
	}

	return n;
}


int WgTextTool::stripTextColorCommands( const char* pSrc, char* pDest, int maxBytes )
{
	int n = 0;
	while( n < maxBytes )
	{
		Uint16 c = readChar( pSrc );

		if( c == 0 )
		{
			pDest[n] = 0;
			break;
		}

		if( c == WG_ESCAPE_CODE && *pSrc == '{')
			pSrc += 9;
		else if( c == WG_ESCAPE_CODE && *pSrc == '}' )
			pSrc++;
		else
			n += writeUTF8( c, pDest+n );
	}

	return n;
}


//____ stripTextCommandsConvert() _____________________________________________

int WgTextTool::stripTextCommandsConvert( const Uint16* pSrc, char* pDest, int maxChars )
{
	int n = 0;
	int ofs = 0;
	while( n < maxChars )
	{
		Uint16 c = * pSrc++;

		if( c == 0 )
		{
			pDest[ofs] = 0;
			break;
		}

		if( c == WG_ESCAPE_CODE )
		{
			if( * pSrc == '(' )
			{
				while( * pSrc != ')' )
					pSrc++;
				pSrc++;
			}
			else if( * pSrc == 'h' || * pSrc == 'u' )
				pSrc += 2;
			else if( * pSrc == '{' )
				pSrc += 9;
			else
				pSrc++;
		}
		else
		{
			ofs += writeUTF8( c, pDest + ofs );
			n++;
		}
	}

	return n;
}


int WgTextTool::stripTextCommandsConvert( const char* pSrc, Uint16* pDest, int maxChars )
{
	int n = 0;
	while( n < maxChars )
	{
		Uint16 c = readChar( pSrc );

		if( c == 0 )
		{
			pDest[n] = 0;
			break;
		}

		if( c == WG_ESCAPE_CODE )
		{
			if( * pSrc == '(' )
			{
				while( * pSrc != ')' )
					pSrc++;
				pSrc++;
			}
			if( * pSrc == 'h' || * pSrc == 'u' )
				pSrc += 2;
			else if( * pSrc == '{' )
				pSrc += 9;
			else
				pSrc++;
		}
		else
			pDest[n++] = c;
	}

	return n;
}


//____ SetColor() _____________________________________________________________

void WgTextTool::SetColor( const WgColor col, WgChar * pChar, Uint32 nb, WgMode mode )
{
	ModifyProperties( PropColorModifier(col,mode), pChar, nb );
}

//____ ClearColor() ___________________________________________________________

void WgTextTool::ClearColor( WgChar * pChar, Uint32 nb, WgMode mode )
{
	ModifyProperties( PropColorClearer(mode), pChar, nb );
}

//____ SetStyle() _____________________________________________________________

void WgTextTool::SetStyle( WgFontStyle style, WgChar * pChar, Uint32 nb, WgMode mode )
{
	ModifyProperties( PropStyleModifier(style,mode), pChar, nb );
}

//____ ClearStyle() ___________________________________________________________

void WgTextTool::ClearStyle( WgChar * pChar, Uint32 nb, WgMode mode )
{
	ModifyProperties( PropStyleModifier(WG_STYLE_NORMAL,mode), pChar, nb );
}

//____ SetUnderlined() ________________________________________________________

void WgTextTool::SetUnderlined( WgChar * pChar, Uint32 nb, WgMode mode )
{
	ModifyProperties( PropUnderlinedModifier(true,mode), pChar, nb );
}

//____ ClearUnderlined() ________________________________________________________

void WgTextTool::ClearUnderlined( WgChar * pChar, Uint32 nb, WgMode mode )
{
	ModifyProperties( PropUnderlinedModifier(false,mode), pChar, nb );
}

//____ SetLink() ______________________________________________________________

void WgTextTool::SetLink( const WgTextLinkPtr& pLink, WgChar * pChar, Uint32 nb )
{
	ModifyProperties( PropLinkModifier(pLink), pChar, nb );
}


//____ SetFont() ______________________________________________________________

void WgTextTool::SetFont( WgFont * pFont, WgChar * pChar, Uint32 nb )
{
	ModifyProperties( PropFontModifier(pFont), pChar, nb );
}

//____ SetGlyph() ______________________________________________________________

void WgTextTool::SetGlyph( Uint16 glyph, WgChar * pChar, Uint32 nb )
{
	for( unsigned int i = 0 ; i < nb ; i++ )
		pChar[i].glyph = glyph;
}

//____ SetChars() ______________________________________________________________

void WgTextTool::SetChars( const WgChar& ch, WgChar * pChar, Uint32 nb )
{
	DerefProps( pChar, nb );

	for( unsigned int i = 0 ; i < nb ; i++ )
		pChar[i].all = ch.all;

	if( ch.PropHandle() != 0 )
		WgTextPropManager::IncRef( ch.PropHandle(), nb );
}



//____ SetProperties() ________________________________________________________

void WgTextTool::SetProperties( const WgTextPropPtr& pProp, WgChar * pChar, Uint32 nb )
{
	Uint32		refCnt = 0;
	Uint32		refCntTotal = 0;
	Uint16		old_prop = 0xFFFF;
	Uint16		new_prop = pProp.GetHandle();

	for( unsigned int i = 0 ; i < nb ; i++ )
	{
		if( pChar[i].properties != old_prop )
		{
			if( refCnt != 0 )
			{
				if( old_prop != 0 )
					WgTextPropManager::DecRef( old_prop, refCnt );

				refCntTotal += refCnt;
				refCnt = 0;
			}
			old_prop = pChar[i].properties;
		}

		pChar[i].properties = new_prop;
		refCnt++;
	}

	if( refCnt != 0 && old_prop != 0 )
		WgTextPropManager::DecRef( old_prop, refCnt );

	if( new_prop != 0 )
		WgTextPropManager::IncRef( new_prop, refCntTotal + refCnt );
}

//____ ModifyProperties() __________________________________________________________

void WgTextTool::ModifyProperties( const PropModifier& modif, WgChar * pChar, Uint32 nb  )
{
	Uint32		refCnt = 0;
	Uint16		old_prop = 0xFFFF;
	Uint16		new_prop = 0xFFFF;

	for( unsigned int i = 0 ; i < nb ; i++ )
	{
		if( pChar[i].properties != old_prop )
		{
			if( refCnt != 0 )
			{
				// Increase first, in case they are the same...

				if( new_prop != 0 )
					WgTextPropManager::IncRef( new_prop, refCnt );

				if( old_prop != 0 )
					WgTextPropManager::DecRef( old_prop, refCnt );

				refCnt = 0;
			}
			old_prop = pChar[i].properties;

			WgTextProp prop = WgTextPropManager::GetProp(pChar[i].properties);
			modif.Modify( prop );
			new_prop = WgTextPropManager::RegisterProp(prop);
		}

		pChar[i].properties = new_prop;
		refCnt++;
	}

	if( refCnt != 0 )
	{
		if( new_prop != 0 )
			WgTextPropManager::IncRef( new_prop, refCnt );

		if( old_prop != 0 )
			WgTextPropManager::DecRef( old_prop, refCnt );
	}
}


//____ IsCombUnderlined() _____________________________________________________

bool WgTextTool::IsCombUnderlined( Uint16 hTextProp, Uint16 hCharProp, WgMode mode )
{
	return (WgTextPropManager::GetProp(hCharProp).m_modeProp[mode].m_bUnderlined ||
			WgTextPropManager::GetProp(hTextProp).m_modeProp[mode].m_bUnderlined ||
			WgBase::GetDefaultTextProp()->m_modeProp[mode].m_bUnderlined );
}

//____ GetCombSize() __________________________________________________________

int WgTextTool::GetCombSize(Uint16 hTextProp, Uint16 hCharProp, WgMode mode)
{
	// Prio 1: Size for character.

	if( WgTextPropManager::GetProp(hCharProp).m_modeProp[mode].m_size != 0 )
		return (WgFontStyle) WgTextPropManager::GetProp(hCharProp).m_modeProp[mode].m_size;

	// Prio 2: Size for text.

	if( WgTextPropManager::GetProp(hTextProp).m_modeProp[mode].m_size != 0 )
		return (WgFontStyle) WgTextPropManager::GetProp(hTextProp).m_modeProp[mode].m_size;

	// Prio 3: Size from default textprop.

	return (WgFontStyle) WgBase::GetDefaultTextProp()->m_modeProp[mode].m_size;
}


//____ GetCombStyle() _________________________________________________________

WgFontStyle WgTextTool::GetCombStyle(Uint16 hTextProp, Uint16 hCharProp, WgMode mode)
{
	// Prio 1: Style for character.

	if( WgTextPropManager::GetProp(hCharProp).m_modeProp[mode].m_style != WG_STYLE_NORMAL )
		return (WgFontStyle) WgTextPropManager::GetProp(hCharProp).m_modeProp[mode].m_style;

	// Prio 2: Style for text.

	if( WgTextPropManager::GetProp(hTextProp).m_modeProp[mode].m_style != WG_STYLE_NORMAL )
		return (WgFontStyle) WgTextPropManager::GetProp(hTextProp).m_modeProp[mode].m_style;

	// Prio 3: Style from default textprop.

	return (WgFontStyle) WgBase::GetDefaultTextProp()->m_modeProp[mode].m_style;
}


//____ GetCombColor() _________________________________________________________

const WgColor WgTextTool::GetCombColor(Uint16 hTextProp, Uint16 hCharProp, WgMode mode)
{
	// Prio 1: Color set for character.

	if( WgTextPropManager::GetProp(hCharProp).m_modeProp[mode].m_bColored )
		return WgTextPropManager::GetProp(hCharProp).m_modeProp[mode].m_color;

	// Prio 2: Color set for text.

	if( WgTextPropManager::GetProp(hTextProp).m_modeProp[mode].m_bColored )
		return WgTextPropManager::GetProp(hTextProp).m_modeProp[mode].m_color;

	// Prio 3: Color from default textprop

	if( WgBase::GetDefaultTextProp()->m_modeProp[mode].m_bColored )
		return WgBase::GetDefaultTextProp()->m_modeProp[mode].m_color;

	// Prio 4: Just a white color...

	return WgColor::White();
}

//____ GetCombLink() __________________________________________________________

WgTextLinkPtr WgTextTool::GetCombLink(Uint16 hTextProp, Uint16 hCharProp)
{
	// Prio 1: Link set for character.

	if( WgTextPropManager::GetProp(hCharProp).m_pLink )
		return WgTextPropManager::GetProp(hCharProp).m_pLink;

	// Prio 2: Link set for text.

	return WgTextPropManager::GetProp(hTextProp).m_pLink;
}

//____ GetCombBreakLevel() ____________________________________________________

int WgTextTool::GetCombBreakLevel( Uint16 hTextProp, Uint16 hCharProp )
{
	// Prio 1: Breaklevel set for character.

	if( WgTextPropManager::GetProp(hCharProp).m_breakLevel != -1 )
		return WgTextPropManager::GetProp(hCharProp).m_breakLevel;

	// Prio 2: Breaklevel set for text.

	if( WgTextPropManager::GetProp(hTextProp).m_breakLevel != -1 )
		return WgTextPropManager::GetProp(hTextProp).m_breakLevel;

	// Prio 3: Breaklevel from default textprop.

	if( WgBase::GetDefaultTextProp()->m_breakLevel != -1 )
		return WgBase::GetDefaultTextProp()->m_breakLevel;

	// Prio 4: Use hardcoded default level...

	return 3;
}


//____ GetCombFont() __________________________________________________________

WgFont * WgTextTool::GetCombFont(Uint16 hTextProp, Uint16 hCharProp)
{
	// Prio 1: Fontset for character

	if( WgTextPropManager::GetProp(hCharProp).m_pFont )
		return WgTextPropManager::GetProp(hCharProp).m_pFont;

	// Prio 2: Fontset for text

	if( WgTextPropManager::GetProp(hTextProp).m_pFont )
		return WgTextPropManager::GetProp(hTextProp).m_pFont;

	// prio 3: Default fontset

	return WgBase::GetDefaultTextProp()->m_pFont;
}

//____ GetCombGlyphSet() ______________________________________________________

WgGlyphSet * WgTextTool::GetCombGlyphSet( Uint16 hTextProp, Uint16 hCharProp, WgMode mode )
{
	WgFont * p = GetCombFont(hTextProp,hCharProp);

	if( !p )
		return 0;

	return p->GetGlyphSet( GetCombStyle(hTextProp, hCharProp, mode), GetCombSize(hTextProp, hCharProp, mode) );
}

//____ GetCombCharVisibility() ________________________________________________

bool WgTextTool::GetCombCharVisibility( Uint16 character, Uint16 hTextProp, Uint16 hCharProp )
{
	// Prio 1: Setting for character.

	if( WgTextPropManager::GetProp(hCharProp).GetCharVisibility(character) )
		return true;

	// Prio 2: Setting for text.

	if( WgTextPropManager::GetProp(hTextProp).GetCharVisibility(character) )
		return true;

	// Prio 3: Default setting.

	return WgBase::GetDefaultTextProp()->GetCharVisibility(character);
}





//____ TextPropEncoder::Constructor ___________________________________________

WgTextTool::TextPropEncoder::TextPropEncoder( const WgResDB * pResDB )
{
	m_pResDB = pResDB;
}

//____ TextPropEncoder::BeginString() _________________________________________

Uint32 WgTextTool::TextPropEncoder::BeginString()
{
	m_bColorTagOpen = false;
	m_bStyleTagOpen = false;
	m_bSizeTagOpen	= false;
	m_bUnderTagOpen = false;

	m_pBaseProp = 0;
	m_pActiveProp = 0;

	return 0;
}

//____ TextPropEncoder::SetProp() _____________________________________________

Uint32 WgTextTool::TextPropEncoder::SetProp( const WgTextPropPtr& pNewProp )
{
	Uint32 i = 0;

	// First, see if we can do this using only current "baseprop" + style/color/size/underline settings.

	if( pNewProp->GetFont() == m_pBaseProp->GetFont() && pNewProp->GetLink() == m_pBaseProp->GetLink() &&
		((pNewProp->IsColored() && pNewProp->IsColorStatic()) || pNewProp->CompareColorTo( m_pBaseProp )) &&
		(pNewProp->IsStyleStatic() || pNewProp->CompareStyleTo( m_pBaseProp )) &&
		(pNewProp->IsSizeStatic() || pNewProp->CompareSizeTo( m_pBaseProp )) &&
		((pNewProp->IsUnderlined() && pNewProp->IsUnderlineStatic()) || pNewProp->CompareUnderlineTo( m_pBaseProp )) )
	{
		// Yes we can!

		// nothing to do here, we are all setup.
	}
	else if( m_pResDB )
	{
		// Secondly, if nullprop isn't our current baseprop we see if we can do this using only nullprop
		// + style/color/size/underline settings.

		if( !m_pBaseProp && pNewProp->GetFont() == 0 && !pNewProp->GetLink() &&
			pNewProp->IsColorStatic() && pNewProp->IsStyleStatic() && pNewProp->IsSizeStatic() && pNewProp->IsUnderlined() )
		{
			// Yes we can! Switch to nullprop as our baseprop

			i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
			strcpy( m_temp + i, "(null)" );
			i += 6;

			m_pBaseProp = 0;
		}
		else
		{
			// Thirdly, see if we have a perfect match

			std::string id = m_pResDB->FindTextPropId( pNewProp );

			if( id.length() > 0 )
			{
				// Yes, we have! Switch to this prop.

				i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
				strcpy( m_temp + i, id.c_str() );
				i += id.length();

				m_pBaseProp = m_pResDB->GetTextProp( id );
			}
			else
			{
				// Fourthly, look for the first possible match which can be combined with style/color/size/underline settings
				// to make a perfect match.

				WgResDB::TextPropRes * pRes = m_pResDB->GetFirstResTextProp();
				while( pRes )
				{
					WgTextPropPtr pProp = pRes->res;

					if( pNewProp->GetFont() == pProp->GetFont() && pNewProp->GetLink() == pProp->GetLink() &&
						((pNewProp->IsColored() && pNewProp->IsColorStatic()) || pNewProp->CompareColorTo( pProp )) &&
						(pNewProp->IsStyleStatic() || pNewProp->CompareStyleTo( pProp )) &&
						(pNewProp->IsSizeStatic() || pNewProp->CompareSizeTo( pProp )) &&
						((pNewProp->IsUnderlined() && pNewProp->IsUnderlineStatic()) || pNewProp->CompareUnderlineTo( pProp )) )
					{
						// This one works! Switch to this prop.

						i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
						strcpy( m_temp + i, pRes->id.c_str() );
						i += pRes->id.length();

						m_pBaseProp = pProp;

					}

					pRes = pRes->Next();
				}
			}
		}

	}
	else
	{
		// Failure! We switch to nullprop and do the best of the situation with style/color/size/underline-settings.

		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		strcpy( m_temp + i, "(null)" );
		i += 6;

		m_pBaseProp = 0;
	}

	//================================================================

	// Possibly end active color

	if( m_bColorTagOpen && (!pNewProp->IsColored() || pNewProp->CompareColorTo( m_pBaseProp ) || !pNewProp->CompareColorTo( m_pActiveProp ) )  )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '}';

		m_bColorTagOpen = false;
	}

	// Possibly start new color.

	if( !m_bColorTagOpen && pNewProp->IsColored() && !pNewProp->CompareColorTo( m_pBaseProp ) )
	{
		WgColor col = pNewProp->GetColor();
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '{';

		Uint8ToAscii( col.a, &m_temp[i], 2 );
		i += 2;
		Uint8ToAscii( col.r, &m_temp[i], 2 );
		i += 2;
		Uint8ToAscii( col.g, &m_temp[i], 2 );
		i += 2;
		Uint8ToAscii( col.b, &m_temp[i], 2 );
		i += 2;

		m_bColorTagOpen = true;
	}

	// Possibly begin/end underline

	if( (pNewProp->IsUnderlined() && pNewProp->IsUnderlineStatic()) &&
		!m_bUnderTagOpen && !pNewProp->CompareUnderlineTo( m_pBaseProp ) )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '_';

		m_bUnderTagOpen = true;
	}
	else if( m_bUnderTagOpen )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '|';

		m_bUnderTagOpen = false;
	}

	// Possibly end current style and/or start new style.

	if( m_bStyleTagOpen && (!pNewProp->CompareStyleTo( m_pActiveProp ) || pNewProp->CompareStyleTo( m_pBaseProp )) )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '#';

		m_bStyleTagOpen = false;
	}

	if( !m_bStyleTagOpen && !pNewProp->CompareStyleTo( m_pBaseProp ) && pNewProp->IsStyleStatic() )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );

		switch( pNewProp->GetStyle() )
		{
			case WG_STYLE_NORMAL:
				m_temp[i++] = 'd';
				break;
			case WG_STYLE_BOLD:
				m_temp[i++] = 'b';
				break;
			case WG_STYLE_BOLD_ITALIC:
				m_temp[i++] = 'i';
				break;
			case WG_STYLE_ITALIC:
				m_temp[i++] = 'I';
				break;
			case WG_STYLE_SUPERSCRIPT:
				m_temp[i++] = 'S';
				break;
			case WG_STYLE_SUBSCRIPT:
				m_temp[i++] = 's';
				break;
			case WG_STYLE_MONOSPACE:
				m_temp[i++] = 'm';
				break;
			case WG_STYLE_HEADING_1:
			case WG_STYLE_HEADING_2:
			case WG_STYLE_HEADING_3:
			case WG_STYLE_HEADING_4:
			case WG_STYLE_HEADING_5:
				m_temp[i++] = 'h';
				m_temp[i++] = '1' + (pNewProp->GetStyle() - WG_STYLE_HEADING_1);
				break;
			case WG_STYLE_USER_1:
			case WG_STYLE_USER_2:
			case WG_STYLE_USER_3:
			case WG_STYLE_USER_4:
			case WG_STYLE_USER_5:
				m_temp[i++] = 'u';
				m_temp[i++] = '1' + (pNewProp->GetStyle() - WG_STYLE_USER_1);
				break;
		}

		m_bStyleTagOpen = true;
	}

	// Possibly end current size and/or start new size.

	if( m_bSizeTagOpen && (!pNewProp->CompareSizeTo( m_pActiveProp ) || pNewProp->CompareSizeTo( m_pBaseProp )) )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = ']';

		m_bSizeTagOpen = false;
	}

	if( !m_bSizeTagOpen && !pNewProp->CompareSizeTo( m_pBaseProp ) && pNewProp->IsSizeStatic() )
	{
		int size = pNewProp->GetSize();
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '[';

		m_temp[i++] = size/100 + '0';
		m_temp[i++] = ((size/10) % 10) + '0';
		m_temp[i++] = (size % 10) + '0';

		m_bSizeTagOpen = true;
	}

	m_pActiveProp = pNewProp;
	m_temp[i] = 0;
	return i;
}

//____ TextPropEncoder::EndString() ___________________________________________

Uint32 WgTextTool::TextPropEncoder::EndString()
{
	Uint32 i = 0;

	if( m_pActiveProp->GetStyle() != WG_STYLE_NORMAL )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '#';
	}

	if( m_pActiveProp->IsUnderlined() )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '|';
	}

	if( m_pActiveProp->IsColored() )
	{
		i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
		m_temp[i++] = '}';
	}

	if( m_pBaseProp )
	{
			i += writeUTF8( WG_ESCAPE_CODE, m_temp+i );
			strcpy( m_temp + i, "(null)" );
			i += 6;
	}

	return i;
}
