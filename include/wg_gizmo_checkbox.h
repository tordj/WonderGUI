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

#ifndef	WG_GIZMO_CHECKBOX_DOT_H
#define	WG_GIZMO_CHECKBOX_DOT_H

#ifndef	WG_GIZMO_DOT_H
#	include <wg_gizmo.h>
#endif

#ifndef WG_INTERFACE_TEXTHOLDER_DOT_H
#	include <wg_interface_textholder.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef	WG_TOOLTIP_DOT_H
#	include <wg_tooltip.h>
#endif

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef WG_ICONHOLDER_DOT_H
#	include <wg_iconholder.h>
#endif

class	WgSurface;

class	WgGizmoCheckbox : public WgGizmo, public Wg_Interface_TextHolder, public WgIconHolder
{
public:
	WgGizmoCheckbox();
	virtual ~WgGizmoCheckbox();

	virtual const char * Type() const;
	static const char * GetMyType();
	virtual WgGizmo * NewOfMyType() const { return new WgGizmoCheckbox(); };

	enum ClickArea
	{
		DEFAULT,		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		ALPHA,			// Alpha test on background and icon.
		GEO,			// Full geometry of Gizmo is clickable.
		ICON,			// Only the icon (alpha test) is clickable.
		TEXT			// Only the text is clickable.
	};

	void			SetClickArea( ClickArea clickArea );
	ClickArea		GetClickArea() const { return m_clickArea; }

	bool			SetSource( const WgBlockSetPtr& pUnchecked, const WgBlockSetPtr& pChecked );

	WgBlockSetPtr	GetCheckedSource() const { return m_pBlockChecked; }
	WgBlockSetPtr	GetUncheckedSource() const { return m_pBlockUnchecked; }

	void			SetIcons( const WgBlockSetPtr& pUnchecked, const WgBlockSetPtr& pChecked, 
							  const WgOrientation& orientation, WgBorders borders = WgBorders(0), float scale = 0.f, bool bPushText = true );
	void			SetIcons( const WgBlockSetPtr& pUnchecked, const WgBlockSetPtr& pChecked );
	void			SetCheckedIcon( const WgBlockSetPtr& pChecked );
	void			SetUncheckedIcon( const WgBlockSetPtr& pUnchecked );


	WgBlockSetPtr	GetCheckedIcon() const { return m_pIconChecked; }
	WgBlockSetPtr	GetUncheckedIcon() const { return m_pIconUnchecked; }

	inline bool		IsChecked() { return m_bChecked; };

	inline bool		Check( void ) { return SetState( true ); };
	inline bool		Uncheck( void ) { return SetState( false ); };
	virtual bool	SetState( bool state );
	inline bool		ChangeState( void ) { return SetState( !m_bChecked ); };

	void			SetFlipOnRelease( bool bFlipOnRelease );
	inline bool		FlipOnRelease() { return m_bFlipOnRelease; }

	Uint32			GetTextAreaWidth();

	DECLARE_TOOLTIP_SUPPORT();

	WgSize			DefaultSize() const;
	bool			IsAutoEllipsisDefault() const { return false; };
	

protected:

	void	_onCloneContent( const WgGizmo * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer );
#ifdef WG_TNG
	void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
#endif
	void	_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj );
	void	_onRefresh();
	void	_onNewSize( const WgSize& size );
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onEnable();
	void	_onDisable();
	
	bool			m_bChecked;

	virtual void	_iconModified();
	virtual void	_textModified();

private:


	void	_refreshTextArea();
	bool	_markTestTextArea( int _x, int _y );
	


	bool			m_bOver;						// Set when mouse is over.							
	bool			m_bPressed;						// Set when mouse is pressed and over.
	bool			m_bFlipOnRelease;				// Set if we want to flip checkbox on press (default), not click.

	WgBlockSetPtr	m_pIconUnchecked;
	WgBlockSetPtr	m_pIconChecked;

	WgBlockSetPtr	m_pBlockUnchecked;
	WgBlockSetPtr	m_pBlockChecked;
	WgText			m_text;

	ClickArea		m_clickArea;
};




#endif //	WG_GIZMO_CHECKBOX_DOT_H
