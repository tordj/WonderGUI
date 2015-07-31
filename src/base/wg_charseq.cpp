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
//#include <wg_legacytext.h>
#include <wg_charbuffer.h>
#include <wg_string.h>

namespace wg 
{
	
	
	//____ Constructors ____________________________________________________________
	
	
	CharSeq::CharSeq( const char * pChar )
	{
		m_type 		= ESCAPED_UTF8;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= TextTool::countNonFormattingChars(pChar);
	}
	
	CharSeq::CharSeq( const char * pChar, int len )
	{
		m_type 		= ESCAPED_UTF8;
		m_pChar		= (void *) pChar;
	    m_nbChars	= TextTool::countNonFormattingChars(pChar, len);
	}
	
	CharSeq::CharSeq( const Uint16 * pChar )
	{
		m_type 		= ESCAPED_UTF16;
		m_pChar		= (void *) pChar;
	    m_nbChars	= TextTool::countNonFormattingChars(pChar);
	
	}
	CharSeq::CharSeq( const Uint16 * pChar, int len )
	{
		m_type 		= ESCAPED_UTF16;
		m_pChar		= (void *) pChar;
	    m_nbChars	= TextTool::countNonFormattingChars(pChar, len);
	}
	
	CharSeq::CharSeq( const std::string& str )
	{
		m_type 		= ESCAPED_UTF8;
	    m_pChar     = (void *) str.c_str();
	    m_nbChars   = TextTool::countNonFormattingChars(str.c_str());
	}
	
	CharSeq::CharSeq( const std::string& str, int ofs, int len )
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
	    m_nbChars   = TextTool::countNonFormattingChars(p+ofs, len);
	}
	
	CharSeq::CharSeq( const std::wstring& str )
	{
	    m_type      = ESCAPED_UTF16;
	    m_pChar     = (void *) str.c_str();
	    m_nbChars   = TextTool::countNonFormattingChars( (Uint16*) str.c_str());
	}
	
	CharSeq::CharSeq( const std::wstring& str, int ofs, int len )
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
	    m_nbChars   = TextTool::countNonFormattingChars( ((Uint16*)p)+ofs, len);
	}
	
	
	CharSeq::CharSeq( const Char * pChar )
	{
	    m_type		= WGCHAR;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= TextTool::strlen(pChar);
	}
	
	CharSeq::CharSeq( const Char * pChar, int len )
	{
	    m_type      = WGCHAR;
	    m_pChar     = (void *) pChar;
	    m_nbChars   = len;
	}
	
	
	
	CharSeq::CharSeq( const CharBuffer * pBuffer )
	{
		m_type		= WGCHAR;
		m_pChar		= (void *) pBuffer->chars();
		m_nbChars	= pBuffer->nbChars();
	}
	
	CharSeq::CharSeq( const CharBuffer * pBuffer, int ofs, int len )
	{
		int realNbChars = (int) pBuffer->nbChars();
		if( ofs + len > realNbChars )
		{
			if( ofs > realNbChars )
				ofs = realNbChars;
			len = realNbChars - ofs;
		}
	
		m_type		= WGCHAR;
		m_pChar		= (void *)  (pBuffer->chars() + ofs);
		m_nbChars	= len;
	}
	
	CharSeq::CharSeq( const String& str )
	{
		m_type		= WGCHAR;
		m_pChar		= (void *) str.chars();
		m_nbChars	= str.length();
	}
	
	CharSeq::CharSeq( const String& str, int ofs, int len )
	{
		int realNbChars = (int) str.length();
		if( ofs + len > realNbChars )
		{
			if( ofs > realNbChars )
				ofs = realNbChars;
			len = realNbChars - ofs;
		}
	
		m_type		= WGCHAR;
		m_pChar		= (void *) (str.chars() + ofs);
		m_nbChars	= len;
	}
	
	CharSeq::CharSeq( const CharSeq& seq, int ofs, int len )
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
				m_pChar = ((const Char *)seq.m_pChar) + ofs;
				break;
			case UTF8:
			{
				const char * p = (const char *) seq.m_pChar;
				TextTool::forwardCharacters( p, ofs );
				m_pChar = p;
				break;
			}
			case UTF16:
				m_pChar = ((const Uint16 *)seq.m_pChar) + ofs;
				break;
			case ESCAPED_UTF8:
			{
				const char * p = (const char *) seq.m_pChar;
				TextTool::forwardEscapedCharacters( p, ofs );
				m_pChar = p;
				break;
			}
			case ESCAPED_UTF16:
			{
				const Uint16 * p = (const Uint16 *) seq.m_pChar;
				TextTool::forwardEscapedCharacters( p, ofs );
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
	
	
	
	//____ lengthUtf8() ___________________________________________________________
	
	int CharSeq::lengthUtf8() const
	{
		switch( m_type )
		{
		case WGCHAR:
			return (int) TextTool::getTextSizeUTF8( (const Char*) m_pChar, m_nbChars );
		case UTF8:
		{
			const char * p = (const char *) m_pChar;
			TextTool::forwardCharacters( p, m_nbChars );
			return p - (const char *) m_pChar;
		}
		case UTF16:
			return (int) TextTool::getTextSizeUTF8( (const Uint16*) m_pChar, m_nbChars );
		case ESCAPED_UTF8:
			return (int) TextTool::getTextSizeStrippedUTF8( (const char*) m_pChar, m_nbChars );
		case ESCAPED_UTF16:
			return (int) TextTool::getTextSizeStrippedUTF8( (const Uint16*) m_pChar, m_nbChars );
		case MAP8:
			return (int) TextTool::getTextSizeUTF8( (const char *) m_pChar, ((CharSeq8*)this)->m_codepage, m_nbChars );
		default:						// EMPTY
			return 0;
		}
	}
	
	
	//____ getNbLines() ____________________________________________________________
	
	int CharSeq::getNbLines() const
	{
		int nbLines = 1;		// We always have at least one line...
	
		switch( m_type )
		{
			case WGCHAR:
				for( int i = 0 ; i < m_nbChars ; i++ )
					if( ((const Char*)m_pChar)[i].isEndOfLine() )
						nbLines++;
				break;
			case UTF8:
			case ESCAPED_UTF8:
			{
				const char * p = (const char *) m_pChar;
				for( int i = 0 ; i < m_nbChars ; i++ )
					if( TextTool::readChar( p ) == '\n' )
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
	
	
	//___ copyTo() _________________________________________________________________
	
	void CharSeq::copyTo( Char * pDest ) const
	{
		switch( m_type )
		{
			case WGCHAR:
				TextTool::copyChars( (Char*) m_pChar, pDest, m_nbChars );
				break;
	
			case UTF8:
			{
				const char * pSrc = (const char*) m_pChar;
	
				TextTool::derefProps( pDest, m_nbChars );
				TextTool::readString( pSrc, pDest, m_nbChars );
				break;
			}
			case UTF16:
			{
				const Uint16 * pSrc = (const Uint16*) m_pChar;
	
				TextTool::derefProps( pDest, m_nbChars );
				TextTool::readString( pSrc, pDest, m_nbChars );
				break;
			}
			case ESCAPED_UTF8:
			{
				const char * pSrc = (const char*) m_pChar;
				TextTool::derefProps( pDest, m_nbChars );
				TextTool::readFormattedString( pSrc, pDest, m_nbChars );
				break;
			}
			case ESCAPED_UTF16:
			{
				const Uint16 * pSrc = (const Uint16*) m_pChar;
				TextTool::derefProps( pDest, m_nbChars );
				TextTool::readFormattedString( pSrc, pDest, m_nbChars );
				break;
			}
			case MAP8:
			{
				const char * pSrc = (const char*) m_pChar;
				TextTool::derefProps( pDest, m_nbChars );
				TextTool::readString( pSrc, ((CharSeq8*)this)->m_codepage, pDest, m_nbChars );
				break;
			}
	
			default:
				break;
		}
	}
	
	//____ getChars() ______________________________________________________________
	
	const CharSeq::CharBasket CharSeq::getChars() const
	{
		CharBasket	basket;
	
		switch( m_type )
		{
			case WGCHAR:
				basket.ptr = (const Char *) m_pChar;
				basket.length = m_nbChars;
				basket.bIsOwner = false;
				return basket;
	
			case UTF8:
			{
				int bytes = sizeof(Char)*m_nbChars;
	
				Char * p = (Char *) new char[bytes];
				memset( p, 0, bytes);
				const char * pSrc = (char*) m_pChar;
				TextTool::readString( pSrc, p, m_nbChars );
	
				basket.ptr = p;
				basket.length = m_nbChars;
				basket.bIsOwner = true;
				return basket;
			}
			case UTF16:
			{
				int bytes = sizeof(Char)*m_nbChars;
	
				Char * p = (Char *) new char[bytes];
				memset( p, 0, bytes);
				const Uint16 * pSrc = (Uint16*) m_pChar;
				TextTool::readString( pSrc, p, m_nbChars );
	
				basket.ptr = p;
				basket.length = m_nbChars;
				basket.bIsOwner = true;
				return basket;
			}
			case ESCAPED_UTF8:
			{
				int bytes = sizeof(Char)*m_nbChars;
	
				Char * p = (Char *) new char[bytes];
				memset( p, 0, bytes);
				const char * pSrc = (char*) m_pChar;
				TextTool::readFormattedString( pSrc, p, m_nbChars );
	
				basket.ptr = p;
				basket.length = m_nbChars;
				basket.bIsOwner = true;
				return basket;
			}
			case ESCAPED_UTF16:
			{
				int bytes = sizeof(Char)*m_nbChars;
	
				Char * p = (Char *) new char[bytes];
				memset( p, 0, bytes);
				const Uint16 * pSrc = (Uint16*) m_pChar;
				TextTool::readFormattedString( pSrc, p, m_nbChars );
	
				basket.ptr = p;
				basket.length = m_nbChars;
				basket.bIsOwner = true;
				return basket;
			}
			case MAP8:
			{
				int bytes = sizeof(Char)*m_nbChars;
	
				Char * p = (Char *) new char[bytes];
				memset( p, 0, bytes);
				const char * pSrc = (char*) m_pChar;
				TextTool::readString( pSrc, ((CharSeq8*)this)->m_codepage, p, m_nbChars );
	
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
	
	
	
	
	
	//____ getUnicode() ____________________________________________________________
	
	const CharSeq::UnicodeBasket CharSeq::getUnicode() const
	{
		UnicodeBasket	basket;
	
		switch( m_type )
		{
			case WGCHAR:
			{
				Uint16 * p = new Uint16[m_nbChars];
				for( int i = 0 ; i < m_nbChars ; i++ )
					p[i] = ((Char*)m_pChar)[i].getGlyph();
				basket.ptr = p;
				basket.length = m_nbChars;
				basket.bIsOwner = true;
				return basket;
			}
			case UTF8:
			{
				Uint16 * p = new Uint16[m_nbChars];
				const char * pSrc = (const char *) m_pChar;
				TextTool::readString( pSrc, p, m_nbChars );
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
				TextTool::stripTextCommandsConvert( pSrc, p, m_nbChars );
				basket.ptr = p;
				basket.length = m_nbChars;
				basket.bIsOwner = true;
				return basket;
			}
	
			case ESCAPED_UTF16:
			{
				Uint16 * p = new Uint16[m_nbChars];
				const Uint16 * pSrc = (const Uint16 *) m_pChar;
				TextTool::stripTextCommands( pSrc, p, m_nbChars );
				basket.ptr = p;
				basket.length = m_nbChars;
				basket.bIsOwner = true;
				return basket;
			}
	
			case MAP8:
			{
				Uint16 * p = new Uint16[m_nbChars];
				const char * pSrc = (const char *) m_pChar;
				TextTool::readString( pSrc, ((CharSeq8*)this)->m_codepage, p, m_nbChars );
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
	
	//____ getStdWstring() ____________________________________________________________
	
	std::wstring CharSeq::getStdWstring() const
	{
		std::wstring str;
	
		switch( m_type )
		{
			case WGCHAR:
			{
				Uint16 * p = new Uint16[m_nbChars];
				for( int i = 0 ; i < m_nbChars ; i++ )
					p[i] = ((Char*)m_pChar)[i].getGlyph();
				str.assign( (const wchar_t *)p, m_nbChars );
				delete [] p;
				return str;
			}
			case UTF8:
			{
				Uint16 * p = new Uint16[m_nbChars];
				const char * pSrc = (const char *) m_pChar;
				TextTool::readString( pSrc, p, m_nbChars );
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
				TextTool::stripTextCommandsConvert( pSrc, p, m_nbChars );
				str.assign( (const wchar_t *)p, m_nbChars );
				delete [] p;
				return str;
			}
	
			case ESCAPED_UTF16:
			{
				Uint16 * p = new Uint16[m_nbChars];
				const Uint16 * pSrc = (const Uint16 *) m_pChar;
				TextTool::stripTextCommands( pSrc, p, m_nbChars );
				str.assign( (const wchar_t *)p, m_nbChars );
				delete [] p;
				return str;
			}
	
			case MAP8:
			{
				Uint16 * p = new Uint16[m_nbChars];
				const char * pSrc = (const char *) m_pChar;
				TextTool::readString( pSrc, ((CharSeq8*)this)->m_codepage, p, m_nbChars );
				str.assign( (const wchar_t *)p, m_nbChars );
				delete [] p;
				return str;
			}
	
			default:					// EMPTY
				return str;
		}
	}
	
	//____ getUtf8() _______________________________________________________________
	
	const CharSeq::UTF8Basket CharSeq::getUtf8() const
	{
		UTF8Basket	basket;
	
		switch( m_type )
		{
			case WGCHAR:
			{
				int size = TextTool::getTextSizeUTF8( (Char*) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::getTextUTF8( (Char*) m_pChar, p, size+1 );
				basket.ptr = p;
				basket.length = size;
				basket.bIsOwner = true;
				return basket;
			}
			case UTF8:
			{
				basket.ptr = (const char *) m_pChar;
				const char * p = (const char *) m_pChar;
				TextTool::forwardCharacters( p, m_nbChars );
				basket.length = p - (const char *) m_pChar;
				basket.bIsOwner = false;
				return basket;
			}
			case UTF16:
			{
				int size = TextTool::getTextSizeUTF8( (Uint16*) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::getTextUTF8( (Uint16*) m_pChar, p, size+1 );
				basket.ptr = p;
				basket.length = size;
				basket.bIsOwner = true;
				return basket;
			}
			case ESCAPED_UTF8:
			{
				int size = TextTool::getTextSizeStrippedUTF8( (char *) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::stripTextCommands( (char*) m_pChar, p, size+1 );
				basket.ptr = p;
				basket.length = size;
				basket.bIsOwner = true;
				return basket;
			}
	
			case ESCAPED_UTF16:
			{
				int size = TextTool::getTextSizeStrippedUTF8( (Uint16 *) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::stripTextCommandsConvert( (Uint16*) m_pChar, p, m_nbChars+1 );
				basket.ptr = p;
				basket.length = size;
				basket.bIsOwner = true;
				return basket;
			}
	
			case MAP8:
			{
				int size = TextTool::getTextSizeUTF8( (char*) m_pChar, ((CharSeq8*)this)->m_codepage, m_nbChars );
				char * p = new char[size+1];
				TextTool::getTextUTF8( (char*) m_pChar, ((CharSeq8*)this)->m_codepage, p, size+1 );
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
	
	//____ getStdString() _________________________________________________________
	
	std::string CharSeq::getStdString() const
	{
	//	char	temp[512];
	
		switch( m_type )
		{
			case WGCHAR:
			{
				int size = TextTool::getTextSizeUTF8( (Char*) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::getTextUTF8( (Char*) m_pChar, p, size+1 );
				std::string str( p, size );
				delete p;
				return str;
			}
			case UTF8:
			{
				const char * p = (const char *) m_pChar;
				TextTool::forwardCharacters( p, m_nbChars );
				int size = p - (const char *) m_pChar;
				return std::string( p, size );
			}
			case UTF16:
			{
				int size = TextTool::getTextSizeUTF8( (Uint16*) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::getTextUTF8( (Uint16*) m_pChar, p, size+1 );
				std::string str( p, size );
				delete p;
				return str;
			}
			case ESCAPED_UTF8:
			{
				int size = TextTool::getTextSizeStrippedUTF8( (char *) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::stripTextCommands( (char*) m_pChar, p, size+1 );
				std::string str( p, size );
				delete p;
				return str;
			}
	
			case ESCAPED_UTF16:
			{
				int size = TextTool::getTextSizeStrippedUTF8( (Uint16 *) m_pChar, m_nbChars );
				char * p = new char[size+1];
				TextTool::stripTextCommandsConvert( (Uint16*) m_pChar, p, m_nbChars+1 );
				std::string str( p, size );
				delete p;
				return str;
			}
	
			case MAP8:
			{
				int size = TextTool::getTextSizeUTF8( (char*) m_pChar, ((CharSeq8*)this)->m_codepage, m_nbChars );
				char * p = new char[size+1];
				TextTool::getTextUTF8( (char*) m_pChar, ((CharSeq8*)this)->m_codepage, p, size+1 );
				std::string str( p, size );
				delete p;
				return str;
			}
	
			default:					// EMPTY
				return "";
		}
	}
	
	
	
	//____ CharBasket::Destructor __________________________________________________
	
	CharSeq::CharBasket::~CharBasket()
	{
		if( bIsOwner )
		{
			TextTool::derefProps( (Char*) ptr, length );
			delete [] (char*) ptr;
		}
	}
	
	//____ CharSeqLiteral::Constructors _________________________________________
	
	CharSeqLiteral::CharSeqLiteral( const char * pChar)
	{
	    m_type		= UTF8;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= TextTool::countChars(pChar);
	}
	
	CharSeqLiteral::CharSeqLiteral( const char * pChar, int len )
	{
	    m_type		= UTF8;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= len;
	}
	
	CharSeqLiteral::CharSeqLiteral( const Uint16 * pChar )
	{
	    m_type		= UTF16;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= TextTool::strlen(pChar);
	}
	
	CharSeqLiteral::CharSeqLiteral( const Uint16 * pChar, int len )
	{
	    m_type		= UTF16;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= len;
	}
	
	CharSeqLiteral::CharSeqLiteral( const std::string& str )
	{
	    m_type      = UTF8;
	    m_pChar     = (void *) str.c_str();
	    m_nbChars   = str.length();
	}
	
	CharSeqLiteral::CharSeqLiteral( const std::string& str, int ofs, int len )
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
	    m_nbChars   = TextTool::countChars(p+ofs, len);
	}
	
	
	CharSeqLiteral::CharSeqLiteral( const std::wstring& str )
	{
	    m_type      = UTF16;
	    m_pChar     = (void *) str.c_str();
	    m_nbChars   = str.length();
	}
	
	CharSeqLiteral::CharSeqLiteral( const std::wstring& str, int ofs, int len )
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
	
	
	//____ CharSeq8::Constructors _________________________________________
	
	
	CharSeq8::CharSeq8( const char * pChar, WgCodePage codePage )
	{
		m_type 		= MAP8;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= strlen(pChar);
		m_codepage	= codePage;
	}
	
	
	CharSeq8::CharSeq8( const char * pChar, int len, WgCodePage codePage )
	{
		m_type 		= MAP8;
	    m_pChar		= (void *) pChar;
	    m_nbChars	= len;
		m_codepage	= codePage;
	}
	
	CharSeq8::CharSeq8( const std::string& str, WgCodePage codePage )
	{
		m_type 		= MAP8;
	    m_pChar		= str.c_str();
		m_nbChars	= str.length();
		m_codepage	= codePage;
	}
	
	CharSeq8::CharSeq8( const std::string& str, int ofs, int len, WgCodePage codePage )
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
	
	

} // namespace wg
