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
#ifndef WG_BUTTON_DOT_H
#define WG_BUTTON_DOT_H

#ifndef WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef	WG_LEGACYTEXTFIELD_DOT_H
#	include <wg_legacytextfield.h>
#endif

#ifndef WG_LEGACYMODTEXT_DOT_H
#	include <wg_legacymodtext.h>
#endif

#ifndef	WG_ICON_DOT_H
#	include <wg_icon.h>
#endif

class WgButton;
typedef	WgStrongPtr<WgButton,WgWidgetPtr>		WgButtonPtr;
typedef	WgWeakPtr<WgButton,WgWidgetWeakPtr>	WgButtonWeakPtr;

//____ WgButton ____________________________________________________________
/**
 * @breif A normal push button widget.
 *
 * A simple push button widget with label and icon.
 **/

class WgButton : public WgWidget, protected WgIconHolder, protected WgLegacyTextHolder
{
public:
	static WgButtonPtr	Create() { return WgButtonPtr(new WgButton()); }

	bool				IsInstanceOf( const char * pClassName ) const;
	const char *		ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgButtonPtr	Cast( const WgObjectPtr& pObject );

	//____ Interfaces ______________________________________

	WgLegacyModText		label;
	WgIcon			icon;

	//____ Methods __________________________________________

	void			SetDownWhenMouseOutside( bool bDown );					///< @brief Set if button should stay in pressed state even if mouse goes outside.
	bool			DownWhenMouseOutside() const { return m_bDownOutside; }	///< @brief Check if button will stay in pressed state even if mouse goes outside.
	
	virtual int		MatchingHeight( int width ) const;
//	virtual int		MatchingWidth( int height ) const;

	WgSize			PreferredSize() const;

	bool			IsAutoEllipsisDefault() const { return false; };


protected:
	WgButton();
	virtual ~WgButton();
	virtual WgWidget* _newOfMyType() const { return new WgButton(); };

	virtual void	_onEvent( const WgEventPtr& pEvent, WgEventHandler * pHandler );
	virtual void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onRefresh();
	virtual void	_onCloneContent( const WgWidget * _pOrg );
	bool			_onAlphaTest( const WgCoord& ofs, const WgSize& sz );
	virtual void	_onNewSize( const WgSize& size );
	void			_onStateChanged( WgState oldState );
	virtual void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );


	WgObject * 		_object() { return this; };
	void			_onFieldDirty( WgField * pField );
	void 			_onFieldResize( WgField * pField );

	WgLegacyTextField		m_text;
	WgIconField		m_icon;

	bool			m_bDownOutside;			// Button remains down when pressed and mouse gets outside?

	bool			m_bPressed;				// Set when left mousebutton was pressed inside.
	bool			m_bReturnPressed;
};

#endif //WG_BUTTON_DOT_H
