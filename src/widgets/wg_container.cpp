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

	const char Container::CLASSNAME[] = {"Container"};

	//____ Constructor _____________________________________________________________

	Container::Container() : m_bSiblingsOverlap(true)
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool Container::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Widget::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * Container::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	Container_p Container::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Container_p( static_cast<Container*>(pObject) );

		return 0;
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

	bool Container::_descendantPos( Widget * pDescendant, CoordI& pos )
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

	CoordI Container::_childGlobalPos( const BasicSlot * pSlot ) const
	{
		return _childPos(pSlot) + _globalPos();
	}

	//____ _isChildVisible() ________________________________________________________

	bool Container::_isChildVisible( const BasicSlot * pSlot ) const
	{
		return true;
	}

	//____ _childWindowSection() ____________________________________________________

	RectI Container::_childWindowSection( const BasicSlot * pSlot ) const
	{
		return RectI( 0,0, _access(pSlot)->_size() );
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

	//____ _object() __________________________________________________________________

	Object * Container::_object()
	{
		return this;
	}

	//____ _childRequestFocus() ______________________________________________________

	bool Container::_childRequestFocus( BasicSlot * pSlot, Widget * pWidget )
	{
		return m_pHolder ? m_pHolder->_childRequestFocus( m_pSlot, pWidget ) : false;
	}

	//____ _ChildReleaseFocus() ______________________________________________________

	bool Container::_childReleaseFocus( BasicSlot * pSlot, Widget * pWidget )
	{
		return m_pHolder ? m_pHolder->_childReleaseFocus( m_pSlot, pWidget ) : false;
	}

	//____ _childRequestInView() _____________________________________________________

	void Container::_childRequestInView( BasicSlot * pSlot )
	{
		if( m_pHolder )
		{
			RectI area( _childPos( pSlot ), _access(pSlot)->_size() );
			m_pHolder->_childRequestInView( m_pSlot, area, area );
		}
	}

	void Container::_childRequestInView( BasicSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea )
	{
		if( m_pHolder )
		{
			CoordI pos( _childPos( pSlot ) );
			m_pHolder->_childRequestInView( m_pSlot, mustHaveArea + pos, niceToHaveArea + pos );
		}
	}


	//____ _isPanel() ______________________________________________________________

	bool Container::_isPanel() const
	{
		return false;
	}

	//____ _findWidget() ____________________________________________________________

	Widget * Container::_findWidget( const CoordI& ofs, SearchMode mode )
	{
		SlotWithGeo	child;
		_firstSlotWithGeo(child);

		while( child.pSlot )
		{
			if( child.geo.contains( ofs ) )
			{
				Widget * pWidget = _access(child.pSlot)->_widget();

				if (pWidget->isContainer())
				{
					Widget * pRes = static_cast<Container*>(pWidget)->_findWidget(ofs - child.geo.pos(), mode);
					if (pRes)
						return pRes;
				}
				else if( mode == SearchMode::Geometry || _access(child.pSlot)->_markTest( ofs - child.geo.pos() ) )
					return pWidget;
			}
			_nextSlotWithGeo( child );
		}

		// Check against ourselves

		if( mode == SearchMode::Geometry || _markTest(ofs) )
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
		WidgetRenderContext( Widget * pWidget, const RectI& geo ) : pWidget(pWidget), geo(geo) {}

		Widget *	pWidget;
		RectI		geo;
		ClipPopData clipPop;
	};

	void Container::_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window )
	{
		// Render container itself

		if( m_pSkin )
			m_pSkin->_render(pDevice, _canvas, m_state );

		// Render children

		RectI	dirtBounds = pixelsToRaw( pDevice->clipBounds() );

		if( m_bSiblingsOverlap )
		{
			// Create WidgetRenderContext's for siblings that might get dirty patches

			std::vector<WidgetRenderContext> renderList;

			SlotWithGeo child;
			_firstSlotWithGeo( child );
			while(child.pSlot)
			{
				RectI geo = child.geo + _canvas.pos();

				if( geo.intersectsWith( dirtBounds ) )
					renderList.push_back( WidgetRenderContext(_access(child.pSlot)->_widget(), geo ) );

				_nextSlotWithGeo( child );
			}

			// Collect dirty patches from gfxDevice

			int nClipRects = pDevice->clipListSize();
			auto pClipRects = pDevice->clipList();
			Patches patches( nClipRects );

			for( int i = 0 ; i < nClipRects ; i++ )
				patches.push(pixelsToRaw(pClipRects[i]));

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
				RectI canvas = child.geo + _canvas.pos();
				if (canvas.intersectsWith(dirtBounds))
				{
					ClipPopData popData = limitClipList(pDevice, rawToPixels(canvas) );

					if( pDevice->clipListSize() > 0 )
						_access(child.pSlot)->_widget()->_render(pDevice, canvas, canvas);

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

	void Container::_collectPatches( Patches& container, const RectI& geo, const RectI& clip )
	{
		if( m_pSkin )
			container.add( RectI( geo, clip ) );
		else
		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				_access(child.pSlot)->_widget()->_collectPatches( container, child.geo + geo.pos(), clip );
				_nextSlotWithGeo( child );
			}
		}
	}

	//____ _maskPatches() __________________________________________________________

	void Container::_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode )
	{
		//TODO: Don't just check isOpaque() globally, check rect by rect.
		if( (m_bOpaque && blendMode == BlendMode::Blend) || blendMode == BlendMode::Replace)
			patches.sub( RectI(geo,clip) );
		else
		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				_access(child.pSlot)->_widget()->_maskPatches( patches, child.geo + geo.pos(), clip, blendMode );
				_nextSlotWithGeo( child );
			}
		}
	}

} // namespace wg
