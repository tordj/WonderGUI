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

#ifndef	WG_STANDARDPRESENTER_DOT_H
#define	WG_STANDARDPRESENTER_DOT_H

#ifndef WG_TEXTPRESENTER_DOT_H
#	include <wg_textpresenter.h>
#endif 

#ifndef WG_TEXTSTYLE_DOT_H
#	include <wg_textstyle.h>
#endif

namespace wg 
{
	
	class StandardPresenter;
	typedef	WgStrongPtr<StandardPresenter,TextPresenter_p>		StandardPresenter_p;
	typedef	WgWeakPtr<StandardPresenter,TextPresenter_wp>	StandardPresenter_wp;
	
	class StandardPresenter : public TextPresenter
	{
	public:
		static StandardPresenter_p create() { return StandardPresenter_p(new StandardPresenter()); }
	
		bool						isInstanceOf( const char * pClassName ) const;
		const char *				className( void ) const;
		static const char			CLASSNAME[];
		static StandardPresenter_p	cast( const Object_p& pObject );
	
		void			setAlignment( WgOrigo alignment );
		WgOrigo			alignment() const { return m_alignment; }
	
		virtual void	addField( PresentableField * pField );
		virtual void	removeField( PresentableField * pField );
	
		virtual int		coordToChar( const PresentableField * pField, Coord pos );
		virtual Rect	charToRect( const PresentableField * pField, int charOfs );
	
	
		virtual void 	renderField( PresentableField * pField, GfxDevice * pDevice, const Rect& canvas, const Rect& clip );
	
		virtual void	onTextModified( PresentableField * pField, int ofs, int charsRemoved, int charsAdded );
		virtual void	onFieldResize( PresentableField * pField, Size newSize );
		virtual void	onStateChange( PresentableField * pField, State newState, State oldState );
		virtual void	onStyleChange( PresentableField * pField );
		virtual void	onRefresh( PresentableField * pField );
	
	
		virtual Size	preferredSize( const PresentableField * pField ) const;
		virtual int		matchingWidth( const PresentableField * pField, int height ) const;
		virtual int		matchingHeight( const PresentableField * pField, int width ) const;
	
		virtual Rect	rectForRange( const PresentableField * pField, int ofs, int length ) const;
	
		virtual String tooltip( const PresentableField * pField ) const;
	
		virtual int		coordToCaretPos( PresentableField * pField, Coord pos );
		virtual int		moveCaret( PresentableField * pField, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif );
	
	
	protected:
		StandardPresenter();
		virtual ~StandardPresenter();
	
	
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
		void *			_reallocBlock( PresentableField * pField, int lines );
		void			_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const CharBuffer * pBuffer, const TextStyle * pBaseStyle,
												State state );
		bool   			_updatePreferredSize( BlockHeader * pHeader, LineInfo * pLines );
		
		inline BlockHeader *	_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
		inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
		inline LineInfo *		_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
		inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }
	
		int				_lineOfsX( LineInfo * pLine, int fieldWidth ) const;
		int				_textOfsY( BlockHeader * pHeader, int fieldHeight ) const;
		
		
		WgOrigo		m_alignment;
		
	};
	

} // namespace wg
#endif //WG_STANDARDPRESENTER_DOT_H
