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

#include <wg_textfield.h>

//____ Constructor _____________________________________________________________

WgTextField::WgTextField( WgTextHolder * pHolder ) : WgPresentableField( pHolder )
{
}

//____ clear() _________________________________________________________________

void WgTextField::clear()
{
	int removed = m_charBuffer.length();
	m_charBuffer.clear();
	_presenter()->onTextModified(this, 0, removed, 0 );
}

//___ set() ____________________________________________________________________

void WgTextField::set( const WgCharSeq& seq )
{
	int removed = m_charBuffer.length();
	m_charBuffer = seq;
	_presenter()->onTextModified(this, 0, removed, m_charBuffer.length() );
}

void WgTextField::set( const WgCharBuffer * buffer )
{
	int removed = m_charBuffer.length();
	m_charBuffer = * buffer;
	_presenter()->onTextModified(this, 0, removed, m_charBuffer.length() );
}

void WgTextField::set( const WgString& str )
{
	int removed = m_charBuffer.length();
	m_charBuffer = str;
	_presenter()->onTextModified(this, 0, removed, m_charBuffer.length() );
}

//____ append() ________________________________________________________________

int WgTextField::append( const WgCharSeq& seq )
{
	int ofs = m_charBuffer.length();
	int len = m_charBuffer.pushBack(seq);
	_presenter()->onTextModified(this, ofs, 0, len );
}

//____ insert() ________________________________________________________________

int WgTextField::insert( int ofs, const WgCharSeq& seq )
{
	m_charBuffer.insert(ofs,seq);
	_presenter()->onTextModified(this, ofs, 0, seq.length() );
}

//____ replace() _______________________________________________________________

int WgTextField::replace( int ofs, int nDelete, const WgCharSeq& seq )
{
	m_charBuffer.replace(ofs,nDelete,seq);
	_presenter()->onTextModified(this, ofs, nDelete, seq.length() );
}

//____ delete() ________________________________________________________________

int WgTextField::remove( int ofs, int len )
{
	m_charBuffer.remove(ofs,len);
	_presenter()->onTextModified(this, ofs, len, 0 );	
}

//____ getMarkedLink() _________________________________________________________

WgTextLinkPtr WgTextField::getMarkedLink() const
{
	//TODO: Implement!
}

