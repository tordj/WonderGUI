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


#include <wg_eventhandler.h>

const char WgRootPanel::CLASSNAME[] = {"RootPanel"};

static const char	c_hookType[] = {"RootHook"};


//____ Constructor ____________________________________________________________

WgRootPanel::WgRootPanel()
{
	m_bVisible = true;
	m_bHasGeo = false;
	m_geo = WgRect(0,0,0,0);
	m_pEventHandler = new WgEventHandler(this);
	m_hook.m_pRoot = this;

}


WgRootPanel::WgRootPanel( const WgGfxDevicePtr& pGfxDevice )
{
	m_bVisible = true;
	m_bHasGeo = false;
	m_geo = pGfxDevice->CanvasSize();
	m_pGfxDevice = pGfxDevice;
	m_pEventHandler = new WgEventHandler(this);
	m_hook.m_pRoot = this;
}

//____ Destructor _____________________________________________________________

WgRootPanel::~WgRootPanel()
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgRootPanel::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgRootPanel::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgRootPanelPtr WgRootPanel::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgRootPanelPtr( static_cast<WgRootPanel*>(pObject.GetRealPtr()) );

	return 0;
}


//____ SetGfxDevice() _________________________________________________________

bool WgRootPanel::SetGfxDevice( const WgGfxDevicePtr& pDevice )
{
	m_pGfxDevice = pDevice;

	if( m_pGfxDevice && !m_bHasGeo && m_hook._widget() )
		m_hook._widget()->_onNewSize( m_pGfxDevice->CanvasSize() );

	return true;
}

//_____ SetGeo() _____________________________________________________________

bool WgRootPanel::SetGeo( const WgRect& geo )
{
	if( geo.x == 0 && geo.y == 0 && geo.w == 0 && geo.h == 0 )
		m_bHasGeo = false;
	else
		m_bHasGeo = true;

	m_geo = geo;
	return true;
}

//____ Geo() __________________________________________________________________

WgRect WgRootPanel::Geo() const
{
	if( m_bHasGeo )
		return m_geo;
	else if( m_pGfxDevice )
	{
		WgRect r( WgCoord(0,0), m_pGfxDevice->CanvasSize() );
		if( r.w == 0 || r.h == 0 )
			int x = 0;
		return r;
	}
	else
		return WgRect(0,0,0,0);
}


//____ SetWidget() _____________________________________________________________

WgHookPtr WgRootPanel::SetWidget( const WgWidgetPtr& pWidget )
{
	if( !pWidget )
		return 0;

	m_hook._setWidget(pWidget.GetRealPtr());
	m_hook._widget()->_onNewSize(m_geo.Size());

	m_hook._widget()->_onCollectPatches( m_dirtyPatches, Geo(), Geo() );

	return &m_hook;
}

//____ RemoveWidget() _________________________________________________________

bool WgRootPanel::RemoveWidget()
{
	if( !m_hook._widget() )
		return false;

	m_hook._setWidget(0);
	m_dirtyPatches.Add(m_geo);
	return true;
}

//____ Clear() ______________________________________________________

bool WgRootPanel::Clear()
{
	return RemoveWidget();
}


//____ SetVisible() ___________________________________________________________

bool WgRootPanel::SetVisible( bool bVisible )
{
	if( bVisible != m_bVisible )
	{
		m_bVisible = bVisible;
		AddDirtyPatch( Geo() );
	}
	return true;
}


//____ Render() _______________________________________________________________

bool WgRootPanel::Render()
{
	return Render( Geo() );
}

bool WgRootPanel::Render( const WgRect& clip )
{
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
	if( !m_pGfxDevice || !m_hook._widget() )
		return false;						// No GFX-device or no widgets to render.

	return m_pGfxDevice->BeginRender();
}


//____ RenderSection() __________________________________________________________

bool WgRootPanel::RenderSection( const WgRect& _clip )
{
	if( !m_pGfxDevice || !m_hook._widget() )
		return false;						// No GFX-device or no widgets to render.

	// Make sure we have a vaild clip rectangle (doesn't go outside our geometry and has an area)

	WgRect canvas = Geo();
	WgRect clip( _clip, canvas );
	if( clip.w == 0 || clip.h == 0 )
		return false;						// Invalid rect area.

	// Nothing to render if our only child is hidden

	if( !m_bVisible )
		return true;						// Not an error, just hidden.

	// Copy and clip our dirty patches

	WgPatches dirtyPatches( m_dirtyPatches.Size() );

	WgRect clipped;
	for( const WgRect * pRect = m_dirtyPatches.Begin() ; pRect != m_dirtyPatches.End() ; pRect++ )
	{
		if( clipped.Intersection( *pRect, clip ) )
			dirtyPatches.Push( clipped );
	}

	// Render the dirty patches recursively

	m_hook._widget()->_renderPatches( m_pGfxDevice.GetRealPtr(), canvas, canvas, &dirtyPatches );

	return true;
}

//____ EndRender() ____________________________________________________________

bool WgRootPanel::EndRender( void )
{
	if( !m_pGfxDevice || !m_hook._widget() )
		return false;						// No GFX-device or no widgets to render.

	// Turn dirty patches into update patches
	//TODO: Optimize by just making a swap.

	m_updatedPatches.Clear();
	m_updatedPatches.Add(&m_dirtyPatches);
	m_dirtyPatches.Clear();

	return m_pGfxDevice->EndRender();
}


//____ _findWidget() _____________________________________________________________

WgWidget * WgRootPanel::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	if( !Geo().Contains(ofs) || !m_hook._widget() )
		return 0;

	if( m_hook._widget() && m_hook._widget()->IsContainer() )
		return static_cast<WgContainer*>(m_hook._widget())->_findWidget( ofs, mode );

	return m_hook._widget();
}


//____ _focusRequested() _______________________________________________________

bool WgRootPanel::_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting )
{
	return m_pEventHandler->SetKeyboardFocus(pWidgetRequesting);
}

//____ _focusReleased() ________________________________________________________

bool WgRootPanel::_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing )
{
	return m_pEventHandler->SetKeyboardFocus(0);
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

WgCoord WgRootPanel::Hook::Pos() const
{
	return m_pRoot->Geo();
}

WgSize WgRootPanel::Hook::Size() const
{
	return m_pRoot->Geo();
}

WgRect WgRootPanel::Hook::Geo() const
{
	return m_pRoot->Geo();
}

WgCoord WgRootPanel::Hook::GlobalPos() const
{
	return m_pRoot->Geo();
}

WgRect WgRootPanel::Hook::GlobalGeo() const
{
	return m_pRoot->Geo();
}

WgRootPanel * WgRootPanel::Hook::_root() const
{
	return m_pRoot;
}

void WgRootPanel::Hook::_requestRender()
{
	if( m_pRoot->m_bVisible )
		m_pRoot->AddDirtyPatch( Geo() );
}

void WgRootPanel::Hook::_requestRender( const WgRect& rect )
{
	if( m_pRoot->m_bVisible )
		m_pRoot->AddDirtyPatch( WgRect( Pos() + rect.Pos(), rect.Size() ) );
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

WgContainer * WgRootPanel::Hook::_parent() const
{
	return 0;
}

