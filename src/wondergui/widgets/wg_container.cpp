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

		RectSPX oldInfluence = pOldSkin ? pOldSkin->_influence(m_size, m_scale) : RectSPX(m_size);
		RectSPX newInfluence = pNewSkin ? pNewSkin->_influence(m_size, m_scale) : RectSPX(m_size);
			
		if( oldInfluence != newInfluence )
			_refreshInfluence();

		_requestRender( RectSPX::bounds(oldInfluence,newInfluence) );
		
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

	//____ _childInfluenceChanged() _______________________________________________

	void Container::_childInfluenceChanged( StaticSlot * pSlot, const RectSPX& oldInfluence, const RectSPX& newInfluence )
	{
		_influenceChanged( oldInfluence, newInfluence );
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

	//____ _influence() ________________________________________________________

	RectSPX Container::_influence() const
	{
		return m_influence;
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
		WidgetRenderContext( Widget * pWidget, const RectSPX& geo, const RectSPX& influence ) : pWidget(pWidget), geo(geo), influence(influence) {}

		Widget *	pWidget;
		RectSPX		geo;
		RectSPX		influence;
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
				RectSPX influence = child.pSlot->_widget()->_influence() + child.geo.pos() + _canvas.pos();

				if( influence.isOverlapping( dirtBounds ) )
					renderList.push_back( WidgetRenderContext(child.pSlot->_widget(), child.geo + _canvas.pos(), influence ) );

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

//				RectSPX	clipBounds = RectSPX::overlap(p->influence,_canvas);
				
				p->clipPop = patchesToClipList(pDevice, p->influence, patches);
				p->pWidget->_maskPatches( patches, p->geo, p->influence );		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

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
				RectSPX influence = child.pSlot->_widget()->_influence() + child.geo.pos() + _canvas.pos();

				if (influence.isOverlapping(dirtBounds))
				{
//					RectSPX	clipBounds = RectSPX::overlap(influence,_canvas);

					ClipPopData popData = limitClipList(pDevice, influence );

					if( pDevice->clipListSize() > 0 )
						child.pSlot->_widget()->_render(pDevice, canvas, canvas);

					popClipList( pDevice, popData );
				}
				_nextSlotWithGeo( child );
			}

		}
	}

	//____ _refreshInfluence() ____________________________________________________

	void Container::_refreshInfluence(bool notifyParent)
	{
		// We can't start with skin influence since an empty one would start at 0,0
		// always creating influence from top-left when adding childrens influence.

		RectSPX influence;
				
		SlotWithGeo slot;
		 _firstSlotWithGeo(slot);
		if( slot.pSlot )
		{
			influence = slot.pSlot->_widget()->_influence() + slot.geo.pos();

			_nextSlotWithGeo(slot);
			while( slot.pSlot )
			{
				influence.growToContain(slot.pSlot->_widget()->_influence() + slot.geo.pos());
				_nextSlotWithGeo(slot);
			}
			
			if( !m_skin.isEmpty() )
				influence.growToContain(m_skin.influence({0,0,m_size}, m_scale));
		}
		else if( !m_skin.isEmpty() )
			influence = m_skin.influence({0,0,m_size}, m_scale);
			
		if( influence != m_influence )
		{
			auto oldInfluence = influence;
			m_influence = influence;
			
			if( notifyParent )
				_influenceChanged(oldInfluence, m_influence);
		}
	}

	//____ _influenceAdded() ____________________________________________________

	void Container::_influenceAdded( const RectSPX& childInfluence )
	{
		if( childInfluence.isEmpty() )
			return;
		
		if( !m_influence.contains(childInfluence) )
		{
			auto oldInfluence = m_influence;
			m_influence.growToContain(childInfluence);
			
			_influenceChanged(oldInfluence, m_influence);
		}
	}

	//____ _influenceRemoved() ____________________________________________________

	void Container::_influenceRemoved( const RectSPX& childInfluence )
	{
		if( childInfluence.isEmpty() )
			return;

		if( childInfluence.top() == m_influence.top() || childInfluence.right() == m_influence.right() ||
			childInfluence.bottom() == m_influence.bottom() || childInfluence.left() == m_influence.left() )
		{
			_refreshInfluence();
		}
	}

	//____ _influenceChanged() ____________________________________________________

	void Container::_influenceChanged( const RectSPX& oldInfluence, const RectSPX& newInfluence )
	{
		if( !oldInfluence.isEmpty() &&
		   ((oldInfluence.top() < newInfluence.top() && oldInfluence.top() == m_influence.top()) ||
		    (oldInfluence.right() > newInfluence.right() && oldInfluence.right() == m_influence.right()) ||
		    (oldInfluence.bottom() > newInfluence.bottom() && oldInfluence.bottom() == m_influence.bottom()) ||
		    (oldInfluence.left() < newInfluence.left() && oldInfluence.left() == m_influence.left()) ))
		{
			// We have possibly shrunk in at least one dimension, full refresh needed.
			
			_refreshInfluence();
		}
		else
		{
			// We have not shrunk in any dimension, just possibly grown.

			if( newInfluence.isEmpty() )
				return;
			
			if( !m_influence.contains(newInfluence) )
			{
				auto oldInfluence = m_influence;
				m_influence.growToContain(newInfluence);
				
				_influenceChanged(oldInfluence, m_influence);
			}
		}
	}

	//____ _maskPatches() __________________________________________________________

	void Container::_maskPatches( PatchesSPX& patches, const RectSPX& geo, const RectSPX& clip )
	{
		RectSPX coverage = m_skin.contentRect(geo, m_scale, m_state);
		
		patches.sub( RectSPX::overlap(coverage,clip) );

		if( coverage.contains(_contentRect(geo)) );
			return;										// No need to loop through children, skins coverage contains them all.
		
		SlotWithGeo child;
		_firstSlotWithGeo( child );

		while(child.pSlot)
		{
			child.pSlot->_widget()->_maskPatches( patches, child.geo + geo.pos(), clip );
			_nextSlotWithGeo( child );
		}
	}

} // namespace wg
