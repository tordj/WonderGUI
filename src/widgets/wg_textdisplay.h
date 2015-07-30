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
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_TEXTDISPLAY_DOT_H
#define	WG_TEXTDISPLAY_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

class WgTextDisplay;
typedef	WgStrongPtr<WgTextDisplay,WgWidgetPtr>		WgTextDisplayPtr;
typedef	WgWeakPtr<WgTextDisplay,WgWidgetWeakPtr>	WgTextDisplayWeakPtr;

class WgTextDisplay:public WgWidget, protected WgTextHolder
{
public:
	static WgTextDisplayPtr	create() { return WgTextDisplayPtr(new WgTextDisplay()); }

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgTextDisplayPtr	cast( const WgObjectPtr& pObject );

	//____ Interfaces ______________________________________

	WgModText		text;

	//____ Methods __________________________________________

	WgPointerStyle		PointerStyle() const;
	WgString			TooltipString() const;

	int		matchingWidth( int height ) const;
	int		matchingHeight( int width ) const;
	WgSize	preferredSize() const;

protected:
	WgTextDisplay();
	virtual ~WgTextDisplay();
	virtual WgWidget* _newOfMyType() const { return new WgTextDisplay(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onNewSize( const WgSize& size );
	void	_onRefresh();
	void	_onMsg( const WgMsgPtr& pMsg );
	void	_onStateChanged( WgState oldState );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );

	WgObject * _object() { return this; }
	void	_onFieldDirty( WgField * pField );
	void	_onFieldResize( WgField * pField );
private:

	WgTextField			m_text;
};



#endif // WG_TEXTDISPLAY_DOT_H
