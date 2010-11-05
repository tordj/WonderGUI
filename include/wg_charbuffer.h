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

	/// @brief Creates a new buffer of the specified initial capacity.

	WgCharBuffer( Uint32 size = 0 );

	/// @brief Creates a copy of the specified buffer.

	WgCharBuffer(const WgCharBuffer& r)
	{
		m_pHead = r.m_pHead;
		m_pHead->m_refCnt++;

		// See if we need to copy the buffer because of write-lock

		if( m_pHead->m_lockCnt != 0 )
			ReshapeBuffer(0,0,m_pHead->m_len,0);
	};

	~WgCharBuffer()
	{
		DerefBuffer();
	};


    WgCharBuffer& operator=( WgCharBuffer const & r);
	WgCharBuffer& operator=( WgString const & r);

	inline bool operator==(const WgCharBuffer& other) const { return CompareBuffers( this->m_pHead, other.m_pHead ); }
	inline bool operator!=(const WgCharBuffer& other) const { return !CompareBuffers( this->m_pHead, other.m_pHead ); }

	WgChar&				operator[](Uint32 i)								{ return *(WgChar*)GetPtr(i); }
	const WgChar&		operator[](Uint32 i) const							{ return *(const WgChar*)GetPtr(i); }

	inline operator bool() const { return m_pHead->m_len != 0?true:false; }


	/// @brief Trims down the buffer to the size of the content, thus removing any extra capacity.

	void	Trim();

	/// @brief Removes white spaces from the beginning and end of the string

	void	TrimWhiteSpace();

	/// @brief Deletes all content and reallocates the buffer to the specified capacity.

	void	Reset( Uint32 size = 0 );

	/// @brief Reallocates the buffer to hold its content plus the specified extra capacity at the front and back.
	/// SetCapacity(0,0) is identical to Trim().
	///
	///	@param front	The amount of space to leave in the front of the buffer to allow for future growth without
	///					having to move the content or reallocate the buffer.
	///
	///	@param back		The amount of space to leave in the back of the buffer to allow for future growth without
	///					having to move the content or reallocate the buffer.
	///

	void	SetCapacity( Uint32 front, Uint32 back ) { ReshapeBuffer( front, 0, m_pHead->m_len, back ); }

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

	WgChar*	BeginWrite();

	/// @brief Ends your direct access to the buffer content.

	void	EndWrite();

	/// @brief Grows the content by pushing whitespace to the front.
	///
	///	@param 	nChars	Number of whitespace characters to push to the front.
	///
	///	@return Number of characters the content was grown with. Always equals nChars.

	Uint32	PushFront( Uint32 nChars );

	/// @brief Grows the content by pushing a character to the front.
	///
	///	@param 	character	Character to push to the front.
	///
	///	@return Number of characters the content was grown with. Always 1.

	Uint32	PushFront( const WgChar& character );

	/// @brief Grows the content by pushing a null-terminated string of characters to the front.
	///
	///	@param 	pChars	Pointer to string of null-terminated characters to push to the front.
	///
	///	@return Number of characters the content was grown with.

	Uint32	PushFront( const WgChar * pChars );

	/// @brief Grows the content by pushing a specified number of characters to the front.
	///
	///	@param 	pChars	Pointer to string of characters to push to the front.
	///
	/// @param	nChars	The number of characters to push.
	///
	/// The string may not end prematurely. If any of the characters pushed contains a null-character the behaviour is undefined.
	///
	///	@return Number of characters the content was grown with. Always equals nChars.

	Uint32	PushFront( const WgChar * pChars, Uint32 nChars );

	/// @brief Grows the content by pushing a character sequence to the front.
	///
	///	@param 	seq		Character sequence to be pushed to the front.
	///
	///	@return Number of characters the content was grown with. Always equals length of the sequence.

	Uint32	PushFront( const WgCharSeq& seq );

	/// @brief Shrinks the content by removing characters from the front.
	///
	///	@param 	nChars	Number of characters to remove from the front. May be larger than content
	///					in which case all content is removed.
	///
	/// Removing characters from the front increases the front capacity by the same amount.
	///
	///	@return Number of characters that was removed from the front of the content.

	Uint32	PopFront( Uint32 nChars );

	/// @brief Grows the content by pushing whitespace to the back.
	///
	///	@param 	nChars	Number of whitespace characters to push to the back.
	///
	///	@return Number of characters the content was grown with. Always equals nChars.

	Uint32	PushBack( Uint32 nChars );

	/// @brief Grows the content by pushing a character to the back.
	///
	///	@param 	character	Character to push to the back.
	///
	///	@return Number of characters the content was grown with. Always 1.

	Uint32	PushBack( const WgChar& character );

	/// @brief Grows the content by pushing a null-terminated string of characters to the back.
	///
	///	@param 	pChars	Pointer to string of null-terminated characters to push to the back.
	///
	///	@return Number of characters the content was grown with.

	Uint32	PushBack( const WgChar * pChars );

	/// @brief Grows the content by pushing a specified number of characters to the back.
	///
	///	@param 	pChars	Pointer to string of characters to push to the back.
	///
	/// @param	nChars	The number of characters to push.
	///
	/// The string may not end prematurely. If any of the characters pushed contains a null-character the behaviour is undefined.
	///
	///	@return Number of characters the content was grown with. Always equals nChars.

	Uint32	PushBack( const WgChar * pChars, Uint32 nChars );

	/// @brief Grows the content by pushing a character sequence to the back.
	///
	///	@param 	seq		Character sequence to be pushed to the back.
	///
	///	@return Number of characters the content was grown with. Always equals length of the sequence.

	Uint32	PushBack( const WgCharSeq& seq );

	/// @brief Shrinks the content by removing characters from the back.
	///
	///	@param 	nChars	Number of characters to remove from the back. May be larger than content
	///					in which case all content is removed.
	///
	/// Removing characters from the back increases the back capacity by the same amount.
	///
	///	@return Number of characters that was removed from the back of the content.

	Uint32	PopBack( Uint32 nChars );

	/// @brief Inserts whitespace characters.
	///
	/// @param ofs		Offset to insert characters at, counting from beginning of buffer content.
	///
	/// @param nChars	Number of whitespace characters to insert.
	///
	/// If ofs is larger than number of characters in buffer the whitespaces are pushed on the back.
	///
	/// @return Number of characters that have been insert. Always equals nChars.

	Uint32	Insert( Uint32 ofs, Uint32 nChars );

	/// @brief Inserts a character.
	///
	/// @param ofs		Offset to insert character at, counting from beginning of buffer content.
	///
	/// @param pChars	Pointer at null-terminated string of characters to insert.
	///
	/// If ofs is larger than number of characters in buffer the character is pushed on the back.
	///
	/// @return Number of characters that have been insert.

	Uint32	Insert( Uint32 ofs, const WgChar& character );

	/// @brief Inserts null-terminated string of characters.
	///
	/// @param ofs		Offset to insert characters at, counting from beginning of buffer content.
	///
	/// @param pChars	Pointer at null-terminated string of characters to insert.
	///
	/// If ofs is larger than number of characters in buffer the content of pChars is pushed on the back.
	///
	/// @return Number of characters that have been insert. Always equals 1.

	Uint32	Insert( Uint32 ofs, const WgChar * pChars );

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

	Uint32	Insert( Uint32 ofs, const WgChar * pChars, Uint32 nChars );

	/// @brief Inserts characters from a character sequence.
	///
	/// @param ofs		Offset to insert characters at, counting from beginning of buffer content.
	///
	/// @param seq		Character sequence to be inserted.
	///
	/// If ofs is larger than number of characters in buffer the content of pChars is pushed on the back.
	///
	/// @return Number of characters that have been insert. Always equals length of sequence.

	Uint32	Insert( Uint32 ofs, const WgCharSeq& seq );

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

	Uint32	Delete( Uint32 ofs, Uint32 nChars = 1 );

	/// @brief Replace a character.
	///
	/// @param ofs		Offset of character to be replaced, counting from beginning of buffer content.
	///
	///	@param character	Character to replace buffer content at ofs with.
	///
	/// If ofs is larger than number of characters then character is pushed on the back of the buffer instead.
	///
	/// @return Number of characters that were replaced. Always equals 1.

	Uint32	Replace( Uint32 ofs, const WgChar& character );

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

	Uint32	Replace( Uint32 ofs, Uint32 nDelete, Uint32 nInsert );

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

	Uint32	Replace( Uint32 ofs, Uint32 nDelete, const WgChar * pChars );

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

	Uint32	Replace( Uint32 ofs, Uint32 nDelete, const WgChar * pChars, Uint32 nInsert );

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

	Uint32	Replace( Uint32 ofs, Uint32 nDelete, const WgCharSeq& seq );

	/// @brief Returns a read-only pointer to the null-terminated content of the buffer.
	///
	/// The pointer is only valid until a non-const buffer method is called.
	///
	/// @return Pointer to the null-terminated content of the buffer. A valid pointer is always returned, never null.


	inline const WgChar * Chars() const { return (const WgChar*) GetPtr(0); }

	/// @brief Returns the number of characters in the buffer.
	///
	/// This method is identical to Length()-
	///
	/// @return Number of characters in the buffer (or length of the buffer content if you so will).

	inline Uint32		NbChars() const { return m_pHead->m_len; }

	/// @brief Returns the number of characters in the buffer.
	///
	/// This method is identical to GetNbChars()-
	///
	/// @return Number of characters in the buffer (or length of the buffer content if you so will).

	inline Uint32		Length() const { return m_pHead->m_len; }

	/// @brief	Returns the size of the buffer itself, not size of content.
	///
	///	The size of the buffer equals its capacity when empty.
	///
	/// @return Number of characters the buffer can hold before it needs to be reallocated.

	inline Uint32		Size() const { return m_pHead->m_size; }

	/// @brief 	Returns available space for new characters in the front of the buffer.
	///
	///	The size of the front capacity determines how many characters you can push to the
	/// front before content needs to be reorganized or buffer reallocated.
	///
	/// @return Number of characters that can be pushed to the front without causing extra overhead.

	inline Uint32		FrontCapacity() const { return m_pHead->m_beg; }

	/// @brief 	Returns available space for new characters in the back of the buffer.
	///
	///	The size of the back capacity determines how many characters you can push to the
	/// back before content needs to be reorganized or buffer reallocated.
	///
	/// @return Number of characters that can be pushed to the back without causing extra overhead.

	inline Uint32		BackCapacity() const { return m_pHead->m_size - (m_pHead->m_beg + m_pHead->m_len); }

	/// @brief 	Returns total available space for new characters in the buffer.
	///
	///	This is identical to GetFrontCapacity() + GetBackCapacity() and determines how many characters can
	/// be pushed or inserted before a bigger buffer needs to be allocated.
	///
	/// @return Number of characters that can be added to the buffer without reallocation.

	inline Uint32		Capacity() const { return m_pHead->m_size - m_pHead->m_len; }


	/// @brief	Fills the buffer with specified character.
	///
	/// @param	ch		Character to fill buffer with.
	///
	/// Fully replaces the content of the buffer by filling it with the specified
	/// character. Only the used section of the buffer is filled, unused capacity
	/// is not affected.

	inline void			Fill( const WgChar& ch )												{ Fill( ch, 0, 0xFFFFFFFF ); }

	/// @brief	Fills a section of the buffer with specified character.
	///
	/// @param ch		Character to fill section with.
	///
	/// @param ofs		Offset of first character to be replaced, counting from beginning of buffer content.
	///
	/// @param len		Number of characters to be replaced.
	///
	/// Fully replaces the content of the specified buffer section by filling it with the specified
	/// character. If the section spans outside the buffer content it will be adjusted properly.

	void			    Fill( const WgChar& ch, Uint32 ofs, Uint32 len );

	/// @brief	Sets the glyph for all characters currently in the buffer.
	///
	/// @param	glyph	The glyph to be used. This may not be 0.
	///
	/// Fully replaces the glyph content of the buffer with the specified glyph.
	/// Only the used section of the buffer is affected, unused capacity is not
	/// filled with the glyph.

	inline void			SetGlyphs( Uint16 glyph )												{ SetGlyphs( glyph, 0, 0xFFFFFFFF ); }

	/// @brief	Sets the glyph for a range of characters.
	///
	/// @param glyph	The glyph to be used. This may not be 0.
	///
	/// @param ofs		Offset of first character to have its glyph changed.
	///
	/// @param len		Number of characters to have their glyphs changed.
	///
	/// This method sets the glyph for all characters in the specified range. If the
	/// range spans outside the buffer content it will be adjusted properly.

	void			    SetGlyphs( Uint16 glyph, Uint32 ofs, Uint32 len );

	/// @brief	Sets the properties for all characters currently in the buffer.
	/// @param	pProp	Pointer to the new properties for the characters.
	///
	/// By setting the properties you erase all previous settings of individual
	/// properties for the characters, like font, color, style and underlined.

	inline void			SetProperties( const WgTextPropPtr& pProp )								{ SetProperties( pProp, 0, 0xFFFFFFFF ); }

	/// @brief	Sets the properties for a range of characters.
	///
	/// @param	pProp	Pointer to the new properties for the characters.
	///
	/// @param ofs		Offset of first character to have its properties set, counting from beginning of buffer content.
	///
	/// @param len		Number of characters to have their properties set.
	///
	/// By setting the properties you erase all previous settings of individual
	/// properties for the characters, like font, color, style, and underlined.
	/// If the range spans outside the buffer content it will be adjusted properly.

	void				SetProperties( const WgTextPropPtr& pProp, Uint32 ofs, Uint32 len);

	/// @brief	Sets the font for all characters currently in the buffer.
	///
	/// @param pFont	The font to be used by all characters.
	///
	/// This method modifies the properties of all characters in the buffer so that they
	/// all specifies the same font.
	///
	/// Setting pFont to null is identical to calling ClearFont().

	void				SetFont( WgFont * pFont )												{ SetFont( pFont, 0, 0xFFFFFFFF ); }

	/// @brief	Sets the font for a range of characters.
	///
	/// @param pFont	The font to be used by all characters.
	///
	/// @param ofs		Offset of first character to have its font changed.
	///
	/// @param len		Number of characters to have their font changed.
	///
	/// This method sets the font for all characters in the specified range. If the
	/// range spans outside the buffer content it will be adjusted properly.
	///
	/// Setting pFont to null is identical to calling ClearFont().

	void				SetFont( WgFont * pFont, Uint32 ofs, Uint32 len );


	/// @brief	Sets the color for all characters currently in the buffer.
	///
	/// @param color	The color to be used by all characters.
	///
	/// @param mode		The color can be changed for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which changes the color for all modes.
	///
	/// This method specifies the color with which the characters glyphs will be tinted when displayed in the specified mode
	/// or all modes.

	inline void			SetColor( const WgColor color, WgMode mode = WG_MODE_ALL )				{ SetColor( color, 0, 0xFFFFFFFF, mode ); }

	/// @brief	Sets the color for a range of characters.
	///
	/// @param color	The color to be used by the characters.
	///
	/// @param ofs		Offset of first character to have its color changed.
	///
	/// @param len		Number of characters to have their color changed.
	///
	/// @param mode		The style can be changed for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
	///
	/// This method specifies the color, with which the characters glyphs will
	/// be tinted when displayed in the specified mode, for all characters in
	/// the specified range. If the range spans outside the buffer content it will be adjusted properly.

	void				SetColor( const WgColor color, Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	/// @brief	Sets the style for all characters currently in the buffer.
	///
	/// @param style	The style to render the characters in.
	///
	/// @param mode		The style can be changed for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
	///
	/// This method specifies the style in which the character is rendered when displayed in the specified mode
	/// or all modes.

	inline void			SetStyle( WgFontStyle style, WgMode mode = WG_MODE_ALL )				{ SetStyle( style, 0, 0xFFFFFFFF, mode ); }

	/// @brief	Sets the style for a range of characters.
	///
	/// @param style	The style to render the characters in.
	///
	/// @param ofs		Offset of first character to have its style changed.
	///
	/// @param len		Number of characters to have their style changed.
	///
	/// @param mode		The style can be changed for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
	///
	/// This method specifies the style in which the character is rendered when
	/// displayed in the specified mode or all modes.
	/// If the range spans outside the buffer content it will be adjusted properly.

	void				SetStyle( WgFontStyle style, Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	/// @brief Sets all characters currently in the buffer to underlined.
	///
	/// @param mode		The characters can be made underlined for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which makes the characters underlined in all modes.
	///
	/// Specifying a single mode as underlined doesn't affect whether other modes are underlined or not.

	inline void			SetUnderlined( WgMode mode = WG_MODE_ALL )								{ SetUnderlined( 0, 0xFFFFFFFF, mode ); }

	/// @brief	Sets a range of characters to underlined.
	///
	/// @param ofs		Offset of first character to become underlined.
	///
	/// @param len		Number of characters to become underlined.
	///
	/// @param mode		The characters can be made underlined for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which makes the characters underlined in all modes.
	///
	/// Specifying a single mode as underlined doesn't affect whether other modes are underlined or not.
	/// If the range spans outside the buffer content it will be adjusted properly.

	void				SetUnderlined( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	/// @brief	Clears the content of the buffer.
	///
	/// Clears the content of the buffer without resizing it.

	inline void			Clear() { Delete( 0, 0xFFFFFFFF ); }

	/// @brief	Clears the properties for all characters currently in the buffer.
	///
	/// By clearing the properties you erase all previous settings of individual
	/// properties for the characters, like font, color, style and underlined.

	inline void			ClearProperties()														{ ClearProperties( 0, 0xFFFFFFFF ); }

	/// @brief	Clears the properties for a range of characters.
	///
	/// @param ofs		Offset of first character to have its properties cleared.
	///
	/// @param len		Number of characters to have their properties cleared.
	///
	/// By clearing the properties you erase all previous settings of individual
	/// properties for the characters, like font, color, style and underlined.

	void				ClearProperties( Uint32 ofs, Uint32 len );

	/// @brief	Clears the font for all characters currently in the buffer.

	inline void			ClearFont()																{ ClearFont( 0, 0xFFFFFFFF ); }

	/// @brief	Clears the font for a range of characters.
	///
	/// @param ofs		Offset of first character to have its font property cleared.
	///
	/// @param len		Number of characters to have their font properties cleared.
	///
	/// This method clears the font for all characters in the specified range. If the
	/// range spans outside the buffer content it will be adjusted properly.

	void				ClearFont( Uint32 ofs, Uint32 len );

	/// @brief	Clears the color setting for all characters currently in the buffer.
	///
	/// @param mode		The color can be cleared for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which clears the color for all modes.
	///
	/// This method clears the color-property of all characters in the buffer.

	inline void			ClearColor( WgMode mode = WG_MODE_ALL )									{ ClearColor( 0, 0xFFFFFFFF, mode ); }

	/// @brief	Clears the color setting for a range of characters.
	///
	/// @param ofs		Offset of first character to have its color setting cleared.
	///
	/// @param len		Number of characters to have their color settings cleared.
	///
	/// @param mode		The color can be cleared for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which clears the color for all modes.
	///
	/// This method clears the color-property of all characters in the specified range.
	/// If the range spans outside the buffer content it will be adjusted properly.

	void				ClearColor( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	/// @brief	Clears the style setting for all characters currently in the buffer.
	///
	/// @param mode		The style can be cleared for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which clears the style for all modes.
	///
	/// This method clears the style-property of all characters in the buffer.

	inline void			ClearStyle( WgMode mode = WG_MODE_ALL )									{ ClearStyle( 0, 0xFFFFFFFF, mode ); }

	/// @brief	Clears the style setting for a range of characters.
	///
	/// @param ofs		Offset of first character to have its style setting cleared.
	///
	/// @param len		Number of characters to have their style settings cleared.
	///
	/// @param mode		The style can be cleared for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which changes the style for all modes.
	///
	/// This method clears the style-property of all characters in the specified range.
	/// If the range spans outside the buffer content it will be adjusted properly.

	void				ClearStyle( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );

	/// @brief Removes underline from all characters currently in the buffer.
	///
	/// @param mode		The characters can have their underline removed from an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which removes underline from the characters in all modes.

	inline void			ClearUnderlined( WgMode mode = WG_MODE_ALL )							{ ClearUnderlined( 0, 0xFFFFFFFF, mode ); }

	/// @brief	Removes underline from a range of characters.
	///
	/// @param ofs		Offset of first character to not be underlined anymore.
	///
	/// @param len		Number of characters to not be underlined anymore.
	///
	/// @param mode		The characters can have their underline removed for an individual mode by specifying it here.
	///					This parameter defaults to WG_MODE_ALL, which removes underline from all modes.
	///
	/// If the range spans outside the buffer content it will be adjusted properly.

	void				ClearUnderlined( Uint32 ofs, Uint32 len, WgMode mode = WG_MODE_ALL );


	/// @brief	Finds the first occurence of the specified character sequence in buffer starting from offset.
	///
	/// @param seq		Sequence of characters to search for.
	///
	/// @param ofs		Offset in buffer to start searching from.
	///
	/// Any character properties such as color, style etc is ignored by the comparison. Only the characters
	/// themselves are compared.
	///
	/// @return Offset in buffer of found match or -1 if none found.

	int					FindFirst( const WgCharSeq& seq, Uint32 ofs = 0 );

	int					FindFirst( Uint16 character, Uint32 ofs = 0 );


	int					CompareTo( const WgCharBuffer * pBuffer );
	int					CompareGlyphsTo( const WgCharBuffer * pBuffer );
	int					CompareGlyphsIgnoreCaseTo( const WgCharBuffer * pBuffer );



	// Methods mostly for debugging and profiling purposes.

	/// @brief Returns number of objects referencing the same memory buffer.
	///
	/// This method only provided for profiling and debugging purposes.
	///
	/// The same buffer of characters can be used by multiple WgCharBuffer, WgString and WgText
	/// objects currently having the same content. This method provides you with the total number of
	/// objects sharing the same buffer as this WgCharBuffer, which is included in the count.
	///
	/// @return Number of objects referencing the same buffer. Always one or higher.

	inline Uint32		RefCount() const { return m_pHead->m_refCnt; }

	/// @brief Returns total number of memory buffers currently in use.
	///
	/// This method only provided for profiling and debugging purposes.
	///
	/// @return Number of memory buffers currently in use, which is @<=
	/// total number of WgCharBuffer, WgString and WgText objects combined.

	inline static Uint32 NbBuffers() { return g_nBuffers; }

private:

	struct BufferHead
	{
		Uint16		m_lockCnt;
		Uint16		m_refCnt;
		Uint32		m_size;					// Size in number of WgChar of buffer.
		Uint32		m_beg;					// Beginning of chars.
		Uint32		m_len;					// Number of chars.
	};


	void        	ClearCharsNoDeref( Uint32 ofs, Uint32 n );  ///< Clears specified characters in buffer without dereferencing properties.
	inline void *	GetPtr( Uint32 ofs ) const { return ((char*) &m_pHead[1]) + sizeof(WgChar)*(m_pHead->m_beg+ofs); }

	void			PushFrontInternal( Uint32 nChars );
	void			PushBackInternal( Uint32 nChars );
	Uint32			ReplaceInternal( Uint32 ofs, Uint32 delChar, Uint32 addChar, const WgChar * pChars = 0);



	inline void		DerefBuffer()
	{
		m_pHead->m_refCnt--;
		if( m_pHead->m_refCnt == 0 )
		{
			DerefProps(0, m_pHead->m_len);
			DestroyBuffer(m_pHead);
		}
	}

	BufferHead *	CreateBuffer( Uint32 size );
	inline void 	DestroyBuffer( BufferHead * pBuffer ) { delete [] (char*) pBuffer; g_nBuffers--; }

	void			CopyChars( BufferHead * pDst, Uint32 ofsDst, const BufferHead * pSrc, Uint32 ofsSrc, Uint32 nChars );
	void			CopyChars( BufferHead * pDst, Uint32 ofsDst, const WgChar * pChars, Uint32 nChars );
	void			ReshapeBuffer( Uint32 begMargin, Uint32 copyOfs, Uint32 copyLen, Uint32 endMargin );

	void			SetChars( Uint32 ofs, Uint32 nChars, Uint32 value );

	inline void		DerefProps( Uint32 ofs, Uint32 n ) { WgTextTool::DerefProps( (WgChar*) GetPtr(ofs), n ); }
	inline void		RefProps( Uint32 ofs, Uint32 n ) { WgTextTool::RefProps( (WgChar*) GetPtr(ofs), n ); }

	static bool		CompareBuffers( const BufferHead * p1, const BufferHead * p2 );


	const static Uint32		c_emptyChar = 0x00000020;	// Value to fill out empty WgChars with.
    static int				g_nBuffers;					// Number of real buffers, <= number of WgCharBuffer.
	static	BufferHead *	g_pEmptyBuffer;				// We keep one common empty buffer as an optimization



	BufferHead *	m_pHead;
};


#endif //WG_CHARBUFFER_DOT_H
