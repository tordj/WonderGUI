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

	spx Capsule::_matchingHeight( spx width, int scale ) const
	{
		scale = _fixScale(scale);

		if( slot._widget() )
		{
			SizeSPX padding = m_skin.contentPaddingSize(scale);

			return slot._widget()->_matchingHeight( width-padding.w, scale ) + padding.h;
		}
		else
			return Widget::_matchingHeight(width, scale);
	}

	//____ _matchingWidth() ________________________________________________________

	spx Capsule::_matchingWidth( spx height, int scale ) const
	{
		scale = _fixScale(scale);

		if( slot._widget() )
		{
			SizeSPX padding = m_skin.contentPaddingSize(scale);

			return slot._widget()->_matchingWidth(height - padding.h) + padding.w;
		}
		else
			return Widget::_matchingWidth(height);
	}

	//____ _defaultSize() ___________________________________________________________

	SizeSPX Capsule::_defaultSize( int scale ) const
	{
		if (slot._widget())
		{
			scale = _fixScale(scale);
			return slot._widget()->_defaultSize(scale) + m_skin.contentPaddingSize(scale);
		}

		return Widget::_defaultSize(scale);
	}

	//____ _childPos() ___________________________________________________________

	CoordSPX Capsule::_childPos( const StaticSlot * pSlot ) const
	{
		return m_skin.contentOfs( m_scale, m_state );
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( StaticSlot * pSlot )
	{
		_requestRender(m_skin.contentRect( m_size, m_scale, m_state ));
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( StaticSlot * pSlot, const RectSPX& rect )
	{
		_requestRender( rect + m_skin.contentOfs( m_scale, m_state ));
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

	void Capsule::_collectPatches( PatchesSPX& container, const RectSPX& geo, const RectSPX& clip )
	{
		if( !m_skin.isEmpty() )
			container.add(RectSPX::getIntersection(geo, clip));
		else if( slot._widget() )
			slot._widget()->_collectPatches(container, _contentRect(geo), clip);
	}

	//____ _maskPatches() ________________________________________________________

	void Capsule::_maskPatches( PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode )
	{
		if (!m_skin.isEmpty())
		{
			if(m_skin.isOpaque( clip, geo.size(), m_scale, m_state ) )
				patches.sub(RectSPX::getIntersection(geo, clip));
			else if( slot._widget() )
				slot._widget()->_maskPatches(patches, _contentRect(geo), clip, blendMode);
		}
		else if( slot._widget() )
			slot._widget()->_maskPatches( patches, geo, clip, blendMode );
	}

	//____ _resize() ____________________________________________________________

	void Capsule::_resize( const SizeSPX& size, int scale )
	{
		Container::_resize( size, scale );

		if ( slot._widget())
		{
			SizeSPX sz = size - m_skin.contentPaddingSize(m_scale);
			slot._widget()->_resize(sz);
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
		if (slot.isEmpty())
		{
			package.pSlot = nullptr;
		}
		else
		{
			package.pSlot = &slot;
			package.geo = m_skin.contentRect(RectSPX(0,0,m_size), m_scale, m_state);
		}
	}

	//____ _nextSlotWithGeo() ______________________________________________________

	void Capsule::_nextSlotWithGeo( SlotWithGeo& package ) const
	{
		package.pSlot = nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void Capsule::_releaseChild(StaticSlot * _pSlot)
	{
		slot._setWidget(nullptr);
		_requestRender();
		_requestResize();
	}

	//____ _replaceChild() ____________________________________________________________

	void Capsule::_replaceChild( StaticSlot * pSlot, Widget * pWidget )
	{
		slot._setWidget(pWidget );

		if (pWidget)
		{
			SizeSPX sz = m_size - m_skin.contentPaddingSize(m_scale);
			pWidget->_resize(sz);
		}

		_requestRender();
		_requestResize();
	}



} // namespace wg