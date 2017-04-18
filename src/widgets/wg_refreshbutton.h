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
#pragma once

#include <wg_button.h>
#include <wg_modtext.h>
#include <wg_gfxanim.h>

namespace wg 
{
	
	class RefreshButton;
	typedef	StrongPtr<RefreshButton,Button_p>		RefreshButton_p;
	typedef	WeakPtr<RefreshButton,Button_wp>		RefreshButton_wp;
	
	//____ WidgetRefreshButton ____________________________________________________________
	
	class RefreshButton : public Button
	{
	public:

		//____ Enums __________________________________________
	
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

		//.____ Creation __________________________________________

		static RefreshButton_p	create() { return RefreshButton_p(new RefreshButton()); }

		//.____ Components _______________________________________
	
		ModText	refreshText;

		//.____ Identification __________________________________________
		
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static RefreshButton_p	cast( const Object_p& pObject );
	
		//.____ Appearance __________________________________________
	
		void			setAnimation( const GfxAnim_p& pAnimation );
		GfxAnim_p		animation() const { return m_pRefreshAnim; }

		void			setRefreshMode( RefreshMode mode );
		RefreshMode		refreshMode() const { return m_refreshMode; }
	
		void			setAnimTarget( AnimTarget target );
		AnimTarget		animTarget() const { return m_animTarget; }
	
		//.____ Behavior ____________________________________________
		
		void			setRestartable( bool bRestartable );
		bool			isRestartable() const { return m_bRestartable; }
	
		void			setAutoStartRefresh( bool bAuto ) { m_bAutoRefresh = bAuto; }
		bool			isAutoStartRefresh( ) const { return m_bAutoRefresh; }
	
		//.____ Control ________________________________________________
		
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
	
		void			_receive( const Msg_p& pMsg );
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Rect& _clip );
		void			_cloneContent( const Widget * _pOrg );
		virtual void 	_setSize( const Size& size );
	
		State			_getRenderState();
	
	
		RouteId			m_tickRouteId;
		GfxAnim_p		m_pRefreshAnim;
		AnimTarget		m_animTarget;
		RefreshMode		m_refreshMode;			// Determines if animation is a progressbar or spinner.
		TextItem		m_refreshText;
		bool			m_bRestartable;
	
		bool			m_bRefreshing;
		bool			m_bAutoRefresh;
		uint32_t		m_animTimer;
		float			m_refreshProgress;
		bool			m_bStopping;
	};
	

} // namespace wg
#endif //WG_REFRESHBUTTON_DOT_H
