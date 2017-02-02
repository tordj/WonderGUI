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

		if( m_hook._widget() )
			m_hook._widget()->_setHolder( nullptr, nullptr );

		m_hook._setWidget(pWidget.rawPtr());
		pWidget->_setHolder( this, nullptr );
		pWidget->_setSize(size());
	
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
		pWidget->_setHolder( nullptr, nullptr );
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
		m_hook._widget()->_setHolder( nullptr, nullptr );
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
	
	//____ _childPos() ___________________________________________________________

	Coord Capsule::_childPos( void * pChildRef ) const
	{
		if( m_pSkin )
			return m_pSkin->contentOfs( m_state );

		return Coord();
	}

	//____ _childSize() __________________________________________________________

	Size Capsule::_childSize( void * pChildRef ) const
	{
		if( m_pSkin )
			return m_size - m_pSkin->contentPadding();

		return m_size;
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( void * pChildRef )
	{
		if( m_pSkin )
			_requestRender( m_pSkin->contentRect( m_size, m_state ));
		else
			_requestRender();
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( void * pChildRef, const Rect& rect )
	{
		if( m_pSkin )
			_requestRender( rect + m_pSkin->contentOfs( m_state ));
		else
			_requestRender( rect );
	}

	//____ _childRequestResize() _________________________________________________

	void Capsule::_childRequestResize( void * pChildRef )
	{
		_requestResize();
	}

	//____ _prevChild() __________________________________________________________

	Widget * Capsule::_prevChild( void * pChildRef ) const
	{
		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * Capsule::_nextChild( void * pChildRef ) const
	{
		return nullptr;
	}


	//____ _collectPatches() _____________________________________________________
	
	void Capsule::_collectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		if( m_hook._widget() )
			m_hook._widget()->_collectPatches( container, geo, clip );
	}
	
	//____ _maskPatches() ________________________________________________________
	
	void Capsule::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		if( m_hook._widget() )
			m_hook._widget()->_maskPatches( patches, geo, clip, blendMode );
	}
	
	//____ _cloneContent() _______________________________________________________
	
	void Capsule::_cloneContent( const Widget * _pOrg )
	{
		Container::_cloneContent( _pOrg );
	}
	
	//____ _setSize() ____________________________________________________________
	
	void Capsule::_setSize( const Size& size )
	{
		Container::_setSize( size );

		if( m_hook._widget() )
			m_hook._widget()->_setSize(size);			
	}
	
	//____ _firstChild() ____________________________________________________________
	
	Widget* Capsule::_firstChild() const
	{
		return m_hook._widget();
	}
	
	//____ _lastChild() _____________________________________________________________
	
	Widget* Capsule::_lastChild() const
	{
		return m_hook._widget();
	}
	
	//____ _firstChildWithGeo() _____________________________________________________
	
	void Capsule::_firstChildWithGeo( WidgetWithGeo& package ) const
	{
		package.pWidget = m_hook._widget();
		package.geo = Rect(0,0,m_size);
	}
	
	//____ _nextChildWithGeo() ______________________________________________________
	
	void Capsule::_nextChildWithGeo( WidgetWithGeo& package ) const
	{
		package.pWidget = nullptr;
	}
	

} // namespace wg
