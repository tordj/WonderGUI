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
#include <wg_patches.h>
#include <wg_internal.h>

namespace wg
{

	const TypeInfo Capsule::TYPEINFO = { "Capsule", &Container::TYPEINFO };


	//____ constructor ____________________________________________________________

	Capsule::Capsule() : slot(this)
	{
	}

	//____ Destructor _____________________________________________________________

	Capsule::~Capsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Capsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _matchingHeight() ________________________________________________________

	int Capsule::_matchingHeight( int width ) const
	{
		if( slot._widget() )
		{
			SizeI padding = m_pSkin ? m_pSkin->_contentPaddingSize() : SizeI();

			return OO(&slot)->_matchingHeight( width-padding.w ) + padding.h;
		}
		else
			return Widget::_matchingHeight(width);
	}

	//____ matchingWidth() ________________________________________________________

	int Capsule::_matchingWidth( int height ) const
	{
		if( slot._widget() )
		{
			SizeI padding = m_pSkin ? m_pSkin->_contentPaddingSize() : SizeI();

			return OO(&slot)->_matchingWidth(height - padding.h) + padding.w;
		}
		else
			return Widget::_matchingWidth(height);
	}

	//____ preferredSize() ___________________________________________________________

	SizeI Capsule::_preferredSize() const
	{
		if( slot._widget() )
		{
			if (m_pSkin)
				return OO(&slot)->_preferredSize() + m_pSkin->_contentPaddingSize();

			return OO(&slot)->_preferredSize();
		}
		else
			return Widget::_preferredSize();
	}

	//____ _childPos() ___________________________________________________________

	CoordI Capsule::_childPos( const StaticSlot * pSlot ) const
	{
		if( m_pSkin )
			return m_pSkin->_contentOfs( m_state );

		return CoordI();
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( StaticSlot * pSlot )
	{
		if( m_pSkin )
			_requestRender( m_pSkin->_contentRect( m_size, m_state ));
		else
			_requestRender();
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( StaticSlot * pSlot, const RectI& rect )
	{
		if( m_pSkin )
			_requestRender( rect + m_pSkin->_contentOfs( m_state ));
		else
			_requestRender( rect );
	}

	//____ _childRequestResize() _________________________________________________

	void Capsule::_childRequestResize( StaticSlot * pSlot )
	{
		_requestResize();
	}

	//____ _prevChild() __________________________________________________________

	Widget * Capsule::_prevChild( const StaticSlot * pSlot ) const
	{
		return nullptr;
	}

	//____ _nextChild() __________________________________________________________

	Widget * Capsule::_nextChild( const StaticSlot * pSlot ) const
	{
		return nullptr;
	}


	//____ _collectPatches() _____________________________________________________

	void Capsule::_collectPatches( Patches& container, const RectI& geo, const RectI& clip )
	{
		if( m_pSkin )
			container.add(RectI(geo, clip));
		else if( slot._widget() )
			OO(slot._widget())->_collectPatches(container, geo, clip);
	}

	//____ _maskPatches() ________________________________________________________

	void Capsule::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		if (m_pSkin)
		{
			if( m_pSkin->isOpaque( clip, geo.size(), m_state ) )
				patches.sub(RectI(geo, clip));
			else if( slot._widget() )
				OO(slot._widget())->_maskPatches(patches, m_pSkin->_contentRect(geo, m_state), clip, blendMode);
		}
		else if( slot._widget() )
			OO( slot._widget())->_maskPatches( patches, geo, clip, blendMode );
	}

	//____ _cloneContent() _______________________________________________________

	void Capsule::_cloneContent( const Widget * _pOrg )
	{
		Container::_cloneContent( _pOrg );
	}

	//____ _resize() ____________________________________________________________

	void Capsule::_resize( const SizeI& size )
	{
		Container::_resize( size );

		if ( slot._widget())
		{
			SizeI sz = m_pSkin ? size - m_pSkin->_contentPaddingSize() : size;
			OO( slot._widget())->_resize(sz);
		}
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	Capsule::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return DynamicSlot::TYPEINFO;
	}

	//____ _firstChild() ____________________________________________________________

	Widget* Capsule::_firstChild() const
	{
		return slot._widget();
	}

	//____ _lastChild() _____________________________________________________________

	Widget* Capsule::_lastChild() const
	{
		return slot._widget();
	}

	//____ _firstSlotWithGeo() _____________________________________________________

	void Capsule::_firstSlotWithGeo( SlotWithGeo& package ) const
	{
		package.pSlot = &slot;
		package.geo = m_pSkin ? m_pSkin->_contentRect(RectI(0,0,m_size),m_state) : RectI(0, 0, m_size);
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void Capsule::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		package.pSlot = nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void Capsule::_releaseChild(StaticSlot * _pSlot)
	{
		OO(&slot)->_setWidget(nullptr);
		_requestRender();
		_requestResize();
	}

	//____ _replaceChild() ____________________________________________________________

	void Capsule::_replaceChild( StaticSlot * pSlot, Widget * pWidget )
	{
		OO(&slot)->_setWidget(pWidget );

		if (pWidget)
		{
			SizeI sz = m_pSkin ? m_size - m_pSkin->_contentPaddingSize() : m_size;
			OO(pWidget)->_resize(sz);
		}

		_requestRender();
		_requestResize();
	}



} // namespace wg
