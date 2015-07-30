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

const char WgHook::CLASSNAME[] = {"Hook"};

//____ Destructor _____________________________________________________________

WgHook::~WgHook()
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

bool WgHook::isInstanceOf( const char * pClassName ) const
{ 
	return (pClassName==CLASSNAME);
}

//____ className() ____________________________________________________________

const char * WgHook::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgHook_p WgHook::cast( const WgHook_p& pHook )
{
	return pHook;
}

//____ widget() _______________________________________________________________

WgWidget_p WgHook::widget() const
{ 
	return m_pWidget; 
}

//____ parent() _______________________________________________________________

WgContainer_p WgHook::parent() const 
{ 
	return _parent(); 
}


//____ _setWidget() __________________________________________________________

void WgHook::_setWidget( WgWidget * pWidget )
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

void WgHook::_relinkWidget()
{
	if( m_pWidget )
		m_pWidget->m_pHook = this;
}

//____ _requestFocus() _________________________________________________________

bool WgHook::_requestFocus()
{
	return parent()->_focusRequested(this, m_pWidget);
}

//____ _releaseFocus() _________________________________________________________

bool WgHook::_releaseFocus()
{
	return parent()->_focusReleased(this, m_pWidget);
}

//____ root() _________________________________________________________________

WgRootPanel_p WgHook::root() const
{
	return _root();
}

//____ _root() _________________________________________________________________

WgRootPanel * WgHook::_root() const
{
	WgContainer * pParent = _parent();

	if( pParent )
	{
		WgHook * pParentHook = pParent->_hook();
		if( pParentHook )
			return pParentHook->_root();
	}

	return 0;
}

//____ _isVisible() ___________________________________________________________

bool WgHook::_isVisible() const
{
	return true;
}


