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
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgSortableHook::MoveDown()
{
	if( _moveDown() )
	{
		Parent()->_onGizmosReordered();
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
			Parent()->_onGizmosReordered();
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
			Parent()->_onGizmosReordered();
			return true;
		}
	}
	return false;
}

bool WgSortableHook::MoveFirst()
{
	if( _moveFirst() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgSortableHook::MoveLast()
{
	if( _moveLast() )
	{
		Parent()->_onGizmosReordered();
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
			Parent()->_onGizmoAppeared(this);
		else
			Parent()->_onGizmoDisappeared(this);
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

WgSortableHook * WgSortablePanel::AddChild( WgGizmo * pGizmo )
{
	if( !pGizmo )
		return 0;

	WgSortableHook * pHook = _newHook();
	m_hooks.PushBack(pHook);
	pHook->_attachGizmo( pGizmo );

	_onGizmoAppeared(pHook);
	return pHook;
}

//____ InsertChild() __________________________________________________________

WgSortableHook * WgSortablePanel::InsertChild( WgGizmo * pGizmo, WgGizmo * pSibling )
{
	if( !pGizmo || !pSibling || !pSibling->Parent() || pSibling->Parent() != this )
		return 0;

	WgSortableHook * pHook = _newHook();
	pHook->_moveBefore(static_cast<WgSortableHook*>(pSibling->Hook()));

	pHook->_attachGizmo( pGizmo );
	
	_onGizmoAppeared(pHook);
	return pHook;
}

//____ InsertChildSorted() ____________________________________________________

WgSortableHook * WgSortablePanel::InsertChildSorted( WgGizmo * pGizmo )
{
	//TODO: Implement

	return 0;
}

//____ DeleteChild() __________________________________________________________

bool WgSortablePanel::DeleteChild( WgGizmo * pGizmo )
{
	if( !pGizmo || !pGizmo->Hook() || pGizmo->Hook()->Parent() != this )
		return false;

	// Disconnect and notify subclass that gizmo has disappeared

	WgSortableHook * pHook = (WgSortableHook *) pGizmo->Hook();
	pHook->_disconnect();

	if( pHook->IsVisible() )
		_onGizmoDisappeared( pHook );

	// Delete the gizmo and return

	delete pHook;
	return true;
}

//____ ReleaseChild() _________________________________________________________

WgGizmo * WgSortablePanel::ReleaseChild( WgGizmo * pGizmo )
{
	if( !pGizmo || !pGizmo->Hook() || pGizmo->Hook()->Parent() != this )
		return 0;

	// Disconnect and notify subclass that gizmo has disappeared

	WgSortableHook * pHook = (WgSortableHook *) pGizmo->Hook();
	pHook->_disconnect();

	if( pHook->IsVisible() )
		_onGizmoDisappeared( pHook );

	return pGizmo;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgSortablePanel::DeleteAllChildren()
{
	m_hooks.Clear();
	_refreshAllGizmos();

	return true;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgSortablePanel::ReleaseAllChildren()
{
	while( m_hooks.First() )
		m_hooks.PopFront();

	_refreshAllGizmos();

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

void WgSortablePanel::SetSortFunction( WgGizmoSortFunc pSortFunc )
{
	m_pSortFunc = pSortFunc;
}

//____ _onCloneContent() ______________________________________________________

void WgSortablePanel::_onCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement
}
