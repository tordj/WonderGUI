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
const char WgVectorHook::CLASSNAME[] = {"VectorHook"};

//____ WgVectorHook::IsInstanceOf() __________________________________________

bool WgVectorHook::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgPanelHook::IsInstanceOf(pClassName);
}

//____ WgVectorHook::ClassName() _____________________________________________

const char * WgVectorHook::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ WgVectorHook::Cast() __________________________________________________

WgVectorHookPtr WgVectorHook::Cast( const WgHookPtr& pHook )
{
	if( pHook && pHook->IsInstanceOf(CLASSNAME) )
		return WgVectorHookPtr( static_cast<WgVectorHook*>(pHook.RawPtr()) );

	return 0;
}


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

WgCoord WgVectorHook::GlobalPos() const
{
	return Parent()->_hookGeo(this).Pos() + Parent()->GlobalPos();

}

WgRect WgVectorHook::GlobalGeo() const
{
	return Parent()->_hookGeo(this) + Parent()->GlobalPos();
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

bool WgVectorHook::MoveBefore( const WgVectorHookPtr& pSibling )
{
	if( pSibling && pSibling->Parent() == Parent() )
	{
		if( _moveBefore(pSibling.RawPtr()) )
		{
			Parent()->_onWidgetsReordered();
			return true;
		}
	}
	return false;
}

bool WgVectorHook::MoveAfter( const WgVectorHookPtr& pSibling )
{
	if( pSibling && pSibling->Parent() == Parent() )
	{
		if( _moveAfter(pSibling.RawPtr()) )
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
		return WgVectorPanelPtr( static_cast<WgVectorPanel*>(pObject.RawPtr()) );

	return 0;
}

//____ _addWidget() _____________________________________________________________

WgVectorHook * WgVectorPanel::_addWidget( WgWidget * pWidget )
{
	if( !pWidget )
		return 0;

	WgVectorHook * pHook = _newHook();
	m_hooks.PushBack(pHook);
	pHook->_setWidget( pWidget );

	_onWidgetAppeared(pHook);
	return pHook;
}

//____ _insertWidget() __________________________________________________________

WgVectorHook * WgVectorPanel::_insertWidget( WgWidget * pWidget, WgWidget * pSibling )
{
	if( !pWidget || !pSibling || !pSibling->Parent() || pSibling->Parent() != this )
		return 0;

	WgVectorPanel * pWdg = (WgVectorPanel*) pSibling;
	WgHook * p = pWdg->m_pHook;

	WgVectorHook * pHook = _newHook();
	pHook->_moveBefore(static_cast<WgVectorHook*>(pSibling->_hook()));

	pHook->_setWidget( pWidget );
	
	_onWidgetAppeared(pHook);
	return pHook;
}

//____ RemoveWidget() __________________________________________________________

bool WgVectorPanel::RemoveWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget || !pWidget->Hook() || pWidget->Hook()->Parent() != this )
		return false;

	// Disconnect and notify subclass that widget has disappeared

	WgVectorHook * pHook = (WgVectorHook *) pWidget->_hook();
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
