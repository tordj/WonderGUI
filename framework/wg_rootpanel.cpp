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
#include <new>

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

#include <wg_eventhandler.h>

static const char	c_hookType[] = {"RootHook"};


//____ Constructor ____________________________________________________________

WgRootPanel::WgRootPanel()
{
	m_bVisible = true;
	m_bHasGeo = false;
	m_geo = WgRect(0,0,0,0);
	m_pGfxDevice = 0;
	m_pEventHandler = new WgEventHandler(this);
	m_hook.m_pRoot = this;
	m_scale = WG_SCALE_BASE;

}


WgRootPanel::WgRootPanel( wg::GfxDevice * pGfxDevice )
{
	m_bVisible = true;
	m_bHasGeo = false;
	if( pGfxDevice )
		m_geo = pGfxDevice->canvasSize();
	m_pGfxDevice = pGfxDevice;
	m_pEventHandler = new WgEventHandler(this);
	m_hook.m_pRoot = this;
	m_scale = WG_SCALE_BASE;
}

//____ Destructor _____________________________________________________________

WgRootPanel::~WgRootPanel()
{
	delete m_pEventHandler;
}

//____ SetGfxDevice() _________________________________________________________

bool WgRootPanel::SetGfxDevice( wg::GfxDevice * pDevice )
{
	m_pGfxDevice = pDevice;

	if( m_pGfxDevice && !m_bHasGeo && m_hook.Widget() )
		m_hook.Widget()->_onNewSize( m_pGfxDevice->canvasSize() );

	return true;
}

//_____ SetPixelGeo() _____________________________________________________________

bool WgRootPanel::SetPixelGeo( const WgRect& geo )
{
	if( geo.x == 0 && geo.y == 0 && geo.w == 0 && geo.h == 0 )
		m_bHasGeo = false;
	else
		m_bHasGeo = true;

	m_geo = geo;

	if( m_hook.Widget() )
		m_hook.Widget()->_onNewSize(geo);
	return true;
}

//____ PixelGeo() __________________________________________________________________

WgRect WgRootPanel::PixelGeo() const
{
	if( m_bHasGeo )
		return m_geo;
	else if( m_pGfxDevice )
	{
		WgRect r( WgCoord(0,0), m_pGfxDevice->canvasSize() );
//		if( r.w == 0 || r.h == 0 )
//			int x = 0;
		return r;
	}
	else
		return WgRect(0,0,0,0);
}


//____ SetChild() _____________________________________________________________

bool WgRootPanel::SetChild( WgWidget * pWidget )
{
	if( !pWidget )
		return false;

	m_hook._attachWidget(pWidget);
	m_hook.Widget()->_onNewSize(m_geo.size());

	m_hook.Widget()->_onCollectPatches( m_dirtyPatches, PixelGeo(), PixelGeo() );

	return true;
}

//____ ReleaseChild() _________________________________________________________

WgWidget * WgRootPanel::ReleaseChild()
{
	return m_hook._releaseWidget();
}

WgWidget * WgRootPanel::ReleaseChild( WgWidget * pWidget )
{
	if( pWidget == m_hook.Widget() )
		return ReleaseChild();

	return 0;
}


//____ DeleteChild() __________________________________________________________

bool WgRootPanel::DeleteChild( WgWidget * pWidget )
{
	if( pWidget == m_hook.Widget() )
		return SetChild(0);

	return false;
}

//____ DeleteAllChildren() ______________________________________________________

bool WgRootPanel::DeleteAllChildren()
{
	DeleteChild();
	return true;
}

//____ ReleaseAllChildren() _____________________________________________________

bool WgRootPanel::ReleaseAllChildren()
{
	return ReleaseChild()==0?false:true;
}

//____ SetVisible() ___________________________________________________________

bool WgRootPanel::SetVisible( bool bVisible )
{
	if( bVisible != m_bVisible )
	{
		m_bVisible = bVisible;
		AddDirtyPatch( PixelGeo() );
	}
	return true;
}

//____ SetUpdatedRectOverlay() _________________________________________________

void WgRootPanel::SetUpdatedRectOverlay( wg::Skin * pUpdatedRectOverlay, int afterglowFrames )
{
	m_pUpdatedRectOverlay = pUpdatedRectOverlay;
	m_afterglowFrames = afterglowFrames;
}



//____ Render() _______________________________________________________________

bool WgRootPanel::Render()
{
	return Render( PixelGeo() );
}

bool WgRootPanel::Render( const WgRect& clip )
{
	// This early out hopefully solves the problem with Console1 taking GPU - time on some machines when not visible.
	// It should prevent us from doing anything with the GPU
	if (!m_bVisible)
	{
		m_updatedPatches.clear(); // Probably not necessary, but wanna make sure they don't fill up...
		m_dirtyPatches.clear();   // Probably not necessary, but wanna make sure they don't fill up...
		return true;              // Not an error, just hidden.
	}

	if( !BeginRender() )
		return false;

	if( !RenderSection(clip) )
	{
		EndRender();
		return false;
	}

	if( !EndRender() )
		return false;

	return true;
}

//____ BeginRender() __________________________________________________________

bool WgRootPanel::BeginRender()
{
	if( !m_pGfxDevice || !m_hook.Widget() )
		return false;						// No GFX-device or no widgets to render.

	//

	for( auto& pWidget : m_preRenderCalls )
		pWidget->_preRender();

	m_preRenderCalls.clear();

	//

	if( m_pUpdatedRectOverlay )
	{
		// Remove from afterglow queue patches that are overlapped by our new dirty patches.

		for( std::deque<wg::Patches>::iterator it = m_afterglowRects.begin() ; it != m_afterglowRects.end() ; ++it )
			it->sub(&m_dirtyPatches);

		// Add our new dirty patches to the top of the afterglow queue.


		m_afterglowRects.push_front(wg::Patches());
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

	// Initialize GfxDevice

	return m_pGfxDevice->beginRender();
}


//____ RenderSection() __________________________________________________________

bool WgRootPanel::RenderSection( const WgRect& _clip )
{
	if( !m_pGfxDevice || !m_hook.Widget() )
		return false;						// No GFX-device or no widgets to render.

	// Make sure we have a vaild clip rectangle (doesn't go outside our geometry and has an area)

	WgRect canvas = PixelGeo();
	WgRect clip( _clip, canvas );
	if( clip.w == 0 || clip.h == 0 )
		return false;						// Invalid rect area.

	// Nothing to render if our only child is hidden

	if( !m_bVisible )
		return true;						// Not an error, just hidden.

	// Copy and clip our dirty patches
	// TODO: Optimize when clip rectangle equals canvas

	wg::Patches dirtyPatches( m_dirtyPatches.size() );

	WgRect clipped;
	for( const WgRect * pRect = m_dirtyPatches.begin() ; pRect != m_dirtyPatches.end() ; pRect++ )
	{
		if( clipped.intersection( *pRect, clip ) )
			dirtyPatches.push( clipped );
	}

	// Render the dirty patches recursively

	if( m_dirtyPatches.size() > 0 )
		m_hook.Widget()->_renderPatches( m_pGfxDevice, canvas, canvas, &dirtyPatches );

	// Handle updated rect overlays

	if( m_pUpdatedRectOverlay )
	{
		// Reset cliplist, we don't need it anyway.

		m_pGfxDevice->clearClipList();

		// Render our new overlays

		for( const WgRect * pRect = m_afterglowRects[0].begin() ; pRect != m_afterglowRects[0].end() ; pRect++ )
		{
			m_pUpdatedRectOverlay->_render( m_pGfxDevice, *pRect*4, WgStateEnum::Focused );
		}

		// Render overlays that have turned into afterglow

		if( m_afterglowRects.size() > 1 )
		{
			for( const WgRect * pRect = m_afterglowRects[1].begin() ; pRect != m_afterglowRects[1].end() ; pRect++ )
			{
				m_pUpdatedRectOverlay->_render( m_pGfxDevice, *pRect*4, WgStateEnum::Normal );
			}
		}
	}

	return true;
}

//____ EndRender() ____________________________________________________________

bool WgRootPanel::EndRender( void )
{
	if( !m_pGfxDevice || !m_hook.Widget() )
		return false;						// No GFX-device or no widgets to render.


	// Turn dirty patches into updated patches
	//TODO: Optimize by just making a swap.

	m_updatedPatches.clear();
	m_updatedPatches.add(&m_dirtyPatches);
	m_dirtyPatches.clear();

	return m_pGfxDevice->endRender();
}


//____ FindWidget() _____________________________________________________________

WgWidget * WgRootPanel::FindWidget( const WgCoord& ofs, WgSearchMode mode )
{
	if( !PixelGeo().contains(ofs) || !m_hook.Widget() )
		return 0;

	if( m_hook.Widget() && m_hook.Widget()->IsContainer() )
		return m_hook.Widget()->CastToContainer()->FindWidget( ofs, mode );

	return m_hook.Widget();
}

//____ SetScale() ______________________________________________________________

void WgRootPanel::SetScale( int scale )
{
	if( m_scale != scale )
	{
		m_scale = scale;
		if( m_hook.Widget() )
			m_hook.Widget()->_setScale(m_scale);
	}
}


//____ _focusRequested() _______________________________________________________

bool WgRootPanel::_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting )
{
	if( m_pEventHandler )
		return m_pEventHandler->SetKeyboardFocus(pWidgetRequesting);
	else
		return false;
}

//____ _focusReleased() ________________________________________________________

bool WgRootPanel::_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing )
{
	if( m_pEventHandler )
		return m_pEventHandler->SetKeyboardFocus(0);
	else
		return false;
}

//____ _inViewRequested() ________________________________________________________

void WgRootPanel::_inViewRequested( WgHook * pChild )
{
	// Do nothing.
}

void WgRootPanel::_inViewRequested( WgHook * pChild, const WgRect& mustHaveArea, const WgRect& niceToHaveArea )
{
	// Do nothing.
}


///////////////////////////////////////////////////////////////////////////////

WgRootPanel::Hook::~Hook()
{
}

const char * WgRootPanel::Hook::Type( void ) const
{
	return ClassType();
}

const char * WgRootPanel::Hook::ClassType()
{
	return c_hookType;
}

WgCoord WgRootPanel::Hook::PixelPos() const
{
	return m_pRoot->PixelGeo();
}

WgSize WgRootPanel::Hook::PixelSize() const
{
	return m_pRoot->PixelGeo();
}

WgRect WgRootPanel::Hook::PixelGeo() const
{
	return m_pRoot->PixelGeo();
}

WgCoord WgRootPanel::Hook::ScreenPixelPos() const
{
	return m_pRoot->PixelGeo();
}

WgRect WgRootPanel::Hook::ScreenPixelGeo() const
{
	return m_pRoot->PixelGeo();
}

WgRootPanel* WgRootPanel::Hook::Root() const
{
	return m_pRoot;
}

void WgRootPanel::Hook::_requestRender()
{
	if( m_pRoot->m_bVisible )
		m_pRoot->AddDirtyPatch( PixelGeo() );
}

void WgRootPanel::Hook::_requestRender( const WgRect& rect )
{
	if( m_pRoot->m_bVisible )
		m_pRoot->AddDirtyPatch( WgRect( PixelPos() + rect.pos(), rect.size() ) );
}

void WgRootPanel::Hook::_requestResize()
{
	// Do nothing, root ignores size requests.
}

WgHook * WgRootPanel::Hook::_prevHook() const
{
	return 0;
}

WgHook * WgRootPanel::Hook::_nextHook() const
{
	return 0;
}

WgWidgetHolder * WgRootPanel::Hook::_holder() const
{
	return m_pRoot;
}


WgContainer * WgRootPanel::Hook::_parent() const
{
	return 0;
}

