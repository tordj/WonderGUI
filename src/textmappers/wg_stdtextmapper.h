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

#ifndef	WG_STDTEXTMAPPER_DOT_H
#define	WG_STDTEXTMAPPER_DOT_H
#pragma once

#include <wg_textmapper.h>
#include <wg_textstyle.h>
#include <wg_caret.h>

namespace wg 
{
	
	class StdTextMapper;
	typedef	StrongPtr<StdTextMapper,TextMapper_p>		StdTextMapper_p;
	typedef	WeakPtr<StdTextMapper,TextMapper_wp>	StdTextMapper_wp;
	
	class StdTextMapper : public TextMapper
	{
	public:
		static StdTextMapper_p create() { return StdTextMapper_p(new StdTextMapper()); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static StdTextMapper_p	cast( const Object_p& pObject );
	
		void			setAlignment( Origo alignment );
		Origo			alignment() const { return m_alignment; }

		void			setCaret( const Caret_p& pCaret );
		Caret_p			caret() const { return m_pCaret; }

		void			setSelectionBack(Color color, BlendMode renderMode = BlendMode::Replace );

		void			setSelectionCharColor(Color color, BlendMode blend = BlendMode::Replace );
	
		Color			selectionBackColor() { return m_selectionBackColor;  }
		BlendMode		selectionBackRenderMode() { return m_selectionBackRenderMode; }

		Color			selectionCharColor() { return m_selectionCharColor; }
		BlendMode		selectionCharColorBlend() { return m_selectionCharBlend; }


		virtual void	addItem( TextBaseItem * pItem );
		virtual void	removeItem( TextBaseItem * pItem );
	
		virtual int		charAtPos( const TextBaseItem * pItem, Coord pos ) const;
		virtual Coord	charPos( const TextBaseItem * pItem, int charOfs ) const;	// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charRect( const TextBaseItem * pItem, int charOfs ) const;
		virtual int		charLine( const TextBaseItem * pItem, int charOfs ) const;

//		virtual int		charLineOfs( const TextBaseItem * pItem, int charOfs ) const;
//		virtual Coord	lineTopLeftPos( const TextBaseItem * pItem, int line ) const;	
//		virtual Coord	lineBaselinePos( const TextBaseItem * pItem, int line ) const;	
//		virtual int		lineWidth( const TextBaseItem * pItem, int line ) const;
//		virtual int		lineHeight( const TextBaseItem * pItem, int line ) const;
//		virtual Rect	lineGeo( const TextBaseItem * pItem, int line ) const;
//		virtual int		lines( const TextBaseItem * pItem ) const;

		virtual int		lineBegin( const TextBaseItem * pItem, int lineNb ) const;
		virtual int		lineEnd( const TextBaseItem * pItem, int lineNb ) const;

		virtual int		wordBegin( const TextBaseItem * pItem, int charOfs ) const;
		virtual int		wordEnd( const TextBaseItem * pItem, int charOfs ) const;		


		virtual void 	receive( const Msg_p& pMsg );
		virtual void 	renderItem( TextBaseItem * pItem, GfxDevice * pDevice, const Rect& canvas, const Rect& clip );

		virtual void	caretMove( TextBaseItem * pText, int newOfs );
		virtual void	selectionChange( TextBaseItem * pText, int newSelectOfs, int newCaretOfs );

		virtual void	onTextModified( TextBaseItem * pItem, int ofs, int charsRemoved, int charsAdded );
		virtual void	onResized( TextBaseItem * pItem, Size newSize, Size oldSize );
		virtual void	onStateChanged( TextBaseItem * pItem, State newState, State oldState );
		virtual void	onStyleChanged( TextBaseItem * pItem, TextStyle * pNewStyle, TextStyle * pOldStyle );
		virtual void	onCharStyleChanged( TextBaseItem * pText, int ofs, int len );
		virtual void	onRefresh( TextBaseItem * pItem );



		virtual Size	preferredSize( const TextBaseItem * pItem ) const;
		virtual int		matchingWidth( const TextBaseItem * pItem, int height ) const;
		virtual int		matchingHeight( const TextBaseItem * pItem, int width ) const;
	
		virtual Rect	rectForRange( const TextBaseItem * pItem, int ofs, int length ) const;
		virtual Rect	rectForCaret( const TextBaseItem * pText ) const;
	
		virtual String 	tooltip( const TextBaseItem * pItem ) const;
	
		virtual Direction 	textDirection( TextBaseItem * pItem, int charOfs ) const;
		virtual int		caretToPos( TextBaseItem * pItem, Coord pos, int& wantedLineOfs ) const;
		virtual int		caretUp( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretDown( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretLeft( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretRight( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretHome( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const;
		virtual int		caretEnd( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const;

		virtual int		caretPrevWord( TextBaseItem * pItem, int charOfs ) const;
		virtual int		caretNextWord( TextBaseItem * pItem, int charOfs ) const;
	
	
	
	protected:
		StdTextMapper();
		virtual ~StdTextMapper();
	
	
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
	
		inline Glyph_p	_getGlyph( Font * pFont, uint16_t charCode ) const;
	
		int				_countLines( const CharBuffer * pBuffer ) const;
		void *			_reallocBlock( TextBaseItem * pItem, int lines );
		void			_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle,
												State state );
		bool   			_updatePreferredSize( TextBaseItem * pItem );
		int				_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state ) const;
		
		inline BlockHeader *		_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
		inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
		inline LineInfo *			_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
		inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }
	
		int				_linePosX( const LineInfo * pLine, int itemWidth ) const;
		int				_linePosY( const void * pBlock, int line, int itemHeight ) const;
		int				_textPosY( const BlockHeader * pHeader, int itemHeight ) const;
		int				_charPosX( const TextBaseItem * pItem, int charOfs ) const;
		
		void 			_renderBack( TextBaseItem * pItem, GfxDevice * pDevice, const Rect& canvas, const Rect& clip );
		void 			_renderBackSection( TextBaseItem * pItem, GfxDevice * pDevice, const Rect& canvas, const Rect& clip, 
											int begChar, int endChar, Color color );

		
		enum struct SelectMode
		{
			Marked,					// Only select character/line that position is within.
			Closest,				// Select character/line that position is within or closest to.
			ClosestBegin,			// Select character/line whose beginning is closest to offset.
			ClosestEnd				// Select character/line whose end is closest to offset.
		};
		
		int				_lineAtPosY( const TextBaseItem * pItem, int posY, SelectMode mode ) const;
		int				_charAtPosX( const TextBaseItem * pItem, int line, int posX, SelectMode mode ) const;
		
		Origo			m_alignment;
		Caret_p			m_pCaret;

		Color			m_selectionBackColor;
		BlendMode		m_selectionBackRenderMode;
		
		Color			m_selectionCharColor;
		BlendMode		m_selectionCharBlend;


		TextBaseItem *	m_pFocusedItem;
		RouteId			m_tickRouteId;
	};



inline Glyph_p	StdTextMapper::_getGlyph( Font * pFont, uint16_t charCode ) const
{
	if( charCode <= 32 )
	{
		return 0;
	}
	else
	{
		Glyph_p p = pFont->getGlyph(charCode);
		if( !p )
		{
			p = pFont->getGlyph(0x25A1);			// White square character
			if( !p )
				p = pFont->getGlyph('?');
		}	
		return p;
	}
}


} // namespace wg
#endif //WG_STDTEXTMAPPER_DOT_H
