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
	
	const char VectorPanel::CLASSNAME[] = {"VectorPanel"};
	const char VectorHook::CLASSNAME[] = {"VectorHook"};
	
	//____ VectorHook::isInstanceOf() __________________________________________
	
	bool VectorHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return PanelHook::isInstanceOf(pClassName);
	}
	
	//____ VectorHook::className() _____________________________________________
	
	const char * VectorHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ VectorHook::cast() __________________________________________________
	
	VectorHook_p VectorHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return VectorHook_p( static_cast<VectorHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	
	Coord VectorHook::pos() const
	{
		return parent()->_hookGeo(this).pos();
	}
	
	Size VectorHook::size() const
	{
		return parent()->_hookGeo(this).size();
	}
	
	Rect VectorHook::geo() const
	{
		return parent()->_hookGeo(this);
	}
	
	Coord VectorHook::globalPos() const
	{
		return parent()->_hookGeo(this).pos() + parent()->globalPos();
	
	}
	
	Rect VectorHook::globalGeo() const
	{
		return parent()->_hookGeo(this) + parent()->globalPos();
	}
	
	bool VectorHook::moveForward()
	{
		if( _moveUp() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool VectorHook::moveBackward()
	{
		if( _moveDown() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool VectorHook::moveBefore( const VectorHook_p& pSibling )
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
	
	bool VectorHook::moveAfter( const VectorHook_p& pSibling )
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
	
	bool VectorHook::moveFirst()
	{
		if( _moveFirst() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool VectorHook::moveLast()
	{
		if( _moveLast() )
		{
			parent()->_onWidgetsReordered();
			return true;
		}
	
		return false;
	}
	
	bool VectorHook::setVisible( bool bVisible )
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
	
	VectorPanel_p VectorHook::parent() const
	{
		return static_cast<VectorPanel*>(_parent());
	}
	
	
	VectorHook::VectorHook()
	{
	}
	
	VectorHook::~VectorHook()
	{
	}
	
	
	void VectorHook::_requestRender()
	{
		VectorPanel * p = static_cast<VectorPanel*>(_parent());
	
		p->_renderRequested(this);
	}
	
	void VectorHook::_requestRender( const Rect& rect )
	{
		parent()->_renderRequested(this, rect);
	}
	
	void VectorHook::_requestResize()
	{
		parent()->_requestResizeRequested(this);
	}
	
	
	Hook * VectorHook::_prevHook() const
	{
		return _prev();
	}
	
	Hook * VectorHook::_nextHook() const
	{
		return _next();
	}
	
	//____ Constructor() __________________________________________________________
	
	VectorPanel::VectorPanel() : m_nChildPanels(0)
	{
	
	}
	
	
	//____ Destructor() ___________________________________________________________
	
	VectorPanel::~VectorPanel()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool VectorPanel::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Panel::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * VectorPanel::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	VectorPanel_p VectorPanel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return VectorPanel_p( static_cast<VectorPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ _addWidget() _____________________________________________________________
	
	VectorHook * VectorPanel::_addWidget( Widget * pWidget )
	{
		if( !pWidget )
			return 0;
	
		VectorHook * pHook = _newHook();
		m_hooks.pushBack(pHook);
		pHook->_setWidget( pWidget );
	
		_onWidgetAppeared(pHook);
		return pHook;
	}
	
	//____ _insertWidget() __________________________________________________________
	
	VectorHook * VectorPanel::_insertWidget( Widget * pWidget, Widget * pSibling )
	{
		if( !pWidget || !pSibling || !pSibling->parent() || pSibling->parent() != this )
			return 0;
		
		VectorHook * pHook = _newHook();
		pHook->_moveBefore(static_cast<VectorHook*>(pSibling->_hook()));
	
		pHook->_setWidget( pWidget );
		
		_onWidgetAppeared(pHook);
		return pHook;
	}
	
	//____ removeWidget() __________________________________________________________
	
	bool VectorPanel::removeWidget( const Widget_p& pWidget )
	{
		if( !pWidget || !pWidget->hook() || pWidget->hook()->parent() != this )
			return false;
	
		// Disconnect and notify subclass that widget has disappeared
	
		VectorHook * pHook = (VectorHook *) pWidget->_hook();
		pHook->_disconnect();
	
		if( pHook->isVisible() )
			_onWidgetDisappeared( pHook );
	
		// Delete the hook and return
	
		delete pHook;
		return true;
	}
	
	//____ clear() ______________________________________________________
	
	bool VectorPanel::clear()
	{
		m_hooks.clear();
		_refreshAllWidgets();
	
		return true;
	}
	
	//____ _cloneContent() ______________________________________________________
	
	void VectorPanel::_cloneContent( const Widget * _pOrg )
	{
		Panel::_cloneContent( _pOrg );

		//TODO: Implement		
	}

} // namespace wg
