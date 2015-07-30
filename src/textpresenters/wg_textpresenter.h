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

class WgPresentableField;

class WgGfxDevice;
class WgCharBuffer;
class WgTextStyle;

class WgTextPresenter;
typedef	WgStrongPtr<WgTextPresenter,WgObject_p>	WgTextPresenter_p;
typedef	WgWeakPtr<WgTextPresenter,WgObject_wp>	WgTextPresenter_wp;

class WgTextPresenter : public WgObject
{
public:
	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgTextPresenter_p	cast( const WgObject_p& pObject );

	virtual void	addField( WgPresentableField * pField ) = 0;
	virtual void	removeField( WgPresentableField * pField ) = 0;

	virtual int		coordToChar( const WgPresentableField * pField, WgCoord pos ) = 0;

	virtual void 	renderField( WgPresentableField * pText, WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& clip ) = 0;

	virtual void	onTextModified( WgPresentableField * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
	virtual void	onFieldResize( WgPresentableField * pText, WgSize newSize ) = 0;
	virtual void	onStateChange( WgPresentableField * pText, WgState newState, WgState oldState ) = 0;
	virtual void	onStyleChange( WgPresentableField * pText ) = 0;
	virtual void	onRefresh( WgPresentableField * pText ) = 0;


	virtual WgSize	preferredSize( const WgPresentableField * pText ) const = 0;
	virtual int		matchingWidth( const WgPresentableField * pText, int height ) const = 0;
	virtual int		matchingHeight( const WgPresentableField * pText, int width ) const = 0;

	virtual WgRect	rectForRange( const WgPresentableField * pText, int ofs, int length ) const = 0;

	virtual WgString tooltip( const WgPresentableField * pText ) const;

	virtual int		coordToCaretPos( WgPresentableField * pField, WgCoord pos ) = 0;
	virtual int		moveCaret( WgPresentableField * pText, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif ) = 0;
	virtual WgRect	charToRect( const WgPresentableField * pField, int charOfs ) = 0;


protected:
	WgTextPresenter() {}
	virtual ~WgTextPresenter() {};


	WgCharBuffer *  _charBuffer( WgPresentableField * pField ) const;
	const WgCharBuffer *  _charBuffer( const WgPresentableField * pField ) const;
	const void *		_fieldDataBlock( const WgPresentableField * pField) const;
	void *			_fieldDataBlock( WgPresentableField * pField) const;
	int				_fieldDataInt( WgPresentableField * pField ) const;
	WgTextStyle *	_baseStyle( WgPresentableField * pField ) const;
	WgState			_state( WgPresentableField * pField ) const;
	
	void			_setFieldDataBlock( WgPresentableField * pField, void * pBlock );
	void			_setFieldDataInt( WgPresentableField * pField, int data );
	
};




#endif //WG_TEXTPRESENTER_DOT_H
