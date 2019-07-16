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

#include <wg_hook.h>
#include <wg_panel.h>
#include <wg_widget.h>
#include <wg_rootpanel.h>
#include <assert.h>


//____ Destructor _____________________________________________________________

WgHook::~WgHook()
{
	if( m_pWidget )
	{
		m_pWidget->m_pHook = 0;
		delete m_pWidget;
	}
}

//____ _attachWidget() __________________________________________________________

void WgHook::_attachWidget( WgWidget * pWidget )
{
    assert( pWidget != nullptr );
	assert( pWidget->Parent() == nullptr );

	if( m_pWidget )
		m_pWidget->_onNewHook(0);

	m_pWidget = pWidget;

	if( pWidget )
		pWidget->_onNewHook(this);
}

//____ _relinkWidget() __________________________________________________________

void WgHook::_relinkWidget()
{
	if( m_pWidget )
		m_pWidget->m_pHook = this;
}

//____ _releaseWidget() _________________________________________________________

WgWidget* WgHook::_releaseWidget()
{
	WgWidget * p = m_pWidget;
	m_pWidget = 0;

	if( p )
		p->m_pHook = 0;

	return p;
}

//____ _requestFocus() _________________________________________________________

bool WgHook::_requestFocus()
{
	return Parent()->_focusRequested(this, m_pWidget);
}

//____ _releaseFocus() _________________________________________________________

bool WgHook::_releaseFocus()
{
	return Parent()->_focusReleased(this, m_pWidget);
}

//____ PointPos() ______________________________________________________________

WgCoord WgHook::PointPos() const
{
    return (PixelPos() * WG_SCALE_BASE ) / Parent()->Scale();
}

WgSize WgHook::PointSize() const
{
	return (PixelSize() * WG_SCALE_BASE ) / Parent()->Scale();
}

WgRect WgHook::PointGeo() const
{
	WgRect geo = PixelGeo();
	int scale = Parent()->Scale();

	geo.x = (geo.x << WG_SCALE_BINALS) / scale;
    geo.y = (geo.y << WG_SCALE_BINALS) / scale;
    geo.w = (geo.w << WG_SCALE_BINALS) / scale;
    geo.h = (geo.h << WG_SCALE_BINALS) / scale;

	return geo;
}

WgCoord WgHook::ScreenPointPos() const
{
    return (ScreenPixelPos() * WG_SCALE_BASE ) / Parent()->Scale();

}

WgRect WgHook::ScreenPointGeo() const
{
	WgRect geo = ScreenPixelGeo();
	int scale = Parent()->Scale();

	geo.x = (geo.x << WG_SCALE_BINALS) / scale;
	geo.y = (geo.y << WG_SCALE_BINALS) / scale;
	geo.w = (geo.w << WG_SCALE_BINALS) / scale;
	geo.h = (geo.h << WG_SCALE_BINALS) / scale;

	return geo;
}


//____ Root() _________________________________________________________________

WgRootPanel * WgHook::Root() const
{
	WgWidgetHolder * pHolder = _holder();

	if( pHolder->IsWidget() )
	{
		WgHook * pHook = pHolder->CastToWidget()->Hook();
		if( pHook )
			return pHook->Root();
	}
	else if( pHolder->IsRoot() )
		return pHolder->CastToRoot();

	return 0;
}

//____ EventHandler() __________________________________________________________

WgEventHandler * WgHook::EventHandler() const
{
	WgRootPanel * pRoot = Root();
	if( pRoot )
		return pRoot->EventHandler();

	return 0;
}

