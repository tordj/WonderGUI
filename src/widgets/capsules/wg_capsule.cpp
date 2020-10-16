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

	//____ matchingHeight() ________________________________________________________

	MU Capsule::matchingHeight( MU width ) const
	{
		if( slot._widget() )
		{
			Size padding = m_skin.contentPaddingSize();

			return slot._widget()->matchingHeight( width-padding.w ) + padding.h;
		}
		else
			return Widget::matchingHeight(width);
	}

	//____ matchingWidth() ________________________________________________________

	MU Capsule::matchingWidth( MU height ) const
	{
		if( slot._widget() )
		{
			Size padding = m_skin.contentPaddingSize();

			return slot._widget()->matchingWidth(height - padding.h) + padding.w;
		}
		else
			return Widget::matchingWidth(height);
	}

	//____ preferredSize() ___________________________________________________________

	Size Capsule::preferredSize() const
	{
		if( slot._widget() )
			return slot._widget()->preferredSize() + m_skin.contentPaddingSize();
		else
			return Widget::preferredSize();
	}

	//____ _childPos() ___________________________________________________________

	Coord Capsule::_childPos( const StaticSlot * pSlot ) const
	{
		return m_skin.contentOfs( m_state );
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( StaticSlot * pSlot )
	{
		_requestRender( m_skin.contentRect( m_size, m_state ));
	}

	//____ _childRequestRender() _________________________________________________

	void Capsule::_childRequestRender( StaticSlot * pSlot, const Rect& rect )
	{
		_requestRender( rect + m_skin.contentOfs( m_state ));
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

	void Capsule::_collectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		if( !m_skin.isEmpty() )
			container.add(Rect(geo, clip));
		else if( slot._widget() )
			OO(slot._widget())->_collectPatches(container, geo, clip);
	}

	//____ _maskPatches() ________________________________________________________

	void Capsule::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		if (!m_skin.isEmpty())
		{
			if( m_skin.isOpaque( clip, geo.size(), m_state ) )
				patches.sub(Rect(geo, clip));
			else if( slot._widget() )
				OO(slot._widget())->_maskPatches(patches, m_skin.contentRect(geo, m_state), clip, blendMode);
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

	void Capsule::_resize( const Size& size )
	{
		Container::_resize( size );

		if ( slot._widget())
		{
			Size sz = size - m_skin.contentPaddingSize();
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
		if (slot.isEmpty())
		{
			package.pSlot = nullptr;
		}
		else
		{
			package.pSlot = &slot;
			package.geo = m_skin.contentRect(Rect(0,0,m_size),m_state);
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
		OO(slot)._setWidget(nullptr);
		_requestRender();
		_requestResize();
	}

	//____ _replaceChild() ____________________________________________________________

	void Capsule::_replaceChild( StaticSlot * pSlot, Widget * pWidget )
	{
		OO(slot)._setWidget(pWidget );

		if (pWidget)
		{
			Size sz = m_size - m_skin.contentPaddingSize();
			OO(pWidget)->_resize(sz);
		}

		_requestRender();
		_requestResize();
	}



} // namespace wg
