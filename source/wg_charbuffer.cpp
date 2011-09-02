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
#include <string.h>

#include <wg_charbuffer.h>
#include <wg_string.h>

int WgCharBuffer::g_nBuffers = 0;

WgCharBuffer::BufferHead * WgCharBuffer::g_pEmptyBuffer = 0;


//____ Constructor() ___________________________________________________________

WgCharBuffer::WgCharBuffer( Uint32 size )
{
	m_pHead = _createBuffer( size );
	m_pHead->m_refCnt++;
}

//____ operator=() _____________________________________________________________

WgCharBuffer& WgCharBuffer::operator=( WgCharBuffer const & r)
{
	if( m_pHead != r.m_pHead )
	{
		_derefBuffer();

		m_pHead = r.m_pHead;
		m_pHead->m_refCnt++;

		// See if we need to copy the buffer because of write-lock

		if( m_pHead->m_lockCnt != 0 )
			_reshapeBuffer(0,0,m_pHead->m_len,0);
	}
	return *this;
}

WgCharBuffer& WgCharBuffer::operator=( WgString const & r)
{
	if( m_pHead != r.m_buffer.m_pHead )
	{
		_derefBuffer();

		m_pHead = r.m_buffer.m_pHead;
		m_pHead->m_refCnt++;

		// See if we need to copy the buffer because of write-lock

		if( m_pHead->m_lockCnt != 0 )
			_reshapeBuffer(0,0,m_pHead->m_len,0);
	}
	return *this;
}



//____ _clearCharsNoDeref() _____________________________________________________

void WgCharBuffer::_clearCharsNoDeref( Uint32 ofs, Uint32 n )
{
	Uint32 * p = (Uint32*) _ptr(ofs);

	for( int i = 0 ; i < (int) n ; i++ )
		* p++ = c_emptyChar;
}


//____ Trim() __________________________________________________________________

void WgCharBuffer::Trim()
{
	// See if we don't have to do anything...

	if( m_pHead->m_size == m_pHead->m_len )
		return;

	// We need to trim the buffer

	_reshapeBuffer(0,0,m_pHead->m_len,0);
}

void WgCharBuffer::TrimWhiteSpace()
{
}

//____ Reset() _________________________________________________________________

/**
	Resets the buffer to an intial state of the specified size, deleting all
	content in the process.
*/

void WgCharBuffer::Reset( Uint32 size )
{
	if( m_pHead->m_size == size && m_pHead->m_refCnt == 1 )
	{
		_derefProps( 0, m_pHead->m_len );
		m_pHead->m_beg = 0;
		m_pHead->m_len = 0;

		*((Uint32*) _ptr(0))= 0;	// NULL-terminate content.
	}
	else
	{
		_derefBuffer();
		m_pHead = _createBuffer( size );
		m_pHead->m_refCnt++;
	}
}


//____ _reshapeBuffer() _____________________________________________________________

/**
	Reshapes the buffer by specifying begin/end margins and what subset of content
	to keep.
*/

void WgCharBuffer::_reshapeBuffer( Uint32 begMargin, Uint32 copyOfs, Uint32 copyLen, Uint32 endMargin )
{

	// First create a buffer of right size and
	// copy the content.

	BufferHead * p = _createBuffer( begMargin + copyLen + endMargin );
	_copyChars( p, begMargin, m_pHead, m_pHead->m_beg + copyOfs, copyLen );
	((Uint32*)&p[1])[begMargin + copyLen] = 0;	// Terminate the buffer content.


	p->m_beg = begMargin;
	p->m_len = copyLen;

	// Do our own quick deref of the old buffer.

	m_pHead->m_refCnt--;
	if( m_pHead->m_refCnt == 0 )
	{
		// We only need to dereference the props we haven't copied to the new buffer

		if( copyOfs > 0 )
			_derefProps( 0, copyOfs );

		if( copyOfs + copyLen < m_pHead->m_len )
			_derefProps( copyOfs + copyLen, m_pHead->m_len - copyOfs - copyLen );
		//

		_destroyBuffer(m_pHead);
	}
	else
	{
		// Since the previous buffer wasn't destroyed, we need to reference the new
		// one since we have one extra copy of all the WgChar.

		_refProps( copyOfs, copyLen );		// Easier to scan through the old buffer, content is identical anyway...
	}

	//

	m_pHead = p;
	p->m_refCnt++;
}



//____ BeginWrite() ____________________________________________________________

WgChar*	WgCharBuffer::BeginWrite()
{
	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	m_pHead->m_lockCnt++;
	return (WgChar*) _ptr(0);
}


//____ EndWrite() ______________________________________________________________

void WgCharBuffer::EndWrite()
{
	m_pHead->m_lockCnt--;
}

//____ PopFront() ______________________________________________________________

Uint32 WgCharBuffer::PopFront( Uint32 nChars )
{
	if( nChars > m_pHead->m_len )
		nChars = m_pHead->m_len;

	if( m_pHead->m_refCnt > 1 )
	{
		_reshapeBuffer(0, nChars, m_pHead->m_len - nChars, 0);
	}
	else
	{
		_derefProps( 0, nChars );

		m_pHead->m_beg += nChars;
		m_pHead->m_len -= nChars;
	}

	return nChars;
}

//____ PopBack() _______________________________________________________________

Uint32 WgCharBuffer::PopBack( Uint32 nChars )
{
	if( nChars > m_pHead->m_len )
		nChars = m_pHead->m_len;

	if( m_pHead->m_refCnt > 1 )
	{
		_reshapeBuffer(0, 0, m_pHead->m_len - nChars, 0);
	}
	else
	{
		_derefProps( m_pHead->m_len - nChars, nChars );
		m_pHead->m_len -= nChars;
		* ((Uint32 *) _ptr(m_pHead->m_len)) = 0;		// null terminate.
	}

	return nChars;
}

//____ _copyChars() _____________________________________________________________

/**
	Copies WgChars in buffers from source to destination. Areas may overlap.
	Property references are not affected and will need to be de/referenced
	separately where needed.

	Please note that both offsets in this method start right after header
	and not where m_beg points to!

	No error checking, parameters are assumed to have been verified before call.
*/

void WgCharBuffer::_copyChars( BufferHead * pDst, Uint32 ofsDst, const BufferHead * pSrc, Uint32 ofsSrc, Uint32 nChars )
{
	memmove( ((char*)&pDst[1])+ofsDst*sizeof(WgChar), ((char*)&pSrc[1])+ofsSrc*sizeof(WgChar), nChars*sizeof(WgChar) );
}

/**
	Copies WgChar-array content to buffer. Areas may not overlap.
	Property references are not affected and will need to be de/referenced
	separately where needed.

	Please note that the offset in this method starts right after header
	and not where m_beg points to!

	No error checking, parameters are assumed to have been verified before call.

*/

void WgCharBuffer::_copyChars( BufferHead * pDst, Uint32 ofsDst, const WgChar * pChars, Uint32 nChars )
{
	memcpy( ((char*)&pDst[1])+ofsDst*sizeof(WgChar), pChars, nChars*sizeof(WgChar) );
}

//____ _setChars() _____________________________________________________________

void WgCharBuffer::_setChars( Uint32 ofs, Uint32 nChars, Uint32 value )
{
	Uint32 * pChar = ((Uint32 *) &m_pHead[1]) + m_pHead->m_beg + ofs;

	for( int i = 0 ; i < (int) nChars ; i++ )
		pChar[i] = value;
}

//____ PushFront() _____________________________________________________________

Uint32 WgCharBuffer::PushFront( const WgChar& character )
{
	_pushFront(1);
	*((Uint32*)_ptr(0)) = character.all;

	if( character.properties )
		WgTextPropManager::IncRef(character.properties, 1 );

	return 1;
}

Uint32 WgCharBuffer::PushFront( Uint32 nChars )
{
	_pushFront(nChars);
	_setChars( 0, nChars, c_emptyChar );
	return nChars;
}

Uint32 WgCharBuffer::PushFront( const WgChar * pChars )
{
	Uint32 len = WgTextTool::strlen( pChars );

	_pushFront(len);

	memcpy( _ptr(0), pChars, sizeof(WgChar)*len );
	_refProps(0,len);
	return len;
}

Uint32 WgCharBuffer::PushFront( const WgChar * pChars, Uint32 nChars )
{
	_pushFront(nChars);

	memcpy( _ptr(0), pChars, sizeof(WgChar)*nChars );
	_refProps(0,nChars);
	return nChars;
}

Uint32 WgCharBuffer::PushFront( const WgCharSeq& seq )
{
	Uint32 len = seq.Length();
	_pushFront(len);
	seq.CopyTo((WgChar*)_ptr(0));
	return len;
}


//____ PushBack() ______________________________________________________________

Uint32 WgCharBuffer::PushBack( const WgChar& character )
{
	_pushBack(1);
	*((Uint32*)_ptr( m_pHead->m_len - 1)) = character.all;

	if( character.properties )
		WgTextPropManager::IncRef(character.properties, 1 );

	return 1;
}

Uint32 WgCharBuffer::PushBack( Uint32 nChars )
{
	_pushBack(nChars);
	_setChars( m_pHead->m_len - nChars, nChars, c_emptyChar );
	return nChars;
}

Uint32 WgCharBuffer::PushBack( const WgChar * pChars )
{
	Uint32 len = WgTextTool::strlen( pChars );

	_pushBack(len);

	memcpy( ((WgChar*) _ptr(0)) + m_pHead->m_len-len, pChars, sizeof(WgChar)*len );
	_refProps(m_pHead->m_len-len,len);
	return len;
}

Uint32 WgCharBuffer::PushBack( const WgChar * pChars, Uint32 nChars )
{
	_pushBack(nChars);

	memcpy( ((WgChar*) _ptr(0)) + m_pHead->m_len-nChars, pChars, sizeof(WgChar)*nChars );
	_refProps(m_pHead->m_len-nChars,nChars);
	return nChars;
}

Uint32 WgCharBuffer::PushBack( const WgCharSeq& seq )
{
	Uint32 nChars = seq.Length();
	_pushBack(nChars);
	memset( ((WgChar*) _ptr(0)) + m_pHead->m_len-nChars, 0, sizeof(WgChar)*nChars );
	seq.CopyTo( ((WgChar*) _ptr(0)) + m_pHead->m_len-nChars );
	return nChars;
}



//____ _pushFront() _____________________________________________________

void WgCharBuffer::_pushFront( Uint32 nChars )
{
	// Check if we can just modify this buffer to fit things in

	if( m_pHead->m_refCnt == 1 && m_pHead->m_size >= m_pHead->m_len + nChars )
	{
		// Check if we have enough space at front

		if( m_pHead->m_beg >= nChars )
		{
			m_pHead->m_beg -= nChars;
		}
		else
		{
			// Not enough space at front, need to move chars...

			_copyChars( m_pHead, nChars, m_pHead, m_pHead->m_beg, m_pHead->m_len+1 );
			m_pHead->m_beg = 0;
		}
	}
	else
	{
		_reshapeBuffer(nChars, 0, m_pHead->m_len, 0 );
		m_pHead->m_beg = 0;
	}


	m_pHead->m_len += nChars;
}

//____ _pushBack() ______________________________________________________

void WgCharBuffer::_pushBack( Uint32 nChars )
{
	// Check if we can just modify this buffer to fit things in

	if( m_pHead->m_refCnt == 1 && m_pHead->m_size >= m_pHead->m_len + nChars )
	{
		// Check if we have enough space at end

		if( m_pHead->m_size - (m_pHead->m_beg + m_pHead->m_len) >= nChars )
		{
		}
		else
		{
			// Not enough space at end, need to move chars.
			// Move it right to the beginning of the buffer even if not
			// needed since it seems more likely we will have use of
			// any extra space at the end of the buffer now...

			_copyChars( m_pHead, 0, m_pHead, m_pHead->m_beg, m_pHead->m_len+1 );
			m_pHead->m_beg = 0;
		}
	}
	else
	{
		_reshapeBuffer( 0, 0, m_pHead->m_len, nChars );
		m_pHead->m_beg = 0;
	}

	m_pHead->m_len += nChars;
	* ((Uint32*) _ptr(m_pHead->m_len)) = 0;					// Null terminate the string.
}

//____ Insert() ________________________________________________________________

Uint32 WgCharBuffer::Insert( Uint32 ofs, const WgChar& character )
{
	_replace( ofs, 0, 1, &character );
	return 1;
}

Uint32 WgCharBuffer::Insert( Uint32 ofs, Uint32 nChars )
{
	_replace( ofs, 0, nChars );
	return nChars;
}

Uint32 WgCharBuffer::Insert( Uint32 ofs, const WgChar * pChars )
{
	Uint32 nInsert = WgTextTool::strlen( pChars );
	_replace( ofs, 0, nInsert, pChars );
	return nInsert;
}

Uint32 WgCharBuffer::Insert( Uint32 ofs, const WgChar * pChars, Uint32 nChars )
{
	_replace( ofs, 0, nChars, pChars );
	return nChars;
}

Uint32 WgCharBuffer::Insert( Uint32 ofs, const WgCharSeq& seq )
{
	if( ofs > m_pHead->m_len )
		ofs = m_pHead->m_len;

	_replace( ofs, 0, seq.Length() );
	WgChar * pChars = BeginWrite();
	seq.CopyTo( pChars+ofs );
	EndWrite();
	return seq.Length();
}

//____ Delete() ________________________________________________________________

Uint32 WgCharBuffer::Delete( Uint32 ofs, Uint32 nChars )
{
	return _replace( ofs, nChars, 0 );
}

//____ Replace() _______________________________________________________________

Uint32 WgCharBuffer::Replace( Uint32 ofs, const WgChar& character )
{
	if( ofs >= m_pHead->m_len )
		PushBack( character );
	else
		*((WgChar*)_ptr(ofs)) = character;
	return 1;
}

Uint32 WgCharBuffer::Replace( Uint32 ofs, Uint32 nDelete, Uint32 nInsert )
{
	_replace( ofs, nDelete, nInsert );
	return nInsert;
}

Uint32 WgCharBuffer::Replace( Uint32 ofs, Uint32 nDelete, const WgChar * pChars )
{
	Uint32 nInsert = WgTextTool::strlen( pChars );
	_replace( ofs, nDelete, nInsert, pChars );
	return nInsert;
}

Uint32 WgCharBuffer::Replace( Uint32 ofs, Uint32 nDelete, const WgChar * pChars, Uint32 nInsert)
{
	_replace( ofs, nDelete, nInsert, pChars );
	return nInsert;
}

Uint32 WgCharBuffer::Replace( Uint32 ofs, Uint32 nDelete, const WgCharSeq& seq )
{
	if( ofs > m_pHead->m_len )
		ofs = m_pHead->m_len;

	_replace( ofs, nDelete, seq.Length() );
	WgChar * pChars = BeginWrite();
	seq.CopyTo( pChars+ofs );
	EndWrite();
	return seq.Length();
}




//____ _replace() ______________________________________________________

Uint32 WgCharBuffer::_replace( Uint32 ofs, Uint32 delChar, Uint32 addSpace, const WgChar * pChars )
{
	// Forcing sane values

	if( ofs > m_pHead->m_len )
		ofs = m_pHead->m_len;

	if( delChar > m_pHead->m_len - ofs )
		delChar = m_pHead->m_len - ofs;


	int sizeChange = addSpace - delChar;

	// Dereference the props of the characters to be deleted

	if( delChar != 0 )
		_derefProps( ofs, delChar );

	// Check if we can just modify this buffer to fit things in

	if( m_pHead->m_refCnt == 1 && m_pHead->m_size >= m_pHead->m_len + sizeChange )
	{
		if( sizeChange != 0 )
		{
			// The changes will fit in this buffer. We will first attempt to just
			// move the beginning or end of the buffer to adjust space depending
			// on what would copy the least characters. Only if that fails we
			// move both buffers after which any extra space is at the end of
			// the buffer.

			int begBuffer = m_pHead->m_beg;
			int endBuffer = m_pHead->m_size - (m_pHead->m_beg + m_pHead->m_len);

			if( begBuffer >= sizeChange && ((ofs < m_pHead->m_len/2) || sizeChange > endBuffer) )
			{
				// Just move the beginning to adjust space.

				_copyChars( m_pHead, m_pHead->m_beg-sizeChange, m_pHead, m_pHead->m_beg, ofs );
				m_pHead->m_beg -= sizeChange;
			}
			else if( endBuffer >= sizeChange )
			{
				// Just move the end to adjust space.

				_copyChars( m_pHead, m_pHead->m_beg+ofs+addSpace, m_pHead, m_pHead->m_beg+ofs+delChar, m_pHead->m_len-ofs-delChar+1 );
			}
			else
			{
				// Move both beginning and end to make space

				_copyChars( m_pHead, 0, m_pHead, m_pHead->m_beg, ofs );
				_copyChars( m_pHead, ofs+addSpace, m_pHead, m_pHead->m_beg+ofs+delChar, sizeChange - begBuffer +1 );
				m_pHead->m_beg = 0;
			}

			m_pHead->m_len += sizeChange;
		}
	}
	else
	{
		// Create a new buffer, copy the content to be keept and leave room
		// at ofs for any characters to be added.

		BufferHead * pBuffer = _createBuffer( m_pHead->m_len + sizeChange );
		_copyChars( pBuffer, 0, m_pHead, m_pHead->m_beg, ofs );
		_copyChars( pBuffer, 0+ofs+addSpace, m_pHead, m_pHead->m_beg+ofs+delChar, m_pHead->m_len-ofs-delChar+1);
		pBuffer->m_len = m_pHead->m_len + sizeChange;

		// Do our own deref buffer

		m_pHead->m_refCnt--;
		if( m_pHead->m_refCnt == 0 )
		{
			_destroyBuffer(m_pHead);
			m_pHead = pBuffer;
		}
		else
		{
			m_pHead = pBuffer;
			_setChars( ofs, addSpace, c_emptyChar );
			_refProps( 0, m_pHead->m_len );
		}

		m_pHead->m_refCnt++;
	}

	// Add the chars and reference the props that were added

	if( addSpace != 0 )
	{
		if( pChars != 0 )
		{
			_copyChars( m_pHead, m_pHead->m_beg + ofs, pChars, addSpace );
			_refProps( ofs, addSpace );
		}
		else
			_setChars( ofs, addSpace, c_emptyChar );
	}
	return delChar;
}


//____ _createBuffer() __________________________________________________________

WgCharBuffer::BufferHead * WgCharBuffer::_createBuffer( Uint32 size )
{
	if( size == 0 && g_pEmptyBuffer )
		return g_pEmptyBuffer;

	int nBytes = sizeof(BufferHead) + sizeof(WgChar)*(size+1);

	BufferHead * pBuffer = (BufferHead *) new char[nBytes];

	pBuffer->m_lockCnt	= 0;
	pBuffer->m_beg      = 0;
	pBuffer->m_len      = 0;
	pBuffer->m_refCnt   = 0;
	pBuffer->m_size     = size;

	* ((Uint32 *) &pBuffer[1]) = 0;		// null terminate.
	g_nBuffers++;

	if( size == 0 )
	{
		g_pEmptyBuffer = pBuffer;
		pBuffer->m_refCnt++;
	}

	return pBuffer;
}

//____ _compareBuffers() _______________________________________________________

bool WgCharBuffer::_compareBuffers( const BufferHead * p1, const BufferHead * p2 )
{
	// Some quick compares for common cases

	if( p1 == p2 )
		return true;

	if( p1->m_len != p2->m_len )
		return false;

	// More exhaustive test

	if( 0 == WgTextTool::strcmp( ((const WgChar*)&p1[1]) + sizeof(WgChar)*p1->m_beg, ((const WgChar*)&p2[1]) + sizeof(WgChar)*p2->m_beg ))
		return true;

	return false;
}

//____ CompareTo() ____________________________________________________________

int WgCharBuffer::CompareTo( const WgCharBuffer * pBuffer )
{
	if( m_pHead == pBuffer->m_pHead )
		return 0;

	return WgTextTool::strcmp( (WgChar*) _ptr(0), (WgChar*) pBuffer->_ptr(0) );
}

//____ CompareGlyphsTo() ______________________________________________________

int WgCharBuffer::CompareGlyphsTo( const WgCharBuffer * pBuffer )
{
	if( m_pHead == pBuffer->m_pHead )
		return 0;

	return WgTextTool::glyphcmp( (WgChar*) _ptr(0), (WgChar*) pBuffer->_ptr(0) );
}

//____ CompareGlyphsIgnoreCaseTo() ____________________________________________

int WgCharBuffer::CompareGlyphsIgnoreCaseTo( const WgCharBuffer * pBuffer )
{
	if( m_pHead == pBuffer->m_pHead )
		return 0;

	return WgTextTool::glyphcmpIgnoreCase( (WgChar*) _ptr(0), (WgChar*) pBuffer->_ptr(0) );
}


//____ Fill() __________________________________________________________________

void WgCharBuffer::Fill( const WgChar& ch, Uint32 ofs, Uint32 len )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetChars( ch, (WgChar*)_ptr(ofs), len );
}

//____ SetGlyphs() _____________________________________________________________

void WgCharBuffer::SetGlyphs( Uint16 glyph, Uint32 ofs, Uint32 len )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetGlyph( glyph, (WgChar*)_ptr(ofs), len );
}



//___ SetProperties() __________________________________________________________

void WgCharBuffer::SetProperties( const WgTextPropPtr& pProp, Uint32 ofs, Uint32 len  )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetProperties( pProp, (WgChar*)_ptr(ofs), len );
}

//___ SetColor() _______________________________________________________________

void WgCharBuffer::SetColor( const WgColor color, Uint32 ofs, Uint32 len, WgMode mode )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetColor(color, (WgChar*)_ptr(ofs), len, mode);
}


//___ SetStyle() _______________________________________________________________

void WgCharBuffer::SetStyle( WgFontStyle style, Uint32 ofs, Uint32 len, WgMode mode )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetStyle(style, (WgChar*)_ptr(ofs), len, mode );
}


//___ SetFont() ________________________________________________________________

void WgCharBuffer::SetFont( WgFont * pFont, Uint32 ofs, Uint32 len  )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetFont( pFont, (WgChar*)_ptr(ofs), len );
}

//___ SetUnderlined() __________________________________________________________

void WgCharBuffer::SetUnderlined( Uint32 ofs, Uint32 len, WgMode mode )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetUnderlined( (WgChar*)_ptr(ofs), len, mode );
}


//___ ClearProperties() ________________________________________________________

void WgCharBuffer::ClearProperties( Uint32 ofs, Uint32 len  )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::ClearProperties( (WgChar*)_ptr(ofs), len );
}

//___ ClearColor() _____________________________________________________________

void WgCharBuffer::ClearColor( Uint32 ofs, Uint32 len, WgMode mode )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::ClearColor( (WgChar*)_ptr(ofs), len, mode );
}

//___ ClearStyle() _____________________________________________________________

void WgCharBuffer::ClearStyle( Uint32 ofs, Uint32 len, WgMode mode )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::ClearStyle( (WgChar*)_ptr(ofs), len, mode );
}


//___ ClearFont() ______________________________________________________________

void WgCharBuffer::ClearFont( Uint32 ofs, Uint32 len  )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::ClearFont( (WgChar*)_ptr(ofs), len );
}

//___ ClearUnderlined() ________________________________________________________

void WgCharBuffer::ClearUnderlined( Uint32 ofs, Uint32 len, WgMode mode  )
{
	if( ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::ClearUnderlined( (WgChar*)_ptr(ofs), len, mode );
}

//____ FindFirst() ____________________________________________________________

int WgCharBuffer::FindFirst( const WgCharSeq& _seq, Uint32 ofs )
{
	WgCharSeq::WgCharBasket seq		= _seq.GetWgChars();
	WgChar *				pBuff	= (WgChar*)_ptr(0);

	while( ofs + seq.length <= m_pHead->m_len )
	{
		int i = 0;
		while( i < seq.length )
		{
			if( pBuff[ofs+i].glyph != seq.ptr[i].glyph )
				break;
			i++;
		}

		if( i == seq.length )
			return ofs;

		ofs++;
	}

	return -1;
}

int WgCharBuffer::FindFirst( Uint16 character, Uint32 ofs )
{
	WgChar *				pBuff	= (WgChar*)_ptr(0);

	while( ofs < m_pHead->m_len )
	{
		if( pBuff[ofs].glyph == character )
			return ofs;

		ofs++;
	}

	return -1;
}
