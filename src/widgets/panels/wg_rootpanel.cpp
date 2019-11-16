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

#include <wg_rootpanel.h>
#include <wg_base.h>
#include <wg_container.h>
#include <wg_boxskin.h>
#include <wg_inputhandler.h>
#include <wg_util.h>

#include <new>


#include <wg_msgrouter.h>


namespace wg
{
	using namespace Util;

	const char RootPanel::CLASSNAME[] = {"RootPanel"};


	//____ Constructor ____________________________________________________________

	RootPanel::RootPanel() : child( &m_child, this )
	{
		m_bVisible = true;
		m_bHasGeo = false;

		m_bDebugMode = false;

		BoxSkin_p pDebugOverlay = BoxSkin::create( 1, Color(255,0,0,128), Color(255,0,0,128) );
		pDebugOverlay->setColors( StateEnum::Focused, Color(255,0,0,128), Color(255,0,0,255) );
		m_pDebugOverlay = pDebugOverlay;
		m_afterglowFrames = 4;
	}

	RootPanel::RootPanel( GfxDevice * pGfxDevice ) : RootPanel()
	{
		if( pGfxDevice )
			m_geo = pixelsToRaw( pGfxDevice->canvasSize() );
		m_pGfxDevice = pGfxDevice;
	}



	//____ Destructor _____________________________________________________________

	RootPanel::~RootPanel()
	{
	}

	//____ isInstanceOf() _________________________________________________________

	bool RootPanel::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return Object::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * RootPanel::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	RootPanel_p RootPanel::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return RootPanel_p( static_cast<RootPanel*>(pObject) );

		return 0;
	}


	//____ setGfxDevice() _________________________________________________________

	bool RootPanel::setGfxDevice( GfxDevice * pDevice )
	{
		m_pGfxDevice = pDevice;

		if( m_pGfxDevice && !m_bHasGeo && m_child.pWidget )
			m_child.pWidget->_resize( pixelsToRaw( m_pGfxDevice->canvasSize() ) );

		m_dirtyPatches.add( _geo() );
		return true;
	}

	//_____ setGeo() _____________________________________________________________

	bool RootPanel::setGeo( const Rect& geo )
	{
		m_geo = qpixToRaw( pixelAligned(geo) );

		if( geo.x == 0 && geo.y == 0 && geo.w == 0 && geo.h == 0 )
			m_bHasGeo = false;
		else
			m_bHasGeo = true;

		return true;
	}

	//____ _geo() __________________________________________________________________

	RectI RootPanel::_geo() const
	{
		if( m_bHasGeo )
			return m_geo;
		else if( m_pGfxDevice )
			return RectI( CoordI(0,0), pixelsToRaw( m_pGfxDevice->canvasSize() ) );
		else
			return RectI(0,0,0,0);
	}

	//____ _setWidget() ____________________________________________________________

	void RootPanel::_setWidget( Slot * pSlot, Widget * pNewWidget )
	{
		if( pSlot->pWidget )
			pSlot->pWidget->_collectPatches( m_dirtyPatches, _geo(), _geo() );

		pSlot->replaceWidget( this, pNewWidget );

		if( pNewWidget )
		{
			pNewWidget->_resize(m_geo.size());
			pNewWidget->_collectPatches( m_dirtyPatches, _geo(), _geo() );
		}
	}

	//____ _object() ____________________________________________________________

	Object * RootPanel::_object()
	{
		return this;
	}
/*
	const Object * RootPanel::_object() const
	{
		return this;
	}
*/
	//____ setVisible() ___________________________________________________________

	bool RootPanel::setVisible( bool bVisible )
	{
		if( bVisible != m_bVisible )
		{
			m_bVisible = bVisible;
			m_dirtyPatches.add( _geo() );
		}
		return true;
	}

	//____ setDebugMode() ______________________________________________________

	void RootPanel::setDebugMode( bool onOff )
	{
		m_bDebugMode = onOff;

		// Make sure to clean up on the screen.

		if( m_bDebugMode == false )
		{
			for( auto it = m_afterglowRects.begin() ; it != m_afterglowRects.end() ; it++ )
				m_dirtyPatches.add( &(*it) );

			m_afterglowRects.clear();
		}
	}


	//____ setDebugOverlay() ____________________________________________________

	void RootPanel::setDebugOverlay( Skin * pOverlaySkin )
	{
		m_pDebugOverlay = pOverlaySkin;
	}

	//____ setDebugAfterglow() ____________________________________________________

	void RootPanel::setDebugAfterglow( int frames )
	{
		limit(frames, 0, 100);
		m_afterglowFrames = frames;
	}


	//____ render() _______________________________________________________________

	bool RootPanel::render()
	{
		return render( _geo() );
	}

	bool RootPanel::render( const Rect& clip )
	{


		if( !beginRender() )
			return false;

		if( !renderSection(clip) )
		{
			endRender();
			return false;
		}

		if( !endRender() )
			return false;

		return true;
	}

	//____ beginRender() __________________________________________________________

	bool RootPanel::beginRender()
	{
		if( !m_pGfxDevice || !m_child.pWidget )
			return false;						// No GFX-device or no widgets to render.

        // Handle preRender calls.
        
        for( auto& pWidget : m_preRenderCalls )
            pWidget->_preRender();
        
        m_preRenderCalls.clear();
        
		// Handle debug overlays.

		if( m_bDebugMode )
		{
			// Remove from afterglow queue patches that are overlapped by our new dirty patches.

			for( std::deque<Patches>::iterator it = m_afterglowRects.begin() ; it != m_afterglowRects.end() ; ++it )
				it->sub(&m_dirtyPatches);

			// Add our new dirty patches to the top of the afterglow queue.


			m_afterglowRects.push_front(Patches());
			m_afterglowRects.front().add(&m_dirtyPatches);

			// Possibly remove overlays from the back, put them into dirty rects for re-render

			while( (int) m_afterglowRects.size() > m_afterglowFrames+1 )
			{
				m_dirtyPatches.add( &m_afterglowRects.back() );
				m_afterglowRects.pop_back();
			}

			// Re-render graphics behind overlays that go from state FOCUSED to NORMAL

			if( m_afterglowRects.size() > 1 )
			{
				m_dirtyPatches.add( &m_afterglowRects[1] );
			}
		}

		// Initialize GFX-device.

		return m_pGfxDevice->beginRender();
	}


	//____ renderSection() __________________________________________________________

	bool RootPanel::renderSection( const Rect& _clip )
	{
		if( !m_pGfxDevice || !m_child.pWidget )
			return false;						// No GFX-device or no widgets to render.

		// Make sure we have a vaild clip rectangle (doesn't go outside our geometry and has an area)

		RectI canvas = _geo();
		RectI clip( qpixToRawAligned( _clip ), canvas );
		if( clip.w == 0 || clip.h == 0 )
			return false;						// Invalid rect area.

		// Nothing to render if our only child is hidden

		if( !m_bVisible )
			return true;						// Not an error, just hidden.

		// Copy and clip our dirty patches

		Patches dirtyPatches( m_dirtyPatches, clip );

		// Render the dirty patches recursively

		if( dirtyPatches.size() > 0 )
		{
			ClipPopData clipPop = patchesToClipList(m_pGfxDevice, dirtyPatches);
			m_child.pWidget->_render( m_pGfxDevice.rawPtr(), canvas, canvas );
			popClipList(m_pGfxDevice, clipPop);
		}

		// Handle updated rect overlays

		if( m_bDebugMode && m_pDebugOverlay )
		{
			// Set clipping rectangle.

			RectI myClip = rawToPixels(clip);

			m_pGfxDevice->setClipList(1, &myClip);

			// Render our new overlays

			for( const RectI * pRect = m_afterglowRects[0].begin() ; pRect != m_afterglowRects[0].end() ; pRect++ )
			{
				m_pDebugOverlay->_render( m_pGfxDevice.rawPtr(), *pRect, StateEnum::Focused );
			}

			// Render overlays that have turned into afterglow

			if( m_afterglowRects.size() > 1 )
			{
				for( const RectI * pRect = m_afterglowRects[1].begin() ; pRect != m_afterglowRects[1].end() ; pRect++ )
				{
					m_pDebugOverlay->_render( m_pGfxDevice.rawPtr(), *pRect, StateEnum::Normal );
				}
			}
		}

		return true;
	}

	//____ endRender() ____________________________________________________________

	bool RootPanel::endRender( void )
	{
		if( !m_pGfxDevice || !m_child.pWidget )
			return false;						// No GFX-device or no widgets to render.

		// Turn dirty patches into update patches
		//TODO: Optimize by just making a swap.

		m_updatedPatches.clear();
		m_updatedPatches.add(&m_dirtyPatches);
		m_dirtyPatches.clear();

		return m_pGfxDevice->endRender();
	}


	//____ _findWidget() _____________________________________________________________

	Widget * RootPanel::_findWidget( const CoordI& ofs, SearchMode mode )
	{
		if( !_geo().contains(ofs) || !m_child.pWidget )
			return 0;

		if(m_child.pWidget &&m_child.pWidget->isContainer() )
			return static_cast<Container*>(m_child.pWidget)->_findWidget( ofs, mode );

		return m_child.pWidget;
	}


	//____ _focusedChild() ______________________________________________________

	Widget * RootPanel::_focusedChild() const
	{
		if( !m_pFocusedChild )
			return m_child.pWidget;

		return m_pFocusedChild.rawPtr();
	}


	//____ _childPos() ________________________________________________________

	CoordI RootPanel::_childPos( Slot * pSlot ) const
	{
		return _geo().pos();
	}

	//____ _childGlobalPos() __________________________________________________

	CoordI RootPanel::_childGlobalPos( Slot * pSlot ) const
	{
		return _geo().pos();
	}

	//____ _childSize() __________________________________________________

	SizeI RootPanel::_childSize( Slot * pSlot ) const
	{
		return _geo().size();
	}

	//____ _isChildVisible() __________________________________________________

	bool RootPanel::_isChildVisible( Slot * pSlot ) const
	{
		return true;
	}

	//____ _childWindowSection() __________________________________________________

	RectI RootPanel::_childWindowSection( Slot * pSlot ) const
	{
		return _geo();
	}

	//____ _childParent() __________________________________________________

	Container * RootPanel::_childParent()
	{
		return nullptr;
	}

	//____ _root() __________________________________________________

	RootPanel * RootPanel::_root()
	{
		return this;
	}

	//____ _childRequestRender() __________________________________________________

	void RootPanel::_childRequestRender( Slot * pSlot )
	{
		if( m_bVisible )
			m_dirtyPatches.add( _geo() );
	}

	void RootPanel::_childRequestRender( Slot * pSlot, const RectI& rect )
	{
		if( m_bVisible )
			m_dirtyPatches.add( RectI( _geo().pos() + rect.pos(), rect.size() ) );
	}

	//____ _childRequestResize() __________________________________________________

	void RootPanel::_childRequestResize( Slot * pSlot )
	{
		pSlot->setSize(m_geo.size());
	}

	//____ _childRequestFocus() __________________________________________________

	bool RootPanel::_childRequestFocus( Slot * pSlot, Widget * pWidget )
	{
		if( pWidget == m_pFocusedChild.rawPtr() )
			return true;

		Widget * pOldFocus = m_pFocusedChild.rawPtr();
		m_pFocusedChild = pWidget;
		return Base::inputHandler()->_focusChanged( this, pOldFocus, pWidget );
	}

	//____ _childReleaseFocus() __________________________________________________

	bool RootPanel::_childReleaseFocus( Slot * pSlot, Widget * pWidget )
	{
		if( pWidget != m_pFocusedChild.rawPtr() )
			return true;					// Never had focus, although widget seems to believe it.

		if( pWidget == m_child.pWidget )
			return false;

		Widget * pOldFocus = m_pFocusedChild.rawPtr();
		m_pFocusedChild =m_child.pWidget;
		return Base::inputHandler()->_focusChanged( this, pOldFocus, m_child.pWidget);
	}

	//____ _childRequestInView() __________________________________________________

	void RootPanel::_childRequestInView( Slot * pSlot )
	{
		// Do nothing, root ignores inView requests.
	}
	void RootPanel::_childRequestInView( Slot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea )
	{
		// Do nothing, root ignores inView requests.
	}

	//____ _prevChild() __________________________________________________

	Widget * RootPanel::_prevChild( const Slot * pSlot ) const
	{
		return nullptr;
	}

	//____ _nextChild() __________________________________________________

	Widget * RootPanel::_nextChild( const Slot * pSlot ) const
	{
		return nullptr;
	}

	//____ _releaseChild() ____________________________________________________

	void RootPanel::_releaseChild(Slot * pSlot)
	{
		_setWidget(pSlot, nullptr);
	}


} // namespace wg
