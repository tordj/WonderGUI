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
#ifndef WG_CHARSEQ_DOT_H
#define WG_CHARSEQ_DOT_H

#include <climits>
#include <string>

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef	WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif

class WgChar;
class WgString;
class WgCharBuffer;
class WgTextField;

/*

	TODO:

	- CopyTo() använder sig av WgTextTool::readString() som inte rensar attributes...

	DÄREFTER REKURSIVT ARBETA IGENOM:

	- WgString
	- WgChar (ta bort softbreak-flaggan).
	- WgText (basera på WgCharBuffer).
	- WgText (ändra metodparametrar, utnyttja WgCharSeq och WgString).
	- WgText (inkludera line-variabler).
	- WgText (stöd för varierande radhöjd).

*/




//____ WgCharSeq _______________________________________________________________

class WgCharSeq
{
public:
	WgCharSeq( const char * pChar );
	WgCharSeq( const char * pChar, int len );
	WgCharSeq( const Uint16 * pChar );
	WgCharSeq( const Uint16 * pChar, int len );
	WgCharSeq( const WgChar * pChar );
	WgCharSeq( const WgChar * pChar, int len );
	WgCharSeq( const WgCharBuffer * pBuffer );
	WgCharSeq( const WgCharBuffer * pBuffer, int ofs, int len );
	WgCharSeq( const std::string& str );
	WgCharSeq( const std::string& str, int ofs, int len );
	WgCharSeq( const std::wstring& str );
	WgCharSeq( const std::wstring& str, int ofs, int len );
	WgCharSeq( const WgString& str );
	WgCharSeq( const WgString& str, int ofs, int len );
	WgCharSeq( const WgCharSeq& seq, int ofs = 0, int len = INT_MAX );

	class WgCharBasket
	{
	public:
		friend class WgCharSeq;

		WgCharBasket() {};
		WgCharBasket(const WgCharBasket& r) { ptr = r.ptr ; length = r.length; bIsOwner = r.bIsOwner; r.bIsOwner = false; }
		~WgCharBasket();

		const WgChar *	ptr;
		int				length;

	private:
		WgCharBasket& operator=(const WgCharBasket& r) { ptr = r.ptr ; length = r.length; bIsOwner = r.bIsOwner; r.bIsOwner = false; return *this; }

		mutable bool	bIsOwner;
	};

	class UTF8Basket
	{
	public:
		friend class WgCharSeq;

		UTF8Basket() {};
		UTF8Basket(const UTF8Basket& r) { ptr = r.ptr ; length = r.length; bIsOwner = r.bIsOwner; r.bIsOwner = false; }
		~UTF8Basket() { if( bIsOwner ) delete [] ptr; }

		const char *	ptr;
		int				length;		// Length of char string, NOT number of UTF8 characters!

	private:
		UTF8Basket& operator=(const UTF8Basket& r) { ptr = r.ptr ; length = r.length; bIsOwner = r.bIsOwner; r.bIsOwner = false; return *this; }

		mutable bool	bIsOwner;
	};

	class UnicodeBasket
	{
	public:
		friend class WgCharSeq;

		UnicodeBasket() {};
		UnicodeBasket(const UnicodeBasket& r) { ptr = r.ptr ; length = r.length; bIsOwner = r.bIsOwner; r.bIsOwner = false; }
		~UnicodeBasket() { if( bIsOwner ) delete [] ptr; }

		const Uint16 *	ptr;
		int				length;

	private:
		UnicodeBasket& operator=(const UnicodeBasket& r) { ptr = r.ptr ; length = r.length; bIsOwner = r.bIsOwner; r.bIsOwner = false; return *this; }

		mutable bool	bIsOwner;
	};

	inline int				Length() const { return m_nbChars; }
	int						LengthUTF8() const;
	const WgCharBasket		GetWgChars() const;
	const UnicodeBasket		GetUnicode() const;
	const UTF8Basket		GetUTF8() const;
	std::string				GetStdString() const;
	std::wstring			GetStdWstring() const;
	int						GetNbLines() const;
	void					CopyTo( WgChar * pDest ) const;
//	void					CopyTo( char * pDest ) const;			//TODO: implement.
//	void					CopyTo( Uint16 * pDest ) const;			//TODO: implement.

	void					CopyFormattedTo( char * pDest ) const;
	void					CopyFormattedTo( Uint16 * pDest ) const;

protected:
	WgCharSeq() {};

	enum SeqType
	{
		EMPTY,
		WGCHAR,
		MAP8,
		UTF8,
		UTF16,
		ESCAPED_UTF8,
		ESCAPED_UTF16

	};

	SeqType			m_type;
	const void * 	m_pChar;
	int				m_nbChars;		// Length of sequence in number of characters.
};


class WgCharSeqLiteral : public WgCharSeq
{
public:
	WgCharSeqLiteral( const char * pChar );
	WgCharSeqLiteral( const char * pChar, int len );
	WgCharSeqLiteral( const Uint16 * pChar );
	WgCharSeqLiteral( const Uint16 * pChar, int len );
	WgCharSeqLiteral( const std::string& str );
	WgCharSeqLiteral( const std::string& str, int ofs, int len );
	WgCharSeqLiteral( const std::wstring& str );
	WgCharSeqLiteral( const std::wstring& str, int ofs, int len );
};




class WgCharSeq8 : public WgCharSeq
{
	friend class WgCharSeq;
public:
	WgCharSeq8( const char * pChar, WgCodePage codePage = WG_DEFAULT_CODEPAGE );
	WgCharSeq8( const char * pChar, int len, WgCodePage codePage = WG_DEFAULT_CODEPAGE );
	WgCharSeq8( const std::string& str, WgCodePage codePage = WG_DEFAULT_CODEPAGE );
	WgCharSeq8( const std::string& str, int ofs, int len, WgCodePage codePage = WG_DEFAULT_CODEPAGE );

protected:
	WgCodePage	m_codepage;
};

#endif //WG_CHARSEQ_DOT_H

