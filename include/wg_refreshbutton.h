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

#ifndef WG_BLOCKSET_DOT_H
#	include <wg_blockset.h>
#endif

#ifndef	WG_TEXT_DOT_H
#	include <wg_text.h>
#endif

#ifndef	WG_INTERFACE_TEXTHOLDER_DOT_H
#	include <wg_interface_textholder.h>
#endif


//____ WgWidgetRefreshButton ____________________________________________________________

class WgRefreshButton : public WgButton
{
public:
	WgRefreshButton();
	~WgRefreshButton();
	

	virtual const char *Type( void ) const;
	static const char * GetClass();
	virtual WgWidget * NewOfMyType() const { return new WgRefreshButton(); };

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

		void			SetRefreshAnimation( WgGfxAnim * pAnimation );
		WgGfxAnim *		GetRefreshAnimation() const { return m_pRefreshAnim; }

		void			SetRefreshMode( RefreshMode mode );
		RefreshMode		GetRefreshMode() const { return m_refreshMode; }

		void			SetAnimTarget( AnimTarget target );
		AnimTarget		GetAnimTarget() const { return m_animTarget; }

		void			SetRefreshText( const WgCharSeq& text );
		const WgChar*	GetRefreshText() const { return m_refreshText.getText(); }

		void			SetRefreshTextProperties( const WgTextpropPtr& pProp );
		WgTextpropPtr	GetRefreshTextProperties() const { return m_refreshText.getProperties(); }

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

		virtual void	SetTextManager( WgTextManager * pManager );

protected:

		void		_onEvent( const WgEvent::Event * pEvent, WgEventHandler * pHandler );
		void		_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip );
		void		_onCloneContent( const WgWidget * _pOrg );
		virtual void _onNewSize( const WgSize& size );

		WgMode		_getRenderMode();



		WgGfxAnim *		m_pRefreshAnim;
		AnimTarget		m_animTarget;
		RefreshMode		m_refreshMode;			// Determines if animation is a progressbar or spinner.
		WgText			m_refreshText;
		bool			m_bRestartable;

		bool			m_bRefreshing;
		bool			m_bAutoRefresh;
		Uint32			m_animTimer;
		float			m_refreshProgress;
		bool			m_bStopping;


};

#endif //WG_REFRESHBUTTON_DOT_H
