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

#ifndef	WG_TEXTLAYOUT_DOT_H
#define	WG_TEXTLAYOUT_DOT_H
#pragma once

#include <wg_userdefines.h>
#include <wg_pointers.h>
#include <wg_geo.h>
#include <wg_string.h>
#include <wg_key.h>
#include <wg_receiver.h>

namespace wg
{
	class EditState;

	class GfxDevice;
	class CharBuffer;
	class TextStyle;

	class Text
	{
		friend class TextLayout;

	private:
		virtual SizeSPX		_textSize() const = 0;
		virtual State		_textState() const = 0;
		virtual TextStyle * _textStyle() const = 0;
		
		virtual const Char *_textBegin() const = 0;
		virtual int 		_textLength() const = 0;

		virtual bool		_caretVisible() const = 0;
		virtual int			_caretOffset() const = 0;
		virtual std::tuple<int,int>	_selectedText() const = 0;		// Begin/end of selection

		virtual void		_mapperRequestRender() = 0;
		virtual void		_mapperRequestRender(const RectSPX& rect) = 0;
		virtual void		_mapperRequestResize() = 0;
		
		virtual int			_scale() const = 0;

		union
		{
			void *			m_pMapperData;
			int				m_mapperData;
		};
		
	};




	class TextLayout;
	typedef	StrongPtr<TextLayout>	TextLayout_p;
	typedef	WeakPtr<TextLayout>	TextLayout_wp;

	class TextLayout : public Receiver
	{
	public:
		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal __________________________________________

		virtual void	addText( Text * pText ) = 0;
		virtual void	removeText( Text * pText ) = 0;

		virtual int		charAtPos( const Text * pText, CoordSPX pos ) const = 0;
		virtual CoordSPX charPos( const Text * pText, int charOfs ) const = 0;		// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual RectSPX	charRect( const Text * pText, int charOfs ) const = 0;
		virtual int		charLine( const Text * pText, int charOfs ) const = 0;
		virtual int		lineBegin( const Text * pText, int lineNb ) const = 0;
		virtual int		lineEnd( const Text * pText, int lineNb ) const = 0;

		virtual int		wordBegin( const Text * pText, int charOfs ) const = 0;
		virtual int		wordEnd( const Text * pText, int charOfs ) const = 0;


		virtual void 	render( Text * pText, GfxDevice * pDevice, const RectSPX& canvas ) = 0;

		// Caret/selection update notification methods, only one of these needs to be called.
		// A selection change implies a caret move.

		virtual void	caretMove( Text * pText, int newOfs, int oldOfs ) = 0;
		virtual void	selectionChange( Text * pText, int selectOfs, int caretOfs, int oldSelectOfs, int oldCaretOfs ) = 0;

		//

		virtual void	onTextModified( Text * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
		virtual void	onResized( Text * pText, SizeSPX newSize, SizeSPX oldSize, int newScale, int oldScale ) = 0;
		virtual void	onStateChanged( Text * pText, State newState, State oldState ) = 0;
		virtual void	onStyleChanged( Text * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) = 0;
		virtual void	onCharStyleChanged( Text * pText, int ofs = 0, int len = INT_MAX ) = 0;
		virtual void	onRefresh( Text * pText ) = 0;


		virtual SizeSPX	defaultSize( const Text * pText, int scale ) const = 0;
		virtual spx		matchingWidth( const Text * pText, spx height, int scale ) const = 0;
		virtual spx		matchingHeight( const Text * pText, spx width, int scale ) const = 0;

		virtual RectSPX	rectForRange( const Text * pText, int ofs, int length ) const = 0;
		virtual RectSPX	rectForCaret( const Text * pText ) const = 0;

		virtual String	tooltip( const Text * pText ) const;


		// Methods for supporting a caret

		virtual Direction 	textDirection( Text * pText, int charOfs ) const = 0;
		virtual int			caretToPos( Text * pText, CoordSPX pos, spx& wantedLineOfs ) const = 0;
		virtual int			caretUp( Text * pText, int charOfs, spx& wantedLineOfs ) const = 0;
		virtual int			caretDown( Text * pText, int charOfs, spx& wantedLineOfs ) const = 0;
		virtual int			caretLeft( Text * pText, int charOfs, spx& wantedLineOfs ) const = 0;
		virtual int			caretRight( Text * pText, int charOfs, spx& wantedLineOfs ) const = 0;
		virtual int			caretHome( Text * pText, int charOfs, spx& wantedLineOfs ) const = 0;
		virtual int			caretEnd( Text * pText, int charOfs, spx& wantedLineOfs ) const = 0;

		virtual int			caretPrevWord( Text * pText, int charOfs ) const = 0;
		virtual int			caretNextWord( Text * pText, int charOfs ) const = 0;


	protected:
		TextLayout() {}
		virtual ~TextLayout() {};

		SizeSPX				_size( const Text * pText ) const;
		int					_length( const Text * pText ) const;
		int					_scale(const Text* pText) const;

		bool				_caretVisible( const Text * pText ) const;
		int					_caretOfs( const Text * pText ) const;
		std::tuple<int,int>	_selection( const Text * pText ) const;		// Begin/end of selection

		const Char *  		_chars( const Text * pText ) const;
		const void *		_dataBlock( const Text * pText) const;
		void *				_dataBlock( Text * pText) const;
		int					_dataInt( const Text * pText ) const;


		const TextStyle *	_baseStyle( const Text * pText ) const;
		State				_state( const Text * pText ) const;

		void			_setTextDataBlock( Text * pText, void * pBlock );
		void			_setTextDataInt( Text * pText, int data );

		void			_setTextDirty( Text * pText );
		void			_setTextDirty( Text * pText, const RectSPX& rect );
		void			_requestTextResize( Text * pText );

	};





} // namespace wg
#endif //WG_TEXTLAYOUT_DOT_H