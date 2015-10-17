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

#include <wg_capsule.h>

namespace wg 
{
	
	const char Capsule::CLASSNAME[] = {"Capsule"};
	const char CapsuleHook::CLASSNAME[] = {"CapsuleHook"};
	
	//____ CapsuleHook::isInstanceOf() __________________________________________
	
	bool CapsuleHook::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Hook::isInstanceOf(pClassName);
	}
	
	//____ CapsuleHook::className() _____________________________________________
	
	const char * CapsuleHook::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ CapsuleHook::cast() __________________________________________________
	
	CapsuleHook_p CapsuleHook::cast( const Hook_p& pHook )
	{
		if( pHook && pHook->isInstanceOf(CLASSNAME) )
			return CapsuleHook_p( static_cast<CapsuleHook*>(pHook.rawPtr()) );
	
		return 0;
	}
	
	
	Coord CapsuleHook::pos() const { return m_pParent->pos(); }
	Size CapsuleHook::size() const { 	return m_pParent->size(); }
	Rect CapsuleHook::geo() const { return m_pParent->geo(); }
	
	Coord CapsuleHook::globalPos() const { return m_pParent->globalPos(); }
	Rect CapsuleHook::globalGeo() const { return m_pParent->globalGeo(); }
	
	Capsule_p CapsuleHook::parent() const { return m_pParent; }
	
	void CapsuleHook::_requestRender() { m_pParent->_requestRender(); }
	void CapsuleHook::_requestRender( const Rect& rect ) { m_pParent->_requestRender(rect); }
	void CapsuleHook::_requestResize() { m_pParent->_requestResize(); }
	
	Hook * CapsuleHook::_prevHook() const { return 0; }
	Hook * CapsuleHook::_nextHook() const { return 0; }
	Container * CapsuleHook::_parent() const { return m_pParent; }
	
	
	//____ Constructor ____________________________________________________________
	
	Capsule::Capsule()
	{
		m_hook.m_pParent = this;
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Capsule::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Container::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Capsule::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Capsule_p Capsule::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Capsule_p( static_cast<Capsule*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setWidget() ______________________________________________________________
	
	CapsuleHook_p Capsule::setWidget( const Widget_p& pWidget )
	{
		if( !pWidget )
			return 0;
	
		m_hook._setWidget(pWidget.rawPtr());
		pWidget->_onNewSize(size());
	
		_requestRender();
		_requestResize();
		return &m_hook;
	}
	
	//____ removeWidget() ___________________________________________________________
	
	bool Capsule::removeWidget( const Widget_p& pWidget )
	{
		if( m_hook._widget() != pWidget.rawPtr() )
			return false;
	
		m_hook._setWidget(0);
		_requestRender();
		_requestResize();
		return true;
	}
	
	//____ clear() __________________________________________________________
	
	bool Capsule::clear()
	{
		if( !m_hook._widget() )
			return false;
	
		m_hook._setWidget(0);
		_requestRender();
		_requestResize();
		return true;
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int Capsule::matchingHeight( int width ) const
	{
		if( m_hook._widget() )
			return m_hook._widget()->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}
	
	//____ matchingWidth() ________________________________________________________
	
	int Capsule::matchingWidth( int height ) const
	{
		if( m_hook._widget() )
			return m_hook._widget()->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}
	
	//____ preferredSize() ___________________________________________________________
	
	Size Capsule::preferredSize() const
	{
		if( m_hook._widget() )
			return m_hook._widget()->preferredSize();
		else
			return Size(1,1);
	}
	
	//____ _onCollectPatches() _____________________________________________________
	
	void Capsule::_onCollectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		if( m_hook._widget() )
			m_hook._widget()->_onCollectPatches( container, geo, clip );
	}
	
	//____ _onMaskPatches() ________________________________________________________
	
	void Capsule::_onMaskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		if( m_hook._widget() )
			m_hook._widget()->_onMaskPatches( patches, geo, clip, blendMode );
	}
	
	//____ _onCloneContent() _______________________________________________________
	
	void Capsule::_onCloneContent( const Widget * _pOrg )
	{
	}
	
	//____ _onNewSize() ____________________________________________________________
	
	void Capsule::_onNewSize( const Size& size )
	{
		if( m_hook._widget() )
			m_hook._widget()->_onNewSize(size);
	}
	
	//____ _firstHook() ____________________________________________________________
	
	Hook* Capsule::_firstHook() const
	{
		if( m_hook._widget() )
			return const_cast<CapsuleHook*>(&m_hook);
		else
			return 0;
	}
	
	//____ _lastHook() _____________________________________________________________
	
	Hook* Capsule::_lastHook() const
	{
		if( m_hook._widget() )
			return const_cast<CapsuleHook*>(&m_hook);
		else
			return 0;
	}
	
	//____ _firstHookWithGeo() _____________________________________________________
	
	Hook * Capsule::_firstHookWithGeo( Rect& geo ) const
	{
		if( m_hook._widget() )
		{
			geo = Rect(0,0,size());
			return const_cast<CapsuleHook*>(&m_hook);
		}
	
		return 0;
	}
	
	//____ _nextHookWithGeo() ______________________________________________________
	
	Hook * Capsule::_nextHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		return 0;
	}
	
	//____ _lastHookWithGeo() ______________________________________________________
	
	Hook * Capsule::_lastHookWithGeo( Rect& geo ) const
	{
		if( m_hook._widget() )
		{
			geo = Rect(0,0,size());
			return const_cast<CapsuleHook*>(&m_hook);
		}
	
		return 0;
	}
	
	//_____ _prevHookWithGeo() _____________________________________________________
	
	Hook * Capsule::_prevHookWithGeo( Rect& geo, Hook * pHook ) const
	{
		return 0;
	}

} // namespace wg
