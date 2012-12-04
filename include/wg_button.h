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

#ifndef WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
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

//____ WgButton ____________________________________________________________

class WgButton : public WgGizmo, public Wg_Interface_TextHolder, public WgIconHolder
{
public:
	WgButton();
	virtual ~WgButton();

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgGizmo * NewOfMyType() const { return new WgButton(); };

	//____ Methods __________________________________________

	void			SetDownWhenMouseOutside( bool bDown );
	bool			DownWhenMouseOutside() const { return m_bDownOutside; }

	bool			SetIcon( const WgBlocksetPtr& pIconGfx, WgOrientation orientation, WgBorders borders = WgBorders(0), float _scale = 0.f, bool _bPushText = true );
	void			SetIcon( const WgBlocksetPtr& pIconGfx );
	WgBlocksetPtr	GetIconSource() const { return m_pIconGfx; }

	bool			SetSource( const WgBlocksetPtr& pGfx );
	WgBlocksetPtr	GetSource() const { return m_pBgGfx; }

	Uint32			GetTextAreaWidth();

	virtual int		HeightForWidth( int width ) const;
//	virtual int		WidthForHeight( int height ) const;

	WgSize			DefaultSize() const;

	bool			IsAutoEllipsisDefault() const { return false; };


protected:

	virtual void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	virtual void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
	void			_onRefresh();
	virtual void	_onCloneContent( const WgGizmo * _pOrg );
	bool			_onAlphaTest( const WgCoord& ofs );
	void			_onEnable();
	void			_onDisable();
	virtual void	_onNewSize( const WgSize& size );
	void			_onGotInputFocus();
	void			_onLostInputFocus();


	void			_textModified();
	void			_iconModified();

	virtual WgMode	_getRenderMode();


	WgText			m_text;

	WgBlocksetPtr	m_pIconGfx;
	WgBlocksetPtr	m_pBgGfx;

	WgMode			m_mode;

	bool			m_bDownOutside;			// Button remains down when pressed and mouse gets outside?

	bool			m_bPressed;				// Set when left mousebutton was pressed inside.
	bool			m_bReturnPressed;
	bool			m_bPointerInside;
};

#endif //WG_BUTTON_DOT_H
