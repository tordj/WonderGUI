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

const char WgVectorPanel::CLASSNAME[] = {"VectorPanel"};

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

WgVectorPanelPtr WgVectorHook::Parent() const
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
	WgVectorPanel * p = static_cast<WgVectorPanel*>(_parent());

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

WgVectorPanel::WgVectorPanel() : m_nChildPanels(0)
{

}


//____ Destructor() ___________________________________________________________

WgVectorPanel::~WgVectorPanel()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgVectorPanel::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanel::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgVectorPanel::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgVectorPanelPtr WgVectorPanel::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgVectorPanelPtr( static_cast<WgVectorPanel*>(pObject.GetRealPtr()) );

	return 0;
}

//____ AddChild() _____________________________________________________________

WgVectorHook * WgVectorPanel::AddChild( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return 0;

	WgVectorHook * pHook = _newHook();
	m_hooks.PushBack(pHook);
	pHook->_setWidget( pWidget.GetRealPtr() );

	_onWidgetAppeared(pHook);
	return pHook;
}

//____ InsertChild() __________________________________________________________

WgVectorHook * WgVectorPanel::InsertChild( const WgWidgetPtr& pWidget, const WgWidgetPtr& pSibling )
{
	if( !pWidget || !pSibling || !pSibling->Parent() || pSibling->Parent() != this )
		return 0;

	WgVectorHook * pHook = _newHook();
	pHook->_moveBefore(static_cast<WgVectorHook*>(pSibling->Hook()));

	pHook->_setWidget( pWidget.GetRealPtr() );
	
	_onWidgetAppeared(pHook);
	return pHook;
}

//____ RemoveChild() __________________________________________________________

bool WgVectorPanel::RemoveChild( const WgWidgetPtr& pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return false;

	// Disconnect and notify subclass that widget has disappeared

	WgVectorHook * pHook = (WgVectorHook *) pWidget->Hook();
	pHook->_disconnect();

	if( pHook->IsVisible() )
		_onWidgetDisappeared( pHook );

	// Delete the hook and return

	delete pHook;
	return true;
}

//____ Clear() ______________________________________________________

bool WgVectorPanel::Clear()
{
	m_hooks.Clear();
	_refreshAllWidgets();

	return true;
}

//____ _onCloneContent() ______________________________________________________

void WgVectorPanel::_onCloneContent( const WgWidget * _pOrg )
{
	//TODO: Implement
}
