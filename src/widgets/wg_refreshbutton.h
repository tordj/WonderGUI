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

#ifndef WG_GFXANIM_DOT_H
#	include <wg_gfxanim.h>
#endif

namespace wg 
{
	
	class RefreshButton;
	typedef	StrongPtr<RefreshButton,Button_p>		RefreshButton_p;
	typedef	WeakPtr<RefreshButton,Button_wp>		RefreshButton_wp;
	
	//____ WidgetRefreshButton ____________________________________________________________
	
	class RefreshButton : public Button
	{
	public:
		static RefreshButton_p	create() { return RefreshButton_p(new RefreshButton()); }
		
		bool		isInstanceOf( const char * pClassName ) const;
		const char *className( void ) const;
		static const char	CLASSNAME[];
		static RefreshButton_p	cast( const Object_p& pObject );
	
		//____ Interfaces ______________________________________
	
		ModText	refreshText;
	
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
	
		void			setRefreshAnimation( const GfxAnim_p& pAnimation );
		GfxAnim_p	getRefreshAnimation() const { return m_pRefreshAnim; }
	
		void			setRefreshMode( RefreshMode mode );
		RefreshMode		getRefreshMode() const { return m_refreshMode; }
	
		void			setAnimTarget( AnimTarget target );
		AnimTarget		getAnimTarget() const { return m_animTarget; }
	
		void			setRestartable( bool bRestartable );
		bool			isRestartable() const { return m_bRestartable; }
	
		void			setAutoStartRefresh( bool bAuto ) { m_bAutoRefresh = bAuto; }
		bool			isAutoStartRefresh( ) const { return m_bAutoRefresh; }
	
		void			startRefresh();
		void			stopRefresh();
		void			stopRefreshNow();
		void			setRefreshProgress( float fraction );
		float			getRefreshProgress() const { return m_refreshProgress; }
		bool			isRefreshing() const { return m_bRefreshing; }
	
	protected:
		RefreshButton();
		virtual ~RefreshButton();
		virtual Widget* _newOfMyType() const { return new RefreshButton(); };
	
		void			_onMsg( const Msg_p& pMsg );
		void			_onRender( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_onCloneContent( const Widget * _pOrg );
		virtual void 	_onNewSize( const Size& size );
	
		State			_getRenderState();
	
	
		RouteId		m_tickRouteId;
		GfxAnim_p	m_pRefreshAnim;
		AnimTarget		m_animTarget;
		RefreshMode		m_refreshMode;			// Determines if animation is a progressbar or spinner.
		TextField		m_refreshText;
		bool			m_bRestartable;
	
		bool			m_bRefreshing;
		bool			m_bAutoRefresh;
		uint32_t			m_animTimer;
		float			m_refreshProgress;
		bool			m_bStopping;
	};
	

} // namespace wg
#endif //WG_REFRESHBUTTON_DOT_H
