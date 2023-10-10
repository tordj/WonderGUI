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

#include <assert.h>
#include <wg_stackpanel.h>
#include <wg_util.h>
#include <wg_patches.h>

#include <wg_dynamicslotvector.impl.h>
#include <wg_panel.impl.h>


namespace wg
{
	using namespace Util;

	template class DynamicSlotVector<StackPanelSlot>;
	template class Panel<StackPanelSlot>;

	const TypeInfo StackPanel::TYPEINFO = { "StackPanel", &Panel::TYPEINFO };
	const TypeInfo StackPanelSlot::TYPEINFO = { "StackPanel::Slot", &DynamicSlot::TYPEINFO };


	//____ Slot::setMargin() _____________________________________________________

	void StackPanelSlot::setMargin(Border margin)
	{
		static_cast<StackPanel*>(DynamicSlot::_holder())->_setSlotMargins(this, 1, margin);
	}

	//____ Slot::setSizePolicy() _________________________________________________

	void StackPanelSlot::setSizePolicy(SizePolicy2D policy)
	{
		static_cast<StackPanel*>(DynamicSlot::_holder())->_setSizePolicy(this, policy);
	}

	//____ Slot::setPlacement() __________________________________________________

	void StackPanelSlot::setPlacement(Placement placement)
	{
		static_cast<StackPanel*>(DynamicSlot::_holder())->_setPlacement(this, placement);
	}

	//____ Slot::_setBlueprint() ____________________________________________________

	bool StackPanelSlot::_setBlueprint( const Blueprint& bp )
	{
		m_margin		= bp.margin;
		m_placement		= bp.placement;
		m_sizePolicy	= bp.sizePolicy;
		m_bVisible		= bp.visible;
		
		return true;
	}


	//____ constructor ____________________________________________________________

	StackPanel::StackPanel() : Panel()
	{
		m_bSiblingsOverlap = true;
	}

	//____ Destructor _____________________________________________________________

	StackPanel::~StackPanel()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StackPanel::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setSlotMargin() _______________________________________________________

	bool StackPanel::setSlotMargin(int index, int amount, Border padding)
	{
		_setSlotMargins( &slots[index], amount, padding);
		return true;
	}

	bool StackPanel::setSlotMargin(iterator beg, iterator end, Border padding)
	{
		_setSlotMargins(beg, int(end - beg), padding);
		return true;
	}

	bool StackPanel::setSlotMargin(int index, int amount, std::initializer_list<Border> padding)
	{
		if( padding.size() < amount )
			return false;
		
		_setSlotMargins( &slots[index], amount, padding.begin());
		return true;
	}

	bool StackPanel::setSlotMargin(iterator beg, iterator end, std::initializer_list<Border> padding)
	{
		if( padding.size() < (end - beg) )
			return false;

		_setSlotMargins(beg, int(end - beg), padding.begin());
		return true;
	}


	//____ _matchingHeight() _______________________________________________________

	spx StackPanel::_matchingHeight( spx width, int scale ) const
	{
		spx height = 0;

		StackPanelSlot * pSlot = slots.begin();
		StackPanelSlot * pEnd = slots.end();

		while( pSlot != pEnd )
		{
			SizeSPX marginSize = Util::align(Util::ptsToSpx(pSlot->m_margin, scale));
			spx h = pSlot->m_pWidget->_matchingHeight(width - marginSize.w, scale) + marginSize.h;
			if( h > height )
				height = h;
			pSlot++;
		}

		return height;
	}

	//____ matchingWidth() _______________________________________________________

	spx StackPanel::_matchingWidth( spx height, int scale ) const
	{
		spx width = 0;

		StackPanelSlot * pSlot = slots.begin();
		StackPanelSlot * pEnd = slots.end();

		while( pSlot != pEnd )
		{
			SizeSPX marginSize = Util::align(Util::ptsToSpx(pSlot->m_margin, scale));
			spx w = pSlot->m_pWidget->_matchingWidth(height - marginSize.h, scale) + marginSize.w;
			if( w > width )
				width = w;
			pSlot++;
		}

		return width;
	}


	//____ _defaultSize() _____________________________________________________________

	SizeSPX StackPanel::_defaultSize(int scale) const
	{
		if (scale == m_scale)
			return m_defaultSize;
		else
			return _calcDefaultSize(scale);
	}

	//____ _resize() ___________________________________________________________

	void StackPanel::_resize( const SizeSPX& size, int scale )
	{
		//TODO: Optimize. If size is same then we only need to update those that have requested resize.

		Panel::_resize(size,scale);
		_updateChildGeo(slots.begin(),slots.end());
	}

	//____ _slotTypeInfo() ________________________________________________________

	const TypeInfo&	StackPanel::_slotTypeInfo(const StaticSlot * pSlot) const
	{
		return StackPanelSlot::TYPEINFO;
	}

	//____ _didAddSlots() ________________________________________________________

	void StackPanel::_didAddSlots( StaticSlot * pSlot, int nb )
	{
		_unhideChildren( (StackPanelSlot*) pSlot, ((StackPanelSlot*)pSlot)+nb );
	}

	//____ _willEraseSlots() ____________________________________________________

	void StackPanel::_willEraseSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (StackPanelSlot*) pSlot, ((StackPanelSlot*)pSlot) + nb);
	}

	//____ _hideSlots() _______________________________________________________

	void StackPanel::_hideSlots( StaticSlot * pSlot, int nb )
	{
		_hideChildren( (StackPanelSlot*) pSlot, ((StackPanelSlot*)pSlot) + nb);
	}

	//____ _unhideSlots() _____________________________________________________

	void StackPanel::_unhideSlots( StaticSlot * pSlot, int nb )
	{
		_unhideChildren( (StackPanelSlot*) pSlot, ((StackPanelSlot*)pSlot) + nb);
	}

	//____ _setSlotMargins() _________________________________________________________

	void StackPanel::_setSlotMargins( StaticSlot * _pSlot, int nb, Border padding )
	{
		StackPanelSlot* pSlot = static_cast<StackPanelSlot*>(_pSlot);

		for (int i = 0; i < nb; i++)
			pSlot[i].m_margin	= padding;

		_updateChildGeo(pSlot,pSlot+nb);
		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		SizeSPX newDefault =_calcDefaultSize(m_scale);
		if (newDefault != m_defaultSize || m_defaultSize != m_size)
			_requestResize();
	}

	void StackPanel::_setSlotMargins(StaticSlot * _pSlot, int nb, const Border * pPaddings)
	{
		StackPanelSlot* pSlot = static_cast<StackPanelSlot*>(_pSlot);

		for (int i = 0; i < nb; i++)
			pSlot[i].m_margin	= *pPaddings++;

		_updateChildGeo(pSlot,pSlot+nb);
		_requestRender();				// This is needed here since children might have repositioned.
										//TODO: Optimize! Only render what really is needed due to changes.

		SizeSPX newDefault = _calcDefaultSize(m_scale);
		if (newDefault != m_defaultSize || m_defaultSize != m_size)
			_requestResize();
	}

	//____ _setSizePolicy() ___________________________________________________

	void StackPanel::_setSizePolicy(StackPanelSlot * pSlot, SizePolicy2D policy)
	{
		if (policy != pSlot->m_sizePolicy)
		{
			RectSPX oldGeo = pSlot->m_geo;
			pSlot->m_sizePolicy = policy;
			RectSPX newGeo = _childGeo(pSlot);

			if (newGeo != oldGeo)
			{
				_requestRender(oldGeo);
				_requestRender(newGeo);
				pSlot->_setSize(newGeo.size(), m_scale);
				pSlot->m_geo = newGeo;
			}
		};
	}

	//____ _setPlacement() ________________________________________________________

	void StackPanel::_setPlacement(StackPanelSlot * pSlot, Placement placement)
	{
		if (placement != pSlot->m_placement)
		{
			RectSPX oldGeo = pSlot->m_geo;
			pSlot->m_placement = placement;
			RectSPX newGeo = _childGeo(pSlot);

			if (newGeo != oldGeo)
			{
				_requestRender(oldGeo);
				_requestRender(newGeo);
				pSlot->_setSize(newGeo.size(), m_scale);
				pSlot->m_geo = newGeo;
			}
		};
	}

	//____ _didMoveSlots() ________________________________________________________

	void StackPanel::_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb )
	{
		_requestRender();	//TODO: Optimize! Only re-render what might have changed.
	}

	//____ _childRequestRender() ______________________________________________

	void StackPanel::_childRequestRender( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<StackPanelSlot*>(_pSlot);

		_childRequestRender( pSlot, pSlot->m_geo );
	}

	void StackPanel::_childRequestRender( StaticSlot * _pSlot, const RectSPX& _rect )
	{
		auto pSlot = static_cast<StackPanelSlot*>(_pSlot);

		if( !pSlot->m_bVisible )
			return;

		// Put our rectangle into patches

		RectSPX rect = _rect + pSlot->m_geo.pos();

		PatchesSPX patches;
		patches.add( rect );

		// Remove portions of patches that are covered by opaque upper siblings

		for( auto pCover = slots.begin() ; pCover < pSlot ; pCover++ )
		{
			RectSPX geo = pCover->m_geo;
			if( pCover->m_bVisible && geo.isOverlapping( rect ) )
				pCover->_widget()->_maskPatches( patches, geo, RectSPX(0,0,65536,65536 ), _getBlendMode() );
		}

		// Make request render calls

		for( const RectSPX * pRect = patches.begin() ; pRect < patches.end() ; pRect++ )
			_requestRender( * pRect );
	}

	//____ _childRequestResize() ______________________________________________

	void StackPanel::_childRequestResize( StaticSlot * _pSlot )
	{
		auto pSlot = static_cast<StackPanelSlot*>(_pSlot);

		SizeSPX newDefault = _calcDefaultSize(m_scale);

		if(newDefault != m_defaultSize || m_defaultSize != m_size )
		{
			m_defaultSize = newDefault;
			_requestResize();
		}
		else
		{
			RectSPX oldGeo = pSlot->m_geo;
			RectSPX newGeo = _childGeo(pSlot);

			if (oldGeo != newGeo)
			{
				pSlot->m_geo = newGeo;
				pSlot->_setSize(newGeo.size(), m_scale);
				_requestRender(oldGeo);
				_requestRender(newGeo);
			}
		}

	}

	//____ _releaseChild() ____________________________________________________

	void StackPanel::_releaseChild(StaticSlot * pSlot)
	{
		_willEraseSlots(pSlot, 1);
		slots.erase(static_cast<StackPanelSlot*>(pSlot));
	}

	//____ _replaceChild() ____________________________________________________

	void StackPanel::_replaceChild(StaticSlot * pSlot, Widget * pNewChild)
	{
		static_cast<StackPanelSlot*>(pSlot)->_setWidget(pNewChild);
		_childRequestRender(pSlot);
		_childRequestResize(pSlot);
	}


	//____ _unhideChildren() _____________________________________________________

	void StackPanel::_unhideChildren( StackPanelSlot * pSlot, StackPanelSlot * pEnd )
	{
		bool	bRequestResize = false;

		while( pSlot != pEnd )
		{
			// Update m_defaultSize

			if( !pSlot->m_bVisible )
			{
				SizeSPX defaultSize = pSlot->_defaultSizeWithMargin(m_scale);

				if(defaultSize.w > m_defaultSize.w )
				{
					m_defaultSize.w = defaultSize.w;
					bRequestResize = true;
				}
				if(defaultSize.h > m_defaultSize.h )
				{
					m_defaultSize.h = defaultSize.h;
					bRequestResize = true;
				}

				// Make unhidden Widgets visible, adapt them to our size and scale and force a render.

				pSlot->m_bVisible = true;
				
				RectSPX geo = _childGeo(pSlot);
				pSlot->m_geo = geo;
				pSlot->_setSize(geo.size(),m_scale);
				_childRequestRender(pSlot);
			}
			pSlot++;
		}

		if( bRequestResize )
			_requestResize();
	}

	//____ _hideChildren() __________________________________________________

	void StackPanel::_hideChildren( StackPanelSlot * pSlot, StackPanelSlot * pEnd )
	{
		bool	bRequestResize = false;

		// Get dirty rectangles for all visible sections of widgets to be removed.

		while( pSlot != pEnd )
		{
			if( pSlot->m_bVisible )
			{
				_childRequestRender( pSlot );
				pSlot->m_bVisible = false;
			}
			pSlot++;
		}
		// Update m_defaultSize

		SizeSPX	defaultSize;
		StackPanelSlot * p = slots.begin();

		while( p != slots.end() )
		{
			if( p->m_bVisible )
			{
				SizeSPX sz = p->_defaultSizeWithMargin(m_scale);
				if( sz.w > defaultSize.w )
					defaultSize.w = sz.w;
				if( sz.h > defaultSize.h )
					defaultSize.h = sz.h;
			}
			p++;
		}

		if( defaultSize != m_defaultSize )
			bRequestResize = true;

		m_defaultSize = defaultSize;

		// Check if removal might affect height for current width
/*
		int height = pToBeRemoved->_widget()->matchingHeight(m_size.w);
		if( height >= m_size.h )
			bRequestResize = true;
*/
		//

		if( bRequestResize )
			_requestResize();
	}


	//____ _refreshAllWidgets() ____________________________________________________
/*
	void StackPanel::_refreshAllWidgets()
	{
		_refreshDefaultSize();
		_adaptChildrenToSize();
		_requestRender();
	}
*/

	//____ _calcDefaultSize() _____________________________________________________

	SizeSPX StackPanel::_calcDefaultSize(int scale) const
	{
		SizeSPX	defaultSize;

		auto * pSlot = slots.begin();
		auto * pEnd = slots.end();

		while( pSlot != pEnd )
		{
			if( pSlot->m_bVisible )
			{
				SizeSPX sz = pSlot->_defaultSizeWithMargin(scale);
				if( sz.w > defaultSize.w )
					defaultSize.w = sz.w;
				if( sz.h > defaultSize.h )
					defaultSize.h = sz.h;
			}
			pSlot++;
		}

		return defaultSize;
	}

	//____ _updateChildGeo() ___________________________________________________________

	void StackPanel::_updateChildGeo(StackPanelSlot* pSlot, StackPanelSlot* pEnd)
	{
		while( pSlot != pEnd )
		{
			if (pSlot->m_bVisible)
			{
				RectSPX geo = _childGeo(pSlot);
				pSlot->m_geo = geo;
				pSlot->_setSize( geo.size(),m_scale );
			}
			pSlot++;
		}
	}

	//____ _childGeo() ___________________________________________________________

	RectSPX StackPanel::_childGeo( const StackPanelSlot * pSlot ) const
	{
		RectSPX base = RectSPX( m_size ) - align(ptsToSpx(pSlot->m_margin,m_scale));

		if( base.w <= 0 || base.h <= 0 )
			return RectSPX();

		switch( pSlot->m_sizePolicy )
		{
			default:
		case SizePolicy2D::Original:
			{
				SizeSPX	size = pSlot->_widget()->_defaultSize(m_scale);
				RectSPX geo = Util::placementToRect( pSlot->m_placement, base, size );

				if( geo.w > base.w )
				{
					geo.x = 0;
					geo.w = base.w;
				}

				if( geo.h > base.h )
				{
					geo.y = 0;
					geo.h = base.h;
				}
				return geo;
			}
			case SizePolicy2D::Stretch:
			{
				return base;
			}
			case SizePolicy2D::Scale:
			{
				SizeSPX	orgSize = pSlot->_widget()->_defaultSize(m_scale);
				SizeSPX	size;

				float	fracX = orgSize.w / (float) base.w;
				float	fracY = orgSize.h / (float) base.h;

				if( fracX > fracY )
				{
					size.w = base.w;
					size.h = int (orgSize.h / fracX);
				}
				else
				{
					size.h = base.h;
					size.w = (int) (orgSize.w / fracY);
				}

				return Util::placementToRect( pSlot->m_placement, base, size );
			}
		}
	}




} // namespace wg
