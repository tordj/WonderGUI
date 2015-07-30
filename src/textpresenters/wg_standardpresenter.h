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

class WgStandardPresenter;
typedef	WgStrongPtr<WgStandardPresenter,WgTextPresenter_p>		WgStandardPresenter_p;
typedef	WgWeakPtr<WgStandardPresenter,WgTextPresenter_wp>	WgStandardPresenter_wp;

class WgStandardPresenter : public WgTextPresenter
{
public:
	static WgStandardPresenter_p create() { return WgStandardPresenter_p(new WgStandardPresenter()); }

	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgStandardPresenter_p	cast( const WgObject_p& pObject );

	void			setAlignment( WgOrigo alignment );
	WgOrigo			alignment() const { return m_alignment; }

	virtual void	addField( WgPresentableField * pField );
	virtual void	removeField( WgPresentableField * pField );

	virtual int		coordToChar( const WgPresentableField * pField, WgCoord pos );
	virtual WgRect	charToRect( const WgPresentableField * pField, int charOfs );


	virtual void 	renderField( WgPresentableField * pField, WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& clip );

	virtual void	onTextModified( WgPresentableField * pField, int ofs, int charsRemoved, int charsAdded );
	virtual void	onFieldResize( WgPresentableField * pField, WgSize newSize );
	virtual void	onStateChange( WgPresentableField * pField, WgState newState, WgState oldState );
	virtual void	onStyleChange( WgPresentableField * pField );
	virtual void	onRefresh( WgPresentableField * pField );


	virtual WgSize	preferredSize( const WgPresentableField * pField ) const;
	virtual int		matchingWidth( const WgPresentableField * pField, int height ) const;
	virtual int		matchingHeight( const WgPresentableField * pField, int width ) const;

	virtual WgRect	rectForRange( const WgPresentableField * pField, int ofs, int length ) const;

	virtual WgString tooltip( const WgPresentableField * pField ) const;

	virtual int		coordToCaretPos( WgPresentableField * pField, WgCoord pos );
	virtual int		moveCaret( WgPresentableField * pField, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif );


protected:
	WgStandardPresenter();
	virtual ~WgStandardPresenter();


	struct BlockHeader
	{
		int nbLines;
		WgSize preferredSize;
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

	int				_countLines( const WgCharBuffer * pBuffer );
	void *			_reallocBlock( WgPresentableField * pField, int lines );
	void			_updateLineInfo( BlockHeader * pHeader, LineInfo * pLines, const WgCharBuffer * pBuffer, const WgTextStyle * pBaseStyle,
											WgState state );
	bool   			_updatePreferredSize( BlockHeader * pHeader, LineInfo * pLines );
	
	inline BlockHeader *	_header( void * pBlock ) { return static_cast<BlockHeader*>(pBlock); }
	inline const BlockHeader *	_header( const void * pBlock ) const { return static_cast<const BlockHeader*>(pBlock); }
	inline LineInfo *		_lineInfo( void * pBlock ) { return reinterpret_cast<LineInfo*>(&(((BlockHeader *) pBlock)[1])); }
	inline const LineInfo *		_lineInfo( const void * pBlock ) const { return reinterpret_cast<const LineInfo*>(&(((const BlockHeader *) pBlock)[1])); }

	int				_lineOfsX( LineInfo * pLine, int fieldWidth ) const;
	int				_textOfsY( BlockHeader * pHeader, int fieldHeight ) const;
	
	
	WgOrigo		m_alignment;
	
};

#endif //WG_STANDARDPRESENTER_DOT_H
