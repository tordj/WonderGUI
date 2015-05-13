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
typedef	WgStrongPtr<WgStandardPresenter,WgTextPresenterPtr>		WgStandardPresenterPtr;
typedef	WgWeakPtr<WgStandardPresenter,WgTextPresenterWeakPtr>	WgStandardPresenterWeakPtr;

class WgStandardPresenter : public WgTextPresenter
{
public:
	static WgStandardPresenterPtr Create() { return WgStandardPresenterPtr(new WgStandardPresenter()); }

	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgStandardPresenterPtr	Cast( const WgObjectPtr& pObject );

	void			SetAlignment( WgOrigo alignment );
	WgOrigo			Alignment() const { return m_alignment; }

	virtual void	AddField( WgPresentableField * pField );
	virtual void	RemoveField( WgPresentableField * pField );

	virtual int		CoordToChar( const WgPresentableField * pField, WgCoord pos );
	virtual WgRect	CharToRect( const WgPresentableField * pField, int charOfs );


	virtual void 	RenderField( WgPresentableField * pField, WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& clip );

	virtual void	OnTextModified( WgPresentableField * pField, int ofs, int charsRemoved, int charsAdded );
	virtual void	OnFieldResize( WgPresentableField * pField, WgSize newSize );
	virtual void	OnStateChange( WgPresentableField * pField, WgState newState, WgState oldState );
	virtual void	OnStyleChange( WgPresentableField * pField );
	virtual void	OnRefresh( WgPresentableField * pField );


	virtual WgSize	PreferredSize( const WgPresentableField * pField ) const;
	virtual int		MatchingWidth( const WgPresentableField * pField, int height ) const;
	virtual int		MatchingHeight( const WgPresentableField * pField, int width ) const;

	virtual WgRect	RectForRange( const WgPresentableField * pField, int ofs, int length ) const;

	virtual WgString Tooltip( const WgPresentableField * pField ) const;

	virtual int		CoordToCaretPos( WgPresentableField * pField, WgCoord pos );
	virtual int		MoveCaret( WgPresentableField * pField, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif );


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
