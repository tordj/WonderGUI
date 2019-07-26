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

	Capsule::Capsule() : child( &m_child, this )
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

	Capsule_p Capsule::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Capsule_p( static_cast<Capsule*>(pObject) );

		return 0;
	}

	//____ _object() _________________________________________________________

	Object * Capsule::_object()
	{
		return this;
	}

	const Object * Capsule::_object() const
	{
		return this;
	}


	//____ _matchingHeight() ________________________________________________________

	int Capsule::_matchingHeight( int width ) const
	{
		if( m_child.pWidget )
			return m_child.matchingHeight( width );
		else
			return Widget::_matchingHeight(width);
	}

	//____ matchingWidth() ________________________________________________________

	int Capsule::_matchingWidth( int height ) const
	{
		if( m_child.pWidget )
			return m_child.matchingWidth( height );
		else
			return Widget::_matchingWidth(height);
	}

	//____ preferredSize() ___________________________________________________________

	SizeI Capsule::_preferredSize() const
	{
		if( m_child.pWidget )
			return m_child.preferredSize();
		else
			return SizeI(0,0);
	}

	//____ _childPos() ___________________________________________________________

	CoordI Capsule::_childPos( Slot * pSlot ) const
	{
		if( m_pSkin )
			return m_pSkin->_contentOfs( m_state );

		return CoordI();
	}

	//____ _childSize() __________________________________________________________

	SizeI Capsule::_childSize( Slot * pSlot ) const
	{
		if( m_pSkin )
			return m_size - m_pSkin->_contentPadding();

		return m_size;
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( Slot * pSlot )
	{
		if( m_pSkin )
			_requestRender( m_pSkin->_contentRect( m_size, m_state ));
		else
			_requestRender();
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( Slot * pSlot, const RectI& rect )
	{
		if( m_pSkin )
			_requestRender( rect + m_pSkin->_contentOfs( m_state ));
		else
			_requestRender( rect );
	}

	//____ _childRequestResize() _________________________________________________

	void Capsule::_childRequestResize( Slot * pSlot )
	{
		_requestResize();
	}

	//____ _prevChild() __________________________________________________________

	Widget * Capsule::_prevChild( const Slot * pSlot ) const
	{
		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * Capsule::_nextChild( const Slot * pSlot ) const
	{
		return nullptr;
	}


	//____ _collectPatches() _____________________________________________________

	void Capsule::_collectPatches( Patches& container, const RectI& geo, const RectI& clip )
	{
		if( m_child.pWidget )
			m_child.pWidget->_collectPatches( container, geo, clip );
	}

	//____ _maskPatches() ________________________________________________________

	void Capsule::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
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

	void Capsule::_setSize( const SizeI& size )
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

	//____ _firstSlotWithGeo() _____________________________________________________

	void Capsule::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		package.pSlot = &m_child;
		package.geo = RectI(0,0,m_size);
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void Capsule::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		package.pSlot = nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void Capsule::_releaseChild(Slot * pSlot)
	{
		pSlot->replaceWidget(this, nullptr);
		_requestRender();
		_requestResize();
	}

	//____ _setWidget() ____________________________________________________________

	void Capsule::_setWidget( Slot * pSlot, Widget * pWidget )
	{
		pSlot->replaceWidget( this, pWidget );

		if( pWidget )
			pWidget->_setSize(m_size);

		_requestRender();
		_requestResize();
	}



} // namespace wg
