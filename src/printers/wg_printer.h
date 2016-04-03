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
	
	class PrintableField;
	
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
	
		virtual void	addField( PrintableField * pField ) = 0;
		virtual void	removeField( PrintableField * pField ) = 0;
	
		virtual int		coordToChar( const PrintableField * pField, Coord pos ) = 0;
		virtual Coord	charToCoord( const PrintableField * pField, int charOfs ) = 0;		// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charToRect( const PrintableField * pField, int charOfs ) = 0;
	
		virtual void 	renderField( PrintableField * pText, GfxDevice * pDevice, const Rect& canvas, const Rect& clip ) = 0;
	
		virtual void	onTextModified( PrintableField * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
		virtual void	onFieldResized( PrintableField * pText, Size newSize, Size oldSize ) = 0;
		virtual void	onStateChanged( PrintableField * pText, State newState, State oldState ) = 0;
		virtual void	onStyleChanged( PrintableField * pText, TextStyle * pNewStyle, TextStyle * pOldStyle ) = 0;
		virtual void	onCharStyleChanged( PrintableField * pText, int ofs = 0, int len = INT_MAX ) = 0;
		virtual void	onRefresh( PrintableField * pText ) = 0;
	
	
		virtual Size	preferredSize( const PrintableField * pText ) const = 0;
		virtual int		matchingWidth( const PrintableField * pText, int height ) const = 0;
		virtual int		matchingHeight( const PrintableField * pText, int width ) const = 0;
	
		virtual Rect	rectForRange( const PrintableField * pText, int ofs, int length ) const = 0;
	
		virtual String	tooltip( const PrintableField * pText ) const;
	
		virtual int		coordToCaretPos( PrintableField * pField, Coord pos ) = 0;
		virtual int		moveCaret( PrintableField * pText, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, ModifierKeys modif ) = 0;
	
	
	protected:
		Printer() {}
		virtual ~Printer() {};
	
	
		CharBuffer *  	_charBuffer( PrintableField * pField ) const;
		const CharBuffer *  _charBuffer( const PrintableField * pField ) const;
		const void *	_fieldDataBlock( const PrintableField * pField) const;
		void *			_fieldDataBlock( PrintableField * pField) const;
		int				_fieldDataInt( PrintableField * pField ) const;
		TextStyle *		_baseStyle( PrintableField * pField ) const;
		State			_state( PrintableField * pField ) const;
		
		void			_setFieldDataBlock( PrintableField * pField, void * pBlock );
		void			_setFieldDataInt( PrintableField * pField, int data );
		
		void			_setFieldDirty( PrintableField * pField );
		void			_setFieldDirty( PrintableField * pField, const Rect& rect );
		void			_requestFieldResize( PrintableField * pField );
		
	};
	
	
	
	

} // namespace wg
#endif //WG_PRINTER_DOT_H
