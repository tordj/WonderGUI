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
#include <wg_internal.h>

namespace wg
{
	using namespace Util;

	const TypeInfo Container::TYPEINFO = { "Container", &Widget::TYPEINFO };

	//____ constructor _____________________________________________________________

	Container::Container() : m_bSiblingsOverlap(true)
	{
	}

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

	//____ _descendantPos() _____________________________________________________

	bool Container::_descendantPos( Widget * pDescendant, Coord& pos )
	{
		pos.clear();

		Widget * p = pDescendant;
		while( p != this )
		{
			if( p == nullptr )
				return false;

			auto pHolder = p->_holder();
			pos += pHolder->_childPos( p->_slot() );
			p = pHolder->_container();
		}

		return true;
	}

	//____ _childGlobalPos() _______________________________________________________

	Coord Container::_childGlobalPos( const StaticSlot * pSlot ) const
	{
		return _childPos(pSlot) + globalPos();
	}

	//____ _childDefaultScale() _____________________________________________________

	int Container::_childDefaultScale() const
	{
		return m_scale;
	}

	//____ _isChildVisible() ________________________________________________________

	bool Container::_isChildVisible( const StaticSlot * pSlot ) const
	{
		return true;
	}

	//____ _childWindowSection() ____________________________________________________

	Rect Container::_childWindowSection( const StaticSlot * pSlot ) const
	{
		return Rect( 0,0, pSlot->size() );
	}

	//____ _container() _____________________________________________________________

	Container * Container::_container()
	{
		return this;
	}

	//____ _root() ___________________________________________________________________

	RootPanel * Container::_root()
	{
		return m_pHolder ? m_pHolder->_root() : nullptr;
	}

	//____ _object() __________________________________________________________

	Object * Container::_object()
	{
		return this;
	}

	const Object * Container::_object() const
	{
		return this;
	}

	//____ _childRequestFocus() ______________________________________________________

	bool Container::_childRequestFocus(StaticSlot * pSlot, Widget * pWidget )
	{
		return m_pHolder ? m_pHolder->_childRequestFocus( m_pSlot, pWidget ) : false;
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
			Rect area( _childPos( pSlot ), pSlot->size() );
			m_pHolder->_childRequestInView( m_pSlot, area, area );
		}
	}

	void Container::_childRequestInView(StaticSlot * pSlot, const Rect& mustHaveArea, const Rect& niceToHaveArea )
	{
		if( m_pHolder )
		{
			Coord pos( _childPos( pSlot ) );
			m_pHolder->_childRequestInView( m_pSlot, mustHaveArea + pos, niceToHaveArea + pos );
		}
	}

	//____ _selectSlots() _____________________________________________________

	void Container::_selectSlots(StaticSlot * pSlot, int nb)
	{
		return;				// By default we don't support selection
	}

	//____ _unselectSlots() ___________________________________________________

	void Container::_unselectSlots(StaticSlot * pSlot, int nb)
	{
		return;				// By default we don't support selection
	}

	//____ _repadSlots() ______________________________________________________

	void Container::_repadSlots(StaticSlot * pSlot, int nb, Border padding)
	{
		return;				// By default we don't support padding
	}

	void Container::_repadSlots(StaticSlot * pSlot, int nb, const Border * pPadding)
	{
		return;				// By default we don't support padding
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

	//____ _hideSlots() _______________________________________________________

	void Container::_hideSlots(StaticSlot * pSlot, int nb)
	{
		return;				// By default we don't support hiding
	}

	//____ _unhideSlots() _____________________________________________________

	void Container::_unhideSlots(StaticSlot * pSlot, int nb)
	{
		return;				// By default we don't support hiding
	}

	//____ _isPanel() ______________________________________________________________

	bool Container::_isPanel() const
	{
		return false;
	}

	//____ _findWidget() ____________________________________________________________

	Widget * Container::_findWidget( const Coord& ofs, SearchMode mode )
	{
		SlotWithGeo	child;
		_firstSlotWithGeo(child);

		while( child.pSlot )
		{
			if( child.geo.contains( ofs ) )
			{
				Widget * pWidget = OO(child.pSlot)->_widget();

				if (pWidget->isContainer())
				{
					Widget * pRes = static_cast<Container*>(pWidget)->_findWidget(ofs - child.geo.pos(), mode);
					if (pRes)
						return pRes;
				}
				else if( mode == SearchMode::Geometry || child.pSlot->_widget()->markTest( ofs - child.geo.pos() ) )
					return pWidget;
			}
			_nextSlotWithGeo( child );
		}

		// Check against ourselves

		if( mode == SearchMode::Geometry || markTest(ofs) )
			return this;

		return nullptr;
	}



	ModalLayer *  Container::_getModalLayer() const
	{
		const Container * p = _parent();

		if( p )
			return p->_getModalLayer();
		else
			return 0;
	}

	PopupLayer * Container::_getPopupLayer() const
	{
		const Container * p = _parent();

		if( p )
			return p->_getPopupLayer();
		else
			return 0;
	}

	//____ _setState() ______________________________________________________

	void Container::_setState( State state )
	{
		State oldState = m_state;

		Widget::_setState(state);						// Doing this call first is an optimization, possibly less dirty rects generated.

		if( oldState.isEnabled() != state.isEnabled() )
		{
			bool bEnabled = state.isEnabled();
			Widget * p = _firstChild();
			while( p )
			{
				p->setEnabled(bEnabled);
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
		WidgetRenderContext( Widget * pWidget, const Rect& geo ) : pWidget(pWidget), geo(geo) {}

		Widget *	pWidget;
		Rect		geo;
		ClipPopData clipPop;
	};

	void Container::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		// Render container itself

		OO(skin)._render(pDevice, _canvas, m_state );

		// Render children

		Rect	dirtBounds = Rect::fromPX( pDevice->clipBounds() );

		if( m_bSiblingsOverlap )
		{


			// Create WidgetRenderContext's for siblings that might get dirty patches

			std::vector<WidgetRenderContext> renderList;

			SlotWithGeo child;
			_firstSlotWithGeo( child );
			while(child.pSlot)
			{
				Rect geo = child.geo + _canvas.pos();

				if( geo.intersectsWith( dirtBounds ) )
					renderList.push_back( WidgetRenderContext(OO(child.pSlot)->_widget(), geo ) );

				_nextSlotWithGeo( child );
			}

			// Collect dirty patches from gfxDevice

			int nClipRects = pDevice->clipListSize();
			auto pClipRects = pDevice->clipList();
			Patches patches( nClipRects );

			for( int i = 0 ; i < nClipRects ; i++ )
				patches.push(Rect::fromPX(pClipRects[i]));

			// Go through WidgetRenderContexts, push and mask dirt

			for (unsigned int i = 0 ; i < renderList.size(); i++)
			{
				WidgetRenderContext * p = &renderList[i];

				p->clipPop = patchesToClipList(pDevice, p->geo, patches);
				p->pWidget->_maskPatches( patches, p->geo, p->geo, pDevice->blendMode() );		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

				if( patches.isEmpty() )
					break;
			}

			// Go through WidgetRenderContexts and render the patches in reverse order (topmost child rendered last).

			for (int i = int(renderList.size()) - 1; i >= 0; i--)
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
				Rect canvas = child.geo + _canvas.pos();
				if (canvas.intersectsWith(dirtBounds))
				{
					ClipPopData popData = limitClipList(pDevice, canvas );

					if( pDevice->clipListSize() > 0 )
						OO(child.pSlot)->_widget()->_render(pDevice, canvas, canvas);

					popClipList( pDevice, popData );
				}
				_nextSlotWithGeo( child );
			}

		}
	}

	//____ _cloneContent() _______________________________________________________

	void Container::_cloneContent( const Widget * _pOrg )
	{
		Widget::_cloneContent( _pOrg );

		const Container * pOrg = static_cast<const Container*>(_pOrg);
		m_bSiblingsOverlap 	= pOrg->m_bSiblingsOverlap;
	}

	//____ _collectPatches() _______________________________________________________

	void Container::_collectPatches( Patches& container, const Rect& geo, const Rect& clip )
	{
		if( skin.isEmpty() )
		{
			SlotWithGeo child;
			_firstSlotWithGeo(child);

			while (child.pSlot)
			{
				OO(child.pSlot)->_widget()->_collectPatches(container, child.geo + geo.pos(), clip);
				_nextSlotWithGeo(child);
			}
		}
		else
			container.add( Rect( geo, clip ) );
	}

	//____ _maskPatches() __________________________________________________________

	void Container::_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
			patches.sub( Rect(geo,clip) );
		else
		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				OO(child.pSlot)->_widget()->_maskPatches( patches, child.geo + geo.pos(), clip, blendMode );
				_nextSlotWithGeo( child );
			}
		}
	}

} // namespace wg
