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

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

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

		virtual void	addText( Text * pText ) override;
		virtual void	removeText( Text * pText ) override;

		virtual int		charAtPos( const Text * pText, CoordI pos ) const override;
		virtual CoordI	charPos( const Text * pText, int charOfs ) const override;	// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual RectI	charRect( const Text * pText, int charOfs ) const override;
		virtual int		charLine( const Text * pText, int charOfs ) const override;

//		virtual int		charLineOfs( const Text * pText, int charOfs ) const;
//		virtual CoordI	lineTopLeftPos( const Text * pText, int line ) const;
//		virtual CoordI	lineBaselinePos( const Text * pText, int line ) const;
//		virtual int		lineWidth( const Text * pText, int line ) const;
//		virtual int		lineHeight( const Text * pText, int line ) const;
//		virtual RectI	lineGeo( const Text * pText, int line ) const;
//		virtual int		lines( const Text * pText ) const;

		virtual int		lineBegin( const Text * pText, int lineNb ) const override;
		virtual int		lineEnd( const Text * pText, int lineNb ) const override;

		virtual int		wordBegin( const Text * pText, int charOfs ) const override;
		virtual int		wordEnd( const Text * pText, int charOfs ) const override;


		virtual void 	receive( Msg * pMsg ) override;
		virtual void 	render( Text * pText, GfxDevice * pDevice, const RectI& canvas ) override;

		virtual void	caretMove( Text * pText, int newOfs, int oldOfs ) override;
		virtual void	selectionChange( Text * pText, int newBeg, int newLen, int oldBeg, int oldLen ) override;

		virtual void	onTextModified( Text * pText, int ofs, int charsRemoved, int charsAdded ) override;
		virtual void	onResized( Text * pText, SizeI newSize, SizeI oldSize ) override;
		virtual void	onStateChanged( Text * pText, State newState, State oldState ) override;
		virtual void	onStyleChanged( Text * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) override;
		virtual void	onCharStyleChanged( Text * pText, int ofs, int len ) override;
		virtual void	onRefresh( Text * pText ) override;



		virtual SizeI	preferredSize( const Text * pText ) const override;
		virtual int		matchingWidth( const Text * pText, int height ) const override;
		virtual int		matchingHeight( const Text * pText, int width ) const override;

		virtual RectI	rectForRange( const Text * pText, int ofs, int length ) const override;
		virtual RectI	rectForCaret( const Text * pText ) const override;

		virtual String 	tooltip( const Text * pText ) const override;

		virtual Direction 	textDirection( Text * pText, int charOfs ) const override;
		virtual int		caretToPos( Text * pText, CoordI pos, int& wantedLineOfs ) const override;
		virtual int		caretUp( Text * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretDown( Text * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretLeft( Text * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretRight( Text * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretHome( Text * pText, int charOfs, int& wantedLineOfs ) const override;
		virtual int		caretEnd( Text * pText, int charOfs, int& wantedLineOfs ) const override;

		virtual int		caretPrevWord( Text * pText, int charOfs ) const override;
		virtual int		caretNextWord( Text * pText, int charOfs ) const override;



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

		int				_countLines( Text * pText, const Char * pChars ) const;

		int				_countFixedLines(const Char * pChars) const;
		int				_countWrapLines(const Char * pChars, const TextStyle * pBaseStyle, State state, int maxLineWidth) const;
		int				_calcMatchingHeight(const Char * pChars, const TextStyle * pBaseStyle, State state, int maxLineWidth) const;

		void *			_reallocBlock( Text * pText, int lines );

		void			_updateLineInfo(Text * pText, void * pBlock, const Char * pChars );

		SizeI			_updateFixedLineInfo(BlockHeader * pHeader, LineInfo * pLines, const Char * pChars, const TextStyle * pBaseStyle, State state);
		SizeI			_updateWrapLineInfo(BlockHeader * pHeader, LineInfo * pLines, const Char * pChars, const TextStyle * pBaseStyle, State state, int maxLineWidth);


		int				_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state ) const;

		inline BlockHeader *		_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
		inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
		inline LineInfo *			_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
		inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }

		int				_linePosX( const LineInfo * pLine, int canvasWidth ) const;
		int				_linePosY( const void * pBlock, int line, int canvasHeight ) const;
		int				_textPosY( const BlockHeader * pHeader, int canvasHeight ) const;
		int				_charPosX( const Text * pText, int charOfs ) const;

		void 			_renderBack( Text * pText, GfxDevice * pDevice, const RectI& canvas );
		void 			_renderBackSection( Text * pText, GfxDevice * pDevice, const RectI& canvas,
											int begChar, int endChar, Color color );


		enum struct SelectMode
		{
			Marked,					// Only select character/line that position is within.
			Closest,				// Select character/line that position is within or closest to.
			ClosestBegin,			// Select character/line whose beginning is closest to offset.
			ClosestEnd				// Select character/line whose end is closest to offset.
		};

		int				_lineAtPosY( const Text * pText, int posY, SelectMode mode ) const;
		int				_charAtPosX( const Text * pText, int line, int posX, SelectMode mode ) const;

		Origo			m_alignment;
		Caret_p			m_pCaret;
		bool			m_bLineWrap;

		Color			m_selectionBackColor;
		BlendMode		m_selectionBackRenderMode;

		Color			m_selectionCharColor;
		BlendMode		m_selectionCharBlend;


		Text *			m_pFocusedText;
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
