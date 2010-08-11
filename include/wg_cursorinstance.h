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

#ifndef	WG_CURSORINSTANCE_DOT_H
#define WG_CURSORINSTANCE_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_CURSOR_DOT_H
#	include <wg_cursor.h>
#endif

class WgText;
class WgCharSeq;

//____ WgCursorInstance ________________________________________________________

class WgCursorInstance
{
public:

	WgCursorInstance( WgText& text );

	bool			incTime( Uint32 ms );
	void			insertMode( bool bInsert );

	void			goUp( Uint32 nLines = 1 );
	void			goDown( Uint32 nLines = 1 );
	inline void		goLeft( Uint32 nChars = 1 );
	inline void		goRight( Uint32 nChars = 1 );

	inline void		goBOF();
	inline void		goEOF();
	void			goBOL();
	void			goEOL();

	void			gotoHardLine( Uint32 line );
	void			gotoSoftLine( Uint32 line );


	void			gotoHardPos( Uint32 line, Uint32 col );
	void			gotoSoftPos( Uint32 line, Uint32 col );

	void			gotoPixel( Sint32 x, Sint32 y );
	void			gotoColumn( Sint32 col );

	void			gotoPrevWord();
	void			gotoBeginningOfWord();
	void			gotoNextWord();
	void			gotoEndOfWord();

	void			getSoftPos( Uint32 &line, Uint32 &col ) const;

	bool			putChar( Uint16 character );
	Uint32			putText( const WgCharSeq& seq );
	void			unputText( int nChar );
	void 			delPrevWord();
	void 			delNextWord();
	bool			delPrevChar();
	bool			delNextChar();

	int				ofsX() const;				// Offset in pixels from beginning of line.
	int				ofsY() const;				// Offset in pixels from top of text.

	inline Uint32	line() const;
	inline Uint32	column() const;
	inline Uint32	time() const;

	inline WgText *	text() const;

	WgCursor::Mode	cursorMode() const;

	void			setSelectionMode(bool bOn);
	bool			hasSelection();
	void			delSelection();
	void			clearSelection();
	void			selectAll();

protected:
	void			gotoPos( Uint32 line, Uint32 col );
	void			UpdateLocation(Uint32 line, Uint32 col);

	WgText *		m_pText;
	bool			m_bInsert;
	bool			m_bSelectMode;
	int				m_selStartLine;
	int				m_selStartColumn;

	Uint32			m_time;					// For the animation
	Uint32			m_line;					// Line of the text containing the cursor (0+)
	Uint32			m_column;				// Character in the line the cursor preceeds (0+)

	Sint32			m_wantedOfsX;			// Desired ofsX for cursor when moving between lines. -1 when not applicable.

};

//____ line() __________________________________________________________________

inline Uint32 WgCursorInstance::line() const
{
	return m_line;
}

//____ column() ________________________________________________________________

inline Uint32 WgCursorInstance::column() const
{
	return m_column;
}

//____ time() __________________________________________________________________

inline Uint32 WgCursorInstance::time() const
{
	return m_time;
}

//____ text() __________________________________________________________________

inline WgText * WgCursorInstance::text() const
{
	return m_pText;
}


inline void WgCursorInstance::goLeft( Uint32 nChars )
{
	//if( m_column <= nChars )
	//	gotoColumn( 0 );
	//else
		gotoColumn( m_column - nChars );
}

inline void WgCursorInstance::goRight( Uint32 nChars )
{
	gotoColumn( m_column + nChars );
}

inline void WgCursorInstance::goBOF()
{
	gotoSoftPos( 0, 0 );
}

inline void WgCursorInstance::goEOF()
{
	gotoSoftPos( 0xFFFFFFFF, 0xFFFFFFFF );
}




#endif //WG_CURSORINSTANCE_DOT_H
