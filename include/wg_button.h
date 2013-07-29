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

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef	WG_INTERFACE_TEXTHOLDER_DOT_H
#	include <wg_interface_textholder.h>
#endif

#ifndef	WG_ICONHOLDER_DOT_H
#	include <wg_iconholder.h>
#endif


class WgButton;
typedef	WgSmartPtr<WgButton,WgWidgetPtr>		WgButtonPtr;
typedef	WgWeakPtr<WgButton,WgWidgetWeakPtr>	WgButtonWeakPtr;

//____ WgButton ____________________________________________________________

class WgButton : public WgWidget, public Wg_Interface_TextHolder, public WgIconHolder
{
public:
	static WgButtonPtr	Create() { return WgButtonPtr(new WgButton()); }

	bool		IsInstanceOf( const char * pClassName ) const;
	const char *ClassName( void ) const;
	static const char	CLASSNAME[];
	static WgButtonPtr	Cast( const WgObjectPtr& pObject );

	//____ Methods __________________________________________

	void			SetDownWhenMouseOutside( bool bDown );
	bool			DownWhenMouseOutside() const { return m_bDownOutside; }

	bool			SetIcon( const WgSkinPtr& pIconGfx, WgOrigo origo, WgBorders padding = WgBorders(0), float _scale = 0.f, bool _bPushText = true );
	void			SetIcon( const WgSkinPtr& pIconGfx );
	WgSkinPtr		IconSkin() const { return m_pIconSkin; }

	int				TextAreaWidth();

	virtual int		HeightForWidth( int width ) const;
//	virtual int		WidthForHeight( int height ) const;

	WgSize			PreferredSize() const;

	bool			IsAutoEllipsisDefault() const { return false; };


protected:
	WgButton();
	virtual ~WgButton();
	virtual WgWidget* _newOfMyType() const { return new WgButton(); };

	virtual void	_onEvent( WgEvent::Event * pEvent, WgEventHandler * pHandler );
	virtual void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onRefresh();
	virtual void	_onCloneContent( const WgWidget * _pOrg );
	bool			_onAlphaTest( const WgCoord& ofs );
	virtual void	_onNewSize( const WgSize& size );
	void			_onStateChanged( WgState oldState, WgState newState );
	virtual void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );


	void			_textModified();
	void			_iconModified();

	WgText			m_text;

	WgSkinPtr		m_pIconSkin;

	bool			m_bDownOutside;			// Button remains down when pressed and mouse gets outside?

	bool			m_bPressed;				// Set when left mousebutton was pressed inside.
	bool			m_bReturnPressed;
};

#endif //WG_BUTTON_DOT_H
