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
	
	class WgMsgRouter;
	class WgLegacyTextField;
	
	//____ WgLegacyTextHolder ___________________________________________________________
	
	struct WgLegacyTextHolder : public WgFieldHolder
	{
	};
	
	
	//_____ WgLegacyTextLine _____________________________________________________________
	
	struct WgLegacyTextLine
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
	
	//____ WgLegacyTextField __________________________________________________________________
	
	class WgLegacyTextField : public WgField
	{
	
	public:
		WgLegacyTextField( WgLegacyTextHolder * pHolder );
		~WgLegacyTextField();
	
		//
	
		void				setProperties( const WgTextprop_p& pProp );
		void				clearProperties();
		WgTextprop_p		properties() const { return m_pBaseProp; }
	
		void				setSelectionProperties( const WgTextprop_p& pProp );
		void				clearSelectionProperties();
		WgTextprop_p		selectionProperties() const { return m_pSelectionProp; }
	
		void				setLinkProperties( const WgTextprop_p& pProp );
		void				clearLinkProperties();
		WgTextprop_p		linkProperties() const { return m_pLinkProp; }
	
		bool				setMaxChars( int max );
		int					maxChars() const { return m_maxChars; }
	
		inline void			setAlignment( WgOrigo alignment ) { m_alignment = alignment; }
		inline WgOrigo		alignment() const { return m_alignment; }
	
		inline void			setTintMode( WgTintMode mode ) { m_tintMode = mode; }
		inline WgTintMode	tintMode() const { return m_tintMode; }
	
		void				clear();
	
		void				set( const WgCharSeq& seq );
		void				set( const WgCharBuffer * buffer );
		void				set( const WgString& str );
	
		int					append( const WgCharSeq& seq );
		int					insert( int ofs, const WgCharSeq& seq );
		int					replace( int ofs, int nDelete, const WgCharSeq& seq );
		int					remove( int ofs, int len );
		void				deleteSelected();
	
		inline void			setLineSpacing( float adjustment ) { m_lineSpaceAdj = adjustment; }
		inline float		lineSpacing() const { return m_lineSpaceAdj; }
	
		void				setWrap( bool bWrap );
		bool				wrap() const { return m_bWrap; }
	
		void				setAutoEllipsis( bool bAutoEllipsis );
		bool				autoEllipsis() const { return m_bAutoEllipsis; }
	
		inline WgState		state() const { return m_state; }
		int					length() const;
		int					lines() const;
		inline bool			isEmpty() const { return length()==0?true:false; }
		inline bool			isEditable() const { return m_editMode == WG_TEXT_EDITABLE; }
		inline bool			isSelectable() const { return m_editMode != WG_TEXT_STATIC; }
	
		int					width() const;
		int					height() const;
	
		void				select( int ofs, int len ) { if(m_pCursor) m_pCursor->selectRange( WgRange(ofs,len)); }	//TODO: Should not be dependent on a cursor!
		void				selectAll() { if(m_pCursor) m_pCursor->selectAll(); }									//TODO: Should not be dependent on a cursor!
		int					selectionStart() const;
		int					selectionLength() const;
		void				clearSelection( );
	
		//
	
		void				setCursorSkin( const WgCaret_p& pCursor );
		inline WgCaret_p	cursorSkin() const { return m_pCursorStyle; }
	
		int					insertAtCursor( const WgCharSeq& str ) { return putText(str); }
		bool				insertAtCursor( Uint16 c ) { return putChar(c); }
	
		inline void			goBot(){ if(m_pCursor) m_pCursor->goBOF(); }
		inline void			goEot(){ if(m_pCursor) m_pCursor->goEOF(); }
		inline void			goBol(){ if(m_pCursor) m_pCursor->goBOL(); }
		inline void			goEol(){ if(m_pCursor) m_pCursor->goEOL(); }
	
	
		//
	
	
		//TODO: operator= should copy the whole object, not just the text.
	
		inline void operator=( const WgLegacyTextField& t) { setText(&t); }; // Fastest order to do this in.
	
		void		setText( const WgLegacyTextField * pText );
		void		clone( const WgLegacyTextField * pText );
	
		int			addChar( const WgChar& character );
		int			insertChar( int ofs, const WgChar& character );
		int			replaceChar( int ofs, const WgChar& character );
		int			deleteChar( int ofs );
	
	
	
	
		inline const WgChar * getText() const { return m_buffer.chars(); }
		inline WgCharBuffer * getBuffer() { return &m_buffer; }
		inline const WgCharBuffer * getBuffer() const { return &m_buffer; }
	
		const WgLegacyTextLine *	getLines() const;
		WgLegacyTextLine *		getLine( int line ) const;
		const WgChar * 		getLineText( int line ) const;
		int					getLineWidth( int line ) const;
		int					getLineOfsY( int line ) const;
	
		const WgLegacyTextLine *	getSoftLines() const;
		WgLegacyTextLine *		getSoftLine( int line ) const;
		const WgChar * 		getSoftLineText( int line ) const;
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
		WgRange				getSelection() const;
	
	//  --------------
	
	
		void				setSelectionBgColor(WgColor c);
		inline WgColor		getSelectionBgColor() const { return m_pSelectionProp->color(); }
	
	
	// -------------
	
		void				getBaseAttr( WgTextAttr& attr ) const;
		bool				getCharAttr( WgTextAttr& attr, int charOfs ) const;
		bool				isCharUnderlined(	int charOfs ) const;
		WgColor				getCharColor(		int charOfs ) const;
		WgColor				getCharBgColor(		int charOfs ) const;
		WgFontAlt			getCharStyle(		int charOfs ) const;
		int					getCharSize(		int charOfs ) const;
		WgFont_p			getCharFont(		int charOfs ) const;
	//	WgGlyphset *		getCharGlyphset(	int charOfs ) const;
	//	bool				getCharVisibility(	int charOfs ) const;
		int					getCharBreakLevel(	int charOfs ) const;
		WgTextLink_p		getCharLink(		int charOfs ) const;
		bool				isCharLink(			int charOfs ) const;
	
	// -------------
	
		void				setValue( double value, const WgValueFormat_p& pFormat );
		void				setScaledValue( Sint64 value, Uint32 scale, const WgValueFormat_p& pFormat );
	//	int				compareTo( const WgLegacyTextField * pOther, bool bCheckCase = true ) const;	// Textual compare in the style of strcmp().
	
		WgSize				unwrappedSize() const;
		int					unwrappedWidth() const;				// Width of text if no lines are wrapped.
	
		int					heightForWidth( int width ) const;
	
		int					softLineHeight( int line );
		int					softLineSpacing( int line );
	
		void				setLineWidth( int width );
		inline int			getLineWidth() const { return m_lineWidth; }
	
		void					setEditMode(WgTextEditMode mode);							// Maybe should be protected with Widgets/Items as friends?
		inline WgTextEditMode	editMode() const { return m_editMode; }
	
	
	
		inline void			setState( WgState state ) { m_state = state; }
	
	
		// Get-methods
	
	
		char *	getTextUTF8() const;
		int		getTextUTF8( char * pDest, int maxBytes ) const;
		int		getTextSizeUTF8() const;
	
		char *	getTextFormattedUTF8() const;
		int		getTextFormattedUTF8( char * pDest, int maxBytes ) const;
		int		getTextSizeFormattedUTF8() const;
	
		char *	getSelectedTextFormattedUTF8() const;
		char *	getSelectedTextUTF8() const;
	
	
		// cursor instance forwards. TODO: merge cursor instance with WgText
		WgCaretInstance*getCursor() const { return m_pCursor; }
	
		void			cursorGotoCoord( const WgCoord& coord, const WgRect& container );
		void			cursorGotoLine( int line, const WgRect& container );
		void			cursorGoUp( int nbLines, const WgRect& container );
		void			cursorGoDown( int nbLines, const WgRect& container );
	
		void			hideCursor() { if(m_pCursor) m_pCursor->hide(); }
		void			showCursor() { if(m_pCursor) m_pCursor->show(); }
		bool			isCursorShowing() const { return m_pCursor ? !m_pCursor->isHidden() : false; }
	
		bool			incTime( int ms ) { return m_pCursor ? m_pCursor->incTime(ms) : false; }
		void			insertMode( bool bInsert ) { if(m_pCursor) m_pCursor->insertMode(bInsert); }
	
		inline void		goLeft( int nChars = 1 ){ if(m_pCursor) m_pCursor->goLeft(nChars); }
		inline void		goRight( int nChars = 1 ){ if(m_pCursor) m_pCursor->goRight(nChars); }
	
		void			gotoSoftLine( int line, const WgRect& container ){ if(m_pCursor) m_pCursor->gotoSoftLine(line, container); }
	
	
		void			gotoSoftPos( int line, int col ){ if(m_pCursor) m_pCursor->gotoSoftPos( line, col );}
		void			gotoColumn( int col ){ if(m_pCursor) m_pCursor->gotoColumn( col );}
	
		void			gotoPrevWord(){ if(m_pCursor) m_pCursor->gotoPrevWord();}
		void			gotoBeginningOfWord(){ if(m_pCursor) m_pCursor->gotoBeginningOfWord();}
		void			gotoNextWord(){ if(m_pCursor) m_pCursor->gotoNextWord();}
		void			gotoEndOfWord(){ if(m_pCursor) m_pCursor->gotoEndOfWord();}
	
		void			getSoftPos( int &line, int &col ) const{if(m_pCursor) m_pCursor->getSoftPos( line, col );}
	
		bool			putChar( Uint16 character ){return m_pCursor ? m_pCursor->putChar( character ):false;}
		int				putText( const WgCharSeq& seq ){return m_pCursor ? m_pCursor->putText( seq ):0;}
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
	
		WgCaret::Mode	cursorMode() const { return m_pCursor ? m_pCursor->cursorMode() : WgCaret::EOL; }
	
		void			setSelectionMode(bool bOn){ if(m_pCursor) m_pCursor->setSelectionMode(bOn); }
		bool			hasSelection()const { return m_pCursor ? m_pCursor->hasSelection() : false; }
		void			delSelection(){ if(m_pCursor) m_pCursor->delSelection(); }
	
		int				lineColToOffset(int line, int col) const;						// HARD LINES!!!!!
	//	void			offsetToSoftLineCol(int ofs, int* wpLine, int* wpCol) const;
	
		//
	
		int				lineStartX( int line, const WgRect& container ) const;
		int				lineStartY( int line, const WgRect& container ) const;
		WgCoord			lineStart( int line, const WgRect& container ) const;
	
		int				coordToLine( const WgCoord& coord, const WgRect& container, bool bCursorMode = false ) const;
		int				coordToColumn( int line, const WgCoord& coord, const WgRect& container, bool bCursorMode = false ) const;
		WgTextPos		coordToPos( const WgCoord& coord, const WgRect& container, bool bCursorMode = false ) const;
		int				coordToOfs( const WgCoord& coord, const WgRect& container, bool bCursorMode = false ) const;
		WgTextLink_p	coordToLink( const WgCoord& coord, const WgRect& container ) const;
	
		// WgTextPos is automatically clamped for all PosTo...-methods.
	
		int				posToCoordX( const WgTextPos& pos, const WgRect& container ) const;
		int				posToCoordY( const WgTextPos& pos, const WgRect& container ) const;
		WgCoord			posToCoord( const WgTextPos& pos, const WgRect& container ) const;
		int				posToOfs( const WgTextPos& pos ) const;
	
		int				ofsToCoordX( int ofs, const WgRect& container ) const;
		int				ofsToCoordY( int ofs, const WgRect& container ) const;
		WgCoord			ofsToCoord( int ofs, const WgRect& container ) const;
		WgTextPos		ofsToPos( int ofs ) const;
	
		WgTextPos		clampPos( WgTextPos ) const;
	
		//
	
		WgCoord			focusWindowOnRange( const WgSize& canvas, const WgRect& window, WgRange range ) const;
	
		bool 			onMsg( const WgMsg_p& pMsg, WgMsgRouter * pMsgRouter, const WgRect& container );
	//	bool			onAction( WgInput::UserAction action, int button_key, const WgRect& textRect, const WgCoord& pointerOfs );
	
		WgTextLink_p	getMarkedLink() const { return m_pMarkedLink; }
		WgState			getMarkedLinkState() const { return m_markedLinkState; }
	
		inline void		setColorSkin( const WgSkin_p& pSkin ) { m_pColorSkin = pSkin; }
		inline WgSkin_p colorSkin() const { return m_pColorSkin; }
	
	protected:
	
		WgLegacyTextHolder *	_holder() { return static_cast<WgLegacyTextHolder*>(m_pHolder); }
	
		static const int	s_parseBufLen = 9+16+1+16+8;
		WgChar *		_parseValue( double value, const WgValueFormat * pFormat, WgChar[s_parseBufLen] );
		WgChar *		_parseScaledValue( Sint64 value, Uint32 scale, const WgValueFormat * pFormat, WgChar[s_parseBufLen] );
	
	
		void			_regenHardLines();		// regenerate the softlines-array (if necessary).
		void			_regenSoftLines();		// regenerate the softlines-array (if necessary).
		int 			_countWriteSoftLines( int maxWidth, const WgChar * pStart, WgLegacyTextLine * pWriteLines, int maxWrite ) const; // Central algorithm of regenSoftLines().
	
		void			_refreshAllLines();
		void			_refreshLineInfo( WgLegacyTextLine * pLine ) const;
	
		int				_cursorMaxWidth() const;
	
	
	
		WgCharBuffer	m_buffer;
		WgCaret_p		m_pCursorStyle;
		WgCaretInstance*	m_pCursor;
	
		WgTextEditMode	m_editMode;
	
	
		WgTintMode		m_tintMode;
		WgOrigo	m_alignment;
		WgState			m_state;
	
		WgTextLink_p	m_pMarkedLink;
		WgState			m_markedLinkState;
	
	//	int				m_markedLinkOfs;	// Offset in buffer for first character of link currently marked or -1 if none.
	
		int				m_selStartLine;
		int				m_selEndLine;
		int				m_selStartCol;
		int				m_selEndCol;
		float			m_lineSpaceAdj;		// Adjustment of linespacing for this text.
	
		WgSkin_p		m_pColorSkin;		// Skin from which we take low-prio text colors (lowest prio).
	
		WgTextprop_p	m_pBaseProp;		// Default properties for this text. Used for all characters who have
											// properties set to 0.
		WgTextprop_p	m_pLinkProp;		// Props used for links, overriding certain text and char properties.
		WgTextprop_p	m_pSelectionProp;	// Props used for selected text, overriding certain text, char and link properties.
	
		WgLegacyTextLine*		m_pHardLines;
		WgLegacyTextLine*		m_pSoftLines;
	
		int				m_nSoftLines;
		int				m_nHardLines;
	
		int				m_lineWidth;
	
		int				m_newCursorPos;
	
		int				m_maxChars;			// Maximum number of characters allowed.
	
		bool			m_bWrap;
		bool			m_bAutoEllipsis;	// Use ellipsis for text that doesn't fit.
	
		static WgLegacyTextLine	g_emptyLine;
	};
	
	

} // namespace wg
#endif // WG_LEGACYTEXTFIELD_DOT_H
