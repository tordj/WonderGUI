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

#include <wg_vectorpanel.h>


WgCoord WgVectorHook::Pos() const
{
	return Parent()->_hookGeo(this).Pos();
}

WgSize WgVectorHook::Size() const
{
	return Parent()->_hookGeo(this).Size();
}

WgRect WgVectorHook::Geo() const
{
	return Parent()->_hookGeo(this);
}

WgCoord WgVectorHook::ScreenPos() const
{
	return Parent()->_hookGeo(this).Pos() + Parent()->ScreenPos();

}

WgRect WgVectorHook::ScreenGeo() const
{
	return Parent()->_hookGeo(this) + Parent()->ScreenPos();
}

bool WgVectorHook::MoveForward()
{
	if( _moveUp() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgVectorHook::MoveBackward()
{
	if( _moveDown() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgVectorHook::MoveBefore( WgVectorHook * pSibling )
{
	if( pSibling && pSibling->Parent() == Parent() )
	{
		if( _moveBefore(pSibling) )
		{
			Parent()->_onWidgetsReordered();
			return true;
		}
	}
	return false;
}

bool WgVectorHook::MoveAfter( WgVectorHook * pSibling )
{
	if( pSibling && pSibling->Parent() == Parent() )
	{
		if( _moveAfter(pSibling) )
		{
			Parent()->_onWidgetsReordered();
			return true;
		}
	}
	return false;
}

bool WgVectorHook::MoveFirst()
{
	if( _moveFirst() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgVectorHook::MoveLast()
{
	if( _moveLast() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgVectorHook::SetVisible( bool bVisible )
{
	if( bVisible != m_bVisible )
	{
		m_bVisible = bVisible;
		if( bVisible )
			Parent()->_onWidgetAppeared(this);
		else
			Parent()->_onWidgetDisappeared(this);
	}

	return true;
}

WgVectorPanel * WgVectorHook::Parent() const
{
	return static_cast<WgVectorPanel*>(_parent());
}


WgVectorHook::WgVectorHook()
{
}

WgVectorHook::~WgVectorHook()
{
}


void WgVectorHook::_requestRender()
{
	WgVectorPanel * p = Parent();

	p->_onRenderRequested(this);
}

void WgVectorHook::_requestRender( const WgRect& rect )
{
	Parent()->_onRenderRequested(this, rect);
}

void WgVectorHook::_requestResize()
{
	Parent()->_onResizeRequested(this);
}


WgHook * WgVectorHook::_prevHook() const
{
	return _prev();
}

WgHook * WgVectorHook::_nextHook() const
{
	return _next();
}

//____ Constructor() __________________________________________________________

WgVectorPanel::WgVectorPanel() : m_sortOrder(WG_SORT_ASCENDING), m_pSortFunc(0), m_nChildPanels(0)
{

}


//____ Destructor() ___________________________________________________________

WgVectorPanel::~WgVectorPanel()
{
}


//____ AddChild() _____________________________________________________________

WgVectorHook * WgVectorPanel::AddChild( WgWidget * pWidget )
{
	if( !pWidget )
		return 0;

	WgVectorHook * pHook = _newHook();
	m_hooks.PushBack(pHook);
	pHook->_attachWidget( pWidget );

	_onWidgetAppeared(pHook);
	return pHook;
}

//____ InsertChild() __________________________________________________________

WgVectorHook * WgVectorPanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling )
{
	if( !pWidget || !pSibling || !pSibling->Parent() || pSibling->Parent() != this )
		return 0;

	WgVectorHook * pHook = _newHook();
	pHook->_moveBefore(static_cast<WgVectorHook*>(pSibling->Hook()));

	pHook->_attachWidget( pWidget );
	
	_onWidgetAppeared(pHook);
	return pHook;
}

//____ InsertChildSorted() ____________________________________________________

WgVectorHook * WgVectorPanel::InsertChildSorted( WgWidget * pWidget )
{
	//TODO: Implement

	return 0;
}

//____ DeleteChild() __________________________________________________________

bool WgVectorPanel::DeleteChild( WgWidget * pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return false;

	// Disconnect and notify subclass that widget has disappeared

	WgVectorHook * pHook = (WgVectorHook *) pWidget->Hook();
	pHook->_disconnect();

	if( pHook->IsVisible() )
		_onWidgetDisappeared( pHook );

	// Delete the widget and return

	delete pHook;
	return true;
}

//____ ReleaseChild() _________________________________________________________

WgWidget * WgVectorPanel::ReleaseChild( WgWidget * pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return 0;

	// Disconnect and notify subclass that widget has disappeared

	WgVectorHook * pHook = (WgVectorHook *) pWidget->Hook();
	pHook->_disconnect();

	if( pHook->IsVisible() )
		_onWidgetDisappeared( pHook );

	return pWidget;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgVectorPanel::DeleteAllChildren()
{
	m_hooks.Clear();
	_refreshAllWidgets();

	return true;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgVectorPanel::ReleaseAllChildren()
{
	while( m_hooks.First() )
		m_hooks.PopFront();

	_refreshAllWidgets();

	return true;
}

//____ SortChildren() ___________________________________________________________

void WgVectorPanel::SortChildren()
{
	//TODO: Implement
}

//____ SetSortOrder() _________________________________________________________

void WgVectorPanel::SetSortOrder( WgSortOrder order )
{
	m_sortOrder = order;
}

//____ SetSortFunction() ______________________________________________________

void WgVectorPanel::SetSortFunction( WgWidgetSortFunc pSortFunc )
{
	m_pSortFunc = pSortFunc;
}

//____ _onCloneContent() ______________________________________________________

void WgVectorPanel::_onCloneContent( const WgWidget * _pOrg )
{
	//TODO: Implement
}
