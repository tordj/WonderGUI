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
//
/// @brief Creates a new buffer of the specified initial capacity.

WgCharBuffer::WgCharBuffer( int size )
{
	if( size < 0 )
		size = 0;

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

void WgCharBuffer::_clearCharsNoDeref( int ofs, int n )
{
	Uint32 * p = (Uint32*) _ptr(ofs);

	for( int i = 0 ; i < (int) n ; i++ )
		* p++ = c_emptyChar;
}


//____ Trim() __________________________________________________________________
//
/// @brief Trims down the buffer to the size of the content, thus removing any extra capacity.

void WgCharBuffer::Trim()
{
	// See if we don't have to do anything...

	if( m_pHead->m_size == m_pHead->m_len )
		return;

	// We need to trim the buffer

	_reshapeBuffer(0,0,m_pHead->m_len,0);
}

//____ TrimWhiteSpace() _______________________________________________________
//
/// @brief Removes white spaces from the beginning and end of the string

void WgCharBuffer::TrimWhiteSpace()
{
}

//____ Reset() _________________________________________________________________
//
/// @brief Deletes all content and reallocates the buffer to the specified capacity.

void WgCharBuffer::Reset( int size )
{
	if( size < 0 )
		size = 0;

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

void WgCharBuffer::_reshapeBuffer( int begMargin, int copyOfs, int copyLen, int endMargin )
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
//
/// @brief Gives access to direct manipulation of the buffer content.
///
/// Any attempt to directly read or manipulate the buffer content needs to be enclosed by calls to BeginWrite()
/// and EndWrite().
///
/// You may NOT call any non-const methods of this buffer until you have called EndWrite() and not write
/// outside the null-terminated content of the buffer even if you know that there is extra capacity.
/// If you need to grow the content before manipulation you can use the appropriate PushFront(), PushBack()
/// or Insert() methods before calling BeginWrite().
///
/// @return Pointer to the content of the buffer, null-terminated.

WgChar*	WgCharBuffer::BeginWrite()
{
	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	m_pHead->m_lockCnt++;
	return (WgChar*) _ptr(0);
}


//____ EndWrite() ______________________________________________________________
//
/// @brief Ends your direct access to the buffer content.

void WgCharBuffer::EndWrite()
{
	m_pHead->m_lockCnt--;
}

//____ PopFront() ______________________________________________________________
//
/// @brief Shrinks the content by removing characters from the front.
///
///	@param 	nChars	Number of characters to remove from the front. May be larger than content
///					in which case all content is removed.
///
/// Removing characters from the front increases the front capacity by the same amount.
///
///	@return Number of characters that was removed from the front of the content.

int WgCharBuffer::PopFront( int nChars )
{
	if( nChars <= 0 )
		return 0;

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
//
/// @brief Shrinks the content by removing characters from the back.
///
///	@param 	nChars	Number of characters to remove from the back. May be larger than content
///					in which case all content is removed.
///
/// Removing characters from the back increases the back capacity by the same amount.
///
///	@return Number of characters that was removed from the back of the content.

int WgCharBuffer::PopBack( int nChars )
{
	if( nChars <= 0 )
		return 0;

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

void WgCharBuffer::_copyChars( BufferHead * pDst, int ofsDst, const BufferHead * pSrc, int ofsSrc, int nChars )
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

void WgCharBuffer::_copyChars( BufferHead * pDst, int ofsDst, const WgChar * pChars, int nChars )
{
	memcpy( ((char*)&pDst[1])+ofsDst*sizeof(WgChar), pChars, nChars*sizeof(WgChar) );
}

//____ _setChars() _____________________________________________________________

void WgCharBuffer::_setChars( int ofs, int nChars, Uint32 value )
{
	Uint32 * pChar = ((Uint32 *) &m_pHead[1]) + m_pHead->m_beg + ofs;

	for( int i = 0 ; i < (int) nChars ; i++ )
		pChar[i] = value;
}

//____ PushFront() _____________________________________________________________

/// @brief Grows the content by pushing a character to the front.
///
///	@param 	character	Character to push to the front.
///
///	@return Number of characters the content was grown with. Always 1.

int WgCharBuffer::PushFront( const WgChar& character )
{
	_pushFront(1);
	*((Uint32*)_ptr(0)) = character.all;

	if( character.properties )
		WgTextpropManager::IncRef(character.properties, 1 );

	return 1;
}


/// @brief Grows the content by pushing whitespace to the front.
///
///	@param 	nChars	Number of whitespace characters to push to the front.
///
///	@return Number of characters the content was grown with. Always equals nChars.

int WgCharBuffer::PushFront( int nChars )
{
	if( nChars <= 0 )
		return 0;

	_pushFront(nChars);
	_setChars( 0, nChars, c_emptyChar );
	return nChars;
}


/// @brief Grows the content by pushing a null-terminated string of characters to the front.
///
///	@param 	pChars	Pointer to string of null-terminated characters to push to the front.
///
///	@return Number of characters the content was grown with.

int WgCharBuffer::PushFront( const WgChar * pChars )
{
	int len = WgTextTool::strlen( pChars );

	_pushFront(len);

	memcpy( _ptr(0), pChars, sizeof(WgChar)*len );
	_refProps(0,len);
	return len;
}


/// @brief Grows the content by pushing a specified number of characters to the front.
///
///	@param 	pChars	Pointer to string of characters to push to the front.
/// @param	nChars	The number of characters to push.
///
/// The string may not end prematurely. If any of the characters pushed contains a null-character the behaviour is undefined.
///
///	@return Number of characters the content was grown with. Always equals nChars.

int WgCharBuffer::PushFront( const WgChar * pChars, int nChars )
{
	if( nChars <= 0 )
		return 0;

	_pushFront(nChars);

	memcpy( _ptr(0), pChars, sizeof(WgChar)*nChars );
	_refProps(0,nChars);
	return nChars;
}


/// @brief Grows the content by pushing a character sequence to the front.
///
///	@param 	seq		Character sequence to be pushed to the front.
///
///	@return Number of characters the content was grown with. Always equals length of the sequence.

int WgCharBuffer::PushFront( const WgCharSeq& seq )
{
	int len = seq.Length();
	_pushFront(len);
	seq.CopyTo((WgChar*)_ptr(0));
	return len;
}


//____ PushBack() ______________________________________________________________
//
/// @brief Grows the content by pushing a character to the back.
///
///	@param 	character	Character to push to the back.
///
///	@return Number of characters the content was grown with. Always 1.

int WgCharBuffer::PushBack( const WgChar& character )
{
	_pushBack(1);
	*((Uint32*)_ptr( m_pHead->m_len - 1)) = character.all;

	if( character.properties )
		WgTextpropManager::IncRef(character.properties, 1 );

	return 1;
}


/// @brief Grows the content by pushing whitespace to the back.
///
///	@param 	nChars	Number of whitespace characters to push to the back.
///
///	@return Number of characters the content was grown with. Always equals nChars.

int WgCharBuffer::PushBack( int nChars )
{
	if( nChars <= 0 )
		return 0;

	_pushBack(nChars);
	_setChars( m_pHead->m_len - nChars, nChars, c_emptyChar );
	return nChars;
}

/// @brief Grows the content by pushing a null-terminated string of characters to the back.
///
///	@param 	pChars	Pointer to string of null-terminated characters to push to the back.
///
///	@return Number of characters the content was grown with.

int WgCharBuffer::PushBack( const WgChar * pChars )
{
	int len = WgTextTool::strlen( pChars );

	_pushBack(len);

	memcpy( ((WgChar*) _ptr(0)) + m_pHead->m_len-len, pChars, sizeof(WgChar)*len );
	_refProps(m_pHead->m_len-len,len);
	return len;
}


/// @brief Grows the content by pushing a specified number of characters to the back.
///
///	@param 	pChars	Pointer to string of characters to push to the back.
///
/// @param	nChars	The number of characters to push.
///
/// The string may not end prematurely. If any of the characters pushed contains a null-character the behaviour is undefined.
///
///	@return Number of characters the content was grown with. Always equals nChars.

int WgCharBuffer::PushBack( const WgChar * pChars, int nChars )
{
	if( nChars <= 0 )
		return 0;

	_pushBack(nChars);

	memcpy( ((WgChar*) _ptr(0)) + m_pHead->m_len-nChars, pChars, sizeof(WgChar)*nChars );
	_refProps(m_pHead->m_len-nChars,nChars);
	return nChars;
}


/// @brief Grows the content by pushing a character sequence to the back.
///
///	@param 	seq		Character sequence to be pushed to the back.
///
///	@return Number of characters the content was grown with. Always equals length of the sequence.

int WgCharBuffer::PushBack( const WgCharSeq& seq )
{
	int nChars = seq.Length();
	_pushBack(nChars);
	memset( ((WgChar*) _ptr(0)) + m_pHead->m_len-nChars, 0, sizeof(WgChar)*nChars );
	seq.CopyTo( ((WgChar*) _ptr(0)) + m_pHead->m_len-nChars );
	return nChars;
}



//____ _pushFront() _____________________________________________________

void WgCharBuffer::_pushFront( int nChars )
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

void WgCharBuffer::_pushBack( int nChars )
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

/// @brief Inserts a character.
///
/// @param ofs		Offset to insert character at, counting from beginning of buffer content.
///
/// @param pChars	Pointer at null-terminated string of characters to insert.
///
/// If ofs is larger than number of characters in buffer the character is pushed on the back.
///
/// @return Number of characters that have been insert.

int WgCharBuffer::Insert( int ofs, const WgChar& character )
{
	if( ofs < 0 )
		ofs = 0;

	_replace( ofs, 0, 1, &character );
	return 1;
}


/// @brief Inserts whitespace characters.
///
/// @param ofs		Offset to insert characters at, counting from beginning of buffer content.
///
/// @param nChars	Number of whitespace characters to insert.
///
/// If ofs is larger than number of characters in buffer the whitespaces are pushed on the back.
///
/// @return Number of characters that have been insert. Always equals nChars.

int WgCharBuffer::Insert( int ofs, int nChars )
{
	if( ofs < 0 )
		ofs = 0;

	if( nChars <= 0 )
		return 0;

	_replace( ofs, 0, nChars );
	return nChars;
}


/// @brief Inserts null-terminated string of characters.
///
/// @param ofs		Offset to insert characters at, counting from beginning of buffer content.
///
/// @param pChars	Pointer at null-terminated string of characters to insert.
///
/// If ofs is larger than number of characters in buffer the content of pChars is pushed on the back.
///
/// @return Number of characters that have been insert. Always equals 1.

int WgCharBuffer::Insert( int ofs, const WgChar * pChars )
{
	if( ofs < 0 )
		ofs = 0;

	int nInsert = WgTextTool::strlen( pChars );
	_replace( ofs, 0, nInsert, pChars );
	return nInsert;
}


/// @brief Inserts a specified number of characters.
///
/// @param ofs		Offset to insert characters at, counting from beginning of buffer content.
///
/// @param pChars	Pointer at string of characters to insert.
///
/// @param nChars	Number of characters to insert.
///
/// The string may not end prematurely. If any of the characters pushed contains a null-character the behaviour is undefined.
/// If ofs is larger than number of characters in buffer the content of pChars is pushed on the back.
///
/// @return Number of characters that have been insert. Always equals nChars.

int WgCharBuffer::Insert( int ofs, const WgChar * pChars, int nChars )
{
	if( ofs < 0 )
		ofs = 0;

	if( nChars <= 0 )
		return 0;

	_replace( ofs, 0, nChars, pChars );
	return nChars;
}

/// @brief Inserts characters from a character sequence.
///
/// @param ofs		Offset to insert characters at, counting from beginning of buffer content.
///
/// @param seq		Character sequence to be inserted.
///
/// If ofs is larger than number of characters in buffer the content of pChars is pushed on the back.
///
/// @return Number of characters that have been insert. Always equals length of sequence.

int WgCharBuffer::Insert( int ofs, const WgCharSeq& seq )
{
	if( ofs < 0 )
		ofs = 0;

	if( ofs > m_pHead->m_len )
		ofs = m_pHead->m_len;

	_replace( ofs, 0, seq.Length() );
	WgChar * pChars = BeginWrite();
	seq.CopyTo( pChars+ofs );
	EndWrite();
	return seq.Length();
}

//____ Delete() ________________________________________________________________
//
/// @brief Deletes characters from the buffer.
///
///
/// @param ofs		Offset to delete characters from, counting from beginning of buffer content.
///
/// @param nChars	Number of characters to delete, counting from ofs. This can be set larger than
///					number of characters available for deletion in which case all content starting
///					from ofs is deleted.
///
/// If ofs is larger than number of characters no characters are deleted.
///
/// @return Number of characters that were deleted.

int WgCharBuffer::Delete( int ofs, int nChars )
{
	if( ofs < 0 )
		ofs = 0;

	if( nChars <= 0 )
		return 0;

	return _replace( ofs, nChars, 0 );
}

//____ Replace() _______________________________________________________________
//
/// @brief Replace a character.
///
/// @param ofs		Offset of character to be replaced, counting from beginning of buffer content.
///
///	@param character	Character to replace buffer content at ofs with.
///
/// If ofs is larger than number of characters then character is pushed on the back of the buffer instead.
///
/// @return Number of characters that were replaced. Always equals 1.

int WgCharBuffer::Replace( int ofs, const WgChar& character )
{
	if( ofs < 0 )
		return 0;

	if( ofs >= m_pHead->m_len )
		PushBack( character );
	else
		*((WgChar*)_ptr(ofs)) = character;
	return 1;
}


/// @brief Replace specified number of characters with (another number of) whitespaces.
///
/// @param ofs		Offset of first character to be replaced, counting from beginning of buffer content.
///
///	@param nDelete	Number of characters to be replaced.
///
/// @param nInsert	Number of whitespaces to replace the characters with.
///
/// If ofs is larger than number of characters nothing will be deleted and the to-be-inserted characters
/// will just be pushed on the back of the buffer. If nDelete is larger than what is left of the buffer
/// everything from ofs and forward is replaced.
///
/// @return Number of characters that were inserted. Always equals nInsert.

int WgCharBuffer::Replace( int ofs, int nDelete, int nInsert )
{
	if( ofs < 0 )
		ofs = 0;

	if( nDelete < 0 )
		nDelete = 0;

	if( nInsert < 0 )
		nInsert = 0;

	_replace( ofs, nDelete, nInsert );
	return nInsert;
}


/// @brief Replace specified number of characters with a specified string.
///
/// @param ofs		Offset of first character to be replaced, counting from beginning of buffer content.
///
///	@param nDelete	Number of characters to be replaced.
///
/// @param pChars	Pointer at null-terminated string to replace the characters with. This string can be
///					longer or shorter than the characters it replaces.
///
/// If ofs is larger than number of characters nothing will be deleted and the to-be-inserted characters
/// will just be pushed on the back of the buffer. If nDelete is larger than what is left of the buffer
/// everything from ofs and forward is replaced.
///
/// @return Number of characters that were inserted.

int WgCharBuffer::Replace( int ofs, int nDelete, const WgChar * pChars )
{
	if( ofs < 0 )
		ofs = 0;
	if( nDelete < 0 )
		nDelete = 0;

	int nInsert = WgTextTool::strlen( pChars );
	_replace( ofs, nDelete, nInsert, pChars );
	return nInsert;
}


/// @brief Replace specified number of characters with a string of specified length.
///
/// @param ofs		Offset of first character to be replaced, counting from beginning of buffer content.
///
///	@param nDelete	Number of characters to be replaced.
///
/// @param pChars	Pointer at a string of characters to replace the characters with.
///
///	@param nInsert 	Length of the string to replace the characters with. This can be
///					longer or shorter than the characters it replaces.
///
/// If ofs is larger than number of characters nothing will be deleted and the to-be-inserted characters
/// will just be pushed on the back of the buffer. If nDelete is larger than what is left of the buffer
/// everything from ofs and forward is replaced.
///
/// @return Number of characters that were inserted. Always equals nInsert.

int WgCharBuffer::Replace( int ofs, int nDelete, const WgChar * pChars, int nInsert)
{
	if( ofs < 0 )
		ofs = 0;
	if( nDelete < 0 )
		nDelete = 0;
	if( nInsert < 0 )
		nInsert = 0;

	_replace( ofs, nDelete, nInsert, pChars );
	return nInsert;
}


/// @brief Replace specified number of characters with content of a char sequence.
///
/// @param ofs		Offset of first character to be replaced, counting from beginning of buffer content.
///
///	@param nDelete	Number of characters to be replaced.
///
/// @param seq		Character sequence to replace with. This can be longer or shorter than the characters it replaces.
///
///	@param nInsert 	Length of the string to replace the characters with. This can be
///					longer or shorter than the characters to replace.
///
/// If ofs is larger than number of characters nothing will be deleted and the to-be-inserted characters
/// will just be pushed on the back of the buffer. If nDelete is larger than what is left of the buffer
/// everything from ofs and forward is replaced.
///
/// @return Number of characters that were inserted. Always equals length of sequence.

int WgCharBuffer::Replace( int ofs, int nDelete, const WgCharSeq& seq )
{
	if( ofs < 0 )
		ofs = 0;
	if( nDelete < 0 )
		nDelete = 0;

	if( ofs > m_pHead->m_len )
		ofs = m_pHead->m_len;

	_replace( ofs, nDelete, seq.Length() );
	WgChar * pChars = BeginWrite();
	seq.CopyTo( pChars+ofs );
	EndWrite();
	return seq.Length();
}




//____ _replace() ______________________________________________________

int WgCharBuffer::_replace( int ofs, int delChar, int addSpace, const WgChar * pChars )
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

WgCharBuffer::BufferHead * WgCharBuffer::_createBuffer( int size )
{
	if( size <= 0 && g_pEmptyBuffer )
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
//
/// @brief	Fills a section of the buffer with specified character.
///
/// @param ch		Character to fill section with.
/// @param ofs		Offset of first character to be replaced, counting from beginning of buffer content.
/// @param len		Number of characters to be replaced.
///
/// Fully replaces the content of the specified buffer section by filling it with the specified
/// character. If the section spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::Fill( const WgChar& ch, int ofs, int len )
{
	if( ofs < 0 || len <= 0 || ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetChars( ch, (WgChar*)_ptr(ofs), len );
}

//____ SetGlyphs() _____________________________________________________________
//
/// @brief	Sets the glyph for a range of characters.
///
/// @param glyph	The glyph to be used. This may not be 0.
/// @param ofs		Offset of first character to have its glyph changed.
/// @param len		Number of characters to have their glyphs changed.
///
/// This method sets the glyph for all characters in the specified range. If the
/// range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::SetGlyphs( Uint16 glyph, int ofs, int len )
{
	if( ofs < 0 || len <= 0 || ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetGlyph( glyph, (WgChar*)_ptr(ofs), len );
}



//___ SetProperties() __________________________________________________________
//
/// @brief	Sets the properties for a range of characters.
///
/// @param	pProp	Pointer to the new properties for the characters.
/// @param	ofs		Offset of first character to have its properties set, counting from beginning of buffer content.
/// @param	len		Number of characters to have their properties set.
///
/// By setting the properties you erase all previous settings of individual
/// properties for the characters, like font, color, style, and underlined.
/// If the range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::SetProperties( const WgTextpropPtr& pProp, int ofs, int len  )
{
	if( ofs < 0 || len <= 0 || ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::SetProperties( pProp, (WgChar*)_ptr(ofs), len );
}

//___ SetColor() _______________________________________________________________
//
/// @brief	Sets the color for a range of characters.
///
/// @param color	The color to be used by the characters.
/// @param ofs		Offset of first character to have its color changed.
/// @param len		Number of characters to have their color changed.
/// @param mode		The style can be changed for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
///
/// This method specifies the color, with which the characters glyphs will
/// be tinted when displayed in the specified mode, for all characters in
/// the specified range. If the range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::SetColor( const WgColor color, int ofs, int len, WgState state )
{
	_modifyProperties( ofs, len, WgTextTool::PropStateColorModifier(color,state) );
}

void WgCharBuffer::SetColor( const WgColor color, int ofs, int len )
{
	_modifyProperties( ofs, len, WgTextTool::PropColorModifier(color) );
}


//___ SetStyle() _______________________________________________________________
//
/// @brief	Sets the style for a range of characters.
///
/// @param style	The style to render the characters in.
/// @param ofs		Offset of first character to have its style changed.
/// @param len		Number of characters to have their style changed.
/// @param mode		The style can be changed for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
///
/// This method specifies the style in which the character is rendered when
/// displayed in the specified mode or all modes.
/// If the range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::SetStyle( WgFontStyle style, int ofs, int len, WgState state )
{
	_modifyProperties( ofs, len, WgTextTool::PropStateStyleModifier(style,state) );
}

void WgCharBuffer::SetStyle( WgFontStyle style, int ofs, int len )
{
	_modifyProperties( ofs, len, WgTextTool::PropStyleModifier(style) );
}


//___ SetFont() ________________________________________________________________
//
/// @brief	Sets the font for a range of characters.
///
/// @param pFont	The font to be used by all characters.
/// @param ofs		Offset of first character to have its font changed.
/// @param len		Number of characters to have their font changed.
///
/// This method sets the font for all characters in the specified range. If the
/// range spans outside the buffer content it will be adjusted properly.
///
/// Setting pFont to null is identical to calling ClearFont().

void WgCharBuffer::SetFont( WgFont * pFont, int ofs, int len )
{
	_modifyProperties( ofs, len, WgTextTool::PropFontModifier(pFont) );
}

//___ SetUnderlined() __________________________________________________________
//
/// @brief	Sets a range of characters to underlined.
///
/// @param ofs		Offset of first character to become underlined.
/// @param len		Number of characters to become underlined.
/// @param mode		The characters can be made underlined for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which makes the characters underlined in all modes.
///
/// Specifying a single mode as underlined doesn't affect whether other modes are underlined or not.
/// If the range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::SetUnderlined( int ofs, int len, WgState state )
{
	_modifyProperties( ofs, len, WgTextTool::PropStateUnderlinedModifier(true,state) );
}

void WgCharBuffer::SetUnderlined( int ofs, int len )
{
	_modifyProperties( ofs, len, WgTextTool::PropUnderlinedModifier(true) );
}

//___ ClearProperties() ________________________________________________________
//
/// @brief	Clears the properties for a range of characters.
///
/// @param ofs		Offset of first character to have its properties cleared.
///
/// @param len		Number of characters to have their properties cleared.
///
/// By clearing the properties you erase all previous settings of individual
/// properties for the characters, like font, color, style and underlined.

void WgCharBuffer::ClearProperties( int ofs, int len  )
{
	if( ofs < 0 || len <= 0 || ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::ClearProperties( (WgChar*)_ptr(ofs), len );
}

//___ ClearColor() _____________________________________________________________
//
/// @brief	Clears the color setting for a range of characters.
///
/// @param ofs		Offset of first character to have its color setting cleared.
/// @param len		Number of characters to have their color settings cleared.
/// @param mode		The color can be cleared for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which clears the color for all modes.
///
/// This method clears the color-property of all characters in the specified range.
/// If the range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::ClearColor( int ofs, int len, WgState state )
{
	_modifyProperties( ofs, len, WgTextTool::PropStateColorClearer(state) );
}

void WgCharBuffer::ClearColor( int ofs, int len )
{
	_modifyProperties( ofs, len, WgTextTool::PropColorClearer() );
}


//___ ClearStyle() _____________________________________________________________
//
/// @brief	Clears the style setting for a range of characters.
///
/// @param ofs		Offset of first character to have its style setting cleared.
/// @param len		Number of characters to have their style settings cleared.
/// @param mode		The style can be cleared for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
///
/// This method clears the style-property of all characters in the specified range.
/// If the range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::ClearStyle( int ofs, int len, WgState state )
{
	_modifyProperties( ofs, len, WgTextTool::PropStateStyleModifier(WG_STYLE_NORMAL,state) );
}

void WgCharBuffer::ClearStyle( int ofs, int len )
{
	_modifyProperties( ofs, len, WgTextTool::PropStyleModifier(WG_STYLE_NORMAL) );
}

//___ ClearFont() ______________________________________________________________
//
/// @brief	Clears the font for a range of characters.
///
/// @param ofs		Offset of first character to have its font property cleared.
/// @param len		Number of characters to have their font properties cleared.
///
/// This method clears the font for all characters in the specified range. If the
/// range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::ClearFont( int ofs, int len  )
{
	_modifyProperties( ofs, len, WgTextTool::PropFontModifier(0) );
}

//___ ClearUnderlined() ________________________________________________________
//
/// @brief	Removes underline from a range of characters.
///
/// @param ofs		Offset of first character to not be underlined anymore.
/// @param len		Number of characters to not be underlined anymore.
/// @param mode		The characters can have their underline removed for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which removes underline from all modes.
///
/// If the range spans outside the buffer content it will be adjusted properly.

void WgCharBuffer::ClearUnderlined( int ofs, int len, WgState state )
{
	_modifyProperties( ofs, len, WgTextTool::PropStateUnderlinedModifier(false,state) );
}

void WgCharBuffer::ClearUnderlined( int ofs, int len )
{
	_modifyProperties( ofs, len, WgTextTool::PropUnderlinedModifier(false) );
}


//____ _modifyProperties() ____________________________________________________

void WgCharBuffer::_modifyProperties( int ofs, int len, const WgTextTool::PropModifier& modif )
{
	if( ofs < 0 || len <= 0 || ofs >= m_pHead->m_len )
		return;

	if( ofs + len > m_pHead->m_len )
		len = m_pHead->m_len - ofs;

	if( m_pHead->m_refCnt > 1 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);

	WgTextTool::ModifyProperties( modif, (WgChar*)_ptr(ofs), len );
}

//____ FindFirst() ____________________________________________________________
//
/// @brief	Finds the first occurence of the specified character sequence in buffer starting from offset.
///
/// @param seq		Sequence of characters to search for.
/// @param ofs		Offset in buffer to start searching from.
///
/// Any character properties such as color, style etc is ignored by the comparison. Only the characters
/// themselves are compared.
///
/// @return Offset in buffer for first match found or -1 if none found.

int WgCharBuffer::FindFirst( const WgCharSeq& _seq, int ofs )
{
	if( ofs < 0 )
		ofs = 0;

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


/// @brief	Finds the first occurence of the specified character in buffer starting from offset.
///
/// @param character	Characters to search for.
/// @param ofs			Offset in buffer to start searching from.
///
/// Any character properties such as color, style etc is ignored by the comparison. Only the characters
/// themselves are compared.
///
/// @return Offset in buffer for first match found or -1 if none found.

int WgCharBuffer::FindFirst( Uint16 character, int ofs )
{
	if( ofs < 0 )
		ofs = 0;

	WgChar *				pBuff	= (WgChar*)_ptr(0);

	while( ofs < m_pHead->m_len )
	{
		if( pBuff[ofs].glyph == character )
			return ofs;

		ofs++;
	}

	return -1;
}
