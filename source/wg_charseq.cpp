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

WgCharSeq::WgCharSeq( const char * pChar)
{
    m_type		= CHAR8;
    m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::countChars(pChar);
}

WgCharSeq::WgCharSeq( const char * pChar, int len )
{
    m_type		= CHAR8;
    m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::countChars(pChar,len);
}

WgCharSeq::WgCharSeq( const Uint16 * pChar )
{
    m_type		= CHAR16;
    m_pChar		= (void *) pChar;
    m_nbChars	= WgTextTool::strlen(pChar);
}

WgCharSeq::WgCharSeq( const Uint16 * pChar, int len )
{
    m_type		= CHAR16;
    m_pChar		= (void *) pChar;
    m_nbChars	= len;
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

WgCharSeq::WgCharSeq( const std::string& str )
{
    m_type      = CHAR8;
    m_pChar     = (void *) str.c_str();
    m_nbChars   = WgTextTool::countChars(str.c_str());
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

    m_type      = CHAR8;
    m_pChar     = p + ofs;
    m_nbChars   = WgTextTool::countChars(p+ofs, len);
}



WgCharSeq::WgCharSeq( const WgCharBuffer * pBuffer )
{
	m_type		= WGCHAR;
	m_pChar		= (void *) pBuffer->GetChars();
	m_nbChars	= pBuffer->GetNbChars();
}

WgCharSeq::WgCharSeq( const WgCharBuffer * pBuffer, int ofs, int len )
{
	int realNbChars = (int) pBuffer->GetNbChars();
	if( ofs + len > realNbChars )
	{
		if( ofs > realNbChars )
			ofs = realNbChars;
		len = realNbChars - ofs;
	}

	m_type		= WGCHAR;
	m_pChar		= (void *)  (pBuffer->GetChars() + ofs);
	m_nbChars	= len;
}

WgCharSeq::WgCharSeq( const WgString& str )
{
	m_type		= WGCHAR;
	m_pChar		= (void *) str.GetChars();
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
	m_pChar		= (void *) (str.GetChars() + ofs);
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
		case CHAR8:
		{
			m_pChar = seq.m_pChar;
			const char * p = (const char *) m_pChar;
			WgTextTool::forwardCharacters( p, ofs );
			break;
		}
		case CHAR16:
			m_pChar = ((const Uint16 *)seq.m_pChar) + ofs;
			break;
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
	case CHAR8:
	{
		const char * p = (const char *) m_pChar;
		WgTextTool::forwardCharacters( p, m_nbChars );
		return p - (const char *) m_pChar;
	}
	case CHAR16:
		return (int) WgTextTool::getTextSizeUTF8( (const Uint16*) m_pChar, m_nbChars );
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
				if( ((WgChar*)m_pChar)[i].IsEndOfLine() )
					nbLines++;
			break;
		case CHAR8:
			for( int i = 0 ; i < m_nbChars ; i++ )
				if( ((char*)m_pChar)[i] == '\n' )
					nbLines++;
			break;
		case CHAR16:
			for( int i = 0 ; i < m_nbChars ; i++ )
				if( ((Uint16*)m_pChar)[i] == '\n' )
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

		case CHAR8:
		{
			const char * pSrc = (char*) m_pChar;

			WgTextTool::DerefProps( pDest, m_nbChars );
			WgTextTool::readString( pSrc, pDest, m_nbChars );
			break;
		}
		case CHAR16:
		{
			const Uint16 * pSrc = (Uint16*) m_pChar;

			WgTextTool::DerefProps( pDest, m_nbChars );
			WgTextTool::readString( pSrc, pDest, m_nbChars );
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

		case CHAR8:
		{
			int bytes = sizeof(WgChar)*m_nbChars;

			basket.ptr = (WgChar *) new char[bytes];
			memset( (void *) basket.ptr, 0, bytes);
			const char * pSrc = (char*) m_pChar;
			WgTextTool::readString( pSrc, (WgChar*) basket.ptr, m_nbChars );

			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case CHAR16:
		{
			int bytes = sizeof(WgChar)*m_nbChars;

			basket.ptr = (WgChar *) new char[bytes];
			memset( (void *) basket.ptr, 0, bytes);
			const Uint16 * pSrc = (Uint16*) m_pChar;
			WgTextTool::readString( pSrc, (WgChar*) basket.ptr, m_nbChars );

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
			basket.ptr = p;
			for( int i = 0 ; i < m_nbChars ; i++ )
				p[i] = ((WgChar*)m_pChar)[i].GetGlyph();
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case CHAR8:
		{
			Uint16 * p = new Uint16[m_nbChars];
			basket.ptr = p;
			const char * pSrc = (const char *) m_pChar;
			WgTextTool::readString( pSrc, p, m_nbChars );
			basket.length = m_nbChars;
			basket.bIsOwner = true;
			return basket;
		}
		case CHAR16:
			basket.ptr = (const Uint16 *) m_pChar;
			basket.length = m_nbChars;
			basket.bIsOwner = false;
			return basket;

		default:					// EMPTY
			basket.ptr = 0;
			basket.length = 0;
			basket.bIsOwner = false;
			return basket;
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
			Uint32 size = WgTextTool::getTextSizeUTF8( (WgChar*) m_pChar, m_nbChars );
			char * p = new char[size+1];
			basket.ptr = p;
			WgTextTool::getTextUTF8( (WgChar*) m_pChar, p, size+1 );
			basket.length = size;
			basket.bIsOwner = true;
			return basket;
		}
		case CHAR8:
		{
			basket.ptr = (const char *) m_pChar;
			const char * p = (const char *) m_pChar;
			WgTextTool::forwardCharacters( p, m_nbChars );
			basket.length = p - (const char *) m_pChar;
			basket.bIsOwner = false;
			return basket;
		}
		case CHAR16:
		{
			Uint32 size = WgTextTool::getTextSizeUTF8( (Uint16*) m_pChar, m_nbChars );
			char * p = new char[size+1];
			basket.ptr = p;
			WgTextTool::getTextUTF8( (Uint16*) m_pChar, p, size+1 );
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

//____ CharBasket::Destructor __________________________________________________

WgCharSeq::WgCharBasket::~WgCharBasket()
{
	if( bIsOwner )
	{
		WgTextTool::DerefProps( (WgChar*) ptr, length );
		delete [] (char*) ptr;
	}
}
