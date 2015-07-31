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

#ifndef	WG_LEGACYTEXTFIELD_DOT_H
#define WG_LEGACYTEXTFIELD_DOT_H

#ifndef	WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_FIELD_DOT_H
#	include <wg_field.h>
#endif

#ifndef WG_COLOR_DOT_H
#	include <wg_color.h>
#endif

#ifndef WG_CHAR_DOT_H
#	include <wg_char.h>
#endif

#ifndef WG_CHARBUFFER_DOT_H
#	include <wg_charbuffer.h>
#endif

#ifndef	WG_CARETINSTANCE_DOT_H
	#include <wg_caretinstance.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

#ifndef WG_MSG_DOT_H
#	include <wg_msg.h>
#endif

namespace wg 
{
	
	class MsgRouter;
	class LegacyTextField;
	
	//____ LegacyTextHolder ___________________________________________________________
	
	struct LegacyTextHolder : public FieldHolder
	{
	};
	
	
	//_____ LegacyTextLine _____________________________________________________________
	
	struct LegacyTextLine
	{
		int		nChars;			// Number of characters on this line (not including break).
		int		ofs;			// Offset in buffer for line.
	
		int		width;			// Width in pixels of line
		short	height;			// Height in pixels of line
		short	lineSpacing;	// Spacing from this line to next in pixels.
		short	baseline;		// Offset in pixels to the baseline.
	
		// Following characters can lead to a soft break:
		//
		// Whitespace, tab and BREAK_PERMITTED_HERE: Gives no hyphen.
		// HYPHEN_BREAK_PERMITTED_HERE: Gives additional hyphen when printed.
	};
	
	//____ LegacyTextField __________________________________________________________________
	
	class LegacyTextField : public Field
	{
	
	public:
		LegacyTextField( LegacyTextHolder * pHolder );
		~LegacyTextField();
	
		//
	
		void				setProperties( const Textprop_p& pProp );
		void				clearProperties();
		Textprop_p		properties() const { return m_pBaseProp; }
	
		void				setSelectionProperties( const Textprop_p& pProp );
		void				clearSelectionProperties();
		Textprop_p		selectionProperties() const { return m_pSelectionProp; }
	
		void				setLinkProperties( const Textprop_p& pProp );
		void				clearLinkProperties();
		Textprop_p		linkProperties() const { return m_pLinkProp; }
	
		bool				setMaxChars( int max );
		int					maxChars() const { return m_maxChars; }
	
		inline void			setAlignment( WgOrigo alignment ) { m_alignment = alignment; }
		inline WgOrigo		alignment() const { return m_alignment; }
	
		inline void			setTintMode( WgTintMode mode ) { m_tintMode = mode; }
		inline WgTintMode	tintMode() const { return m_tintMode; }
	
		void				clear();
	
		void				set( const CharSeq& seq );
		void				set( const CharBuffer * buffer );
		void				set( const String& str );
	
		int					append( const CharSeq& seq );
		int					insert( int ofs, const CharSeq& seq );
		int					replace( int ofs, int nDelete, const CharSeq& seq );
		int					remove( int ofs, int len );
		void				deleteSelected();
	
		inline void			setLineSpacing( float adjustment ) { m_lineSpaceAdj = adjustment; }
		inline float		lineSpacing() const { return m_lineSpaceAdj; }
	
		void				setWrap( bool bWrap );
		bool				wrap() const { return m_bWrap; }
	
		void				setAutoEllipsis( bool bAutoEllipsis );
		bool				autoEllipsis() const { return m_bAutoEllipsis; }
	
		inline State		state() const { return m_state; }
		int					length() const;
		int					lines() const;
		inline bool			isEmpty() const { return length()==0?true:false; }
		inline bool			isEditable() const { return m_editMode == WG_TEXT_EDITABLE; }
		inline bool			isSelectable() const { return m_editMode != WG_TEXT_STATIC; }
	
		int					width() const;
		int					height() const;
	
		void				select( int ofs, int len ) { if(m_pCursor) m_pCursor->selectRange( Range(ofs,len)); }	//TODO: Should not be dependent on a cursor!
		void				selectAll() { if(m_pCursor) m_pCursor->selectAll(); }									//TODO: Should not be dependent on a cursor!
		int					selectionStart() const;
		int					selectionLength() const;
		void				clearSelection( );
	
		//
	
		void				setCursorSkin( const Caret_p& pCursor );
		inline Caret_p	cursorSkin() const { return m_pCursorStyle; }
	
		int					insertAtCursor( const CharSeq& str ) { return putText(str); }
		bool				insertAtCursor( Uint16 c ) { return putChar(c); }
	
		inline void			goBot(){ if(m_pCursor) m_pCursor->goBOF(); }
		inline void			goEot(){ if(m_pCursor) m_pCursor->goEOF(); }
		inline void			goBol(){ if(m_pCursor) m_pCursor->goBOL(); }
		inline void			goEol(){ if(m_pCursor) m_pCursor->goEOL(); }
	
	
		//
	
	
		//TODO: operator= should copy the whole object, not just the text.
	
		inline void operator=( const LegacyTextField& t) { setText(&t); }; // Fastest order to do this in.
	
		void		setText( const LegacyTextField * pText );
		void		clone( const LegacyTextField * pText );
	
		int			addChar( const Char& character );
		int			insertChar( int ofs, const Char& character );
		int			replaceChar( int ofs, const Char& character );
		int			deleteChar( int ofs );
	
	
	
	
		inline const Char * getText() const { return m_buffer.chars(); }
		inline CharBuffer * getBuffer() { return &m_buffer; }
		inline const CharBuffer * getBuffer() const { return &m_buffer; }
	
		const LegacyTextLine *	getLines() const;
		LegacyTextLine *		getLine( int line ) const;
		const Char * 		getLineText( int line ) const;
		int					getLineWidth( int line ) const;
		int					getLineOfsY( int line ) const;
	
		const LegacyTextLine *	getSoftLines() const;
		LegacyTextLine *		getSoftLine( int line ) const;
		const Char * 		getSoftLineText( int line ) const;
		int					getSoftLineWidth( int line ) const;
		int					getSoftLineWidthPart( int line, int startCol, int nCol = INT_MAX ) const;
		int					nbSoftLines() const;
		int					getSoftLineSelectionWidth( int line ) const;
	
		void				posSoft2Hard( int &line, int &col ) const;
		void				posHard2Soft( int &line, int &col ) const;
	
	
		void				refresh();
	
	//  --------------
	
		void				selectText( int startLine, int startCol, int endLine, int endCol );
		bool				getSelection( int& startLine, int& startCol, int& endLine, int& endCol ) const;
		Range				getSelection() const;
	
	//  --------------
	
	
		void				setSelectionBgColor(Color c);
		inline Color		getSelectionBgColor() const { return m_pSelectionProp->color(); }
	
	
	// -------------
	
		void				getBaseAttr( TextAttr& attr ) const;
		bool				getCharAttr( TextAttr& attr, int charOfs ) const;
		bool				isCharUnderlined(	int charOfs ) const;
		Color				getCharColor(		int charOfs ) const;
		Color				getCharBgColor(		int charOfs ) const;
		FontAlt			getCharStyle(		int charOfs ) const;
		int					getCharSize(		int charOfs ) const;
		Font_p			getCharFont(		int charOfs ) const;
	//	Glyphset *		getCharGlyphset(	int charOfs ) const;
	//	bool				getCharVisibility(	int charOfs ) const;
		int					getCharBreakLevel(	int charOfs ) const;
		TextLink_p		getCharLink(		int charOfs ) const;
		bool				isCharLink(			int charOfs ) const;
	
	// -------------
	
		void				setValue( double value, const ValueFormat_p& pFormat );
		void				setScaledValue( Sint64 value, Uint32 scale, const ValueFormat_p& pFormat );
	//	int				compareTo( const LegacyTextField * pOther, bool bCheckCase = true ) const;	// Textual compare in the style of strcmp().
	
		Size				unwrappedSize() const;
		int					unwrappedWidth() const;				// Width of text if no lines are wrapped.
	
		int					heightForWidth( int width ) const;
	
		int					softLineHeight( int line );
		int					softLineSpacing( int line );
	
		void				setLineWidth( int width );
		inline int			getLineWidth() const { return m_lineWidth; }
	
		void					setEditMode(TextEditMode mode);							// Maybe should be protected with Widgets/Items as friends?
		inline TextEditMode	editMode() const { return m_editMode; }
	
	
	
		inline void			setState( State state ) { m_state = state; }
	
	
		// Get-methods
	
	
		char *	getTextUTF8() const;
		int		getTextUTF8( char * pDest, int maxBytes ) const;
		int		getTextSizeUTF8() const;
	
		char *	getTextFormattedUTF8() const;
		int		getTextFormattedUTF8( char * pDest, int maxBytes ) const;
		int		getTextSizeFormattedUTF8() const;
	
		char *	getSelectedTextFormattedUTF8() const;
		char *	getSelectedTextUTF8() const;
	
	
		// cursor instance forwards. TODO: merge cursor instance with Text
		CaretInstance*getCursor() const { return m_pCursor; }
	
		void			cursorGotoCoord( const Coord& coord, const Rect& container );
		void			cursorGotoLine( int line, const Rect& container );
		void			cursorGoUp( int nbLines, const Rect& container );
		void			cursorGoDown( int nbLines, const Rect& container );
	
		void			hideCursor() { if(m_pCursor) m_pCursor->hide(); }
		void			showCursor() { if(m_pCursor) m_pCursor->show(); }
		bool			isCursorShowing() const { return m_pCursor ? !m_pCursor->isHidden() : false; }
	
		bool			incTime( int ms ) { return m_pCursor ? m_pCursor->incTime(ms) : false; }
		void			insertMode( bool bInsert ) { if(m_pCursor) m_pCursor->insertMode(bInsert); }
	
		inline void		goLeft( int nChars = 1 ){ if(m_pCursor) m_pCursor->goLeft(nChars); }
		inline void		goRight( int nChars = 1 ){ if(m_pCursor) m_pCursor->goRight(nChars); }
	
		void			gotoSoftLine( int line, const Rect& container ){ if(m_pCursor) m_pCursor->gotoSoftLine(line, container); }
	
	
		void			gotoSoftPos( int line, int col ){ if(m_pCursor) m_pCursor->gotoSoftPos( line, col );}
		void			gotoColumn( int col ){ if(m_pCursor) m_pCursor->gotoColumn( col );}
	
		void			gotoPrevWord(){ if(m_pCursor) m_pCursor->gotoPrevWord();}
		void			gotoBeginningOfWord(){ if(m_pCursor) m_pCursor->gotoBeginningOfWord();}
		void			gotoNextWord(){ if(m_pCursor) m_pCursor->gotoNextWord();}
		void			gotoEndOfWord(){ if(m_pCursor) m_pCursor->gotoEndOfWord();}
	
		void			getSoftPos( int &line, int &col ) const{if(m_pCursor) m_pCursor->getSoftPos( line, col );}
	
		bool			putChar( Uint16 character ){return m_pCursor ? m_pCursor->putChar( character ):false;}
		int				putText( const CharSeq& seq ){return m_pCursor ? m_pCursor->putText( seq ):0;}
		void			unputText( int nChar ){if(m_pCursor) m_pCursor->unputText( nChar );}
		void 			delPrevWord(){if(m_pCursor) m_pCursor->delPrevWord();}
		void 			delNextWord(){if(m_pCursor) m_pCursor->delNextWord();}
		bool			delPrevChar(){return m_pCursor ? m_pCursor->delPrevChar():false;}
		bool			delNextChar(){return m_pCursor ? m_pCursor->delNextChar():false;}
	
		int				ofsX() const{return m_pCursor ? m_pCursor->ofsX():0;}
		int				ofsY() const{return m_pCursor ? m_pCursor->ofsY():0;}
	
		inline int		line() const{return m_pCursor ? m_pCursor->line():0;}
		inline int		column() const{return m_pCursor ? m_pCursor->column():0;}
		inline int		time() const{return m_pCursor ? m_pCursor->time():0;}
	
		Caret::Mode	cursorMode() const { return m_pCursor ? m_pCursor->cursorMode() : Caret::EOL; }
	
		void			setSelectionMode(bool bOn){ if(m_pCursor) m_pCursor->setSelectionMode(bOn); }
		bool			hasSelection()const { return m_pCursor ? m_pCursor->hasSelection() : false; }
		void			delSelection(){ if(m_pCursor) m_pCursor->delSelection(); }
	
		int				lineColToOffset(int line, int col) const;						// HARD LINES!!!!!
	//	void			offsetToSoftLineCol(int ofs, int* wpLine, int* wpCol) const;
	
		//
	
		int				lineStartX( int line, const Rect& container ) const;
		int				lineStartY( int line, const Rect& container ) const;
		Coord			lineStart( int line, const Rect& container ) const;
	
		int				coordToLine( const Coord& coord, const Rect& container, bool bCursorMode = false ) const;
		int				coordToColumn( int line, const Coord& coord, const Rect& container, bool bCursorMode = false ) const;
		TextPos		coordToPos( const Coord& coord, const Rect& container, bool bCursorMode = false ) const;
		int				coordToOfs( const Coord& coord, const Rect& container, bool bCursorMode = false ) const;
		TextLink_p	coordToLink( const Coord& coord, const Rect& container ) const;
	
		// TextPos is automatically clamped for all PosTo...-methods.
	
		int				posToCoordX( const TextPos& pos, const Rect& container ) const;
		int				posToCoordY( const TextPos& pos, const Rect& container ) const;
		Coord			posToCoord( const TextPos& pos, const Rect& container ) const;
		int				posToOfs( const TextPos& pos ) const;
	
		int				ofsToCoordX( int ofs, const Rect& container ) const;
		int				ofsToCoordY( int ofs, const Rect& container ) const;
		Coord			ofsToCoord( int ofs, const Rect& container ) const;
		TextPos		ofsToPos( int ofs ) const;
	
		TextPos		clampPos( TextPos ) const;
	
		//
	
		Coord			focusWindowOnRange( const Size& canvas, const Rect& window, Range range ) const;
	
		bool 			onMsg( const Msg_p& pMsg, MsgRouter * pMsgRouter, const Rect& container );
	//	bool			onAction( WgInput::UserAction action, int button_key, const Rect& textRect, const Coord& pointerOfs );
	
		TextLink_p	getMarkedLink() const { return m_pMarkedLink; }
		State			getMarkedLinkState() const { return m_markedLinkState; }
	
		inline void		setColorSkin( const Skin_p& pSkin ) { m_pColorSkin = pSkin; }
		inline Skin_p colorSkin() const { return m_pColorSkin; }
	
	protected:
	
		LegacyTextHolder *	_holder() { return static_cast<LegacyTextHolder*>(m_pHolder); }
	
		static const int	s_parseBufLen = 9+16+1+16+8;
		Char *		_parseValue( double value, const ValueFormat * pFormat, Char[s_parseBufLen] );
		Char *		_parseScaledValue( Sint64 value, Uint32 scale, const ValueFormat * pFormat, Char[s_parseBufLen] );
	
	
		void			_regenHardLines();		// regenerate the softlines-array (if necessary).
		void			_regenSoftLines();		// regenerate the softlines-array (if necessary).
		int 			_countWriteSoftLines( int maxWidth, const Char * pStart, LegacyTextLine * pWriteLines, int maxWrite ) const; // Central algorithm of regenSoftLines().
	
		void			_refreshAllLines();
		void			_refreshLineInfo( LegacyTextLine * pLine ) const;
	
		int				_cursorMaxWidth() const;
	
	
	
		CharBuffer	m_buffer;
		Caret_p		m_pCursorStyle;
		CaretInstance*	m_pCursor;
	
		TextEditMode	m_editMode;
	
	
		WgTintMode		m_tintMode;
		WgOrigo	m_alignment;
		State			m_state;
	
		TextLink_p	m_pMarkedLink;
		State			m_markedLinkState;
	
	//	int				m_markedLinkOfs;	// Offset in buffer for first character of link currently marked or -1 if none.
	
		int				m_selStartLine;
		int				m_selEndLine;
		int				m_selStartCol;
		int				m_selEndCol;
		float			m_lineSpaceAdj;		// Adjustment of linespacing for this text.
	
		Skin_p		m_pColorSkin;		// Skin from which we take low-prio text colors (lowest prio).
	
		Textprop_p	m_pBaseProp;		// Default properties for this text. Used for all characters who have
											// properties set to 0.
		Textprop_p	m_pLinkProp;		// Props used for links, overriding certain text and char properties.
		Textprop_p	m_pSelectionProp;	// Props used for selected text, overriding certain text, char and link properties.
	
		LegacyTextLine*		m_pHardLines;
		LegacyTextLine*		m_pSoftLines;
	
		int				m_nSoftLines;
		int				m_nHardLines;
	
		int				m_lineWidth;
	
		int				m_newCursorPos;
	
		int				m_maxChars;			// Maximum number of characters allowed.
	
		bool			m_bWrap;
		bool			m_bAutoEllipsis;	// Use ellipsis for text that doesn't fit.
	
		static LegacyTextLine	g_emptyLine;
	};
	
	

} // namespace wg
#endif // WG_LEGACYTEXTFIELD_DOT_H
