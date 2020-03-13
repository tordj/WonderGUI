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
#include <wg_ctextdisplay.h>
#include <wg_gfxanim.h>

namespace wg
{

	class RefreshButton;
	typedef	StrongPtr<RefreshButton>		RefreshButton_p;
	typedef	WeakPtr<RefreshButton>		RefreshButton_wp;

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

		CTextDisplay	refreshText;

		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const override;
		const char *		className( void ) const override;
		static const char	CLASSNAME[];

		//.____ Appearance __________________________________________

		void			setAnimation( GfxAnim * pAnimation );
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
		virtual Widget* _newOfMyType() const override { return new RefreshButton(); };

		void			_receive( Msg * pMsg ) override;
		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		virtual void 	_resize( const SizeI& size ) override;

		class TextAccess : public CTextDisplay { friend class RefreshButton; };

		const TextAccess& _text() const { return static_cast<const TextAccess&>(label); }
		TextAccess&	_text() { return static_cast<TextAccess&>(label); }

		const TextAccess& _refreshText() const { return static_cast<const TextAccess&>(refreshText); }
		TextAccess& _refreshText() { return static_cast<TextAccess&>(refreshText); }

		class IconAccess : public CIconDisplay { friend class RefreshButton; };
		const IconAccess& _icon() const { return static_cast<const IconAccess&>(icon); }
		IconAccess& _icon() { return static_cast<IconAccess&>(icon); }


		RouteId			m_tickRouteId;
		GfxAnim_p		m_pRefreshAnim;
		AnimTarget		m_animTarget;
		RefreshMode		m_refreshMode;			// Determines if animation is a progressbar or spinner.
		bool			m_bRestartable;

		bool			m_bRefreshing;
		bool			m_bAutoRefresh;
		uint32_t		m_animTimer;
		float			m_refreshProgress;
		bool			m_bStopping;
	};


} // namespace wg
#endif //WG_REFRESHBUTTON_DOT_H
