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

#include <wg_sortablepanel.h>


WgCoord WgSortableHook::Pos() const
{
	return Parent()->_hookGeo(this).Pos();
}

WgSize WgSortableHook::Size() const
{
	return Parent()->_hookGeo(this).Size();
}

WgRect WgSortableHook::Geo() const
{
	return Parent()->_hookGeo(this);
}

WgCoord WgSortableHook::ScreenPos() const
{
	return Parent()->_hookGeo(this).Pos() + Parent()->ScreenPos();

}

WgRect WgSortableHook::ScreenGeo() const
{
	return Parent()->_hookGeo(this) + Parent()->ScreenPos();
}

bool WgSortableHook::MoveUp()
{
	if( _moveUp() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgSortableHook::MoveDown()
{
	if( _moveDown() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgSortableHook::MoveBefore( WgSortableHook * pSibling )
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

bool WgSortableHook::MoveAfter( WgSortableHook * pSibling )
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

bool WgSortableHook::MoveFirst()
{
	if( _moveFirst() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgSortableHook::MoveLast()
{
	if( _moveLast() )
	{
		Parent()->_onWidgetsReordered();
		return true;
	}

	return false;
}

bool WgSortableHook::SetVisible( bool bVisible )
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

WgSortablePanel * WgSortableHook::Parent() const
{
	return static_cast<WgSortablePanel*>(_parent());
}


WgSortableHook::WgSortableHook()
{
}

WgSortableHook::~WgSortableHook()
{
}


void WgSortableHook::_requestRender()
{
	WgSortablePanel * p = Parent();

	p->_onRenderRequested(this);
}

void WgSortableHook::_requestRender( const WgRect& rect )
{
	Parent()->_onRenderRequested(this, rect);
}

void WgSortableHook::_requestResize()
{
	Parent()->_onResizeRequested(this);
}


WgHook * WgSortableHook::_prevHook() const
{
	return _prev();
}

WgHook * WgSortableHook::_nextHook() const
{
	return _next();
}

//____ Constructor() __________________________________________________________

WgSortablePanel::WgSortablePanel() : m_sortOrder(WG_SORT_ASCENDING), m_pSortFunc(0), m_nChildPanels(0)
{

}


//____ Destructor() ___________________________________________________________

WgSortablePanel::~WgSortablePanel()
{
}


//____ AddChild() _____________________________________________________________

WgSortableHook * WgSortablePanel::AddChild( WgWidget * pWidget )
{
	if( !pWidget )
		return 0;

	WgSortableHook * pHook = _newHook();
	m_hooks.PushBack(pHook);
	pHook->_attachWidget( pWidget );

	_onWidgetAppeared(pHook);
	return pHook;
}

//____ InsertChild() __________________________________________________________

WgSortableHook * WgSortablePanel::InsertChild( WgWidget * pWidget, WgWidget * pSibling )
{
	if( !pWidget || !pSibling || !pSibling->Parent() || pSibling->Parent() != this )
		return 0;

	WgSortableHook * pHook = _newHook();
	pHook->_moveBefore(static_cast<WgSortableHook*>(pSibling->Hook()));

	pHook->_attachWidget( pWidget );
	
	_onWidgetAppeared(pHook);
	return pHook;
}

//____ InsertChildSorted() ____________________________________________________

WgSortableHook * WgSortablePanel::InsertChildSorted( WgWidget * pWidget )
{
	//TODO: Implement

	return 0;
}

//____ DeleteChild() __________________________________________________________

bool WgSortablePanel::DeleteChild( WgWidget * pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return false;

	// Disconnect and notify subclass that widget has disappeared

	WgSortableHook * pHook = (WgSortableHook *) pWidget->Hook();
	pHook->_disconnect();

	if( pHook->IsVisible() )
		_onWidgetDisappeared( pHook );

	// Delete the widget and return

	delete pHook;
	return true;
}

//____ ReleaseChild() _________________________________________________________

WgWidget * WgSortablePanel::ReleaseChild( WgWidget * pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return 0;

	// Disconnect and notify subclass that widget has disappeared

	WgSortableHook * pHook = (WgSortableHook *) pWidget->Hook();
	pHook->_disconnect();

	if( pHook->IsVisible() )
		_onWidgetDisappeared( pHook );

	return pWidget;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgSortablePanel::DeleteAllChildren()
{
	m_hooks.Clear();
	_refreshAllWidgets();

	return true;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgSortablePanel::ReleaseAllChildren()
{
	while( m_hooks.First() )
		m_hooks.PopFront();

	_refreshAllWidgets();

	return true;
}

//____ SortChildren() ___________________________________________________________

void WgSortablePanel::SortChildren()
{
	//TODO: Implement
}

//____ SetSortOrder() _________________________________________________________

void WgSortablePanel::SetSortOrder( WgSortOrder order )
{
	m_sortOrder = order;
}

//____ SetSortFunction() ______________________________________________________

void WgSortablePanel::SetSortFunction( WgWidgetSortFunc pSortFunc )
{
	m_pSortFunc = pSortFunc;
}

//____ _onCloneContent() ______________________________________________________

void WgSortablePanel::_onCloneContent( const WgWidget * _pOrg )
{
	//TODO: Implement
}
