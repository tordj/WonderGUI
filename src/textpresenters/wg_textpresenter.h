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
typedef	WgStrongPtr<WgTextPresenter,WgObjectPtr>	WgTextPresenterPtr;
typedef	WgWeakPtr<WgTextPresenter,WgObjectWeakPtr>	WgTextPresenterWeakPtr;

class WgTextPresenter : public WgObject
{
public:
	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgTextPresenterPtr	Cast( const WgObjectPtr& pObject );

	virtual void	AddField( WgPresentableField * pField ) = 0;
	virtual void	RemoveField( WgPresentableField * pField ) = 0;

	virtual int		CoordToChar( const WgPresentableField * pField, WgCoord pos ) = 0;

	virtual void 	RenderField( WgPresentableField * pText, WgGfxDevice * pDevice, const WgRect& canvas, const WgRect& clip ) = 0;

	virtual void	OnTextModified( WgPresentableField * pText, int ofs, int charsRemoved, int charsAdded ) = 0;
	virtual void	OnFieldResize( WgPresentableField * pText, WgSize newSize ) = 0;
	virtual void	OnStateChange( WgPresentableField * pText, WgState newState, WgState oldState ) = 0;
	virtual void	OnStyleChange( WgPresentableField * pText ) = 0;
	virtual void	OnRefresh( WgPresentableField * pText ) = 0;


	virtual WgSize	PreferredSize( const WgPresentableField * pText ) const = 0;
	virtual int		MatchingWidth( const WgPresentableField * pText, int height ) const = 0;
	virtual int		MatchingHeight( const WgPresentableField * pText, int width ) const = 0;

	virtual WgRect	RectForRange( const WgPresentableField * pText, int ofs, int length ) const = 0;

	virtual WgString Tooltip( const WgPresentableField * pText ) const;

	virtual int		CoordToCaretPos( WgPresentableField * pField, WgCoord pos ) = 0;
	virtual int		MoveCaret( WgPresentableField * pText, int caretOfs, int wantedPixelOfs, int verticalSteps, int horizontalSteps, WgModifierKeys modif ) = 0;
	virtual WgRect	CharToRect( const WgPresentableField * pField, int charOfs ) = 0;


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
