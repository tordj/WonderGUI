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

#ifndef	WG_VALUEDISPLAY_DOT_H
#define	WG_VALUEDISPLAY_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_VALUEFORMAT_DOT_H
#	include <wg_valueformat.h>
#endif

#ifndef WG_TEXTPROP_DOT_H
#	include <wg_textprop.h>
#endif

#ifndef WG_INTERFACE_VALUEHOLDER_DOT_H
#	include <wg_interface_valueholder.h>
#endif

class	WgFont;

class WgValueDisplay;
typedef	WgStrongPtr<WgValueDisplay,WgWidgetPtr>		WgValueDisplayPtr;
typedef	WgWeakPtr<WgValueDisplay,WgWidgetWeakPtr>	WgValueDisplayWeakPtr;

class WgValueDisplay : public WgWidget, protected WgLegacyTextHolder, public Wg_Interface_ValueHolder
{
public:
	static WgValueDisplayPtr	Create() { return WgValueDisplayPtr(new WgValueDisplay()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgValueDisplayPtr	Cast( const WgObjectPtr& pObject );

	//____ Interfaces _______________________________________

	WgText		text;

	//____ Methods __________________________________________

	void	SetFormat( const WgValueFormatPtr& pFormat );
	WgValueFormatPtr	Format() const { return m_pFormat; }

	WgSize	PreferredSize() const;

protected:
	WgValueDisplay();
	virtual ~WgValueDisplay();
	virtual WgWidget* _newOfMyType() const { return new WgValueDisplay(); };

	void	_onRefresh();
	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onStateChanged( WgState oldState );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );

	void	_regenText();

	WgWidget* _getWidget() { return this; }	// Needed for WgSilderTarget.

private:
	void	_valueModified();				///< Called when value has been modified.
	void	_rangeModified();				///< Called when range (and thus fractional value) has been modified.

	WgObject * _object() { return this; }
	void	_onFieldDirty( WgField * pField );
	void	_onFieldResize( WgField * pField );


	WgValueFormatPtr	m_pFormat;
	WgLegacyTextField			m_text;
};


#endif // WG_VALUEDISPLAY_DOT_H
