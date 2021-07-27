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

		void			setPlacement( Placement placement );
		Placement		placement() const { return m_placement; }

		void			setLineWrap(bool wrap);
		bool			lineWrap() const { return m_bLineWrap; }


		//.____ Misc __________________________________________________

		void			setCaret( Caret * pCaret );
		Caret_p			caret() const { return m_pCaret; }

		//.____ Appearance _________________________________________________

		void			setSelectionBack(HiColor color, BlendMode renderMode = BlendMode::Replace );

		void			setSelectionCharColor(HiColor color, BlendMode blend = BlendMode::Replace );

		HiColor			selectionBackColor() { return m_selectionBackColor;  }
		BlendMode		selectionBackRenderMode() { return m_selectionBackRenderMode; }

		HiColor			selectionCharColor() { return m_selectionCharColor; }
		BlendMode		selectionCharColorBlend() { return m_selectionCharBlend; }


		//.____ Internal __________________________________________

		virtual void	addText( Text * pText ) override;
		virtual void	removeText( Text * pText ) override;

		virtual int		charAtPos( const Text * pText, CoordSPX pos ) const override;
		virtual CoordSPX charPos( const Text * pText, int charOfs ) const override;	// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual RectSPX	charRect( const Text * pText, int charOfs ) const override;
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

		virtual void 	render( Text * pText, GfxDevice * pDevice, const RectSPX& canvas ) override;

		virtual void	caretMove( Text * pText, int newOfs, int oldOfs ) override;
		virtual void	selectionChange( Text * pText, int newBeg, int newLen, int oldBeg, int oldLen ) override;

		virtual void	onTextModified( Text * pText, int ofs, int charsRemoved, int charsAdded ) override;
		virtual void	onResized( Text * pText, SizeSPX newSize, SizeSPX oldSize, int newScale, int oldScale ) override;
		virtual void	onStateChanged( Text * pText, State newState, State oldState ) override;
		virtual void	onStyleChanged( Text * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) override;
		virtual void	onCharStyleChanged( Text * pText, int ofs, int len ) override;
		virtual void	onRefresh( Text * pText ) override;



		virtual SizeSPX	preferredSize( const Text * pText, int scale ) const override;
		virtual spx		matchingWidth( const Text * pText, spx height, int scale ) const override;
		virtual spx		matchingHeight( const Text * pText, spx width, int scale ) const override;

		virtual RectSPX	rectForRange( const Text * pText, int ofs, int length ) const override;
		virtual RectSPX	rectForCaret( const Text * pText ) const override;

		virtual String 	tooltip( const Text * pText ) const override;

		virtual Direction 	textDirection( Text * pText, int charOfs ) const override;
		virtual int		caretToPos( Text * pText, CoordSPX pos, spx& wantedLineOfs ) const override;
		virtual int		caretUp( Text * pText, int charOfs, spx& wantedLineOfs ) const override;
		virtual int		caretDown( Text * pText, int charOfs, spx& wantedLineOfs ) const override;
		virtual int		caretLeft( Text * pText, int charOfs, spx& wantedLineOfs ) const override;
		virtual int		caretRight( Text * pText, int charOfs, spx& wantedLineOfs ) const override;
		virtual int		caretHome( Text * pText, int charOfs, spx& wantedLineOfs ) const override;
		virtual int		caretEnd( Text * pText, int charOfs, spx& wantedLineOfs ) const override;

		virtual int		caretPrevWord( Text * pText, int charOfs ) const override;
		virtual int		caretNextWord( Text * pText, int charOfs ) const override;



	protected:
		StdTextMapper();
		virtual ~StdTextMapper();


		struct BlockHeader
		{
			int nbLines;
			SizeSPX preferredSize;
			SizeSPX textSize;
		};

		struct LineInfo
		{
			int offset;				// Line start as offset in characters from beginning of text.
			int length;				// Length of line in characters, incl. line terminator,
			spx width;				// Width of line.
			spx height;				// Height of line.
			spx base;				// Offset for baseline from top of line.
			spx spacing;			// Offset from start of line to start of next line.
		};

		void			_update(int microPassed, int64_t microsecTimestamp) override;

		inline void		_getGlyphWithoutBitmap( Font * pFont, uint16_t charCode, Glyph& glyph ) const;
		inline void		_getGlyphWithBitmap(Font* pFont, uint16_t charCode, Glyph& glyph) const;

		int				_countLines( Text * pText, const Char * pChars ) const;

		int				_countFixedLines(const Char * pChars) const;
		int				_countWrapLines(const Char * pChars, const TextStyle * pBaseStyle, int scale, State state, spx maxLineWidth) const;
		spx				_calcMatchingHeight(const Char * pChars, const TextStyle * pBaseStyle, int scale, State state, spx maxLineWidth) const;

		void *			_reallocBlock( Text * pText, int lines );

		void			_updateLineInfo(Text * pText, void * pBlock, const Char * pChars );

		SizeSPX			_updateFixedLineInfo(BlockHeader * pHeader, LineInfo * pLines, const Char * pChars, const TextStyle * pBaseStyle, int scale, State state);
		SizeSPX			_updateWrapLineInfo(BlockHeader * pHeader, LineInfo * pLines, const Char * pChars, const TextStyle * pBaseStyle, int scale, State state, spx maxLineWidth);


		spx				_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, int scale, State state ) const;

		inline BlockHeader *		_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
		inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
		inline LineInfo *			_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
		inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }

		spx				_linePosX( const LineInfo * pLine, spx canvasWidth ) const;
		spx				_linePosY( const void * pBlock, int line, spx canvasHeight ) const;
		spx				_textPosY( const BlockHeader * pHeader, spx canvasHeight ) const;
		spx				_charPosX( const Text * pText, int charOfs ) const;

		void 			_renderBack( Text * pText, GfxDevice * pDevice, const RectSPX& canvas );
		void 			_renderBackSection( Text * pText, GfxDevice * pDevice, const RectSPX& canvas,
											int begChar, int endChar, HiColor color );


		enum struct SelectMode
		{
			Marked,					// Only select character/line that position is within.
			Closest,				// Select character/line that position is within or closest to.
			ClosestBegin,			// Select character/line whose beginning is closest to offset.
			ClosestEnd				// Select character/line whose end is closest to offset.
		};

		int				_lineAtPosY( const Text * pText, spx posY, SelectMode mode ) const;
		int				_charAtPosX( const Text * pText, int line, spx posX, SelectMode mode ) const;

		Placement		m_placement;
		Caret_p			m_pCaret;
		bool			m_bLineWrap;
		bool			m_bReceivingUpdates = false;

		HiColor			m_selectionBackColor;
		BlendMode		m_selectionBackRenderMode;

		HiColor			m_selectionCharColor;
		BlendMode		m_selectionCharBlend;


		Text *			m_pFocusedText;
		RouteId			m_tickRouteId;
	};



inline void StdTextMapper::_getGlyphWithoutBitmap( Font * pFont, uint16_t charCode, Glyph& glyph ) const
{
	if( charCode <= 32 )
	{
		glyph.pFont = nullptr;
		return;
	}
	else
	{
		pFont->getGlyphWithoutBitmap(charCode,glyph);
		if( !glyph.pFont )
		{
			pFont->getGlyphWithoutBitmap(0x25A1,glyph);			// White square character
			if( !glyph.pFont )
				pFont->getGlyphWithoutBitmap('?',glyph);
		}
	}
}

inline void StdTextMapper::_getGlyphWithBitmap(Font* pFont, uint16_t charCode, Glyph& glyph) const
{
	if (charCode <= 32)
	{
		glyph.pFont = nullptr;
		return;
	}
	else
	{
		pFont->getGlyphWithBitmap(charCode, glyph);
		if (!glyph.pFont)
		{
			pFont->getGlyphWithBitmap(0x25A1, glyph);			// White square character
			if (!glyph.pFont)
				pFont->getGlyphWithBitmap('?', glyph);
		}
	}
}


} // namespace wg
#endif //WG_STDTEXTMAPPER_DOT_H
