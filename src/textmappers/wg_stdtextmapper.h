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
	typedef	StrongPtr<StdTextMapper>		StdTextMapper_p;
	typedef	WeakPtr<StdTextMapper>	StdTextMapper_wp;

	class StdTextMapper : public TextMapper
	{
	public:
		//.____ Creation __________________________________________

		static StdTextMapper_p create() { return StdTextMapper_p(new StdTextMapper()); }

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const override;
		const char *			className( void ) const override;
		static const char		CLASSNAME[];
		static StdTextMapper_p	cast( Object * pObject );

		//.____ Behavior __________________________________________________

		void			setAlignment( Origo alignment );
		Origo			alignment() const { return m_alignment; }

		void			setLineWrap(bool wrap);
		bool			lineWrap() const { return m_bLineWrap; }


		//.____ Misc __________________________________________________

		void			setCaret( Caret * pCaret );
		Caret_p			caret() const { return m_pCaret; }

		//.____ Appearance _________________________________________________

		void			setSelectionBack(Color color, BlendMode renderMode = BlendMode::Replace );

		void			setSelectionCharColor(Color color, BlendMode blend = BlendMode::Replace );

		Color			selectionBackColor() { return m_selectionBackColor;  }
		BlendMode		selectionBackRenderMode() { return m_selectionBackRenderMode; }

		Color			selectionCharColor() { return m_selectionCharColor; }
		BlendMode		selectionCharColorBlend() { return m_selectionCharBlend; }


		//.____ Internal __________________________________________

		virtual void	addComponent( CText * pText ) override;
		virtual void	removeComponent( CText * pText ) override;

		virtual int		charAtPos( const CText * pText, CoordI pos ) const override;
		virtual CoordI	charPos( const CText * pText, int charOfs ) const override;	// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual RectI	charRect( const CText * pText, int charOfs ) const override;
		virtual int		charLine( const CText * pText, int charOfs ) const override;

//		virtual int		charLineOfs( const CText * pText, int charOfs ) const;
//		virtual CoordI	lineTopLeftPos( const CText * pText, int line ) const;
//		virtual CoordI	lineBaselinePos( const CText * pText, int line ) const;
//		virtual int		lineWidth( const CText * pText, int line ) const;
//		virtual int		lineHeight( const CText * pText, int line ) const;
//		virtual RectI	lineGeo( const CText * pText, int line ) const;
//		virtual int		lines( const CText * pText ) const;

		virtual int		lineBegin( const CText * pText, int lineNb ) const override;
		virtual int		lineEnd( const CText * pText, int lineNb ) const override;

		virtual int		wordBegin( const CText * pText, int charOfs ) const override;
		virtual int		wordEnd( const CText * pText, int charOfs ) const override;


		virtual void 	receive( Msg * pMsg ) override;
		virtual void 	render( CText * pText, GfxDevice * pDevice, const RectI& canvas ) override;

		virtual void	caretMove( CText * pText, int newOfs ) override;
		virtual void	selectionChange( CText * pText, int newSelectOfs, int newCaretOfs ) override;

		virtual void	onTextModified( CText * pText, int ofs, int charsRemoved, int charsAdded ) override;
		virtual void	onResized( CText * pText, SizeI newSize, SizeI oldSize ) override;
		virtual void	onStateChanged( CText * pText, State newState, State oldState ) override;
		virtual void	onStyleChanged( CText * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) override;
		virtual void	onCharStyleChanged( CText * pText, int ofs, int len ) override;
		virtual void	onRefresh( CText * pText ) override;



		virtual SizeI	preferredSize( const CText * pText ) const override;
		virtual int		matchingWidth( const CText * pText, int height ) const override;
		virtual int		matchingHeight( const CText * pText, int width ) const override;

		virtual RectI	rectForRange( const CText * pText, int ofs, int length ) const override;
		virtual RectI	rectForCaret( const CText * pText ) const override;

		virtual String 	tooltip( const CText * pText ) const override;

		virtual Direction 	textDirection( CText * pText, int charOfs ) const override;
		virtual int		caretToPos( CText * pText, CoordI pos, int& wantedLineOfs ) const override;
		virtual int		caretUp( CText * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretDown( CText * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretLeft( CText * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretRight( CText * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretHome( CText * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretEnd( CText * pText, int charOfs, int& wantedLineOfs ) const override;

		virtual int		caretPrevWord( CText * pText, int charOfs ) const override;
		virtual int		caretNextWord( CText * pText, int charOfs ) const override;



	protected:
		StdTextMapper();
		virtual ~StdTextMapper();


		struct BlockHeader
		{
			int nbLines;
			SizeI preferredSize;
			SizeI textSize;
		};

		struct LineInfo
		{
			int offset;				// Line start as offset in characters from beginning of text.
			int length;				// Length of line in characters, incl. line terminator,
			int width;				// Width of line in pixels.
			short height;			// Height of line in pixels.
			short base;				// Offset for baseline from top of line in pixels.
			short spacing;			// Offset from start of line to start of next line.
		};

		inline Glyph_p	_getGlyph( Font * pFont, uint16_t charCode ) const;

		int				_countLines( CText * pText, const CharBuffer * pBuffer ) const;

		int				_countFixedLines(const CharBuffer * pBuffer) const;
		int				_countWrapLines(const CharBuffer * pBuffer, const TextStyle * pBaseStyle, State state, int maxLineWidth) const;
		int				_calcMatchingHeight(const CharBuffer * pBuffer, const TextStyle * pBaseStyle, State state, int maxLineWidth) const;

		void *			_reallocBlock( CText * pText, int lines );

		void			_updateLineInfo(CText * pText, void * pBlock, const CharBuffer * pBuffer );

		SizeI			_updateFixedLineInfo(BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle, State state);
		SizeI			_updateWrapLineInfo(BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle, State state, int maxLineWidth);


		int				_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state ) const;

		inline BlockHeader *		_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
		inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
		inline LineInfo *			_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
		inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }

		int				_linePosX( const LineInfo * pLine, int canvasWidth ) const;
		int				_linePosY( const void * pBlock, int line, int canvasHeight ) const;
		int				_textPosY( const BlockHeader * pHeader, int canvasHeight ) const;
		int				_charPosX( const CText * pText, int charOfs ) const;

		void 			_renderBack( CText * pText, GfxDevice * pDevice, const RectI& canvas );
		void 			_renderBackSection( CText * pText, GfxDevice * pDevice, const RectI& canvas,
											int begChar, int endChar, Color color );


		enum struct SelectMode
		{
			Marked,					// Only select character/line that position is within.
			Closest,				// Select character/line that position is within or closest to.
			ClosestBegin,			// Select character/line whose beginning is closest to offset.
			ClosestEnd				// Select character/line whose end is closest to offset.
		};

		int				_lineAtPosY( const CText * pText, int posY, SelectMode mode ) const;
		int				_charAtPosX( const CText * pText, int line, int posX, SelectMode mode ) const;

		Origo			m_alignment;
		Caret_p			m_pCaret;
		bool			m_bLineWrap;

		Color			m_selectionBackColor;
		BlendMode		m_selectionBackRenderMode;

		Color			m_selectionCharColor;
		BlendMode		m_selectionCharBlend;


		CText *	m_pFocusedComponent;
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
