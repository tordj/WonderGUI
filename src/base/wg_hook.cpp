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

namespace wg 
{
	
	const char Hook::CLASSNAME[] = {"MyHook"};
	
	//____ Destructor _____________________________________________________________
	
	Hook::~Hook()
	{
		if( m_pWidget )
		{
			m_pWidget->m_pHook = 0;
			m_pWidget->_decRefCount();
		}
	
		if( m_pPtrHub )
			m_pPtrHub->pObj = 0;
	}
	
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Hook::isInstanceOf( const char * pClassName ) const
	{ 
		return (pClassName==CLASSNAME);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Hook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Hook_p Hook::cast( const Hook_p& pHook )
	{
		return pHook;
	}
	
	//____ widget() _______________________________________________________________
	
	Widget_p Hook::widget() const
	{ 
		return m_pWidget; 
	}
	
	//____ parent() _______________________________________________________________
	
	Container_p Hook::parent() const 
	{ 
		return _parent(); 
	}
	
	
	//____ _setWidget() __________________________________________________________
	
	void Hook::_setWidget( Widget * pWidget )
	{
		assert( pWidget == 0 || pWidget->_parent() == 0 );
	
		if( pWidget )
		{
			pWidget->m_pHook = this;
			pWidget->_incRefCount();
		}
	
		if( m_pWidget )
		{
			m_pWidget->m_pHook = 0;
			m_pWidget->_decRefCount();
		}
	
		m_pWidget = pWidget;
	
	}
	
	//____ _relinkWidget() __________________________________________________________
	
	void Hook::_relinkWidget()
	{
		if( m_pWidget )
			m_pWidget->m_pHook = this;
	}
	
	//____ _requestFocus() _________________________________________________________
	
	bool Hook::_requestFocus(Widget * pWidget)
	{
		return _parent()->_focusRequested(this, pWidget);
	}
	
	//____ _releaseFocus() _________________________________________________________
	
	bool Hook::_releaseFocus(Widget * pWidget)
	{
		return _parent()->_focusReleased(this, pWidget);
	}
	
	//____ root() _________________________________________________________________
	
	RootPanel_p Hook::root() const
	{
		return _root();
	}
	
	//____ _root() _________________________________________________________________
	
	RootPanel * Hook::_root() const
	{
		Container * pParent = _parent();
	
		if( pParent )
		{
			Hook * pParentHook = pParent->_hook();
			if( pParentHook )
				return pParentHook->_root();
		}
	
		return 0;
	}
	
	//____ _isVisible() ___________________________________________________________
	
	bool Hook::_isVisible() const
	{
		return true;
	}
	
	

} // namespace wg
