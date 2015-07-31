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

namespace wg 
{
	
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
		it by a suitable amount using setCapacity() if it starts to run low.
		A WgCharBuffer is never automatically shrunk. If you want to shrink a buffer
		you will have to do it manually by calling trim() or setCapacity().
	
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
	
		buffer.pushBack( "Hello World!" );
		buffer.trim();
		for( int i = 0 ; i < 100 ; i++ )
			str[i] = buffer;
		@endcode
	
		The above is fast and efficient, the buffer and all strings points to the same
		memory buffer. No memory allocation or copying of data is done.
		However, if you would remove the trim() call then you would end up with 100 identical
		memory buffers, all individually allocated and with data copied from the buffer.
	
		On the other hand, you shouldn't truncate your buffer unnecessarily since it means
		allocation and dealocation of memory (unless it was already trimmed).
	
		Example:
	
		@code
		WgCharBuffer	buffer(256);
		WgString		str[3];
	
		buffer.pushBack( "1... " );
		buffer.trim();
		str[0] = buffer;
		buffer.pushBack( "2... " );
		buffer.trim();
		str[1] = buffer;
		buffer.pushBack( "3!" );
		buffer.trim();
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
	
		// These operator[] are slow, please use chars() or beginWrite() instead.
	
		WgChar&				operator[](int i)								{ if( m_pHead->m_refCnt > 1 ) _reshapeBuffer(0,0,m_pHead->m_len,0); return *(WgChar*)_ptr(i); }
		const WgChar&		operator[](int i) const							{ return *(const WgChar*)_ptr(i); }
	
		inline operator bool() const { return m_pHead->m_len != 0?true:false; }
	
		void	trim();
		void	trimWhiteSpace();
		void	reset( int size = 0 );
	
		inline void	setCapacity( int capacity );
		inline void	setUnusedCapacity( int front, int back );
		WgChar*	beginWrite();
		void	endWrite();
	
		int		pushFront( int nChars );
		int		pushFront( const WgChar& character );
		int		pushFront( const WgChar * pChars );
		int		pushFront( const WgChar * pChars, int nChars );
		int		pushFront( const WgCharSeq& seq );
	
		int		popFront( int nChars );
	
		int		pushBack( int nChars );
		int		pushBack( const WgChar& character );
		int		pushBack( const WgChar * pChars );
		int		pushBack( const WgChar * pChars, int nChars );
		int		pushBack( const WgCharSeq& seq );
	
		int		popBack( int nChars );
	
		int		insert( int ofs, int nChars );
		int		insert( int ofs, const WgChar& character );
		int		insert( int ofs, const WgChar * pChars );
		int		insert( int ofs, const WgChar * pChars, int nChars );
		int		insert( int ofs, const WgCharSeq& seq );
	
		int		remove( int ofs, int nChars = 1 );
	
		int		replace( int ofs, const WgChar& character );
		int		replace( int ofs, int nDelete, int nInsert );
		int		replace( int ofs, int nDelete, const WgChar * pChars );
		int		replace( int ofs, int nDelete, const WgChar * pChars, int nInsert );
		int		replace( int ofs, int nDelete, const WgCharSeq& seq );
	
		inline const WgChar * chars() const;
	
		inline int			nbChars() const;
		inline int			length() const;
	
		inline int			capacity() const;
	
		inline int			unusedFrontCapacity() const;
		inline int			unusedBackCapacity() const;
		inline int			unusedCapacity() const;
	
		inline void			fill( const WgChar& ch );
		void			    fill( const WgChar& ch, int ofs, int len );
	
	
		inline void			setGlyphs( Uint16 glyph );
		void			    setGlyphs( Uint16 glyph, int ofs, int len );
	
		inline void			setProperties( const WgTextprop_p& pProp );
		void				setProperties( const WgTextprop_p& pProp, int ofs, int len);
	
		inline void			setFont( const WgFont_p& pFont );
		void				setFont( const WgFont_p& pFont, int ofs, int len );
	
		inline void			setColor( const WgColor color );
		void				setColor( const WgColor color, int ofs, int len );
		inline void			setColor( const WgColor color, WgState state );
		void				setColor( const WgColor color, int ofs, int len, WgState state );
	
		inline void			setStyle( WgFontAlt style );
		void				setStyle( WgFontAlt style, int ofs, int len );
		inline void			setStyle( WgFontAlt style, WgState state );
		void				setStyle( WgFontAlt style, int ofs, int len, WgState state );
	
		inline void			setUnderlined();
		void				setUnderlined( int ofs, int len );
		inline void			setUnderlined( WgState state );
		void				setUnderlined( int ofs, int len, WgState state );
	
	
		inline void			clear();
	
		inline void 		clearProperties();
		void				clearProperties( int ofs, int len );
	
		inline void			clearFont();
		void				clearFont( int ofs, int len );
	
		inline void			clearColor();
		void				clearColor( int ofs, int len );
		inline void			clearColor( WgState state );
		void				clearColor( int ofs, int len, WgState state );
	
		inline void 		clearStyle();
		void				clearStyle( int ofs, int len );
		inline void 		clearStyle( WgState state );
		void				clearStyle( int ofs, int len, WgState state );
	
		inline void			clearUnderlined();
		void				clearUnderlined( int ofs, int len );
		inline void			clearUnderlined( WgState state );
		void				clearUnderlined( int ofs, int len, WgState state );
	
	
		int					findFirst( const WgCharSeq& seq, int ofs = 0 );
		int					findFirst( Uint16 character, int ofs = 0 );
	
	
		int					compareTo( const WgCharBuffer * pBuffer );
		int					compareGlyphsTo( const WgCharBuffer * pBuffer );
		int					compareGlyphsIgnoreCaseTo( const WgCharBuffer * pBuffer );
	
	
	
		// Methods mostly for debugging and profiling purposes.
	
		inline int		refCount() const;
		inline static int nbBuffers();
	
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
	
		inline void		_derefProps( int ofs, int n ) { WgTextTool::derefProps( (WgChar*) _ptr(ofs), n ); }
		inline void		_refProps( int ofs, int n ) { WgTextTool::refProps( (WgChar*) _ptr(ofs), n ); }
	
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
	
	//____ setCapacity() __________________________________________________________
	//
	/// @brief Reallocates the buffer to the specified size.
	/// If content doesn't fit in the new size, the content is cut at the end. Any unused capacity
	/// will be at the end of the buffer.
	///
	///	@param capacity	The new size of the buffer.
	///
	
	void WgCharBuffer::setCapacity( int capacity )
	{
		if( capacity < 0 )
			capacity = 0;
	
		if( m_pHead->m_len > capacity )
			_reshapeBuffer( 0, 0, capacity, 0 );
		else
			_reshapeBuffer( 0, 0, m_pHead->m_len, capacity-m_pHead->m_len );
	}
	
	
	//____ setUnusedCapacity() __________________________________________________________
	//
	/// @brief Reallocates the buffer to hold its content plus the specified extra capacity at the front and back.
	/// setUnusedCapacity(0,0) is identical to trim().
	///
	///	@param front	The amount of space to leave in the front of the buffer to allow for future growth without
	///					having to move the content or reallocate the buffer.
	///
	///	@param back		The amount of space to leave in the back of the buffer to allow for future growth without
	///					having to move the content or reallocate the buffer.
	///
	
	void WgCharBuffer::setUnusedCapacity( int front, int back )
	{
		_reshapeBuffer( front, 0, m_pHead->m_len, back );
	}
	
	//____ _Chars() _______________________________________________________________
	
	/// @brief Returns a read-only pointer to the null-terminated content of the buffer.
	///
	/// The pointer is only valid until a non-const buffer method is called.
	///
	/// @return Pointer to the null-terminated content of the buffer. A valid pointer is always returned, never null.
	
	const WgChar * WgCharBuffer::chars() const
	{
		return (const WgChar*) _ptr(0);
	}
	
	//____ nbChars() ______________________________________________________________
	
	/// @brief Returns the number of characters in the buffer.
	///
	/// This method is identical to length()-
	///
	/// @return Number of characters in the buffer (or length of the buffer content if you so will).
	
	int WgCharBuffer::nbChars() const
	{
		return m_pHead->m_len;
	}
	
	//____ length() _______________________________________________________________
	
	/// @brief Returns the number of characters in the buffer.
	///
	/// This method is identical to getNbChars()-
	///
	/// @return Number of characters in the buffer (or length of the buffer content if you so will).
	
	int WgCharBuffer::length() const
	{
		return m_pHead->m_len;
	}
	
	//____ capacity() _________________________________________________________________
	
	/// @brief	Returns the size of the buffer itself, not size of content.
	///
	/// @return Number of characters the buffer can hold before it needs to be reallocated.
	
	int WgCharBuffer::capacity() const
	{
		return m_pHead->m_size;
	}
	
	//____ unusedFrontCapacity() ________________________________________________________
	
	/// @brief 	Returns available space for new characters in the front of the buffer.
	///
	///	The size of the front capacity determines how many characters you can push to the
	/// front before content needs to be reorganized or buffer reallocated.
	///
	/// @return Number of characters that can be pushed to the front without causing extra overhead.
	
	inline int WgCharBuffer::unusedFrontCapacity() const
	{
		return m_pHead->m_beg;
	}
	
	//____ unusedBackCapacity() _________________________________________________________
	
	/// @brief 	Returns available space for new characters in the back of the buffer.
	///
	///	The size of the back capacity determines how many characters you can push to the
	/// back before content needs to be reorganized or buffer reallocated.
	///
	/// @return Number of characters that can be pushed to the back without causing extra overhead.
	
	int WgCharBuffer::unusedBackCapacity() const
	{
		return m_pHead->m_size - (m_pHead->m_beg + m_pHead->m_len);
	}
	
	//____ unusedCapacity() _____________________________________________________________
	
	/// @brief 	Returns total available space for new characters in the buffer.
	///
	///	This is identical to unusedFrontCapacity() + unusedBackCapacity() and determines how many characters can
	/// be pushed or inserted before a bigger buffer needs to be allocated.
	///
	/// @return Number of characters that can be added to the buffer without reallocation.
	
	int WgCharBuffer::unusedCapacity() const
	{
		return m_pHead->m_size - m_pHead->m_len;
	}
	
	//____ fill() _________________________________________________________________
	
	/// @brief	Fills the buffer with specified character.
	///
	/// @param	ch		Character to fill buffer with.
	///
	/// Fully replaces the content of the buffer by filling it with the specified
	/// character. Only the used section of the buffer is filled, unused capacity
	/// is not affected.
	
	void WgCharBuffer::fill( const WgChar& ch )
	{
		fill( ch, 0, INT_MAX );
	}
	
	//____ 	setGlyphs() ___________________________________________________________
	
	/// @brief	Sets the glyph for all characters currently in the buffer.
	///
	/// @param	glyph	The glyph to be used. This may not be 0.
	///
	/// Fully replaces the glyph content of the buffer with the specified glyph.
	/// Only the used section of the buffer is affected, unused capacity is not
	/// filled with the glyph.
	
	void WgCharBuffer::setGlyphs( Uint16 glyph )
	{
		setGlyphs( glyph, 0, INT_MAX );
	}
	
	//____ setProperties() ________________________________________________________
	
	/// @brief	Sets the properties for all characters currently in the buffer.
	///
	/// @param	pProp	Pointer to the new properties for the characters.
	///
	/// By setting the properties you erase all previous settings of individual
	/// properties for the characters, like font, color, style and underlined.
	
	void WgCharBuffer::setProperties( const WgTextprop_p& pProp )
	{
		setProperties( pProp, 0, INT_MAX );
	}
	
	//____ setFont() ______________________________________________________________
	//
	/// @brief	Sets the font for all characters currently in the buffer.
	///
	/// @param pFont	The font to be used by all characters.
	///
	/// This method modifies the properties of all characters in the buffer so that they
	/// all specifies the same font.
	///
	/// Setting pFont to null is identical to calling clearFont().
	
	void WgCharBuffer::setFont( const WgFont_p& pFont )
	{
		setFont( pFont, 0, INT_MAX );
	}
	
	//____ setColor() _____________________________________________________________
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
	
	void WgCharBuffer::setColor( const WgColor color, WgState state )
	{
		setColor( color, 0, INT_MAX, state );
	}
	
	void WgCharBuffer::setColor( const WgColor color )
	{
		setColor( color, 0, INT_MAX );
	}
	
	
	//____ setStyle() _____________________________________________________________
	
	/// @brief	Sets the style for all characters currently in the buffer.
	///
	/// @param style	The style to render the characters in.
	/// @param mode		The style can be changed for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
	///
	/// This method specifies the style in which the character is rendered when displayed in the specified mode
	/// or all modes.
	
	void WgCharBuffer::setStyle( WgFontAlt style, WgState state )
	{
		setStyle( style, 0, INT_MAX, state );
	}
	
	void WgCharBuffer::setStyle( WgFontAlt style )
	{
		setStyle( style, 0, INT_MAX );
	}
	
	
	//____ setUnderline() _________________________________________________________
	
	/// @brief Sets all characters currently in the buffer to underlined.
	///
	/// @param mode		The characters can be made underlined for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which makes the characters underlined in all modes.
	///
	/// Specifying a single mode as underlined doesn't affect whether other modes are underlined or not.
	
	void WgCharBuffer::setUnderlined( WgState state )
	{
		setUnderlined( 0, INT_MAX, state );
	}
	
	void WgCharBuffer::setUnderlined()
	{
		setUnderlined( 0, INT_MAX );
	}
	
	
	//____ clear() ________________________________________________________________
	
	/// @brief	Clears the content of the buffer.
	///
	/// Clears the content of the buffer without resizing it.
	
	void WgCharBuffer::clear()
	{
		remove( 0, INT_MAX );
	}
	
	//____ clearProperties() ______________________________________________________
	//
	/// @brief	Clears the properties for all characters currently in the buffer.
	///
	/// By clearing the properties you erase all previous settings of individual
	/// properties for the characters, like font, color, style and underlined.
	
	void WgCharBuffer::clearProperties()
	{
		clearProperties( 0, INT_MAX );
	}
	
	
	//____ clearFont() ____________________________________________________________
	
	/// @brief	Clears the font for all characters currently in the buffer.
	
	void WgCharBuffer::clearFont()
	{
		clearFont( 0, INT_MAX );
	}
	
	//____ clearColor() ___________________________________________________________
	//
	/// @brief	Clears the color setting for all characters currently in the buffer.
	///
	/// @param mode		The color can be cleared for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which clears the color for all modes.
	///
	/// This method clears the color-property of all characters in the buffer.
	
	void WgCharBuffer::clearColor( WgState state )
	{
		clearColor( 0, INT_MAX, state );
	}
	
	void WgCharBuffer::clearColor()
	{
		clearColor( 0, INT_MAX );
	}
	
	//____ clearStyle() ___________________________________________________________
	//
	/// @brief	Clears the style setting for all characters currently in the buffer.
	///
	/// @param mode		The style can be cleared for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_SPECIAL, which clears the style for all modes.
	///
	/// This method clears the style-property of all characters in the buffer.
	
	void WgCharBuffer::clearStyle( WgState state )
	{
		clearStyle( 0, INT_MAX, state );
	}
	
	void WgCharBuffer::clearStyle()
	{
		clearStyle( 0, INT_MAX );
	}
	
	//____ clearUnderlined() ______________________________________________________
	//
	/// @brief Removes underline from all characters currently in the buffer.
	///
	/// @param mode		The characters can have their underline removed from an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which removes underline from the characters in all modes.
	
	void WgCharBuffer::clearUnderlined( WgState state )
	{
		clearUnderlined( 0, INT_MAX, state );
	}
	
	void WgCharBuffer::clearUnderlined()
	{
		clearUnderlined( 0, INT_MAX );
	}
	
	
	//____ refCount() _____________________________________________________________
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
	
	int WgCharBuffer::refCount() const
	{
		return m_pHead->m_refCnt;
	}
	
	//____ nbBuffers() ____________________________________________________________
	//
	/// @brief Returns total number of memory buffers currently in use.
	///
	/// This method only provided for profiling and debugging purposes.
	///
	/// @return Number of memory buffers currently in use, which is @<=
	/// total number of WgCharBuffer, WgString and WgText objects combined.
	
	int WgCharBuffer::nbBuffers()
	{
		return g_nBuffers;
	}
	
	

} // namespace wg
#endif //WG_CHARBUFFER_DOT_H
