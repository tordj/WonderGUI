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

#ifndef	WG_PRINTER_DOT_H
#define	WG_PRINTER_DOT_H

#ifndef WG_USERDEFINES_DOT_H
#	include <wg_userdefines.h>
#endif 

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_STRING_DOT_H
#	include <wg_string.h>
#endif

#ifndef WG_KEY_DOT_H
#	include <wg_key.h>
#endif

namespace wg 
{
	
	class PrintableItem;
	
	class GfxDevice;
	class CharBuffer;
	class TextStyle;
	
	class Printer;
	typedef	StrongPtr<Printer,Object_p>	Printer_p;
	typedef	WeakPtr<Printer,Object_wp>	Printer_wp;
	
	class Printer : public Object
	{
	public:
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static Printer_p	cast( const Object_p& pObject );
	
		virtual void	addItem( PrintableItem * pItem ) = 0;
		virtual void	removeItem( PrintableItem * pItem ) = 0;
	
		virtual int		charAtPos( const PrintableItem * pItem, Coord pos ) const = 0;
		virtual Coord	charPos( const PrintableItem * pItem, int charOfs ) const = 0;		// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charRect( const PrintableItem * pItem, int charOfs ) const = 0;
		virtual int		charLine( const PrintableItem * pItem, int charOfs ) const = 0;
		virtual int		lineBegin( const PrintableItem * pItem, int lineNb ) const = 0;
		virtual int		lineEnd( const PrintableItem * pItem, int lineNb ) const = 0;		

		virtual int		wordBegin( const PrintableItem * pItem, int charOfs ) const = 0;
		virtual int		wordEnd( const PrintableItem * pItem, int charOfs ) const = 0;		

	
		virtual void 	renderItem( PrintableItem * pText, GfxDevice * pDevice, const Rect& canvas, const Rect& clip ) = 0;
	
		virtual void	onTextModified( PrintableItem * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
		virtual void	requestResized( PrintableItem * pText, Size newSize, Size oldSize ) = 0;
		virtual void	onStateChanged( PrintableItem * pText, State newState, State oldState ) = 0;
		virtual void	onStyleChanged( PrintableItem * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) = 0;
		virtual void	onCharStyleChanged( PrintableItem * pText, int ofs = 0, int len = INT_MAX ) = 0;
		virtual void	onRefresh( PrintableItem * pText ) = 0;
	
	
		virtual Size	preferredSize( const PrintableItem * pText ) const = 0;
		virtual int		matchingWidth( const PrintableItem * pText, int height ) const = 0;
		virtual int		matchingHeight( const PrintableItem * pText, int width ) const = 0;
	
		virtual Rect	rectForRange( const PrintableItem * pText, int ofs, int length ) const = 0;
	
		virtual String	tooltip( const PrintableItem * pText ) const;
	
		// Methods for supporting a caret
	
		virtual Direction 	textDirection( PrintableItem * pItem, int charOfs ) const = 0;
		virtual int			caretToPos( PrintableItem * pItem, Coord pos, int& wantedLineOfs ) const = 0;
		virtual int			caretUp( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretDown( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretLeft( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretRight( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretHome( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;
		virtual int			caretEnd( PrintableItem * pItem, int charOfs, int& wantedLineOfs ) const = 0;

		virtual int			caretPrevWord( PrintableItem * pItem, int charOfs ) const = 0;
		virtual int			caretNextWord( PrintableItem * pItem, int charOfs ) const = 0;
		
	
	protected:
		Printer() {}
		virtual ~Printer() {};
	
	
		CharBuffer *  	_charBuffer( PrintableItem * pItem ) const;
		const CharBuffer *  _charBuffer( const PrintableItem * pItem ) const;
		const void *	_itemDataBlock( const PrintableItem * pItem) const;
		void *			_itemDataBlock( PrintableItem * pItem) const;
		int					_itemDataInt( const PrintableItem * pItem ) const;
		const TextStyle *	_baseStyle( const PrintableItem * pItem ) const;
		State			_state( const PrintableItem * pItem ) const;
		
		void			_setItemDataBlock( PrintableItem * pItem, void * pBlock );
		void			_setItemDataInt( PrintableItem * pItem, int data );
		
		void			_setItemDirty( PrintableItem * pItem );
		void			_setItemDirty( PrintableItem * pItem, const Rect& rect );
		void			_requestItemResize( PrintableItem * pItem );
		
	};
	
	
	
	

} // namespace wg
#endif //WG_PRINTER_DOT_H
