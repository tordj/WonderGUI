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
  closed-source pro#jects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_STANDARDPRINTER_DOT_H
#define	WG_STANDARDPRINTER_DOT_H

#ifndef WG_PRINTER_DOT_H
#	include <wg_printer.h>
#endif 

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

#ifndef WG_CARET_DOT_H
#	include <wg_caret.h>
#endif

namespace wg 
{
	
	class StandardPrinter;
	typedef	StrongPtr<StandardPrinter,Printer_p>		StandardPrinter_p;
	typedef	WeakPtr<StandardPrinter,Printer_wp>	StandardPrinter_wp;
	
	class StandardPrinter : public Printer
	{
	public:
		static StandardPrinter_p create() { return StandardPrinter_p(new StandardPrinter()); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static StandardPrinter_p	cast( const Object_p& pObject );
	
		void			setAlignment( Origo alignment );
		Origo			alignment() const { return m_alignment; }

		void			setCaret( const Caret_p& pCaret );
		Caret_p			caret() const { return m_pCaret; }

	
		virtual void	addItem( PrintableItem * pItem );
		virtual void	removeItem( PrintableItem * pItem );
	
		virtual int		charAtPos( const PrintableItem * pItem, Coord pos ) const;
		virtual Coord	charPos( const PrintableItem * pItem, int charOfs ) const;	// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charRect( const PrintableItem * pItem, int charOfs ) const;
		virtual int		charLine( const PrintableItem * pItem, int charOfs ) const;

//		virtual int		charLineOfs( const PrintableItem * pItem, int charOfs ) const;
//		virtual Coord	lineTopLeftPos( const PrintableItem * pItem, int line ) const;	
//		virtual Coord	lineBaselinePos( const PrintableItem * pItem, int line ) const;	
//		virtual int		lineWidth( const PrintableItem * pItem, int line ) const;
//		virtual int		lineHeight( const PrintableItem * pItem, int line ) const;
//		virtual Rect	lineGeo( const PrintableItem * pItem, int line ) const;
//		virtual int		lines( const PrintableItem * pItem ) const;

		virtual int		lineBegin( const PrintableItem * pItem, int lineNb ) const;
		virtual int		lineEnd( const PrintableItem * pItem, int lineNb ) const;

		virtual int		wordBegin( const PrintableItem * pItem, int charOfs ) const;
		virtual int		wordEnd( const PrintableItem * pItem, int charOfs ) const;		
	
	
		virtual void 	receive( const Msg_p& pMsg );
		virtual void 	renderItem( PrintableItem * pItem, GfxDevice * pDevice, const Rect& canvas, const Rect& clip );
		virtual void	pokeCaret( PrintableItem * pText );
	
		virtual void	onTextModified( PrintableItem * pItem, int ofs, int charsRemoved, int charsAdded );
		virtual void	requestResized( PrintableItem * pItem, Size newSize, Size oldSize );
		virtual void	onStateChanged( PrintableItem * pItem, State newState, State oldState );
		virtual void	onStyleChanged( PrintableItem * pItem, TextStyle * pNewStyle, TextStyle * pOldStyle );
		virtual void	onCharStyleChanged( PrintableItem * pText, int ofs, int len );
		virtual void	onRefresh( PrintableItem * pItem );

	
	
		virtual Size	preferredSize( const PrintableItem * pItem ) const;
		virtual int		matchingWidth( const PrintableItem * pItem, int height ) const;
		virtual int		matchingHeight( const PrintableItem * pItem, int width ) const;
	
		virtual Rect	rectForRange( const PrintableItem * pItem, int ofs, int length ) const;
	
		virtual String 	tooltip( const PrintableItem * pItem ) const;
	
		virtual Direction 	textDirection( PrintableItem * pItem, int charOfs ) const;
		virtual int		caretToPos( PrintableItem * pItem, Coord pos, int& wantedLineOfs ) const;
		virtual int		caretUp( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretDown( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretLeft( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretRight( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretHome( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretEnd( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const;

		virtual int		caretPrevWord( PrintableItem * pItem, int charOfs ) const;
		virtual int		caretNextWord( PrintableItem * pItem, int charOfs ) const;
	
	
	protected:
		StandardPrinter();
		virtual ~StandardPrinter();
	
	
		struct BlockHeader
		{
			int nbLines;
			Size preferredSize;
		};
	
		struct LineInfo
		{
			int offset;				// Line start as offset in characters from beginning of text.
			int length;				// Length of line in characters, incl. line terminator,
			int width;				// Width of line in pixels.
			int height;				// Height of line in pixels.
			int base;				// Offset for baseline from top of line in pixels.
			int spacing;			// Offset from start of line to start of next line.
		};
	
		int				_countLines( const CharBuffer * pBuffer ) const;
		void *			_reallocBlock( PrintableItem * pItem, int lines );
		void			_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle,
												State state );
		bool   			_updatePreferredSize( PrintableItem * pItem );
		int				_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state ) const;
		
		inline BlockHeader *		_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
		inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
		inline LineInfo *			_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
		inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }
	
		int				_linePosX( const LineInfo * pLine, int itemWidth ) const;
		int				_linePosY( const void * pBlock, int line, int itemHeight ) const;
		int				_textPosY( const BlockHeader * pHeader, int itemHeight ) const;
		int				_charPosX( const PrintableItem * pItem, int charOfs ) const;
		
		
		enum struct SelectMode
		{
			Marked,					// Only select character/line that position is within.
			Closest,				// Select character/line that position is within or closest to.
			ClosestBegin,			// Select character/line whose beginning is closest to offset.
			ClosestEnd				// Select character/line whose end is closest to offset.
		};
		
		int				_lineAtPosY( const PrintableItem * pItem, int posY, SelectMode mode ) const;
		int				_charAtPosX( const PrintableItem * pItem, int line, int posX, SelectMode mode ) const;
		
		Origo			m_alignment;
		Caret_p			m_pCaret;
		PrintableItem * m_pFocusedItem;
		RouteId			m_tickRouteId;
	};

} // namespace wg
#endif //WG_STANDARDPRINTER_DOT_H
