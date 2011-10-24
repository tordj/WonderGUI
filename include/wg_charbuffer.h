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

#ifndef WG_CHARBUFFER_DOT_H
#define WG_CHARBUFFER_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CHAR_DOT_H
#	include <wg_char.h>
#endif

class WgCharSeq;

//____ WgCharBuffer ____________________________________________________________

/**
	WgCharBuffer is a buffer class providing convenient and efficient ways for
	building and manipulating WgChar-based texts to be displayed. It's essentially
	a buffer for WgChars allocated to a specified capacity in which a string can
	grow and shrink dynamically. The content is always NULL-terminated.

	If a WgCharBuffer exhausts its capacity it will automatically be reallocated
	to the smallest size possible for holding the new content, so there is no risk
	for overflow. However, you should for performance reasons not rely too much
	on this behaviour but allocate a big enough buffer from start and then grow
	it by a suitable amount using SetCapacity() if it starts to run low.
	A WgCharBuffer is never automatically shrunk. If you want to shrink a buffer
	you will have to do it manually by calling Trim() or SetCapacity().

	WgCharBuffers are reference counted copy-on-change objects which can
	share the buffer itself with other WgCharBuffer and  WgString objects.
	When copying a WgCharBuffer to a WgString you should keep in mind that WgString
	always trims its buffer if it has some unused capacity. You should therefore
	trim the WgCharBuffer first if you copy its content to multiple strings without
	any change inbetween.

	Example:

	@code
	WgCharBuffer	buffer(256);
	WgString		str[100];

	buffer.PushBack( "Hello World!" );
	buffer.Trim();
	for( int i = 0 ; i < 100 ; i++ )
		str[i] = buffer;
	@endcode

	The above is fast and efficient, the buffer and all strings points to the same
	memory buffer. No memory allocation or copying of data is done.
	However, if you would remove the Trim() call then you would end up with 100 identical
	memory buffers, all individually allocated and with data copied from the buffer.

	On the other hand, you shouldn't truncate your buffer unnecessarily since it means
	allocation and dealocation of memory (unless it was already trimmed).

	Example:

	@code
	WgCharBuffer	buffer(256);
	WgString		str[3];

	buffer.PushBack( "1... " );
	buffer.Trim();
	str[0] = buffer;
	buffer.PushBack( "2... " );
	buffer.Trim();
	str[1] = buffer;
	buffer.PushBack( "3!" );
	buffer.Trim();
	str[2] = buffer;
	@endcode

	We here get three different strings containing the text "1... ", "1... 2... " and "1... 2... 3!"
	respectively. Trimming the buffer as done in the example is NOT a good idea. It doesn't save us
	anything since each string needs an individual buffer allocated anyway. Even worse, we actually
	add overhead by trimming down the buffer shortly before we need to grow it again to hold more
	content, thus we introduce one unnecessary memory allocation and copying of content for each step.


*/


class WgCharBuffer
{
public:

	WgCharBuffer( Uint32 size = 0 );
	inline WgCharBuffer(const WgCharBuffer& r);

	~WgCharBuffer() { _derefBuffer(); }


    WgCharBuffer& operator=( WgCharBuffer const & r);
	WgCharBuffer& operator=( WgString const & r);

	inline bool operator==(const WgCharBuffer& other) const { return _compareBuffers( this->m_pHead, other.m_pHead ); }
	inline bool operator!=(const WgCharBuffer& other) const { return !_compareBuffers( this->m_pHead, other.m_pHead ); }

	// These operator[] are slow, please use Chars() or BeginWrite() instead.

	WgChar&				operator[](Uint32 i)								{ if( m_pHead->m_refCnt > 1 ) _reshapeBuffer(0,0,m_pHead->m_len,0); return *(WgChar*)_ptr(i); }
	const WgChar&		operator[](Uint32 i) const							{ return *(const WgChar*)_ptr(i); }

	inline operator bool() const { return m_pHead->m_len != 0?true:false; }

	void	Trim();
	void	TrimWhiteSpace();
	void	Reset( Uint32 size = 0 );

	inline void	SetCapacity( Uint32 front, Uint32 back );
	WgChar*	BeginWrite();
	void	EndWrite();

	Uint32	PushFront( Uint32 nChars );
	Uint32	PushFront( const WgChar& character );
	Uint32	PushFront( const WgChar * pChars );
	Uint32	PushFront( const WgChar * pChars, Uint32 nChars );
	Uint32	PushFront( const WgCharSeq& seq );

	Uint32	PopFront( Uint32 nChars );

	Uint32	PushBack( Uint32 nChars );
	Uint32	PushBack( const WgChar& character );
	Uint32	PushBack( const WgChar * pChars );
	Uint32	PushBack( const WgChar * pChars, Uint32 nChars );
	Uint32	PushBack( const WgCharSeq& seq );

	Uint32	PopBack( Uint32 nChars );

	Uint32	Insert( Uint32 ofs, Uint32 nChars );
	Uint32	Insert( Uint32 ofs, const WgChar& character );
	Uint32	Insert( Uint32 ofs, const WgChar * pChars );
	Uint32	Insert( Uint32 ofs, const WgChar * pChars, Uint32 nChars );
	Uint32	Insert( Uint32 ofs, const WgCharSeq& seq );

	Uint32	Delete( Uint32 ofs, Uint32 nChars = 1 );

	Uint32	Replace( Uint32 ofs, const WgChar& character );
	Uint32	Replace( Uint32 ofs, Uint32 nDelete, Uint32 nInsert );
	Uint32	Replace( Uint32 ofs, Uint32 nDelete, const WgChar * pChars );
	Uint32	Replace( Uint32 ofs, Uint32 nDelete, const WgChar * pChars, Uint32 nInsert );
	Uint32	Replace( Uint32 ofs, Uint32 nDelete, const WgCharSeq& seq );

	inline const WgChar * Chars() const;

	inline Uint32		NbChars() const;
	inline Uint32		Length() const;

	inline Uint32		Size() const;

	inline Uint32		FrontCapacity() const;
	inline Uint32		BackCapacity() const;
	inline Uint32		Capacity() const;

	inline void			Fill( const WgChar& ch );
	void			    Fill( const WgChar& ch, Uint32 ofs, Uint32 len );


	inline void			SetGlyphs( Uint16 glyph );
	void			    SetGlyphs( Uint16 glyph, Uint32 ofs, Uint32 len );

	inline void			SetProperties( const WgTextPropPtr& pProp );
	void				SetProperties( const WgTextPropPtr& pProp, Uint32 ofs, Uint32 len);

	inline void			SetFont( WgFont * pFont );
	void				SetFont( WgFont * pFont, Uint32 ofs, Uint32 len );

	inline void			SetColor( const WgColor color, WgMode mode = WG_MODE_ALL );
	void				SetColor( const WgColor color, Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	inline void			SetStyle( WgFontStyle style, WgMode mode = WG_MODE_ALL );
	void				SetStyle( WgFontStyle style, Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	inline void			SetUnderlined( WgMode mode = WG_MODE_ALL );
	void				SetUnderlined( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );


	inline void			Clear();

	inline void 		ClearProperties();
	void				ClearProperties( Uint32 ofs, Uint32 len );

	inline void			ClearFont();
	void				ClearFont( Uint32 ofs, Uint32 len );

	inline void			ClearColor( WgMode mode = WG_MODE_ALL );
	void				ClearColor( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	inline void 		ClearStyle( WgMode mode = WG_MODE_ALL );
	void				ClearStyle( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	inline void			ClearUnderlined( WgMode mode = WG_MODE_ALL );
	void				ClearUnderlined( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );


	int					FindFirst( const WgCharSeq& seq, Uint32 ofs = 0 );
	int					FindFirst( Uint16 character, Uint32 ofs = 0 );


	int					CompareTo( const WgCharBuffer * pBuffer );
	int					CompareGlyphsTo( const WgCharBuffer * pBuffer );
	int					CompareGlyphsIgnoreCaseTo( const WgCharBuffer * pBuffer );



	// Methods mostly for debugging and profiling purposes.

	inline Uint32		RefCount() const;
	inline static Uint32 NbBuffers();

private:

	struct BufferHead
	{
		Uint32		m_lockCnt;
		Uint32		m_refCnt;
		Uint32		m_size;					// Size in number of WgChar of buffer.
		Uint32		m_beg;					// Beginning of chars.
		Uint32		m_len;					// Number of chars.
	};


	void        	_clearCharsNoDeref( Uint32 ofs, Uint32 n );  ///< Clears specified characters in buffer without dereferencing properties.
	inline void *	_ptr( Uint32 ofs ) const { return ((char*) &m_pHead[1]) + sizeof(WgChar)*(m_pHead->m_beg+ofs); }

	void			_pushFront( Uint32 nChars );
	void			_pushBack( Uint32 nChars );
	Uint32			_replace( Uint32 ofs, Uint32 delChar, Uint32 addChar, const WgChar * pChars = 0);



	inline void		_derefBuffer()
	{
		m_pHead->m_refCnt--;
		if( m_pHead->m_refCnt == 0 )
		{
			_derefProps(0, m_pHead->m_len);
			_destroyBuffer(m_pHead);
		}
	}

	BufferHead *	_createBuffer( Uint32 size );
	inline void 	_destroyBuffer( BufferHead * pBuffer ) { delete [] (char*) pBuffer; g_nBuffers--; }

	void			_copyChars( BufferHead * pDst, Uint32 ofsDst, const BufferHead * pSrc, Uint32 ofsSrc, Uint32 nChars );
	void			_copyChars( BufferHead * pDst, Uint32 ofsDst, const WgChar * pChars, Uint32 nChars );
	void			_reshapeBuffer( Uint32 begMargin, Uint32 copyOfs, Uint32 copyLen, Uint32 endMargin );

	void			_setChars( Uint32 ofs, Uint32 nChars, Uint32 value );

	inline void		_derefProps( Uint32 ofs, Uint32 n ) { WgTextTool::DerefProps( (WgChar*) _ptr(ofs), n ); }
	inline void		_refProps( Uint32 ofs, Uint32 n ) { WgTextTool::RefProps( (WgChar*) _ptr(ofs), n ); }

	static bool		_compareBuffers( const BufferHead * p1, const BufferHead * p2 );


	const static Uint32		c_emptyChar = 0x00000020;	// Value to fill out empty WgChars with.
    static int				g_nBuffers;					// Number of real buffers, <= number of WgCharBuffer.
	static	BufferHead *	g_pEmptyBuffer;				// We keep one common empty buffer as an optimization



	BufferHead *	m_pHead;
};



//____ Constructor ____________________________________________________________
//
/// @brief Creates a copy of the specified buffer.

WgCharBuffer::WgCharBuffer(const WgCharBuffer& r)
{
	m_pHead = r.m_pHead;
	m_pHead->m_refCnt++;

	// See if we need to copy the buffer because of write-lock

	if( m_pHead->m_lockCnt != 0 )
		_reshapeBuffer(0,0,m_pHead->m_len,0);
};

//____ SetCapacity() __________________________________________________________
//
/// @brief Reallocates the buffer to hold its content plus the specified extra capacity at the front and back.
/// SetCapacity(0,0) is identical to Trim().
///
///	@param front	The amount of space to leave in the front of the buffer to allow for future growth without
///					having to move the content or reallocate the buffer.
///
///	@param back		The amount of space to leave in the back of the buffer to allow for future growth without
///					having to move the content or reallocate the buffer.
///

void WgCharBuffer::SetCapacity( Uint32 front, Uint32 back )
{
	_reshapeBuffer( front, 0, m_pHead->m_len, back );
}

//____ _Chars() _______________________________________________________________

/// @brief Returns a read-only pointer to the null-terminated content of the buffer.
///
/// The pointer is only valid until a non-const buffer method is called.
///
/// @return Pointer to the null-terminated content of the buffer. A valid pointer is always returned, never null.

const WgChar * WgCharBuffer::Chars() const
{
	return (const WgChar*) _ptr(0);
}

//____ NbChars() ______________________________________________________________

/// @brief Returns the number of characters in the buffer.
///
/// This method is identical to Length()-
///
/// @return Number of characters in the buffer (or length of the buffer content if you so will).

Uint32 WgCharBuffer::NbChars() const
{
	return m_pHead->m_len;
}

//____ Length() _______________________________________________________________

/// @brief Returns the number of characters in the buffer.
///
/// This method is identical to GetNbChars()-
///
/// @return Number of characters in the buffer (or length of the buffer content if you so will).

Uint32 WgCharBuffer::Length() const
{
	return m_pHead->m_len;
}

//____ Size() _________________________________________________________________

/// @brief	Returns the size of the buffer itself, not size of content.
///
///	The size of the buffer equals its capacity when empty.
///
/// @return Number of characters the buffer can hold before it needs to be reallocated.

Uint32 WgCharBuffer::Size() const
{
	return m_pHead->m_size;
}

//____ FrontCapacity() ________________________________________________________

/// @brief 	Returns available space for new characters in the front of the buffer.
///
///	The size of the front capacity determines how many characters you can push to the
/// front before content needs to be reorganized or buffer reallocated.
///
/// @return Number of characters that can be pushed to the front without causing extra overhead.

inline Uint32 WgCharBuffer::FrontCapacity() const
{
	return m_pHead->m_beg;
}

//____ BackCapacity() _________________________________________________________

/// @brief 	Returns available space for new characters in the back of the buffer.
///
///	The size of the back capacity determines how many characters you can push to the
/// back before content needs to be reorganized or buffer reallocated.
///
/// @return Number of characters that can be pushed to the back without causing extra overhead.

Uint32 WgCharBuffer::BackCapacity() const
{
	return m_pHead->m_size - (m_pHead->m_beg + m_pHead->m_len);
}

//____ Capacity() _____________________________________________________________

/// @brief 	Returns total available space for new characters in the buffer.
///
///	This is identical to GetFrontCapacity() + GetBackCapacity() and determines how many characters can
/// be pushed or inserted before a bigger buffer needs to be allocated.
///
/// @return Number of characters that can be added to the buffer without reallocation.

Uint32 WgCharBuffer::Capacity() const
{
	return m_pHead->m_size - m_pHead->m_len;
}

//____ Fill() _________________________________________________________________

/// @brief	Fills the buffer with specified character.
///
/// @param	ch		Character to fill buffer with.
///
/// Fully replaces the content of the buffer by filling it with the specified
/// character. Only the used section of the buffer is filled, unused capacity
/// is not affected.

void WgCharBuffer::Fill( const WgChar& ch )
{
	Fill( ch, 0, INT_MAX );
}

//____ 	SetGlyphs() ___________________________________________________________

/// @brief	Sets the glyph for all characters currently in the buffer.
///
/// @param	glyph	The glyph to be used. This may not be 0.
///
/// Fully replaces the glyph content of the buffer with the specified glyph.
/// Only the used section of the buffer is affected, unused capacity is not
/// filled with the glyph.

void WgCharBuffer::SetGlyphs( Uint16 glyph )
{
	SetGlyphs( glyph, 0, INT_MAX );
}

//____ SetProperties() ________________________________________________________

/// @brief	Sets the properties for all characters currently in the buffer.
///
/// @param	pProp	Pointer to the new properties for the characters.
///
/// By setting the properties you erase all previous settings of individual
/// properties for the characters, like font, color, style and underlined.

void WgCharBuffer::SetProperties( const WgTextPropPtr& pProp )
{
	SetProperties( pProp, 0, INT_MAX );
}

//____ SetFont() ______________________________________________________________
//
/// @brief	Sets the font for all characters currently in the buffer.
///
/// @param pFont	The font to be used by all characters.
///
/// This method modifies the properties of all characters in the buffer so that they
/// all specifies the same font.
///
/// Setting pFont to null is identical to calling ClearFont().

void WgCharBuffer::SetFont( WgFont * pFont )
{
	SetFont( pFont, 0, INT_MAX );
}

//____ SetColor() _____________________________________________________________
//
/// @brief	Sets the color for all characters currently in the buffer.
///
/// @param color	The color to be used by all characters.
///
/// @param mode		The color can be changed for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which changes the color for all modes.
///
/// This method specifies the color with which the characters glyphs will be tinted when displayed in the specified mode
/// or all modes.

void WgCharBuffer::SetColor( const WgColor color, WgMode mode )
{
	SetColor( color, 0, INT_MAX, mode );
}

//____ SetStyle() _____________________________________________________________

/// @brief	Sets the style for all characters currently in the buffer.
///
/// @param style	The style to render the characters in.
/// @param mode		The style can be changed for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
///
/// This method specifies the style in which the character is rendered when displayed in the specified mode
/// or all modes.

void WgCharBuffer::SetStyle( WgFontStyle style, WgMode mode )
{
	SetStyle( style, 0, INT_MAX, mode );
}

//____ SetUnderline() _________________________________________________________

/// @brief Sets all characters currently in the buffer to underlined.
///
/// @param mode		The characters can be made underlined for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which makes the characters underlined in all modes.
///
/// Specifying a single mode as underlined doesn't affect whether other modes are underlined or not.

void WgCharBuffer::SetUnderlined( WgMode mode )
{
	SetUnderlined( 0, INT_MAX, mode );
}

//____ Clear() ________________________________________________________________

/// @brief	Clears the content of the buffer.
///
/// Clears the content of the buffer without resizing it.

void WgCharBuffer::Clear()
{
	Delete( 0, INT_MAX );
}

//____ ClearProperties() ______________________________________________________
//
/// @brief	Clears the properties for all characters currently in the buffer.
///
/// By clearing the properties you erase all previous settings of individual
/// properties for the characters, like font, color, style and underlined.

void WgCharBuffer::ClearProperties()
{
	ClearProperties( 0, INT_MAX );
}


//____ ClearFont() ____________________________________________________________

/// @brief	Clears the font for all characters currently in the buffer.

void WgCharBuffer::ClearFont()
{
	ClearFont( 0, INT_MAX );
}

//____ ClearColor() ___________________________________________________________
//
/// @brief	Clears the color setting for all characters currently in the buffer.
///
/// @param mode		The color can be cleared for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which clears the color for all modes.
///
/// This method clears the color-property of all characters in the buffer.

void WgCharBuffer::ClearColor( WgMode mode )
{
	ClearColor( 0, INT_MAX, mode );
}

//____ ClearStyle() ___________________________________________________________
//
/// @brief	Clears the style setting for all characters currently in the buffer.
///
/// @param mode		The style can be cleared for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which clears the style for all modes.
///
/// This method clears the style-property of all characters in the buffer.

void WgCharBuffer::ClearStyle( WgMode mode )
{
	ClearStyle( 0, INT_MAX, mode );
}

//____ ClearUnderlined() ______________________________________________________
//
/// @brief Removes underline from all characters currently in the buffer.
///
/// @param mode		The characters can have their underline removed from an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which removes underline from the characters in all modes.

void WgCharBuffer::ClearUnderlined( WgMode mode )
{
	ClearUnderlined( 0, INT_MAX, mode );
}


//____ RefCount() _____________________________________________________________
//
/// @brief Returns number of objects referencing the same memory buffer.
///
/// This method only provided for profiling and debugging purposes.
///
/// The same buffer of characters can be used by multiple WgCharBuffer, WgString and WgText
/// objects currently having the same content. This method provides you with the total number of
/// objects sharing the same buffer as this WgCharBuffer, which is included in the count.
///
/// @return Number of objects referencing the same buffer. Always one or higher.

Uint32 WgCharBuffer::RefCount() const
{
	return m_pHead->m_refCnt;
}

//____ NbBuffers() ____________________________________________________________
//
/// @brief Returns total number of memory buffers currently in use.
///
/// This method only provided for profiling and debugging purposes.
///
/// @return Number of memory buffers currently in use, which is @<=
/// total number of WgCharBuffer, WgString and WgText objects combined.

Uint32 WgCharBuffer::NbBuffers()
{
	return g_nBuffers;
}


#endif //WG_CHARBUFFER_DOT_H
