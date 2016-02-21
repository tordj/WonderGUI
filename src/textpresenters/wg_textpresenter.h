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

#ifndef	WG_TEXTPRESENTER_DOT_H
#define	WG_TEXTPRESENTER_DOT_H

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
	
	class PresentableField;
	
	class GfxDevice;
	class CharBuffer;
	class TextStyle;
	
	class TextPresenter;
	typedef	StrongPtr<TextPresenter,Object_p>	TextPresenter_p;
	typedef	WeakPtr<TextPresenter,Object_wp>	TextPresenter_wp;
	
	class TextPresenter : public Object
	{
	public:
		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static TextPresenter_p	cast( const Object_p& pObject );
	
		virtual void	addField( PresentableField * pField ) = 0;
		virtual void	removeField( PresentableField * pField ) = 0;
	
		virtual int		coordToChar( const PresentableField * pField, Coord pos ) = 0;
		virtual Coord	charToCoord( const PresentableField * pField, int charOfs ) = 0;		// Note: characters position on the baseline, not upper left corner of rectangle!
		virtual Rect	charToRect( const PresentableField * pField, int charOfs ) = 0;
	
		virtual void 	renderField( PresentableField * pText, GfxDevice * pDevice, const Rect& canvas, const Rect& clip ) = 0;
	
		virtual void	onTextModified( PresentableField * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
		virtual void	onFieldResize( PresentableField * pText, Size newSize ) = 0;
		virtual void	onStateChange( PresentableField * pText, State newState, State oldState ) = 0;
		virtual void	onStyleChange( PresentableField * pText ) = 0;
		virtual void	onRefresh( PresentableField * pText ) = 0;
	
	
		virtual Size	preferredSize( const PresentableField * pText ) const = 0;
		virtual int		matchingWidth( const PresentableField * pText, int height ) const = 0;
		virtual int		matchingHeight( const PresentableField * pText, int width ) const = 0;
	
		virtual Rect	rectForRange( const PresentableField * pText, int ofs, int length ) const = 0;
	
		virtual String	tooltip( const PresentableField * pText ) const;
	
		virtual int		coordToCaretPos( PresentableField * pField, Coord pos ) = 0;
		virtual int		moveCaret( PresentableField * pText, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, ModifierKeys modif ) = 0;
	
	
	protected:
		TextPresenter() {}
		virtual ~TextPresenter() {};
	
	
		CharBuffer *  _charBuffer( PresentableField * pField ) const;
		const CharBuffer *  _charBuffer( const PresentableField * pField ) const;
		const void *		_fieldDataBlock( const PresentableField * pField) const;
		void *			_fieldDataBlock( PresentableField * pField) const;
		int				_fieldDataInt( PresentableField * pField ) const;
		TextStyle *	_baseStyle( PresentableField * pField ) const;
		State			_state( PresentableField * pField ) const;
		
		void			_setFieldDataBlock( PresentableField * pField, void * pBlock );
		void			_setFieldDataInt( PresentableField * pField, int data );
		
	};
	
	
	
	

} // namespace wg
#endif //WG_TEXTPRESENTER_DOT_H
