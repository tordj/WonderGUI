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
	class TextBaseItem;
	
	class GfxDevice;
	class CharBuffer;
	class TextStyle;
	
	class TextMapper;
	typedef	StrongPtr<TextMapper,Object_p>	TextMapper_p;
	typedef	WeakPtr<TextMapper,Object_wp>	TextMapper_wp;
	
	class TextMapper : public Receiver
	{
	public:
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static TextMapper_p	cast( const Object_p& pObject );

		//.____ Internal __________________________________________
	
		virtual void	addItem( TextBaseItem * pItem ) = 0;
		virtual void	removeItem( TextBaseItem * pItem ) = 0;
	
		virtual int		charAtPos( const TextBaseItem * pItem, Coord pos ) const = 0;
		virtual Coord	charPos( const TextBaseItem * pItem, int charOfs ) const = 0;		// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charRect( const TextBaseItem * pItem, int charOfs ) const = 0;
		virtual int		charLine( const TextBaseItem * pItem, int charOfs ) const = 0;
		virtual int		lineBegin( const TextBaseItem * pItem, int lineNb ) const = 0;
		virtual int		lineEnd( const TextBaseItem * pItem, int lineNb ) const = 0;		

		virtual int		wordBegin( const TextBaseItem * pItem, int charOfs ) const = 0;
		virtual int		wordEnd( const TextBaseItem * pItem, int charOfs ) const = 0;		

	
		virtual void 	renderItem( TextBaseItem * pText, GfxDevice * pDevice, const Rect& canvas, const Rect& clip ) = 0;

		// Caret/selection update notification methods, only one of these needs to be called.
		// A selection change implies a caret move.

		virtual void	caretMove( TextBaseItem * pText, int newOfs ) = 0;
		virtual void	selectionChange( TextBaseItem * pText, int newSelectOfs, int newCaretOfs ) = 0;
	
		//
	
		virtual void	onTextModified( TextBaseItem * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
		virtual void	onResized( TextBaseItem * pText, Size newSize, Size oldSize ) = 0;
		virtual void	onStateChanged( TextBaseItem * pText, State newState, State oldState ) = 0;
		virtual void	onStyleChanged( TextBaseItem * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) = 0;
		virtual void	onCharStyleChanged( TextBaseItem * pText, int ofs = 0, int len = INT_MAX ) = 0;
		virtual void	onRefresh( TextBaseItem * pText ) = 0;
	
	
		virtual Size	preferredSize( const TextBaseItem * pText ) const = 0;
		virtual int		matchingWidth( const TextBaseItem * pText, int height ) const = 0;
		virtual int		matchingHeight( const TextBaseItem * pText, int width ) const = 0;
	
		virtual Rect	rectForRange( const TextBaseItem * pText, int ofs, int length ) const = 0;
		virtual Rect	rectForCaret( const TextBaseItem * pText ) const = 0;
	
		virtual String	tooltip( const TextBaseItem * pText ) const;
	
	
		// Methods for supporting a caret
	
		virtual Direction 	textDirection( TextBaseItem * pItem, int charOfs ) const = 0;
		virtual int			caretToPos( TextBaseItem * pItem, Coord pos, int& wantedLineOfs ) const = 0;
		virtual int			caretUp( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretDown( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretLeft( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretRight( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretHome( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretEnd( TextBaseItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;

		virtual int			caretPrevWord( TextBaseItem * pItem, int charOfs ) const = 0;
		virtual int			caretNextWord( TextBaseItem * pItem, int charOfs ) const = 0;
		
	
	protected:
		TextMapper() {}
		virtual ~TextMapper() {};
	
	
		CharBuffer *  		_charBuffer( TextBaseItem * pItem ) const;
		const CharBuffer *  _charBuffer( const TextBaseItem * pItem ) const;
		const void *		_itemDataBlock( const TextBaseItem * pItem) const;
		void *				_itemDataBlock( TextBaseItem * pItem) const;
		int					_itemDataInt( const TextBaseItem * pItem ) const;

		const TextStyle *	_baseStyle( const TextBaseItem * pItem ) const;
		State				_state( const TextBaseItem * pItem ) const;
		const EditState	* 	_editState( const TextBaseItem * pItem ) const;
		
		void			_setItemDataBlock( TextBaseItem * pItem, void * pBlock );
		void			_setItemDataInt( TextBaseItem * pItem, int data );
		
		void			_setItemDirty( TextBaseItem * pItem );
		void			_setItemDirty( TextBaseItem * pItem, const Rect& rect );
		void			_requestItemResize( TextBaseItem * pItem );
		
	};
	
	
	
	

} // namespace wg
#endif //WG_TEXTMAPPER_DOT_H
