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

#include <memory.h>

#include <wg_charseq.h>
//#include <wg_texttool.h>
//#include <wg_text.h>
#include <wg_charbuffer.h>
#include <wg_string.h>


//____ Constructors ____________________________________________________________


WgCharSeq::WgCharSeq( const char * pChar )
{
	m_type 		= ESCAPED_UTF8;
    m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::countNonFormattingChars(pChar);
}

WgCharSeq::WgCharSeq( const char * pChar, int len )
{
	m_type 		= ESCAPED_UTF8;
	m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::countNonFormattingChars(pChar, len);
}

WgCharSeq::WgCharSeq( const Uint16 * pChar )
{
	m_type 		= ESCAPED_UTF16;
	m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::countNonFormattingChars(pChar);

}
WgCharSeq::WgCharSeq( const Uint16 * pChar, int len )
{
	m_type 		= ESCAPED_UTF16;
	m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::countNonFormattingChars(pChar, len);
}

WgCharSeq::WgCharSeq( const std::string& str )
{
	m_type 		= ESCAPED_UTF8;
    m_pChar     = (void *) str.c_str();
    m_nbChars   = WgTextTool::countNonFormattingChars(str.c_str());
}

WgCharSeq::WgCharSeq( const std::string& str, int ofs, int len )
{
	const char * p = str.c_str();
	int strlen = str.length();

	if( ofs + len > strlen )
	{
		if( ofs > strlen )
			ofs = strlen;
		len = strlen - ofs;
	}

	m_type 		= ESCAPED_UTF8;
    m_pChar     = p + ofs;
    m_nbChars   = WgTextTool::countNonFormattingChars(p+ofs, len);
}

WgCharSeq::WgCharSeq( const std::wstring& str )
{
    m_type      = ESCAPED_UTF16;
    m_pChar     = (void *) str.c_str();
    m_nbChars   = WgTextTool::countNonFormattingChars( (Uint16*) str.c_str());
}

WgCharSeq::WgCharSeq( const std::wstring& str, int ofs, int len )
{
	const wchar_t * p = str.c_str();
	int strlen = str.length();

	if( ofs + len > strlen )
	{
		if( ofs > strlen )
			ofs = strlen;
		len = strlen - ofs;
	}

    m_type      = ESCAPED_UTF16;
    m_pChar     = p + ofs;
    m_nbChars   = WgTextTool::countNonFormattingChars( ((Uint16*)p)+ofs, len);
}


WgCharSeq::WgCharSeq( const WgChar * pChar )
{
    m_type		= WGCHAR;
    m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::strlen(pChar);
}

WgCharSeq::WgCharSeq( const WgChar * pChar, int len )
{
    m_type      = WGCHAR;
    m_pChar     = (void *) pChar;
    m_nbChars   = len;
}



WgCharSeq::WgCharSeq( const WgCharBuffer * pBuffer )
{
	m_type		= WGCHAR;
	m_pChar		= (void *) pBuffer->Chars();
	m_nbChars	= pBuffer->NbChars();
}

WgCharSeq::WgCharSeq( const WgCharBuffer * pBuffer, int ofs, int len )
{
	int realNbChars = (int) pBuffer->NbChars();
	if( ofs + len > realNbChars )
	{
		if( ofs > realNbChars )
			ofs = realNbChars;
		len = realNbChars - ofs;
	}

	m_type		= WGCHAR;
	m_pChar		= (void *)  (pBuffer->Chars() + ofs);
	m_nbChars	= len;
}

WgCharSeq::WgCharSeq( const WgString& str )
{
	m_type		= WGCHAR;
	m_pChar		= (void *) str.Chars();
	m_nbChars	= str.Length();
}

WgCharSeq::WgCharSeq( const WgString& str, int ofs, int len )
{
	int realNbChars = (int) str.Length();
	if( ofs + len > realNbChars )
	{
		if( ofs > realNbChars )
			ofs = realNbChars;
		len = realNbChars - ofs;
	}

	m_type		= WGCHAR;
	m_pChar		= (void *) (str.Chars() + ofs);
	m_nbChars	= len;
}

WgCharSeq::WgCharSeq( const WgCharSeq& seq, int ofs, int len )
{
	if( ofs > seq.m_nbChars )
		ofs = seq.m_nbChars;

	if( len > seq.m_nbChars - ofs )
		len = seq.m_nbChars - ofs;

	m_type = seq.m_type;
	m_nbChars = len;

	switch( seq.m_type )
	{
		case WGCHAR:
			m_pChar = ((const WgChar *)seq.m_pChar) + ofs;
			break;
		case UTF8:
		{
			const char * p = (const char *) seq.m_pChar;
			WgTextTool::forwardCharacters( p, ofs );
			m_pChar = p;
			break;
		}
		case UTF16:
			m_pChar = ((const Uint16 *)seq.m_pChar) + ofs;
			break;
		case ESCAPED_UTF8:
		{
			const char * p = (const char *) seq.m_pChar;
			WgTextTool::forwardEscapedCharacters( p, ofs );
			m_pChar = p;
			break;
		}
		case ESCAPED_UTF16:
		{
			const Uint16 * p = (const Uint16 *) seq.m_pChar;
			WgTextTool::forwardEscapedCharacters( p, ofs );
			m_pChar = p;
			break;
		}
		case MAP8:
		{
			m_pChar = (const char*) seq.m_pChar + ofs;
			break;
		}
		default:						// EMPTY
			m_pChar = seq.m_pChar;
			break;
	}
}



//____ LengthUTF8() ___________________________________________________________

int WgCharSeq::LengthUTF8() const
{
	switch( m_type )
	{
	case WGCHAR:
		return (int) WgTextTool::getTextSizeUTF8( (const WgChar*) m_pChar, m_nbChars );
	case UTF8:
	{
		const char * p = (const char *) m_pChar;
		WgTextTool::forwardCharacters( p, m_nbChars );
		return p - (const char *) m_pChar;
	}
	case UTF16:
		return (int) WgTextTool::getTextSizeUTF8( (const Uint16*) m_pChar, m_nbChars );
	case ESCAPED_UTF8:
		return (int) WgTextTool::getTextSizeStrippedUTF8( (const char*) m_pChar, m_nbChars );
	case ESCAPED_UTF16:
		return (int) WgTextTool::getTextSizeStrippedUTF8( (const Uint16*) m_pChar, m_nbChars );
	case MAP8:
		return (int) WgTextTool::getTextSizeUTF8( (const char *) m_pChar, ((WgCharSeq8*)this)->m_codepage, m_nbChars );
	default:						// EMPTY
		return 0;
	}
}


//____ GetNbLines() ____________________________________________________________

int WgCharSeq::GetNbLines() const
{
	int nbLines = 1;		// We always have at least one line...

	switch( m_type )
	{
		case WGCHAR:
			for( int i = 0 ; i < m_nbChars ; i++ )
				if( ((const WgChar*)m_pChar)[i].IsEndOfLine() )
					nbLines++;
			break;
		case UTF8:
		case ESCAPED_UTF8:
		{
			const char * p = (const char *) m_pChar;
			for( int i = 0 ; i < m_nbChars ; i++ )
				if( WgTextTool::readChar( p ) == '\n' )
					nbLines++;
			break;
		}
		case UTF16:
		case ESCAPED_UTF16:
			for( int i = 0 ; i < m_nbChars ; i++ )
				if( ((const Uint16*)m_pChar)[i] == '\n' )
					nbLines++;
			break;
		case MAP8:
			for( int i = 0 ; i < m_nbChars ; i++ )
				if( ((const char*)m_pChar)[i] == '\n' )
					nbLines++;
			break;

		default:					// EMPTY
			break;
	}

	return nbLines;
}


//___ CopyTo() _________________________________________________________________

void WgCharSeq::CopyTo( WgChar * pDest ) const
{
	switch( m_type )
	{
		case WGCHAR:
			WgTextTool::CopyChars( (WgChar*) m_pChar, pDest, m_nbChars );
			break;

		case UTF8:
		{
			const char * pSrc = (const char*) m_pChar;

			WgTextTool::DerefProps( pDest, m_nbChars );
			WgTextTool::readString( pSrc, pDest, m_nbChars );
			break;
		}
		case UTF16:
		{
			const Uint16 * pSrc = (const Uint16*) m_pChar;

			WgTextTool::DerefProps( pDest, m_nbChars );
			WgTextTool::readString( pSrc, pDest, m_nbChars );
			break;
		}
		case ESCAPED_UTF8:
		{
			const char * pSrc = (const char*) m_pChar;
			WgTextTool::DerefProps( pDest, m_nbChars );
			WgTextTool::readFormattedString( pSrc, pDest, m_nbChars );
			break;
		}
		case ESCAPED_UTF16:
		{
			const Uint16 * pSrc = (const Uint16*) m_pChar;
			WgTextTool::DerefProps( pDest, m_nbChars );
			WgTextTool::readFormattedString( pSrc, pDest, m_nbChars );
			break;
		}
		case MAP8:
		{
			const char * pSrc = (const char*) m_pChar;
			WgTextTool::DerefProps( pDest, m_nbChars );
			WgTextTool::readString( pSrc, ((WgCharSeq8*)this)->m_codepage, pDest, m_nbChars );
			break;
		}

		default:
			break;
	}
}

//____ GetWgChars() ______________________________________________________________

const WgCharSeq::WgCharBasket WgCharSeq::GetWgChars() const
{
	WgCharBasket	basket;

	switch( m_type )
	{
		case WGCHAR:
			basket.ptr = (const WgChar *) m_pChar;
			basket.length = m_nbChars;
			basket.bIsOwner = false;
			return basket;

		case UTF8:
		{
			int bytes = sizeof(WgChar)*m_nbChars;

			WgChar * p = (WgChar *) new char[bytes];
			memset( p, 0, bytes);
			const char * pSrc = (char*) m_pChar;
			WgTextTool::readString( pSrc, p, m_nbChars );

			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case UTF16:
		{
			int bytes = sizeof(WgChar)*m_nbChars;

			WgChar * p = (WgChar *) new char[bytes];
			memset( p, 0, bytes);
			const Uint16 * pSrc = (Uint16*) m_pChar;
			WgTextTool::readString( pSrc, p, m_nbChars );

			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case ESCAPED_UTF8:
		{
			int bytes = sizeof(WgChar)*m_nbChars;

			WgChar * p = (WgChar *) new char[bytes];
			memset( p, 0, bytes);
			const char * pSrc = (char*) m_pChar;
			WgTextTool::readFormattedString( pSrc, p, m_nbChars );

			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case ESCAPED_UTF16:
		{
			int bytes = sizeof(WgChar)*m_nbChars;

			WgChar * p = (WgChar *) new char[bytes];
			memset( p, 0, bytes);
			const Uint16 * pSrc = (Uint16*) m_pChar;
			WgTextTool::readFormattedString( pSrc, p, m_nbChars );

			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case MAP8:
		{
			int bytes = sizeof(WgChar)*m_nbChars;

			WgChar * p = (WgChar *) new char[bytes];
			memset( p, 0, bytes);
			const char * pSrc = (char*) m_pChar;
			WgTextTool::readString( pSrc, ((WgCharSeq8*)this)->m_codepage, p, m_nbChars );

			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}

		default:					// EMPTY

			basket.ptr = 0;
			basket.length = 0;
			basket.bIsOwner = false;
			return basket;
	}
}





//____ GetUnicode() ____________________________________________________________

const WgCharSeq::UnicodeBasket WgCharSeq::GetUnicode() const
{
	UnicodeBasket	basket;

	switch( m_type )
	{
		case WGCHAR:
		{
			Uint16 * p = new Uint16[m_nbChars];
			for( int i = 0 ; i < m_nbChars ; i++ )
				p[i] = ((WgChar*)m_pChar)[i].Glyph();
			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case UTF8:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const char * pSrc = (const char *) m_pChar;
			WgTextTool::readString( pSrc, p, m_nbChars );
			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case UTF16:
			basket.ptr = (const Uint16 *) m_pChar;
			basket.length = m_nbChars;
			basket.bIsOwner = false;
			return basket;

		case ESCAPED_UTF8:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const char * pSrc = (const char *) m_pChar;
			WgTextTool::stripTextCommandsConvert( pSrc, p, m_nbChars );
			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}

		case ESCAPED_UTF16:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const Uint16 * pSrc = (const Uint16 *) m_pChar;
			WgTextTool::stripTextCommands( pSrc, p, m_nbChars );
			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}

		case MAP8:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const char * pSrc = (const char *) m_pChar;
			WgTextTool::readString( pSrc, ((WgCharSeq8*)this)->m_codepage, p, m_nbChars );
			basket.ptr = p;
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}

		default:					// EMPTY
			basket.ptr = 0;
			basket.length = 0;
			basket.bIsOwner = false;
			return basket;
	}
}

//____ GetStdWstring() ____________________________________________________________

std::wstring WgCharSeq::GetStdWstring() const
{
	std::wstring str;

	switch( m_type )
	{
		case WGCHAR:
		{
			Uint16 * p = new Uint16[m_nbChars];
			for( int i = 0 ; i < m_nbChars ; i++ )
				p[i] = ((WgChar*)m_pChar)[i].Glyph();
			str.assign( (const wchar_t *)p, m_nbChars );
			delete [] p;
			return str;
		}
		case UTF8:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const char * pSrc = (const char *) m_pChar;
			WgTextTool::readString( pSrc, p, m_nbChars );
			str.assign( (const wchar_t *)p, m_nbChars );
			delete [] p;
			return str;
		}
		case UTF16:
			str.assign( (const wchar_t *) m_pChar, m_nbChars );
			return str;

		case ESCAPED_UTF8:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const char * pSrc = (const char *) m_pChar;
			WgTextTool::stripTextCommandsConvert( pSrc, p, m_nbChars );
			str.assign( (const wchar_t *)p, m_nbChars );
			delete [] p;
			return str;
		}

		case ESCAPED_UTF16:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const Uint16 * pSrc = (const Uint16 *) m_pChar;
			WgTextTool::stripTextCommands( pSrc, p, m_nbChars );
			str.assign( (const wchar_t *)p, m_nbChars );
			delete [] p;
			return str;
		}

		case MAP8:
		{
			Uint16 * p = new Uint16[m_nbChars];
			const char * pSrc = (const char *) m_pChar;
			WgTextTool::readString( pSrc, ((WgCharSeq8*)this)->m_codepage, p, m_nbChars );
			str.assign( (const wchar_t *)p, m_nbChars );
			delete [] p;
			return str;
		}

		default:					// EMPTY
			return str;
	}
}

//____ GetUTF8() _______________________________________________________________

const WgCharSeq::UTF8Basket WgCharSeq::GetUTF8() const
{
	UTF8Basket	basket;

	switch( m_type )
	{
		case WGCHAR:
		{
			int size = WgTextTool::getTextSizeUTF8( (WgChar*) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::getTextUTF8( (WgChar*) m_pChar, p, size+1 );
			basket.ptr = p;
			basket.length = size;
			basket.bIsOwner = true;
			return basket;
		}
		case UTF8:
		{
			basket.ptr = (const char *) m_pChar;
			const char * p = (const char *) m_pChar;
			WgTextTool::forwardCharacters( p, m_nbChars );
			basket.length = p - (const char *) m_pChar;
			basket.bIsOwner = false;
			return basket;
		}
		case UTF16:
		{
			int size = WgTextTool::getTextSizeUTF8( (Uint16*) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::getTextUTF8( (Uint16*) m_pChar, p, size+1 );
			basket.ptr = p;
			basket.length = size;
			basket.bIsOwner = true;
			return basket;
		}
		case ESCAPED_UTF8:
		{
			int size = WgTextTool::getTextSizeStrippedUTF8( (char *) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::stripTextCommands( (char*) m_pChar, p, size+1 );
			basket.ptr = p;
			basket.length = size;
			basket.bIsOwner = true;
			return basket;
		}

		case ESCAPED_UTF16:
		{
			int size = WgTextTool::getTextSizeStrippedUTF8( (Uint16 *) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::stripTextCommandsConvert( (Uint16*) m_pChar, p, m_nbChars+1 );
			basket.ptr = p;
			basket.length = size;
			basket.bIsOwner = true;
			return basket;
		}

		case MAP8:
		{
			int size = WgTextTool::getTextSizeUTF8( (char*) m_pChar, ((WgCharSeq8*)this)->m_codepage, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::getTextUTF8( (char*) m_pChar, ((WgCharSeq8*)this)->m_codepage, p, size+1 );
			basket.ptr = p;
			basket.length = size;
			basket.bIsOwner = true;
			return basket;
		}

		default:					// EMPTY

			basket.ptr = 0;
			basket.length = 0;
			basket.bIsOwner = false;
			return basket;
	}
}

//____ GetStdString() _________________________________________________________

std::string WgCharSeq::GetStdString() const
{
//	char	temp[512];

	switch( m_type )
	{
		case WGCHAR:
		{
			int size = WgTextTool::getTextSizeUTF8( (WgChar*) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::getTextUTF8( (WgChar*) m_pChar, p, size+1 );
			std::string str( p, size );
			delete p;
			return str;
		}
		case UTF8:
		{
			const char * p = (const char *) m_pChar;
			WgTextTool::forwardCharacters( p, m_nbChars );
			int size = p - (const char *) m_pChar;
			return std::string( p, size );
		}
		case UTF16:
		{
			int size = WgTextTool::getTextSizeUTF8( (Uint16*) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::getTextUTF8( (Uint16*) m_pChar, p, size+1 );
			std::string str( p, size );
			delete p;
			return str;
		}
		case ESCAPED_UTF8:
		{
			int size = WgTextTool::getTextSizeStrippedUTF8( (char *) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::stripTextCommands( (char*) m_pChar, p, size+1 );
			std::string str( p, size );
			delete p;
			return str;
		}

		case ESCAPED_UTF16:
		{
			int size = WgTextTool::getTextSizeStrippedUTF8( (Uint16 *) m_pChar, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::stripTextCommandsConvert( (Uint16*) m_pChar, p, m_nbChars+1 );
			std::string str( p, size );
			delete p;
			return str;
		}

		case MAP8:
		{
			int size = WgTextTool::getTextSizeUTF8( (char*) m_pChar, ((WgCharSeq8*)this)->m_codepage, m_nbChars );
			char * p = new char[size+1];
			WgTextTool::getTextUTF8( (char*) m_pChar, ((WgCharSeq8*)this)->m_codepage, p, size+1 );
			std::string str( p, size );
			delete p;
			return str;
		}

		default:					// EMPTY
			return "";
	}
}



//____ CharBasket::Destructor __________________________________________________

WgCharSeq::WgCharBasket::~WgCharBasket()
{
	if( bIsOwner )
	{
		WgTextTool::DerefProps( (WgChar*) ptr, length );
		delete [] (char*) ptr;
	}
}

//____ WgCharSeqLiteral::Constructors _________________________________________

WgCharSeqLiteral::WgCharSeqLiteral( const char * pChar)
{
    m_type		= UTF8;
    m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::countChars(pChar);
}

WgCharSeqLiteral::WgCharSeqLiteral( const char * pChar, int len )
{
    m_type		= UTF8;
    m_pChar		= (void *) pChar;
    m_nbChars	= len;
}

WgCharSeqLiteral::WgCharSeqLiteral( const Uint16 * pChar )
{
    m_type		= UTF16;
    m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::strlen(pChar);
}

WgCharSeqLiteral::WgCharSeqLiteral( const Uint16 * pChar, int len )
{
    m_type		= UTF16;
    m_pChar		= (void *) pChar;
    m_nbChars	= len;
}

WgCharSeqLiteral::WgCharSeqLiteral( const std::string& str )
{
    m_type      = UTF8;
    m_pChar     = (void *) str.c_str();
    m_nbChars   = str.length();
}

WgCharSeqLiteral::WgCharSeqLiteral( const std::string& str, int ofs, int len )
{
	const char * p = str.c_str();
	int strlen = str.length();

	if( ofs + len > strlen )
	{
		if( ofs > strlen )
			ofs = strlen;
		len = strlen - ofs;
	}

    m_type      = UTF8;
    m_pChar     = p + ofs;
    m_nbChars   = WgTextTool::countChars(p+ofs, len);
}


WgCharSeqLiteral::WgCharSeqLiteral( const std::wstring& str )
{
    m_type      = UTF16;
    m_pChar     = (void *) str.c_str();
    m_nbChars   = str.length();
}

WgCharSeqLiteral::WgCharSeqLiteral( const std::wstring& str, int ofs, int len )
{
	const wchar_t * p = str.c_str();
	int strlen = str.length();

	if( ofs + len > strlen )
	{
		if( ofs > strlen )
			ofs = strlen;
		len = strlen - ofs;
	}

    m_type      = UTF16;
    m_pChar     = p + ofs;
    m_nbChars   = len;
}


//____ WgCharSeq8::Constructors _________________________________________


WgCharSeq8::WgCharSeq8( const char * pChar, WgCodePage codePage )
{
	m_type 		= MAP8;
    m_pChar		= (void *) pChar;
    m_nbChars	= strlen(pChar);
	m_codepage	= codePage;
}


WgCharSeq8::WgCharSeq8( const char * pChar, int len, WgCodePage codePage )
{
	m_type 		= MAP8;
    m_pChar		= (void *) pChar;
    m_nbChars	= len;
	m_codepage	= codePage;
}

WgCharSeq8::WgCharSeq8( const std::string& str, WgCodePage codePage )
{
	m_type 		= MAP8;
    m_pChar		= str.c_str();
	m_nbChars	= str.length();
	m_codepage	= codePage;
}

WgCharSeq8::WgCharSeq8( const std::string& str, int ofs, int len, WgCodePage codePage )
{
	const char * p = str.c_str();
	int strlen = str.length();

	if( ofs + len > strlen )
	{
		if( ofs > strlen )
			ofs = strlen;
		len = strlen - ofs;
	}

	m_type 		= MAP8;
    m_pChar     = p + ofs;
    m_nbChars   = len;
	m_codepage	= codePage;
}


