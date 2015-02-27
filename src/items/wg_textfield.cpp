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

//____ Clear() _________________________________________________________________

void WgTextField::Clear()
{
	int removed = m_charBuffer.Length();
	m_charBuffer.Clear();
	_presenter()->OnTextModified(this, 0, removed, 0 );
}

//___ Set() ____________________________________________________________________

void WgTextField::Set( const WgCharSeq& seq )
{
	int removed = m_charBuffer.Length();
	m_charBuffer = seq;
	_presenter()->OnTextModified(this, 0, removed, m_charBuffer.Length() );
}

void WgTextField::Set( const WgCharBuffer * buffer )
{
	int removed = m_charBuffer.Length();
	m_charBuffer = * buffer;
	_presenter()->OnTextModified(this, 0, removed, m_charBuffer.Length() );
}

void WgTextField::Set( const WgString& str )
{
	int removed = m_charBuffer.Length();
	m_charBuffer = str;
	_presenter()->OnTextModified(this, 0, removed, m_charBuffer.Length() );
}

//____ Append() ________________________________________________________________

int WgTextField::Append( const WgCharSeq& seq )
{
	int ofs = m_charBuffer.Length();
	int len = m_charBuffer.PushBack(seq);
	_presenter()->OnTextModified(this, ofs, 0, len );
}

//____ Insert() ________________________________________________________________

int WgTextField::Insert( int ofs, const WgCharSeq& seq )
{
	m_charBuffer.Insert(ofs,seq);
	_presenter()->OnTextModified(this, ofs, 0, seq.Length() );
}

//____ Replace() _______________________________________________________________

int WgTextField::Replace( int ofs, int nDelete, const WgCharSeq& seq )
{
	m_charBuffer.Replace(ofs,nDelete,seq);
	_presenter()->OnTextModified(this, ofs, nDelete, seq.Length() );
}

//____ Delete() ________________________________________________________________

int WgTextField::Delete( int ofs, int len )
{
	m_charBuffer.Delete(ofs,len);
	_presenter()->OnTextModified(this, ofs, len, 0 );	
}

//____ GetMarkedLink() _________________________________________________________

WgTextLinkPtr WgTextField::GetMarkedLink() const
{
	//TODO: Implement!
}

