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
#ifndef WG_REFRESHBUTTON_DOT_H
#define WG_REFRESHBUTTON_DOT_H

#ifndef WG_BUTTON_DOT_H
#	include <wg_button.h>
#endif

#ifndef	WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

class WgRefreshButton;
typedef	WgStrongPtr<WgRefreshButton,WgButton_p>		WgRefreshButton_p;
typedef	WgWeakPtr<WgRefreshButton,WgButton_wp>		WgRefreshButton_wp;

//____ WgWidgetRefreshButton ____________________________________________________________

class WgRefreshButton : public WgButton
{
public:
	static WgRefreshButton_p	create() { return WgRefreshButton_p(new WgRefreshButton()); }
	
	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgRefreshButton_p	cast( const WgObject_p& pObject );

	//____ Interfaces ______________________________________

	WgModText	refreshText;

	//____ Methods __________________________________________

	enum RefreshMode
	{
		SPINNING,
		PROGRESS
	};

	enum AnimTarget
	{
		ICON,
		BUTTON_CENTERED,
		BUTTON_STRETCHED
	};

	void			SetRefreshAnimation( const WgGfxAnim_p& pAnimation );
	WgGfxAnim_p	GetRefreshAnimation() const { return m_pRefreshAnim; }

	void			SetRefreshMode( RefreshMode mode );
	RefreshMode		GetRefreshMode() const { return m_refreshMode; }

	void			SetAnimTarget( AnimTarget target );
	AnimTarget		GetAnimTarget() const { return m_animTarget; }

	void			SetRestartable( bool bRestartable );
	bool			IsRestartable() const { return m_bRestartable; }

	void			SetAutoStartRefresh( bool bAuto ) { m_bAutoRefresh = bAuto; }
	bool			IsAutoStartRefresh( ) const { return m_bAutoRefresh; }

	void			StartRefresh();
	void			StopRefresh();
	void			StopRefreshNow();
	void			SetRefreshProgress( float fraction );
	float			GetRefreshProgress() const { return m_refreshProgress; }
	bool			IsRefreshing() const { return m_bRefreshing; }

protected:
	WgRefreshButton();
	virtual ~WgRefreshButton();
	virtual WgWidget* _newOfMyType() const { return new WgRefreshButton(); };

	void			_onMsg( const WgMsg_p& pMsg );
	void			_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
	void			_onCloneContent( const WgWidget * _pOrg );
	virtual void 	_onNewSize( const WgSize& size );

	WgState			_getRenderState();


	WgRouteId		m_tickRouteId;
	WgGfxAnim_p	m_pRefreshAnim;
	AnimTarget		m_animTarget;
	RefreshMode		m_refreshMode;			// Determines if animation is a progressbar or spinner.
	WgTextField		m_refreshText;
	bool			m_bRestartable;

	bool			m_bRefreshing;
	bool			m_bAutoRefresh;
	Uint32			m_animTimer;
	float			m_refreshProgress;
	bool			m_bStopping;
};

#endif //WG_REFRESHBUTTON_DOT_H
