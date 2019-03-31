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

#ifndef	WG_TEXTMAPPER_DOT_H
#define	WG_TEXTMAPPER_DOT_H
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
	class CText;
	
	class GfxDevice;
	class CharBuffer;
	class TextStyle;
	
	class TextMapper;
	typedef	StrongPtr<TextMapper>	TextMapper_p;
	typedef	WeakPtr<TextMapper>	TextMapper_wp;
	
	class TextMapper : public Receiver
	{
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TextMapper_p	cast( Object * pObject );

		//.____ Internal __________________________________________
	
		virtual void	addComponent( CText * pText ) = 0;
		virtual void	removeComponent( CText * pText ) = 0;
	
		virtual int		charAtPos( const CText * pText, Coord pos ) const = 0;
		virtual Coord	charPos( const CText * pText, int charOfs ) const = 0;		// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charRect( const CText * pText, int charOfs ) const = 0;
		virtual int		charLine( const CText * pText, int charOfs ) const = 0;
		virtual int		lineBegin( const CText * pText, int lineNb ) const = 0;
		virtual int		lineEnd( const CText * pText, int lineNb ) const = 0;		

		virtual int		wordBegin( const CText * pText, int charOfs ) const = 0;
		virtual int		wordEnd( const CText * pText, int charOfs ) const = 0;		

	
		virtual void 	render( CText * pText, GfxDevice * pDevice, const Rect& canvas ) = 0;

		// Caret/selection update notification methods, only one of these needs to be called.
		// A selection change implies a caret move.

		virtual void	caretMove( CText * pText, int newOfs ) = 0;
		virtual void	selectionChange( CText * pText, int newSelectOfs, int newCaretOfs ) = 0;
	
		//
	
		virtual void	onTextModified( CText * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
		virtual void	onResized( CText * pText, Size newSize, Size oldSize ) = 0;
		virtual void	onStateChanged( CText * pText, State newState, State oldState ) = 0;
		virtual void	onStyleChanged( CText * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) = 0;
		virtual void	onCharStyleChanged( CText * pText, int ofs = 0, int len = INT_MAX ) = 0;
		virtual void	onRefresh( CText * pText ) = 0;
	
	
		virtual Size	preferredSize( const CText * pText ) const = 0;
		virtual int		matchingWidth( const CText * pText, int height ) const = 0;
		virtual int		matchingHeight( const CText * pText, int width ) const = 0;
	
		virtual Rect	rectForRange( const CText * pText, int ofs, int length ) const = 0;
		virtual Rect	rectForCaret( const CText * pText ) const = 0;
	
		virtual String	tooltip( const CText * pText ) const;
	
	
		// Methods for supporting a caret
	
		virtual Direction 	textDirection( CText * pText, int charOfs ) const = 0;
		virtual int			caretToPos( CText * pText, Coord pos, int& wantedLineOfs ) const = 0;
		virtual int			caretUp( CText * pText, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretDown( CText * pText, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretLeft( CText * pText, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretRight( CText * pText, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretHome( CText * pText, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretEnd( CText * pText, int charOfs, int& wantedLineOfs ) const = 0;

		virtual int			caretPrevWord( CText * pText, int charOfs ) const = 0;
		virtual int			caretNextWord( CText * pText, int charOfs ) const = 0;
		
	
	protected:
		TextMapper() {}
		virtual ~TextMapper() {};
	
	
		CharBuffer *  		_charBuffer( CText * pText ) const;
		const CharBuffer *  _charBuffer( const CText * pText ) const;
		const void *		_dataBlock( const CText * pText) const;
		void *				_dataBlock( CText * pText) const;
		int					_dataInt( const CText * pText ) const;

		const TextStyle *	_baseStyle( const CText * pText ) const;
		State				_state( const CText * pText ) const;
		const EditState	* 	_editState( const CText * pText ) const;
		
		void			_setComponentDataBlock( CText * pText, void * pBlock );
		void			_setComponentDataInt( CText * pText, int data );
		
		void			_setComponentDirty( CText * pText );
		void			_setComponentDirty( CText * pText, const Rect& rect );
		void			_requestComponentResize( CText * pText );
		
	};
	
	
	
	

} // namespace wg
#endif //WG_TEXTMAPPER_DOT_H
