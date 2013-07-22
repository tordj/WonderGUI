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
#ifndef WG_SWITCH_DOT_H
#define WG_SWITCH_DOT_H


#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

class WgSwitch;
typedef	WgSmartChildPtr<WgSwitch,WgWidgetPtr>		WgSwitchPtr;
typedef	WgWeakChildPtr<WgSwitch,WgWidgetWeakPtr>	WgSwitchWeakPtr;

//____ WgSwitch ____________________________________________________________

class WgSwitch : public WgWidget
{
public:
	static WgSwitchPtr	Create() { return WgSwitchPtr(new WgSwitch()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgSwitchPtr	Cast( const WgObjectPtr& pObject );

    void    SetValue( int value );
    
	WgSize	PreferredSize() const;

protected:
	WgSwitch();
	virtual ~WgSwitch();
	virtual WgWidget* _newOfMyType() const { return new WgSwitch(); };

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	bool	_onAlphaTest( const WgCoord& ofs );

private:

};


#endif //WG_SWITCH_DOT_H
