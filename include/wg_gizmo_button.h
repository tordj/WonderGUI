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
#ifndef WG_GIZMO_BUTTON_DOT_H
#define WG_GIZMO_BUTTON_DOT_H

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


//____ WgGizmoButton ____________________________________________________________

class WgGizmoButton : public WgGizmo, public Wg_Interface_TextHolder
{
public:
	WgGizmoButton();
	virtual ~WgGizmoButton();

	virtual const char *Type( void ) const;
	static const char * GetMyType();

	//____ Methods __________________________________________

	void			SetPressAnim( bool button1, bool button2, bool button3, bool bDownWhenMouseOutside = false );
	void			GetPressAnim( bool& button1, bool& button2, bool& button3, bool& bDownWhenMouseOutside );

	void			SetIcon( const WgBlockSetPtr& pIconGfx, const WgOrigo& origo, Sint8 xOfs, Sint8 yOfs );
	WgBlockSetPtr	GetIconSource() const { return m_pIconGfx; }
	WgOrigo			GetIconOrigo() const { return m_iconOrigo; }
	Sint8			GetIconOfsX() const { return m_iconOfs.x; }
	Sint8			GetIconOfsY() const { return m_iconOfs.y; }

	bool			SetSource( const WgBlockSetPtr& pGfx );
	WgBlockSetPtr	GetSource() const { return m_pBgGfx; }

	void			SetIconAlignment( WgOrigo alignment );
	void			SetIconOffset( Sint8 xOfs, Sint8 yOfs );

    bool			SetDisplacement( Sint8 xUp = 0, Sint8 yUp = 0, Sint8 xOver = 0, Sint8 yOver = 0, Sint8 xDown = 0, Sint8 yDown = 0 );
	void			GetDisplacement( Sint8& xUp, Sint8& yUp, Sint8& xOver, Sint8& yOver, Sint8& xDown, Sint8& yDown ) const;

	Uint32			GetTextAreaWidth();

protected:
/*
	virtual int		HeightForWidth( int width ) const;
	virtual int		WidthForHeight( int height ) const;

	virtual WgSize	MinSize() const;
	virtual WgSize	MaxSize() const;
*/
	WgSize			BestSize() const;


	virtual void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
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


	void			TextModified();

	virtual WgMode	GetRenderMode();


	WgText			m_text;

	WgBlockSetPtr	m_pIconGfx;
	WgBlockSetPtr	m_pBgGfx;

	WgMode			m_mode;

	WgOrigo			m_iconOrigo;
	WgCoord8			m_iconOfs;

	bool			m_bRenderDown[WG_MAX_BUTTONS];	// Render down-version if [button] pressed?
	bool			m_bDownOutside;			// Button remains down when pressed and mouse gets outside?
	WgCoord8			m_aDisplace[4];			// Text displacement for up, mouse_over, down and disabled.

	bool			m_bPressedInside[WG_MAX_BUTTONS];
	bool			m_bReturnPressed;
	bool			m_bPointerInside;
};

#endif //WG_GIZMO_BUTTON_DOT_H
