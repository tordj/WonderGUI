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

#include <wg_orderedlayout.h>


WgCord WgOrderedHook::Pos() const
{
	return Parent()->_hookGeo().Pos();
}

WgSize WgOrderedHook::Size() const
{
	return Parent()->_hookGeo().Size();
}

WgRect WgOrderedHook::Geo() const
{
	return Parent()->_hookGeo();
}

WgCord WgOrderedHook::ScreenPos() const
{
	return Parent()->_hookGeo().Pos() + Parent()->ScreenPos();

}

WgRect WgOrderedHook::ScreenGeo() const
{
	return Parent()->_hookGeo() + Parent()->ScreenPos();
}

bool WgOrderedHook::MoveUp()
{
	if( _moveUp() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgOrderedHook::MoveDown()
{
	if( _moveDown() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgOrderedHook::MoveBefore( WgOrderedHook * pSibling )
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

bool WgOrderedHook::MoveAfter( WgOrderedHook * pSibling )
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

bool WgOrderedHook::MoveFirst()
{
	if( _moveFirst() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

bool WgOrderedHook::MoveLast()
{
	if( _moveLast() )
	{
		Parent()->_onGizmosReordered();
		return true;
	}

	return false;
}

void WgOrderedHook::SetHidden( bool bHide )
{
	if( bHide != m_bHidden )
	{
		m_bHidden = bHide;
		if( bHide )
			Parent()->_onGizmoDisappeared(this);
		else
			Parent()->_onGizmoAppeared(this);
	}
}

WgWidget* WgOrderedHook::GetRoot()
{
}

WgOrderedHook::WgOrderedHook( WgGizmo * pGizmo, WgGizmoOrderedGeo * pParent )
{
}

WgOrderedHook::~WgOrderedHook()
{
}


void WgOrderedHook::RequestRender()
{
}

void WgOrderedHook::RequestRender( const WgRect& rect )
{
}

void WgOrderedHook::RequestResize()
{
}


WgGizmoHook * WgOrderedHook::_prevHook() const
{
}

WgGizmoHook * WgOrderedHook::_nextHook() const
{
}



WgOrderedHook * WgOrderedLayout::AddGizmo( WgGizmo * pGizmo )
{
}

WgOrderedHook * WgOrderedLayout::InsertGizmo( WgGizmo * pGizmo, WgGizmo * pSibling )
{
}

WgOrderedHook * WgOrderedLayout::InsertGizmoSorted( WgGizmo * pGizmo )
{
}


bool WgOrderedLayout::DeleteGizmo( WgGizmo * pGizmo )
{
}

bool WgOrderedLayout::ReleaseGizmo( WgGizmo * pGizmo )
{
}


void WgOrderedLayout::DeleteAllGizmos()
{
}

void WgOrderedLayout::ReleaseAllGizmos()
{
}


void WgOrderedLayout::SortGizmos()
{
}

void WgOrderedLayout::SetSortOrder( WgSortOrder order )
{
}


void WgOrderedLayout::SetSortFunction( WgGizmoSortFunct pSortFunc )
{
}

void WgOrderedLayout::ScrollIntoView( WgGizmo * pGizmo )
{
}

WgGizmo * WgOrderedLayout::FindGizmo( const WgCord& ofs, WgSearchMode mode )
{
}

void WgOrderedLayout::_onCloneContent( const WgGizmo * _pOrg )
{
}

void WgOrderedLayout::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
}

bool WgOrderedLayout::_onAlphaTest( const WgCord& ofs )
{
}

