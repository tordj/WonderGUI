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

	WgCharBuffer( int size = 0 );
	inline WgCharBuffer(const WgCharBuffer& r);

	~WgCharBuffer() { _derefBuffer(); }


    WgCharBuffer& operator=( WgCharBuffer const & r);
	WgCharBuffer& operator=( WgString const & r);
	WgCharBuffer& operator=( WgCharSeq const & r);

	inline bool operator==(const WgCharBuffer& other) const { return _compareBuffers( this->m_pHead, other.m_pHead ); }
	inline bool operator!=(const WgCharBuffer& other) const { return !_compareBuffers( this->m_pHead, other.m_pHead ); }

	// These operator[] are slow, please use Chars() or BeginWrite() instead.

	WgChar&				operator[](int i)								{ if( m_pHead->m_refCnt > 1 ) _reshapeBuffer(0,0,m_pHead->m_len,0); return *(WgChar*)_ptr(i); }
	const WgChar&		operator[](int i) const							{ return *(const WgChar*)_ptr(i); }

	inline operator bool() const { return m_pHead->m_len != 0?true:false; }

	void	Trim();
	void	TrimWhiteSpace();
	void	Reset( int size = 0 );

	inline void	SetCapacity( int capacity );
	inline void	SetUnusedCapacity( int front, int back );
	WgChar*	BeginWrite();
	void	EndWrite();

	int		PushFront( int nChars );
	int		PushFront( const WgChar& character );
	int		PushFront( const WgChar * pChars );
	int		PushFront( const WgChar * pChars, int nChars );
	int		PushFront( const WgCharSeq& seq );

	int		PopFront( int nChars );

	int		PushBack( int nChars );
	int		PushBack( const WgChar& character );
	int		PushBack( const WgChar * pChars );
	int		PushBack( const WgChar * pChars, int nChars );
	int		PushBack( const WgCharSeq& seq );

	int		PopBack( int nChars );

	int		Insert( int ofs, int nChars );
	int		Insert( int ofs, const WgChar& character );
	int		Insert( int ofs, const WgChar * pChars );
	int		Insert( int ofs, const WgChar * pChars, int nChars );
	int		Insert( int ofs, const WgCharSeq& seq );

	int		Delete( int ofs, int nChars = 1 );

	int		Replace( int ofs, const WgChar& character );
	int		Replace( int ofs, int nDelete, int nInsert );
	int		Replace( int ofs, int nDelete, const WgChar * pChars );
	int		Replace( int ofs, int nDelete, const WgChar * pChars, int nInsert );
	int		Replace( int ofs, int nDelete, const WgCharSeq& seq );

	inline const WgChar * Chars() const;

	inline int			NbChars() const;
	inline int			Length() const;

	inline int			Capacity() const;

	inline int			UnusedFrontCapacity() const;
	inline int			UnusedBackCapacity() const;
	inline int			UnusedCapacity() const;

	inline void			Fill( const WgChar& ch );
	void			    Fill( const WgChar& ch, int ofs, int len );


	inline void			SetGlyphs( Uint16 glyph );
	void			    SetGlyphs( Uint16 glyph, int ofs, int len );

	inline void			SetProperties( const WgTextpropPtr& pProp );
	void				SetProperties( const WgTextpropPtr& pProp, int ofs, int len);

	inline void			SetFont( const WgFontPtr& pFont );
	void				SetFont( const WgFontPtr& pFont, int ofs, int len );

	inline void			SetColor( const WgColor color );
	void				SetColor( const WgColor color, int ofs, int len );
	inline void			SetColor( const WgColor color, WgState state );
	void				SetColor( const WgColor color, int ofs, int len, WgState state );

	inline void			SetStyle( WgFontAlt style );
	void				SetStyle( WgFontAlt style, int ofs, int len );
	inline void			SetStyle( WgFontAlt style, WgState state );
	void				SetStyle( WgFontAlt style, int ofs, int len, WgState state );

	inline void			SetUnderlined();
	void				SetUnderlined( int ofs, int len );
	inline void			SetUnderlined( WgState state );
	void				SetUnderlined( int ofs, int len, WgState state );


	inline void			Clear();

	inline void 		ClearProperties();
	void				ClearProperties( int ofs, int len );

	inline void			ClearFont();
	void				ClearFont( int ofs, int len );

	inline void			ClearColor();
	void				ClearColor( int ofs, int len );
	inline void			ClearColor( WgState state );
	void				ClearColor( int ofs, int len, WgState state );

	inline void 		ClearStyle();
	void				ClearStyle( int ofs, int len );
	inline void 		ClearStyle( WgState state );
	void				ClearStyle( int ofs, int len, WgState state );

	inline void			ClearUnderlined();
	void				ClearUnderlined( int ofs, int len );
	inline void			ClearUnderlined( WgState state );
	void				ClearUnderlined( int ofs, int len, WgState state );


	int					FindFirst( const WgCharSeq& seq, int ofs = 0 );
	int					FindFirst( Uint16 character, int ofs = 0 );


	int					CompareTo( const WgCharBuffer * pBuffer );
	int					CompareGlyphsTo( const WgCharBuffer * pBuffer );
	int					CompareGlyphsIgnoreCaseTo( const WgCharBuffer * pBuffer );



	// Methods mostly for debugging and profiling purposes.

	inline int		RefCount() const;
	inline static int NbBuffers();

private:

	struct BufferHead
	{
		int			m_lockCnt;
		int			m_refCnt;
		int			m_size;					// Size in number of WgChar of buffer.
		int			m_beg;					// Beginning of chars.
		int			m_len;					// Number of chars.
	};


	void        	_clearCharsNoDeref( int ofs, int n );  ///< Clears specified characters in buffer without dereferencing properties.
	inline void *	_ptr( int ofs ) const { return ((char*) &m_pHead[1]) + sizeof(WgChar)*(m_pHead->m_beg+ofs); }

	void			_pushFront( int nChars );
	void			_pushBack( int nChars );
	int				_replace( int ofs, int delChar, int addChar, const WgChar * pChars = 0);



	inline void		_derefBuffer()
	{
		m_pHead->m_refCnt--;
		if( m_pHead->m_refCnt == 0 )
		{
			_derefProps(0, m_pHead->m_len);
			_destroyBuffer(m_pHead);
		}
	}

	BufferHead *	_createBuffer( int size );
	inline void 	_destroyBuffer( BufferHead * pBuffer ) { delete [] (char*) pBuffer; g_nBuffers--; }

	void			_copyChars( BufferHead * pDst, int ofsDst, const BufferHead * pSrc, int ofsSrc, int nChars );
	void			_copyChars( BufferHead * pDst, int ofsDst, const WgChar * pChars, int nChars );
	void			_reshapeBuffer( int begMargin, int copyOfs, int copyLen, int endMargin );

	void			_setChars( int ofs, int nChars, Uint32 value );

	inline void		_derefProps( int ofs, int n ) { WgTextTool::DerefProps( (WgChar*) _ptr(ofs), n ); }
	inline void		_refProps( int ofs, int n ) { WgTextTool::RefProps( (WgChar*) _ptr(ofs), n ); }

	static bool		_compareBuffers( const BufferHead * p1, const BufferHead * p2 );

	void			_modifyProperties( int ofs, int len, const WgTextTool::PropModifier& modif );


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
/// @brief Reallocates the buffer to the specified size.
/// If content doesn't fit in the new size, the content is cut at the end. Any unused capacity
/// will be at the end of the buffer.
///
///	@param capacity	The new size of the buffer.
///

void WgCharBuffer::SetCapacity( int capacity )
{
	if( capacity < 0 )
		capacity = 0;

	if( m_pHead->m_len > capacity )
		_reshapeBuffer( 0, 0, capacity, 0 );
	else
		_reshapeBuffer( 0, 0, m_pHead->m_len, capacity-m_pHead->m_len );
}


//____ SetUnusedCapacity() __________________________________________________________
//
/// @brief Reallocates the buffer to hold its content plus the specified extra capacity at the front and back.
/// SetUnusedCapacity(0,0) is identical to Trim().
///
///	@param front	The amount of space to leave in the front of the buffer to allow for future growth without
///					having to move the content or reallocate the buffer.
///
///	@param back		The amount of space to leave in the back of the buffer to allow for future growth without
///					having to move the content or reallocate the buffer.
///

void WgCharBuffer::SetUnusedCapacity( int front, int back )
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

int WgCharBuffer::NbChars() const
{
	return m_pHead->m_len;
}

//____ Length() _______________________________________________________________

/// @brief Returns the number of characters in the buffer.
///
/// This method is identical to GetNbChars()-
///
/// @return Number of characters in the buffer (or length of the buffer content if you so will).

int WgCharBuffer::Length() const
{
	return m_pHead->m_len;
}

//____ Capacity() _________________________________________________________________

/// @brief	Returns the size of the buffer itself, not size of content.
///
/// @return Number of characters the buffer can hold before it needs to be reallocated.

int WgCharBuffer::Capacity() const
{
	return m_pHead->m_size;
}

//____ UnusedFrontCapacity() ________________________________________________________

/// @brief 	Returns available space for new characters in the front of the buffer.
///
///	The size of the front capacity determines how many characters you can push to the
/// front before content needs to be reorganized or buffer reallocated.
///
/// @return Number of characters that can be pushed to the front without causing extra overhead.

inline int WgCharBuffer::UnusedFrontCapacity() const
{
	return m_pHead->m_beg;
}

//____ UnusedBackCapacity() _________________________________________________________

/// @brief 	Returns available space for new characters in the back of the buffer.
///
///	The size of the back capacity determines how many characters you can push to the
/// back before content needs to be reorganized or buffer reallocated.
///
/// @return Number of characters that can be pushed to the back without causing extra overhead.

int WgCharBuffer::UnusedBackCapacity() const
{
	return m_pHead->m_size - (m_pHead->m_beg + m_pHead->m_len);
}

//____ UnusedCapacity() _____________________________________________________________

/// @brief 	Returns total available space for new characters in the buffer.
///
///	This is identical to UnusedFrontCapacity() + UnusedBackCapacity() and determines how many characters can
/// be pushed or inserted before a bigger buffer needs to be allocated.
///
/// @return Number of characters that can be added to the buffer without reallocation.

int WgCharBuffer::UnusedCapacity() const
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

void WgCharBuffer::SetProperties( const WgTextpropPtr& pProp )
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

void WgCharBuffer::SetFont( const WgFontPtr& pFont )
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

void WgCharBuffer::SetColor( const WgColor color, WgState state )
{
	SetColor( color, 0, INT_MAX, state );
}

void WgCharBuffer::SetColor( const WgColor color )
{
	SetColor( color, 0, INT_MAX );
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

void WgCharBuffer::SetStyle( WgFontAlt style, WgState state )
{
	SetStyle( style, 0, INT_MAX, state );
}

void WgCharBuffer::SetStyle( WgFontAlt style )
{
	SetStyle( style, 0, INT_MAX );
}


//____ SetUnderline() _________________________________________________________

/// @brief Sets all characters currently in the buffer to underlined.
///
/// @param mode		The characters can be made underlined for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which makes the characters underlined in all modes.
///
/// Specifying a single mode as underlined doesn't affect whether other modes are underlined or not.

void WgCharBuffer::SetUnderlined( WgState state )
{
	SetUnderlined( 0, INT_MAX, state );
}

void WgCharBuffer::SetUnderlined()
{
	SetUnderlined( 0, INT_MAX );
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

void WgCharBuffer::ClearColor( WgState state )
{
	ClearColor( 0, INT_MAX, state );
}

void WgCharBuffer::ClearColor()
{
	ClearColor( 0, INT_MAX );
}

//____ ClearStyle() ___________________________________________________________
//
/// @brief	Clears the style setting for all characters currently in the buffer.
///
/// @param mode		The style can be cleared for an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_SPECIAL, which clears the style for all modes.
///
/// This method clears the style-property of all characters in the buffer.

void WgCharBuffer::ClearStyle( WgState state )
{
	ClearStyle( 0, INT_MAX, state );
}

void WgCharBuffer::ClearStyle()
{
	ClearStyle( 0, INT_MAX );
}

//____ ClearUnderlined() ______________________________________________________
//
/// @brief Removes underline from all characters currently in the buffer.
///
/// @param mode		The characters can have their underline removed from an individual mode by specifying it here.
///					This parameter defaults to WG_MODE_ALL, which removes underline from the characters in all modes.

void WgCharBuffer::ClearUnderlined( WgState state )
{
	ClearUnderlined( 0, INT_MAX, state );
}

void WgCharBuffer::ClearUnderlined()
{
	ClearUnderlined( 0, INT_MAX );
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

int WgCharBuffer::RefCount() const
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

int WgCharBuffer::NbBuffers()
{
	return g_nBuffers;
}


#endif //WG_CHARBUFFER_DOT_H
