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


class	WgValueFormat;
class	WgFont;

//_____ WgTextLine _____________________________________________________________

struct WgTextLine
{
	Uint32	nChars;			// Number of characters on this line (not including break).
	WgChar * pText;			// May NOT be null, line terminated by glyph:
							// zero = end of text.
							// /n   = hard end of line (user specified).
							// character with soft-break flag set (automatic on wrapText).

	// Following characters can lead to a soft break:
	//
	// Whitespace, tab and BREAK_PERMITTED_HERE: Gives no hyphen.
	// HYPHEN_BREAK_PERMITTED_HERE: Gives additional hyphen when printed.
};

//____ WgText __________________________________________________________________

class WgText
{
public:
	WgText();
	WgText( const char * pText );
	WgText( const Uint16 * pText );

	virtual 			~WgText();

	inline void operator=( const WgText& t) { setText(&t); }; // Fastest order to do this in.

	virtual Uint32				nbChars() const;
	virtual Uint32				nbLines() const;

	virtual const WgChar *		getText() const;

	virtual const WgTextLine *	getLines() const;
	virtual WgTextLine *		getLine( Uint32 line ) const;
	virtual WgChar * 			getLineText( Uint32 line ) const;
	virtual Uint32				getLineWidth( Uint32 line ) const;
	virtual Uint32				getLineWidthPart( Uint32 line, Uint32 startCol, Uint32 nCol ) const;

	virtual const WgTextLine *	getSoftLines() const;
	virtual WgTextLine *		getSoftLine( Uint32 line ) const;
	virtual WgChar * 			getSoftLineText( Uint32 line ) const;
	virtual Uint32				getSoftLineWidth( Uint32 line ) const;
	virtual Uint32				getSoftLineWidthPart( Uint32 line, Uint32 startCol, Uint32 nCol ) const;
	virtual Uint32				nbSoftLines() const;

	virtual	void				posSoft2Hard( Uint32 &line, Uint32 &col ) const;
	virtual void				posHard2Soft( Uint32 &line, Uint32 &col ) const;


	inline void			setText( const char * pText, Uint32 nChar = 0xFFFFFFFF )
								{ setFormattedText( pText, nChar ); }
	inline void			setText( const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF )
								{ setFormattedText( pText, nChar ); }
	void				setText( const WgChar * pText, Uint32 nChar = 0xFFFFFFFF );
	inline void			setText( const WgText * pText ) { setText( pText->getText() ); }

	Uint32				setFormattedText( const char * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );
	Uint32				setFormattedText( const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );

	void				clear();
	virtual void		refresh();

//  --------------

	inline const WgTextPropPtr&  getDefaultProperties() const { return m_pProp; }
	inline WgColor		getDefaultColor(WgMode mode) const { return m_pProp->GetColor(mode); }
	inline WgFontStyle	getDefaultStyle(WgMode mode) const { return m_pProp->GetStyle(mode); }
	inline WgFont *	getDefaultFont() const { return m_pProp->GetFont(); }

//	--------------

	void				setDefaultProperties( const WgTextPropPtr& pProp );

	void				setDefaultColor( const WgColor color );
	void				setDefaultColor( const WgColor color, WgMode mode );

	void				setDefaultStyle( WgFontStyle style );
	void				setDefaultStyle( WgFontStyle style, WgMode mode );

	void				setDefaultFont( WgFont * pFont );

// -------------

	void				clearDefaultProperties();

	void				clearDefaultColor();
	void				clearDefaultColor( WgMode mode );

	void				clearDefaultStyle();
	void				clearDefaultStyle( WgMode mode );

	void				clearDefaultFont();

// -------------

	void				setProperties( const WgTextPropPtr& pProp );
//	void				setProperties( const WgTextPropPtr& pProp, const WgTextBlock& section );

	inline void			setColor( const WgColor color ) { setDefaultColor(color); }
	void				setColor( const WgColor color, WgMode mode );
//	void				setColor( const WgColor color, const WgTextBlock& section );
//	void				setColor( const WgColor color, const WgTextBlock& section, WgMode mode );

	void				setStyle( WgFontStyle style );
	void				setStyle( WgFontStyle style, WgMode mode );
//	void				setStyle( WgFontStyle style, const WgTextBlock& section );
//	void				setStyle( WgFontStyle style, const WgTextBlock& section, WgMode mode );

	void				setFont( WgFont * pFont );
//	void				setFont( WgFont * pFont, const WgTextBlock& section );

	void				setUnderlined();
	void				setUnderlined( WgMode mode );
//	void				setUnderlined( const WgTextBlock& section );
//	void				setUnderlined( const WgTextBlock& section, WgMode mode );

// -------------

	void				clearProperties();
//	void				clearProperties( const WgTextBlock& section );

	void				clearColor();
	void				clearColor( WgMode mode );
//	void				clearColor( const WgTextBlock& section );
//	void				clearColor( const WgTextBlock& section, WgMode mode );

	void				clearStyle();
	void				clearStyle( WgMode mode );
//	void				clearStyle( const WgTextBlock& section );
//	void				clearStyle( const WgTextBlock& section, WgMode mode );

	void				clearFont();
	void				clearFont( const WgTextBlock& section );

	void				clearUnderlined();
//	void				clearUnderlined( const WgTextBlock& section );
	void				clearUnderlined( WgMode mode );
//	void				clearUnderlined( const WgTextBlock& section, WgMode mode );

// -------------

	void				setValue( double value, const WgValueFormat& form );
	void				setScaledValue( Sint32 value, Uint32 scale, const WgValueFormat& form );
	Sint32				compareTo( const WgText * pOther, bool bCheckCase = true ) const;	// Textual compare in the style of strcmp().
    bool                adjustBlock( WgTextBlock * pBlock ) const;

	virtual Uint32		width();
	virtual Uint32		height();

	virtual Uint32		softLineHeight( Uint32 line );

	void				setLineWidth( Uint32 width );
	inline Uint32		getLineWidth() const { return m_lineWidth; }

	void				SetWrap( bool bWrap );
	bool				IsWrap() const { return m_bWrap; }


	inline void			setFontSet( WgFont * pFont ) { setFont(pFont); }		///< DEPRECATED!!! Use setDefaultFont() instead.
	inline WgFont *		getFontSet() const { return getDefaultFont(); }			///< DEPRECATED!!! Use getDefaultFont() instead.

	inline void			setMode( WgMode mode ) { m_mode = mode; }
	inline void			setAlignment( const WgOrigo& origo ) { m_origo = origo; }
	inline void			setTintMode( WgTintMode mode ) { m_tintMode = mode; }
	inline void			setLineSpaceAdjustment( Sint8 adjustment ) { m_lineSpaceAdj = adjustment; }


	inline WgMode		mode() const { return m_mode; }
	inline const WgOrigo& alignment() const { return m_origo; }
	inline WgTintMode	tintMode() const { return m_tintMode; }
	inline Sint8		lineSpaceAdjustment() const { return m_lineSpaceAdj; }

	virtual Uint32		addText( const char * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		addText( const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		addText( const WgChar* pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		addText( const WgCharBuffer* pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		addText( const WgText * pText, Uint32 fromLine = 0, Uint32 nLines = 0xFFFFFFFF );

	virtual Uint32		addFormattedText( const char * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );


	virtual Uint32		removeText( Uint32 line, Uint32 col, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		replaceText( Uint32 line, Uint32 col, Uint32 nChar, const char * pNewText, Uint32 nNewChar = 0xFFFFFFFF );
	virtual Uint32		replaceText( Uint32 line, Uint32 col, Uint32 nChar, const Uint16 * pNewText, Uint32 nNewChar = 0xFFFFFFFF );
	virtual Uint32		replaceText( Uint32 line, Uint32 col, Uint32 nChar, const WgText * pNewText );

	virtual Uint32		insertText( Uint32 line, Uint32 col, const char * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		insertText( Uint32 line, Uint32 col, const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		insertText( Uint32 line, Uint32 col, const WgChar * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		insertText( Uint32 line, Uint32 col, const WgCharBuffer * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual Uint32		insertText( Uint32 line, Uint32 col, const WgText * pText, Uint32 fromLine = 0, Uint32 nLines = 0xFFFFFFFF );

	virtual Uint32		insertFormattedText( Uint32 line, Uint32 col, const char * pText, Uint32 nChar = 0xFFFFFFFF, const WgResDB * pResDB = 0 );

	virtual bool		addLine( const char * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual bool		addLine( const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual bool		addLine( const WgChar * pText, Uint32 nChar = 0xFFFFFFFF );

	virtual bool		replaceLine( Uint32 line, const char * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual bool		replaceLine( Uint32 line, const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual bool		replaceLine( Uint32 line, const WgChar * pText, Uint32 nChar = 0xFFFFFFFF );

	virtual bool		insertLine( Uint32 line, const char * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual bool		insertLine( Uint32 line, const Uint16 * pText, Uint32 nChar = 0xFFFFFFFF );
	virtual bool		insertLine( Uint32 line, const WgChar * pText, Uint32 nChar = 0xFFFFFFFF );
	inline	bool		removeLine( Uint32 line ) { if( removeLines( line, 1 ) == 0) return false; return true; };
	virtual Uint32		removeLines( Uint32 start, Uint32 nLines = 0xFFFFFFFF );

	virtual bool		addChar( Uint16 character );
	virtual bool		replaceChar( Uint32 line, Uint32 col, Uint16 character );
	virtual bool		insertChar( Uint32 line, Uint32 col, Uint16 character );
	virtual bool		removeChar( Uint32 line, Uint32 col );

	virtual bool		joinLines( Uint32 firstLine );
	virtual bool		splitLine( Uint32 line, Uint32 col );


	// Get-methods

	char *	getLineUTF8( Uint32 line ) const;
	Uint32	getLineUTF8( Uint32 line, char * pDest, Uint32 maxBytes ) const;
	Uint32	getLineSizeUTF8( Uint32 line ) const;

	char *	getTextUTF8() const;
	Uint32	getTextUTF8( char * pDest, Uint32 maxBytes ) const;
	Uint32	getTextSizeUTF8() const;

	char *	getTextFormattedUTF8() const;
	Uint32	getTextFormattedUTF8( char * pDest, Uint32 maxBytes ) const;
	Uint32	getTextSizeFormattedUTF8() const;





protected:


	inline WgChar * beginChange( Uint32 line, Uint32 col, Uint32 addChar, Uint32 delChar,
								 Uint32 addLines, Uint32 delLines )
	{
		return beginChange( line, col, addChar, delChar, addLines, delLines, WgChar(0x20) );
	}


	WgChar *		beginChange( Uint32 line, Uint32 col, Uint32 addChar, Uint32 delChar, 
								 Uint32 addLines, Uint32 delLines, WgChar ch );

	void			endChange( Uint32 startline, Uint32 nLines );


	static const int	parseBufLen = 9+16+1+16+8;
	WgChar *	parseValue( double value, const WgValueFormat& form, WgChar[parseBufLen] );
	WgChar *	parseScaledValue( Sint32 value, Uint32 scale, const WgValueFormat& form, WgChar[parseBufLen] );


	void			regenSoftLines();		// Set/remove softbreaks and regenerate the softlines-array (if necessary).

	WgTintMode		m_tintMode;
	WgOrigo			m_origo;
	WgMode			m_mode;
	Sint8			m_lineSpaceAdj;		// Adjustment of linespacing for this text.
	WgTextPropPtr	m_pProp;			// Default properties for this text. Used for all characters who have
										// properties set to 0.

	WgTextLine*		m_pHardLines;
	WgTextLine*		m_pSoftLines;

	Uint32			m_nSoftLines;
	Uint32			m_nHardLines;

	Uint32			m_lineWidth;

	bool			m_bWrap;



	static WgChar		g_emptyText;
	static WgTextLine	g_emptyLine;
};


#endif // WG_TEXT_DOT_H

