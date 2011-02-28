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

#ifndef	WG_TEXT_DOT_H
#define WG_TEXT_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_ORIGO_DOT_H
#	include <wg_origo.h>
#endif

#ifndef WG_CHAR_DOT_H
#	include <wg_char.h>
#endif

#ifndef WG_CHARBUFFER_DOT_H
#	include <wg_charbuffer.h>
#endif

#ifndef WG_TEXTMANAGER_DOT_H
#	include <wg_textmanager.h>
#endif

#ifndef	WG_CURSORINSTANCE_DOT_H
	#include "wg_cursorinstance.h"
#endif

class	WgValueFormat;
class	WgFont;

//____ WgTextHolder ___________________________________________________________

struct WgTextHolder
{
public:
	virtual void		TextModified( WgText * pText ) = 0;
};


//_____ WgTextLine _____________________________________________________________

struct WgTextLine
{
	Uint32	nChars;			// Number of characters on this line (not including break).
	Uint32	ofs;			// Offset in buffer for line.
/*
	int		width;			// Width in pixels of line
	short	height;			// Height in pixels of line
	short	lineSpacing;	// Spacing from this line to next in pixels.
	short	baseline;		// Offset in pixels to the baseline.
*/
	// Following characters can lead to a soft break:
	//
	// Whitespace, tab and BREAK_PERMITTED_HERE: Gives no hyphen.
	// HYPHEN_BREAK_PERMITTED_HERE: Gives additional hyphen when printed.
};

//____ WgText __________________________________________________________________

class WgText
{
friend class WgTextNode;

public:
	WgText();
	WgText( const WgCharSeq& seq );
	WgText( const WgCharBuffer * pBuffer );
	WgText( const WgString& str );

	void	Init();

	~WgText();

	//TODO: operator= should copy the whole object, not just the text.

	inline void operator=( const WgText& t) { setText(&t); }; // Fastest order to do this in.

	void		setText( const WgCharSeq& seq );
	void		setText( const WgCharBuffer * buffer );
	void		setText( const WgString& str );
	void		setText( const WgText * pText );

	int			addText( const WgCharSeq& seq );
	int			insertText( int ofs, const WgCharSeq& seq );
	int			replaceText( int ofs, int nDelete, const WgCharSeq& seq );
	int			deleteText( int ofs, int len );
	void		deleteSelectedText();

	int			addChar( const WgChar& character );
	int			insertChar( int ofs, const WgChar& character );
	int			replaceChar( int ofs, const WgChar& character );
	int			deleteChar( int ofs );


	Uint32		nbChars() const;
	Uint32		nbLines() const;

	inline const WgChar * getText() const { return m_buffer.Chars(); }
	inline WgCharBuffer * getBuffer() { return &m_buffer; }
	inline const WgCharBuffer * getBuffer() const { return &m_buffer; }

	const WgTextLine *	getLines() const;
	WgTextLine *		getLine( int line ) const;
	const WgChar * 		getLineText( int line ) const;
	Uint32				getLineWidth( Uint32 line ) const;
	Uint32				getLineWidthPart( Uint32 line, Uint32 startCol, Uint32 nCol = 0xFFFFFFFF ) const;

	const WgTextLine *	getSoftLines() const;
	WgTextLine *		getSoftLine( int line ) const;
	const WgChar * 		getSoftLineText( int line ) const;
	Uint32				getSoftLineWidth( Uint32 line ) const;
	Uint32				getSoftLineWidthPart( Uint32 line, Uint32 startCol, Uint32 nCol = 0xFFFFFFFF ) const;
	Uint32				nbSoftLines() const;
	Uint32				getSoftLineSelectionWidth( Uint32 line ) const;

	void				posSoft2Hard( Uint32 &line, Uint32 &col ) const;
	void				posHard2Soft( Uint32 &line, Uint32 &col ) const;


	void				clear();
	void				refresh();

//  --------------
	void				selectText( Uint32 startLine, Uint32 startCol, Uint32 endLine, Uint32 endCol );
	bool				getSelection( Uint32& startLine, Uint32& startCol, Uint32& endLine, Uint32& endCol ) const;
	void				clearSelection( );
//  --------------

	void				setManager( WgTextManager * pManager );
	inline WgTextManager *	getManager() const { return m_pManagerNode?m_pManagerNode->GetManager():0; }
	inline WgTextNode *	getNode() const { return m_pManagerNode; }

	void				setHolder( WgTextHolder * pHolder ) { m_pHolder = pHolder; }

//  --------------

	inline const WgTextPropPtr&	getProperties() const { return m_pProp; }
	inline WgColor				getColor() const { return m_pProp->GetColor(); }
	inline WgColor				getColor(WgMode mode) const { return m_pProp->GetColor(mode); }
	inline WgFontStyle			getStyle(WgMode mode) const { return m_pProp->GetStyle(mode); }
	inline int					getBreakLevel() const { return m_pProp->GetBreakLevel(); }
	inline WgFont *				getFont() const { return m_pProp->GetFont(); }

//	--------------

	void				setProperties( const WgTextPropPtr& pProp );

	void				setColor( const WgColor color );
	void				setColor( const WgColor color, WgMode mode );

	void				setStyle( WgFontStyle style );
	void				setStyle( WgFontStyle style, WgMode mode );

	void				setBreakLevel( int level );

	void				setFont( WgFont * pFont );

// -------------

	void				clearProperties();

	void				clearColor();
	void				clearColor( WgMode mode );

	void				clearStyle();
	void				clearStyle( WgMode mode );

	void				clearBreakLevel();

	void				clearFont();

// -------------

	void				setSelectionColor(WgColor c) { m_selColor = c; }
	WgColor				getSelectionColor() const { return m_selColor; }

// -------------

	void				setValue( double value, const WgValueFormat& form );
	void				setScaledValue( Sint64 value, Uint32 scale, const WgValueFormat& form );
//	Sint32				compareTo( const WgText * pOther, bool bCheckCase = true ) const;	// Textual compare in the style of strcmp().

	Uint32				width() const;
	Uint32				height() const;

	int					heightForWidth( int width ) const;

	Uint32				softLineHeight( Uint32 line );
	Uint32				softLineSpacing( Uint32 line );

	void				setLineWidth( Uint32 width );
	inline Uint32		getLineWidth() const { return m_lineWidth; }

	void				SetWrap( bool bWrap );
	bool				IsWrap() const { return m_bWrap; }



	inline void			setMode( WgMode mode ) { m_mode = mode; }
	inline void			setAlignment( const WgOrigo& origo ) { m_origo = origo; }
	inline void			setTintMode( WgTintMode mode ) { m_tintMode = mode; }
	inline void			setLineSpaceAdjustment( Sint8 adjustment ) { m_lineSpaceAdj = adjustment; }


	inline WgMode		mode() const { return m_mode; }
	inline const WgOrigo& alignment() const { return m_origo; }
	inline WgTintMode	tintMode() const { return m_tintMode; }
	inline Sint8		lineSpaceAdjustment() const { return m_lineSpaceAdj; }


	// Get-methods


	char *	getTextUTF8() const;
	Uint32	getTextUTF8( char * pDest, Uint32 maxBytes ) const;
	Uint32	getTextSizeUTF8() const;

	char *	getTextFormattedUTF8() const;
	Uint32	getTextFormattedUTF8( char * pDest, Uint32 maxBytes ) const;
	Uint32	getTextSizeFormattedUTF8() const;

	char *	getSelectedTextFormattedUTF8() const;
	char *	getSelectedTextUTF8() const;


	// cursor instance forwards. TODO: merge cursor instance with WgText
	void			CreateCursor() { m_pCursor = new WgCursorInstance(*this); }
	void			DestroyCursor() { delete m_pCursor; m_pCursor = 0; }
	WgCursorInstance*GetCursor() const { return m_pCursor; }

	bool			incTime( Uint32 ms ) { return m_pCursor ? m_pCursor->incTime(ms) : false; }
	void			insertMode( bool bInsert ) { if(m_pCursor) m_pCursor->insertMode(bInsert); }

	void			goUp( Uint32 nLines = 1 ) { if(m_pCursor) m_pCursor->goUp(nLines); }
	void			goDown( Uint32 nLines = 1 )	 { if(m_pCursor) m_pCursor->goDown(nLines); }
	inline void		goLeft( Uint32 nChars = 1 ){ if(m_pCursor) m_pCursor->goLeft(nChars); }
	inline void		goRight( Uint32 nChars = 1 ){ if(m_pCursor) m_pCursor->goRight(nChars); }

	inline void		goBOF(){ if(m_pCursor) m_pCursor->goBOF(); }
	inline void		goEOF(){ if(m_pCursor) m_pCursor->goEOF(); }
	void			goBOL(){ if(m_pCursor) m_pCursor->goBOL(); }
	void			goEOL(){ if(m_pCursor) m_pCursor->goEOL(); }

	void			gotoHardLine( Uint32 line ){ if(m_pCursor) m_pCursor->gotoHardLine(line); }
	void			gotoSoftLine( Uint32 line ){ if(m_pCursor) m_pCursor->gotoSoftLine(line); }


//	void			gotoHardPos( Uint32 line, Uint32 col ){ if(m_pCursor) m_pCursor->gotoHardPos( line, col );}
	void			gotoSoftPos( Uint32 line, Uint32 col ){ if(m_pCursor) m_pCursor->gotoSoftPos( line, col );}

	void			gotoPixel( Sint32 x, Sint32 y ){ if(m_pCursor) m_pCursor->gotoPixel( x, y );}
	void			gotoColumn( Sint32 col ){ if(m_pCursor) m_pCursor->gotoColumn( col );}

	void			gotoPrevWord(){ if(m_pCursor) m_pCursor->gotoPrevWord();}
	void			gotoBeginningOfWord(){ if(m_pCursor) m_pCursor->gotoBeginningOfWord();}
	void			gotoNextWord(){ if(m_pCursor) m_pCursor->gotoNextWord();}
	void			gotoEndOfWord(){ if(m_pCursor) m_pCursor->gotoEndOfWord();}

	void			getSoftPos( Uint32 &line, Uint32 &col ) const{if(m_pCursor) m_pCursor->getSoftPos( line, col );}

	bool			putChar( Uint16 character ){return m_pCursor ? m_pCursor->putChar( character ):false;}
	Uint32			putText( const WgCharSeq& seq ){return m_pCursor ? m_pCursor->putText( seq ):0;}
	void			unputText( int nChar ){if(m_pCursor) m_pCursor->unputText( nChar );}
	void 			delPrevWord(){if(m_pCursor) m_pCursor->delPrevWord();}
	void 			delNextWord(){if(m_pCursor) m_pCursor->delNextWord();}
	bool			delPrevChar(){return m_pCursor ? m_pCursor->delPrevChar():false;}
	bool			delNextChar(){return m_pCursor ? m_pCursor->delNextChar():false;}

	int				ofsX() const{return m_pCursor ? m_pCursor->ofsX():0;}
	int				ofsY() const{return m_pCursor ? m_pCursor->ofsY():0;}

	inline Uint32	line() const{return m_pCursor ? m_pCursor->line():0;}
	inline Uint32	column() const{return m_pCursor ? m_pCursor->column():0;}
	inline Uint32	time() const{return m_pCursor ? m_pCursor->time():0;}

	WgCursor::Mode	cursorMode() const { return m_pCursor ? m_pCursor->cursorMode() : WgCursor::EOL; }

	void			setSelectionMode(bool bOn){ if(m_pCursor) m_pCursor->setSelectionMode(bOn); }
	bool			hasSelection(){ return m_pCursor ? m_pCursor->hasSelection() : false; }
	void			delSelection(){ if(m_pCursor) m_pCursor->delSelection(); }

	void			selectAll() { if(m_pCursor) m_pCursor->selectAll(); }

	Uint32			LineColToOffset(int line, int col);

protected:


	static const int	parseBufLen = 9+16+1+16+8;
	WgChar *	parseValue( double value, const WgValueFormat& form, WgChar[parseBufLen] );
	WgChar *	parseScaledValue( Sint64 value, Uint32 scale, const WgValueFormat& form, WgChar[parseBufLen] );


	void			regenHardLines();		// Set/remove softbreaks and regenerate the softlines-array (if necessary).
	void			regenSoftLines();		// Set/remove softbreaks and regenerate the softlines-array (if necessary).

	int 			countWriteSoftLines( const WgChar * pStart, WgTextLine * pWriteLines, int maxWrite ); // Central algorithm of regenSoftLines().

	WgCharBuffer	m_buffer;

	WgCursorInstance*	m_pCursor;
	WgTextNode *	m_pManagerNode;

	WgColor			m_selColor;
	WgTintMode		m_tintMode;
	WgOrigo			m_origo;
	WgMode			m_mode;
	int				m_selStartLine;
	int				m_selEndLine;
	int				m_selStartCol;
	int				m_selEndCol;
	Sint8			m_lineSpaceAdj;		// Adjustment of linespacing for this text.
	WgTextPropPtr	m_pProp;			// Default properties for this text. Used for all characters who have
										// properties set to 0.

	WgTextLine*		m_pHardLines;
	WgTextLine*		m_pSoftLines;

	int				m_nSoftLines;
	int				m_nHardLines;

	Uint32			m_lineWidth;

	Uint32			m_newCursorPos;

	bool			m_bWrap;

	WgTextHolder *	m_pHolder;

	static WgTextLine	g_emptyLine;
};


#endif // WG_TEXT_DOT_H

