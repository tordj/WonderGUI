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

#include <vector>
#include <wg_container.h>
#include <wg_panel.h>
#include <wg_rootpanel.h>
#include <wg_patches.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Container::TYPEINFO = { "Container", &Widget::TYPEINFO };

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Container::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isContainer() ______________________________________________________________

	bool Container::isContainer() const
	{
		return true;
	}

	//____ setSkin() _____________________________________________________________

	void Container::setSkin(Skin* pNewSkin)
	{
		Skin_p pOldSkin = m_skin.get();

		m_skin.set(pNewSkin);

		m_bOpaque = pNewSkin ? pNewSkin->isOpaque(m_state) : false;

		bool bOldSkinOverflows = pOldSkin ? pOldSkin->_overflowsGeo() : false;
		bool bNewSkinOverflows = pNewSkin ? pNewSkin->_overflowsGeo() : false;

		if( bNewSkinOverflows || bOldSkinOverflows )
		{
			RectSPX oldCoverage = pOldSkin ? RectSPX(m_size) : pOldSkin->_coverage(m_size, m_scale);
			RectSPX newCoverage = pNewSkin ? RectSPX(m_size) : pNewSkin->_coverage(m_size, m_scale);
			
			if( oldCoverage != newCoverage )
				_refreshCoverage();

			_requestRender( RectSPX::bounds(oldCoverage,newCoverage) );
		}
		else
			_requestRender();
		
		if (!pNewSkin || !pOldSkin || pNewSkin->_contentBorderSize(m_scale) != pOldSkin->_contentBorderSize(m_scale) ||
			pNewSkin->_defaultSize(m_scale) != pOldSkin->_defaultSize(m_scale) || pNewSkin->_minSize(m_scale) != pOldSkin->_minSize(m_scale))
			_requestResize();
	}

	//____ contains() _________________________________________________________

	bool Container::contains(const Widget * pWidget ) const
	{
		if (!pWidget)
			return false;

		pWidget = pWidget->parent();
		while (pWidget)
		{
			if (pWidget == this)
				return true;
			pWidget = pWidget->parent();
		}

		return false;
	}

	//____ _childLocalToGlobal() _______________________________________________

	RectSPX Container::_childLocalToGlobal(const StaticSlot* pSlot, const RectSPX& rect) const
	{
		return _toGlobal(rect + _slotGeo(pSlot).pos());
	}

	//____ _globalToChildLocal() ________________________________________________

	RectSPX Container::_globalToChildLocal(const StaticSlot* pSlot, const RectSPX& rect) const
	{
		return _toLocal(rect) - _slotGeo(pSlot).pos();

	}

	//____ _globalPtsToChildLocalSpx() ___________________________________________

	RectSPX Container::_globalPtsToChildLocalSpx(const StaticSlot* pSlot, const Rect& rect) const
	{
		RectSPX rectSPX = m_pHolder ? m_pHolder->_globalPtsToChildLocalSpx(m_pSlot, rect) : Util::align(Util::ptsToSpx(rect, m_scale));

		return rectSPX - _slotGeo(pSlot).pos();
	}

	//____ _childLocalSpxToGlobalPts() ___________________________________________

	Rect Container::_childLocalSpxToGlobalPts(const StaticSlot* pSlot, const RectSPX& _rect) const
	{
		RectSPX rect = _rect + _slotGeo(pSlot).pos();
		
		if( m_pHolder )
			return m_pHolder->_childLocalSpxToGlobalPts( m_pSlot, rect );
		else
			return Util::spxToPts(rect, m_scale);
	}

	//____ _isChildVisible() __________________________________________________

	bool Container::_isChildVisible( const StaticSlot * pSlot ) const
	{
		return true;
	}

	//____ _childWindowSection() ____________________________________________________

	RectSPX Container::_childWindowSection( const StaticSlot * pSlot ) const
	{
		return RectSPX( 0,0, pSlot->_widget()->_size() );
	}

	//____ _container() _____________________________________________________________

	Container * Container::_container()
	{
		return this;
	}

	//____ _root() ___________________________________________________________________

	Root * Container::_root()
	{
		return m_pHolder ? m_pHolder->_root() : nullptr;
	}

	//____ _scale() ________________________________________________________________

	int Container::_scale() const
	{
		return m_scale;
	}

	//____ _childOverflowChanged() _______________________________________________

	void Container::_childOverflowChanged( StaticSlot * pSlot )
	{
		_refreshCoverage();
	}

	//____ _childRequestFocus() ______________________________________________________

	bool Container::_childRequestFocus(StaticSlot * pSlot, Widget * pWidget, bool bRaiseWindow )
	{
		return m_pHolder ? m_pHolder->_childRequestFocus( m_pSlot, pWidget, bRaiseWindow ) : false;
	}

	//____ _ChildReleaseFocus() ______________________________________________________

	bool Container::_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget )
	{
		return m_pHolder ? m_pHolder->_childReleaseFocus( m_pSlot, pWidget ) : false;
	}

	//____ _childRequestInView() _____________________________________________________

	void Container::_childRequestInView(StaticSlot * pSlot )
	{
		if( m_pHolder )
		{
			RectSPX area = _slotGeo( pSlot );
			m_pHolder->_childRequestInView( m_pSlot, area, area );
		}
	}

	void Container::_childRequestInView(StaticSlot * pSlot, const RectSPX& mustHaveArea, const RectSPX& niceToHaveArea )
	{
		if( m_pHolder )
		{
			CoordSPX pos( _slotGeo( pSlot ).pos() );
			m_pHolder->_childRequestInView( m_pSlot, mustHaveArea + pos, niceToHaveArea + pos );
		}
	}

	//____ _didAddSlots() _____________________________________________________

	void Container::_didAddSlots(StaticSlot * pSlot, int nb)
	{
		return;				// By default we don't support adding/moving/erasing slots
	}

	//____ _didMoveSlots() ____________________________________________________

	void Container::_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb)
	{
		return;				// By default we don't support adding/moving/erasing slots
	}

	//____ _willEraseSlots() __________________________________________________

	void Container::_willEraseSlots(StaticSlot * pSlot, int nb)
	{
		return;				// By default we don't support adding/moving/erasing slots
	}

	//____ _findWidget() ____________________________________________________________

	Widget * Container::_findWidget( const CoordSPX& ofs, SearchMode mode )
	{
		SlotWithGeo	child;
		_firstSlotWithGeo(child);

		while( child.pSlot )
		{
			if( child.geo.contains( ofs ) )
			{
				Widget * pWidget = child.pSlot->_widget();

				if (pWidget->isContainer())
				{
					Widget * pRes = static_cast<Container*>(pWidget)->_findWidget(ofs - child.geo.pos(), mode);
					if (pRes)
						return pRes;
				}
				else if( mode == SearchMode::Geometry || child.pSlot->_widget()->_markTest( ofs - child.geo.pos() ) )
					return pWidget;
			}
			_nextSlotWithGeo( child );
		}

		// Check against ourselves

		if( mode == SearchMode::Geometry || _markTest(ofs) )
			return this;

		return nullptr;
	}

	//____ _getModalOverlay() _________________________________________________

	ModalOverlay *  Container::_getModalOverlay() const
	{
		const Container * p = _parent();

		if( p )
			return p->_getModalOverlay();
		else
			return 0;
	}

	//____ _getPopupOverlay() _________________________________________________

	PopupOverlay * Container::_getPopupOverlay() const
	{
		const Container * p = _parent();

		if( p )
			return p->_getPopupOverlay();
		else
			return 0;
	}

	//____ _setState() ______________________________________________________

	void Container::_setState( State state )
	{
		State oldState = m_state;

		Widget::_setState(state);						// Doing this call first is an optimization, possibly less dirty rects generated.

		if( oldState.isDisabled() != state.isDisabled() )
		{
			bool bDisabled = state.isDisabled();
			Widget * p = _firstChild();
			while( p )
			{
				p->setDisabled(bDisabled);
				p = p->_nextSibling();
			}
		}

		if( oldState.isSelected() != state.isSelected() )
		{
			bool bSelected = state.isSelected();
			Widget * p = _firstChild();
			while( p )
			{
				State s = p->m_state;
				s.setSelected(bSelected);
				p->_setState( s );
				p = p->_nextSibling();
			}
		}
	}

	//____ _render() _____________________________________________________
	// Default implementation for panel rendering patches.
	class WidgetRenderContext
	{
	public:
		WidgetRenderContext() : pWidget(0) {}
		WidgetRenderContext( Widget * pWidget, const RectSPX& geo, const RectSPX& coverage ) : pWidget(pWidget), geo(geo), coverage(coverage) {}

		Widget *	pWidget;
		RectSPX		geo;
		RectSPX		coverage;
		ClipPopData clipPop;
	};

	void Container::_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window )
	{
		// Render container itself

		m_skin.render(pDevice, _canvas, m_scale, m_state );

		// Render children

		RectSPX	dirtBounds = pDevice->clipBounds();

		if( m_bSiblingsOverlap )
		{
			// Create WidgetRenderContext's for siblings that might get dirty patches

			std::vector<WidgetRenderContext> renderList;

			SlotWithGeo child;
			_firstSlotWithGeo( child );
			while(child.pSlot)
			{
				RectSPX coverage = child.pSlot->_widget()->_coverage() + child.geo.pos() + _canvas.pos();

				if( coverage.isOverlapping( dirtBounds ) )
					renderList.push_back( WidgetRenderContext(child.pSlot->_widget(), child.geo + _canvas.pos(), coverage ) );

				_nextSlotWithGeo( child );
			}

			// Collect dirty patches from gfxDevice

			int nClipRects = pDevice->clipListSize();
			auto pClipRects = pDevice->clipList();
			PatchesSPX patches( nClipRects );

			for( int i = 0 ; i < nClipRects ; i++ )
				patches.push(pClipRects[i]);

			// Go through WidgetRenderContexts, push and mask dirt

			int lastWithDirt = int(renderList.size()) -1;
			for (unsigned int i = 0 ; i < renderList.size(); i++)
			{
				WidgetRenderContext * p = &renderList[i];

				RectSPX	clipBounds = RectSPX::overlap(p->coverage,_canvas);
				
				p->clipPop = patchesToClipList(pDevice, clipBounds, patches);
				p->pWidget->_maskPatches( patches, p->geo, clipBounds, pDevice->blendMode() );		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

				if( patches.isEmpty() )
				{
					lastWithDirt = int(i);	// Prevent poping clipLists that have not been set.
					break;
				}
			}

			// Go through WidgetRenderContexts and render the patches in reverse order (topmost child rendered last).

			for (int i = lastWithDirt; i >= 0; i--)
			{
				WidgetRenderContext * p = &renderList[i];
				p->pWidget->_render( pDevice, p->geo, p->geo );
				popClipList(pDevice,p->clipPop);
			}
		}
		else
		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				RectSPX canvas = child.geo + _canvas.pos();
				RectSPX coverage = child.pSlot->_widget()->_coverage() + child.geo.pos() + _canvas.pos();

				if (canvas.isOverlapping(dirtBounds))
				{
					RectSPX	clipBounds = RectSPX::overlap(coverage,_canvas);

					ClipPopData popData = limitClipList(pDevice, clipBounds );

					if( pDevice->clipListSize() > 0 )
						child.pSlot->_widget()->_render(pDevice, canvas, canvas);

					popClipList( pDevice, popData );
				}
				_nextSlotWithGeo( child );
			}

		}
	}

	//____ _refreshCoverage() ____________________________________________________

	void Container::_refreshCoverage()
	{
		RectSPX coverage;
				
		SlotWithGeo slot;
		 _firstSlotWithGeo(slot);
		if( slot.pSlot )
		{
			coverage = slot.pSlot->_widget()->_coverage() + slot.geo.pos();

			_nextSlotWithGeo(slot);
			while( slot.pSlot )
			{
				coverage.growToContain(slot.pSlot->_widget()->_coverage() + slot.geo.pos());
				_nextSlotWithGeo(slot);
			}
			
			if( !m_skin.isEmpty() )
				coverage.growToContain(m_skin.coverage({0,0,m_size}, m_scale));
		}
		else if( !m_skin.isEmpty() )
			coverage = m_skin.coverage({0,0,m_size}, m_scale);
			
		if( coverage != m_coverage )
		{
			m_coverage = coverage;
			bool bOverflowsGeo = !RectSPX(m_size).contains(coverage);

			// Signal if overflow has changed
			
			if( bOverflowsGeo || m_bOverflowsGeo )
				_overflowChanged();
		}
	}

	//____ _collectPatches() _______________________________________________________

	void Container::_collectPatches( PatchesSPX& container, const RectSPX& geo, const RectSPX& clip )
	{
		if( m_skin.isEmpty() )
		{
			SlotWithGeo child;
			_firstSlotWithGeo(child);

			while (child.pSlot)
			{
				child.pSlot->_widget()->_collectPatches(container, child.geo + geo.pos(), clip);
				_nextSlotWithGeo(child);
			}
		}
		else
			Widget::_collectPatches(container, geo, clip);
	}

	//____ _maskPatches() __________________________________________________________

	void Container::_maskPatches( PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
			patches.sub( RectSPX::overlap(geo,clip) );
		else
		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				child.pSlot->_widget()->_maskPatches( patches, child.geo + geo.pos(), clip, blendMode );
				_nextSlotWithGeo( child );
			}
		}
	}

} // namespace wg
