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
		m_pWidget->m_refCount--;
		if( m_pWidget->m_refCount == 0 )
			delete m_pWidget;
	}
}

WgWidgetPtr WgHook::Widget() const
{ 
	return m_pWidget; 
}

WgWidgetHolder * WgHook::Holder() const 
{ 
	return _holder(); 
}

WgContainerPtr WgHook::Parent() const 
{ 
	return _parent(); 
}


//____ _setWidget() __________________________________________________________

void WgHook::_setWidget( WgWidget * pWidget )
{
	assert( pWidget == 0 || pWidget->Parent() == 0 );

	if( m_pWidget )
	{
		m_pWidget->m_pHook = 0;
		m_pWidget->m_refCount--;
		if( m_pWidget->m_refCount == 0 && m_pWidget != pWidget )
			delete m_pWidget;
	}

	m_pWidget = pWidget;

	if( pWidget )
	{
		pWidget->m_pHook = this;
		m_pWidget->m_refCount++;
	}
}

//____ _relinkWidget() __________________________________________________________

void WgHook::_relinkWidget()
{
	if( m_pWidget )
		m_pWidget->m_pHook = this;
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

//____ Root() _________________________________________________________________

WgRootPanelPtr WgHook::Root() const
{
	return _root();
}

//____ _root() _________________________________________________________________

WgRootPanel * WgHook::_root() const
{
	WgWidgetHolder * pHolder = _holder();

	if( pHolder->IsWidget() )
	{
		WgHook * pHook = pHolder->CastToWidget()->Hook();
		if( pHook )
			return pHook->_root();
	}
	else if( pHolder->IsRoot() )
		return pHolder->CastToRoot();

	return 0;
}


//____ EventHandler() __________________________________________________________

WgEventHandler * WgHook::EventHandler() const
{
	WgRootPanelPtr pRoot = Root();		//TODO: Optimize?
	if( pRoot )
		return pRoot->EventHandler();

	return 0;
}

