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
	
		virtual void	addField( PrintableField * pField );
		virtual void	removeField( PrintableField * pField );
	
		virtual int		coordToChar( const PrintableField * pField, Coord pos );
		virtual Coord	charToCoord( const PrintableField * pField, int charOfs );	// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charToRect( const PrintableField * pField, int charOfs );
		virtual int		lineOfChar( const PrintableField * pField, int charOfs );
		virtual int		lineBegin( const PrintableField * pField, int lineNb );
		virtual int		lineEnd( const PrintableField * pField, int lineNb );
	
	
		virtual void 	renderField( PrintableField * pField, GfxDevice * pDevice, const Rect& canvas, const Rect& clip );
	
		virtual void	onTextModified( PrintableField * pField, int ofs, int charsRemoved, int charsAdded );
		virtual void	onFieldResized( PrintableField * pField, Size newSize, Size oldSize );
		virtual void	onStateChanged( PrintableField * pField, State newState, State oldState );
		virtual void	onStyleChanged( PrintableField * pField, TextStyle * pNewStyle, TextStyle * pOldStyle );
		virtual void	onCharStyleChanged( PrintableField * pText, int ofs, int len );
		virtual void	onRefresh( PrintableField * pField );
	
	
		virtual Size	preferredSize( const PrintableField * pField ) const;
		virtual int		matchingWidth( const PrintableField * pField, int height ) const;
		virtual int		matchingHeight( const PrintableField * pField, int width ) const;
	
		virtual Rect	rectForRange( const PrintableField * pField, int ofs, int length ) const;
	
		virtual String 	tooltip( const PrintableField * pField ) const;
	
		virtual int		coordToCaretPos( PrintableField * pField, Coord pos );
		virtual int		moveCaret( PrintableField * pField, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, ModifierKeys modif );
	
	
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
	
		int				_countLines( const CharBuffer * pBuffer );
		void *			_reallocBlock( PrintableField * pField, int lines );
		void			_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle,
												State state );
		bool   			_updatePreferredSize( BlockHeader * pHeader, LineInfo * pLines );
		int				_charDistance( const Char * pFirst, const Char * pLast, const TextAttr& baseAttr, State state );
		
		inline BlockHeader *		_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
		inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
		inline LineInfo *			_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
		inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }
	
		int				_lineOfsX( const LineInfo * pLine, int fieldWidth ) const;
		int				_textOfsY( const BlockHeader * pHeader, int fieldHeight ) const;
		
		
		Origo		m_alignment;
		
	};
	

} // namespace wg
#endif //WG_STANDARDPRINTER_DOT_H
