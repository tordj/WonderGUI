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

namespace wg
{

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



	 Coord Container::_childGlobalPos( Slot * pSlot ) const
	 {
		 return _childPos(pSlot) + globalPos();
	 }

	 bool Container::_isChildVisible( Slot * pSlot ) const
	 {
		 return true;
	 }

	Rect Container::_childWindowSection( Slot * pSlot ) const
	{
		return Rect( 0,0, _childSize( pSlot ) );
	}

	 Container * Container::_childParent()
	 {
		 return this;
	 }

	 RootPanel * Container::_root()
	 {
		 return m_pHolder ? m_pHolder->_root() : nullptr;
	 }

	 bool Container::_childRequestFocus( Slot * pSlot, Widget * pWidget )
	 {
		 return m_pHolder ? m_pHolder->_childRequestFocus( m_pSlot, pWidget ) : false;
	 }

	 bool Container::_childReleaseFocus( Slot * pSlot, Widget * pWidget )
	 {
		 return m_pHolder ? m_pHolder->_childReleaseFocus( m_pSlot, pWidget ) : false;
	 }

	 void Container::_childRequestInView( Slot * pSlot )
	 {
		 if( m_pHolder )
		 {
			 Rect area( _childPos( pSlot ), _childSize( pSlot ) );
			 m_pHolder->_childRequestInView( m_pSlot, area, area );
		 }
	 }

	 void Container::_childRequestInView( Slot * pSlot, const Rect& mustHaveArea, const Rect& niceToHaveArea )
	 {
		 if( m_pHolder )
		 {
			 Coord pos( _childPos( pSlot ) );
			 m_pHolder->_childRequestInView( m_pSlot, mustHaveArea + pos, niceToHaveArea + pos );
		 }
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
				if (child.pSlot->pWidget->isContainer())
				{
					Widget * pRes = static_cast<Container*>(child.pSlot->pWidget)->_findWidget(ofs - child.geo.pos(), mode);
					if (pRes)
						return pRes;
				}
				else if( mode == SearchMode::Geometry || child.pSlot->pWidget->markTest( ofs - child.geo.pos() ) )
					return child.pSlot->pWidget;
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

	//____ _renderPatches() _____________________________________________________
	// Default implementation for panel rendering patches.
	class WidgetRenderContext
	{
	public:
		WidgetRenderContext() : pWidget(0) {}
		WidgetRenderContext( Widget * pWidget, const Rect& geo ) : pWidget(pWidget), geo(geo) {}

		Widget *	pWidget;
		Rect		geo;
		Patches	patches;
	};

	void Container::_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, const Patches& _patches )
	{
		Patches patches( _patches );

		// Render container itself

		pDevice->setClipList(patches.size(), patches.begin());
		_render(pDevice, _canvas, _window );

		// Render children

		Rect	dirtBounds = pDevice->clipBounds();

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
					renderList.push_back( WidgetRenderContext(child.pSlot->pWidget, geo ) );

				_nextSlotWithGeo( child );
			}

			// Go through WidgetRenderContexts, push and mask dirt

			for (unsigned int i = 0 ; i < renderList.size(); i++)
			{
				WidgetRenderContext * p = &renderList[i];

				p->patches.trimPush( patches, p->geo );
				p->pWidget->_maskPatches( patches, p->geo, p->geo, pDevice->blendMode() );		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

				if( patches.isEmpty() )
					break;
			}

			// Go through WidgetRenderContexts and render the patches in reverse order (topmost child rendered last).

			for (int i = int(renderList.size()) - 1; i >= 0; i--)
			{
				WidgetRenderContext * p = &renderList[i];
				p->pWidget->_renderPatches( pDevice, p->geo, p->geo, p->patches );
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
					Patches childPatches(patches, canvas);
					if( !childPatches.isEmpty() )
						child.pSlot->pWidget->_renderPatches(pDevice, canvas, canvas, childPatches);
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
		if( m_pSkin )
			container.add( Rect( geo, clip ) );
		else
		{
			SlotWithGeo child;
			_firstSlotWithGeo( child );

			while(child.pSlot)
			{
				child.pSlot->pWidget->_collectPatches( container, child.geo + geo.pos(), clip );
				_nextSlotWithGeo( child );
			}
		}
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
				child.pSlot->pWidget->_maskPatches( patches, child.geo + geo.pos(), clip, blendMode );
				_nextSlotWithGeo( child );
			}
		}
	}

} // namespace wg
