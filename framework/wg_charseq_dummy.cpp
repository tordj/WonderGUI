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

class WgResDB;

//____ Constructors ____________________________________________________________



WgCharSeq::WgCharSeq( const char * pChar)
{

}

WgCharSeq::WgCharSeq( const char * pChar, int len)
{

}

WgCharSeq::WgCharSeq( const Uint16 * pChar)
{

}
WgCharSeq::WgCharSeq( const Uint16 * pChar, int len)
{

}

WgCharSeq::WgCharSeq( const std::string& str)
{

}

WgCharSeq::WgCharSeq( const std::string& str, int ofs, int len)
{

}

WgCharSeq::WgCharSeq( const std::wstring& str)
{

}

WgCharSeq::WgCharSeq( const std::wstring& str, int ofs, int len)
{

}


WgCharSeq::WgCharSeq( const WgChar * pChar )
{

}

WgCharSeq::WgCharSeq( const WgChar * pChar, int len )
{

}



WgCharSeq::WgCharSeq( const WgCharBuffer * pBuffer )
{

}

WgCharSeq::WgCharSeq( const WgCharBuffer * pBuffer, int ofs, int len )
{

}

WgCharSeq::WgCharSeq( const WgString& str )
{

}

WgCharSeq::WgCharSeq( const WgString& str, int ofs, int len )
{

}

WgCharSeq::WgCharSeq( const WgCharSeq& seq, int ofs, int len )
{

}


//____ LengthUTF8() ___________________________________________________________

int WgCharSeq::LengthUTF8() const
{
  return 0;
}


//____ GetNbLines() ____________________________________________________________

int WgCharSeq::GetNbLines() const
{
  return 0;
}


//___ CopyTo() _________________________________________________________________

void WgCharSeq::CopyTo( WgChar * pDest ) const
{
  return;
}

//____ GetWgChars() ______________________________________________________________

const WgCharSeq::WgCharBasket WgCharSeq::GetWgChars() const
{
  return WgCharSeq::WgCharBasket();
}


//____ GetUnicode() ____________________________________________________________

const WgCharSeq::UnicodeBasket WgCharSeq::GetUnicode() const
{
  return WgCharSeq::UnicodeBasket();
}

//____ GetStdWstring() ____________________________________________________________

std::wstring WgCharSeq::GetStdWstring() const
{
  return std::wstring();
}

//____ GetUTF8() _______________________________________________________________

const WgCharSeq::UTF8Basket WgCharSeq::GetUTF8() const
{
  return WgCharSeq::UTF8Basket();
}

//____ GetStdString() _________________________________________________________

std::string WgCharSeq::GetStdString() const
{
  return "";
}



//____ CharBasket::Destructor __________________________________________________

WgCharSeq::WgCharBasket::~WgCharBasket()
{

}

//____ WgCharSeqLiteral::Constructors _________________________________________

WgCharSeqLiteral::WgCharSeqLiteral( const char * pChar)
{

}

WgCharSeqLiteral::WgCharSeqLiteral( const char * pChar, int len )
{

}

WgCharSeqLiteral::WgCharSeqLiteral( const Uint16 * pChar )
{

}

WgCharSeqLiteral::WgCharSeqLiteral( const Uint16 * pChar, int len )
{

}

WgCharSeqLiteral::WgCharSeqLiteral( const std::string& str )
{

}

WgCharSeqLiteral::WgCharSeqLiteral( const std::string& str, int ofs, int len )
{

}


WgCharSeqLiteral::WgCharSeqLiteral( const std::wstring& str )
{

}

WgCharSeqLiteral::WgCharSeqLiteral( const std::wstring& str, int ofs, int len )
{

}


//____ WgCharSeq8::Constructors _________________________________________


WgCharSeq8::WgCharSeq8( const char * pChar, WgCodePage codePage )
{

}


WgCharSeq8::WgCharSeq8( const char * pChar, int len, WgCodePage codePage )
{

}

WgCharSeq8::WgCharSeq8( const std::string& str, WgCodePage codePage )
{

}

WgCharSeq8::WgCharSeq8( const std::string& str, int ofs, int len, WgCodePage codePage )
{

}


