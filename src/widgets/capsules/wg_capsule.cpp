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
	
		
	//____ Constructor ____________________________________________________________
	
	Capsule::Capsule() : child( this, &m_child )
	{
	}
	
	//____ Destructor _____________________________________________________________

	Capsule::~Capsule()
	{
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
		
	//____ removeChild() ___________________________________________________________
	
	bool Capsule::removeChild( const Widget_p& pWidget )
	{
		if( m_child.pWidget != pWidget.rawPtr() )
			return false;
	
		_replaceChild( &m_child, nullptr );
		return true;
	}
	
	//____ clear() __________________________________________________________
	
	bool Capsule::clear()
	{
		if( m_child.pWidget )
			_replaceChild( &m_child, nullptr );

		return true;
	}
	
	//____ matchingHeight() ________________________________________________________
	
	int Capsule::matchingHeight( int width ) const
	{
		if( m_child.pWidget )
			return m_child.pWidget->matchingHeight( width );
		else
			return Widget::matchingHeight(width);
	}
	
	//____ matchingWidth() ________________________________________________________
	
	int Capsule::matchingWidth( int height ) const
	{
		if( m_child.pWidget )
			return m_child.pWidget->matchingWidth( height );
		else
			return Widget::matchingWidth(height);
	}
	
	//____ preferredSize() ___________________________________________________________
	
	Size Capsule::preferredSize() const
	{
		if( m_child.pWidget )
			return m_child.pWidget->preferredSize();
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
		if( m_child.pWidget )
			m_child.pWidget->_collectPatches( container, geo, clip );
	}
	
	//____ _maskPatches() ________________________________________________________
	
	void Capsule::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		if( m_child.pWidget )
			m_child.pWidget->_maskPatches( patches, geo, clip, blendMode );
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

		if( m_child.pWidget )
			m_child.pWidget->_setSize(size);			
	}
	
	//____ _firstChild() ____________________________________________________________
	
	Widget* Capsule::_firstChild() const
	{
		return m_child.pWidget;
	}
	
	//____ _lastChild() _____________________________________________________________
	
	Widget* Capsule::_lastChild() const
	{
		return m_child.pWidget;
	}
	
	//____ _firstChildWithGeo() _____________________________________________________
	
	void Capsule::_firstChildWithGeo( WidgetWithGeo& package ) const
	{
		package.pWidget = m_child.pWidget;
		package.geo = Rect(0,0,m_size);
	}
	
	//____ _nextChildWithGeo() ______________________________________________________
	
	void Capsule::_nextChildWithGeo( WidgetWithGeo& package ) const
	{
		package.pWidget = nullptr;
	}
	
	//____ _replaceChild() ____________________________________________________________

	void Capsule::_replaceChild( ChildSlot * pSlot, Widget * pNewWidget )
	{
		Container::_replaceChild( pSlot, pNewWidget );

		pNewWidget->_setSize(size());
		_requestRender();
		_requestResize();
	}



} // namespace wg
