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

#ifndef	WG_CHECKBOX_DOT_H
#define	WG_CHECKBOX_DOT_H

#ifndef	WG_WIDGET_DOT_H
#	include <wg_widget.h>
#endif

#ifndef WG_INTERFACE_TEXTHOLDER_DOT_H
#	include <wg_interface_textholder.h>
#endif

#ifndef WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef WG_ICONHOLDER_DOT_H
#	include <wg_iconholder.h>
#endif

class	WgSurface;

class	WgCheckBox : public WgWidget, public Wg_Interface_TextHolder, public WgIconHolder
{
public:
	WgCheckBox();
	virtual ~WgCheckBox();

	virtual const char * Type() const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgCheckBox(); };

	enum ClickArea
	{
		DEFAULT,		// Full geometry of icon (no alpha test) + text + area between + alpha test on background.
		ALPHA,			// Alpha test on background and icon.
		GEO,			// Full geometry of Widget is clickable.
		ICON,			// Only the icon (alpha test) is clickable.
		TEXT			// Only the text is clickable.
	};

	void			SetClickArea( ClickArea clickArea );
	ClickArea		GetClickArea() const { return m_clickArea; }

	void			SetIcon( const WgSkinPtr& pIconSkin, const WgOrigo& origo, 
							 WgBorders borders = WgBorders(0), float scale = 0.f, bool bPushText = true );
	void			SetIcon( const WgSkinPtr& pIconSkin );
	WgSkinPtr		Icon() const { return m_pIconSkin; }

	inline bool		IsSelected() { return m_state.IsSelected(); };
	virtual bool	SetSelected( bool bSelected );

	void			SetFlipOnRelease( bool bFlipOnRelease );
	inline bool		FlipOnRelease() { return m_bFlipOnRelease; }

	WgSize			PreferredSize() const;
	bool			IsAutoEllipsisDefault() const { return false; };
	

protected:

	void	_onCloneContent( const WgWidget * _pOrg );
	void	_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void	_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
	void	_onRefresh();
	void	_onNewSize( const WgSize& size );
	bool	_onAlphaTest( const WgCoord& ofs );
	void	_onStateChanged( WgState oldState, WgState newState );
	void	_onSkinChanged( const WgSkinPtr& pOldSkin, const WgSkinPtr& pNewSkin );
	
	virtual void	_iconModified();
	virtual void	_textModified();

private:

	void	_refreshTextArea();
	bool	_markTestTextArea( int _x, int _y );
	
	bool			m_bPressed;						// Set when mouse is pressed and over.
	bool			m_bReturnPressed;
	bool			m_bFlipOnRelease;				// Set if we want to flip checkbox on press (default), not click.

	WgSkinPtr		m_pIconSkin;
	WgText			m_text;

	ClickArea		m_clickArea;
};


#endif //	WG_CHECKBOX_DOT_H
