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

#ifndef	WG_FPSDISPLAY_DOT_H
#define WG_FPSDISPLAY_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include	<wg_text.h>
#endif

class WgFont;

class WgFpsDisplay;
typedef	WgSmartPtr<WgFpsDisplay,WgWidgetPtr>		WgFpsDisplayPtr;
typedef	WgWeakPtr<WgFpsDisplay,WgWidgetWeakPtr>	WgFpsDisplayWeakPtr;

class WgFpsDisplay:public WgWidget, public WgTextHolder
{
public:
	static WgFpsDisplayPtr	Create() { return WgFpsDisplayPtr(new WgFpsDisplay()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgFpsDisplayPtr	Cast( const WgObjectPtr& pObject );
		

	//____ Methods __________________________________________

	void	SetTextProperties( const WgTextpropPtr& pProp );
	WgSize	PreferredSize() const;

	inline WgIModifTextPtr	Labels() { return WgIModifTextPtr(this,&m_labelsText); } 
	inline WgIStaticTextPtr	Values() { return WgIStaticTextPtr(this,&m_valuesText); } 


protected:
	WgFpsDisplay();
	virtual ~WgFpsDisplay();
	virtual WgWidget* _newOfMyType() const { return new WgFpsDisplay(); };

	void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	void	_onStateChanged( WgState oldState, WgState newState );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );
	void	_textModified( WgText * pText );


private:

	WgText		m_labelsText;
	WgText		m_valuesText;
	int *		m_pTickBuffer;
	int			m_tickBufferOfs;

};



#endif //WgFpsDisplay_DOT_H
