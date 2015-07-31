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

namespace wg 
{
	
	const char WgVectorPanel::CLASSNAME[] = {"VectorPanel"};
	const char WgVectorHook::CLASSNAME[] = {"VectorHook"};
	
	//____ WgVectorHook::isInstanceOf() __________________________________________
	
	bool WgVectorHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgPanelHook::isInstanceOf(pClassName);
	}
	
	//____ WgVectorHook::className() _____________________________________________
	
	const char * WgVectorHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ WgVectorHook::cast() __________________________________________________
	
	WgVectorHook_p WgVectorHook::cast( const WgHook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return WgVectorHook_p( static_cast<WgVectorHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	
	WgCoord WgVectorHook::pos() const
	{
		return parent()->_hookGeo(this).pos();
	}
	
	WgSize WgVectorHook::size() const
	{
		return parent()->_hookGeo(this).size();
	}
	
	WgRect WgVectorHook::geo() const
	{
		return parent()->_hookGeo(this);
	}
	
	WgCoord WgVectorHook::globalPos() const
	{
		return parent()->_hookGeo(this).pos() + parent()->globalPos();
	
	}
	
	WgRect WgVectorHook::globalGeo() const
	{
		return parent()->_hookGeo(this) + parent()->globalPos();
	}
	
	bool WgVectorHook::moveForward()
	{
		if( _moveUp() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool WgVectorHook::moveBackward()
	{
		if( _moveDown() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool WgVectorHook::moveBefore( const WgVectorHook_p& pSibling )
	{
		if( pSibling && pSibling->parent() == parent() )
		{
			if( _moveBefore(pSibling.rawPtr()) )
			{
				parent()->_onWidgetsReordered();
				return true;
			}
		}
		return false;
	}
	
	bool WgVectorHook::moveAfter( const WgVectorHook_p& pSibling )
	{
		if( pSibling && pSibling->parent() == parent() )
		{
			if( _moveAfter(pSibling.rawPtr()) )
			{
				parent()->_onWidgetsReordered();
				return true;
			}
		}
		return false;
	}
	
	bool WgVectorHook::moveFirst()
	{
		if( _moveFirst() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool WgVectorHook::moveLast()
	{
		if( _moveLast() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool WgVectorHook::setVisible( bool bVisible )
	{
		if( bVisible != m_bVisible )
		{
			m_bVisible = bVisible;
			if( bVisible )
				parent()->_onWidgetAppeared(this);
			else
				parent()->_onWidgetDisappeared(this);
		}
	
		return true;
	}
	
	WgVectorPanel_p WgVectorHook::parent() const
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
		parent()->_onRenderRequested(this, rect);
	}
	
	void WgVectorHook::_requestResize()
	{
		parent()->_onResizeRequested(this);
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
	
	//____ isInstanceOf() _________________________________________________________
	
	bool WgVectorPanel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return WgPanel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * WgVectorPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	WgVectorPanel_p WgVectorPanel::cast( const WgObject_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return WgVectorPanel_p( static_cast<WgVectorPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ _addWidget() _____________________________________________________________
	
	WgVectorHook * WgVectorPanel::_addWidget( WgWidget * pWidget )
	{
		if( !pWidget )
			return 0;
	
		WgVectorHook * pHook = _newHook();
		m_hooks.pushBack(pHook);
		pHook->_setWidget( pWidget );
	
		_onWidgetAppeared(pHook);
		return pHook;
	}
	
	//____ _insertWidget() __________________________________________________________
	
	WgVectorHook * WgVectorPanel::_insertWidget( WgWidget * pWidget, WgWidget * pSibling )
	{
		if( !pWidget || !pSibling || !pSibling->parent() || pSibling->parent() != this )
			return 0;
	
		WgVectorPanel * pWdg = (WgVectorPanel*) pSibling;
		WgHook * p = pWdg->m_pHook;
	
		WgVectorHook * pHook = _newHook();
		pHook->_moveBefore(static_cast<WgVectorHook*>(pSibling->_hook()));
	
		pHook->_setWidget( pWidget );
		
		_onWidgetAppeared(pHook);
		return pHook;
	}
	
	//____ removeWidget() __________________________________________________________
	
	bool WgVectorPanel::removeWidget( const WgWidget_p& pWidget )
	{
		if( !pWidget || !pWidget->hook() || pWidget->hook()->parent() != this )
			return false;
	
		// Disconnect and notify subclass that widget has disappeared
	
		WgVectorHook * pHook = (WgVectorHook *) pWidget->_hook();
		pHook->_disconnect();
	
		if( pHook->isVisible() )
			_onWidgetDisappeared( pHook );
	
		// Delete the hook and return
	
		delete pHook;
		return true;
	}
	
	//____ clear() ______________________________________________________
	
	bool WgVectorPanel::clear()
	{
		m_hooks.clear();
		_refreshAllWidgets();
	
		return true;
	}
	
	//____ _onCloneContent() ______________________________________________________
	
	void WgVectorPanel::_onCloneContent( const WgWidget * _pOrg )
	{
		//TODO: Implement
	}

} // namespace wg
