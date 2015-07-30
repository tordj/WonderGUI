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
#include <new>


#include <wg_msgrouter.h>

const char WgRootPanel::CLASSNAME[] = {"RootPanel"};

static const char	c_hookType[] = {"RootHook"};


//____ Constructor ____________________________________________________________

WgRootPanel::WgRootPanel()
{
	m_bVisible = true;
	m_bHasGeo = false;
	m_geo = WgRect(0,0,0,0);
	m_hook.m_pRoot = this;

}


WgRootPanel::WgRootPanel( const WgGfxDevice_p& pGfxDevice )
{
	m_bVisible = true;
	m_bHasGeo = false;
	m_geo = pGfxDevice->canvasSize();
	m_pGfxDevice = pGfxDevice;
	m_hook.m_pRoot = this;
}

//____ Destructor _____________________________________________________________

WgRootPanel::~WgRootPanel()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgRootPanel::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgObject::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgRootPanel::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgRootPanel_p WgRootPanel::cast( const WgObject_p& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgRootPanel_p( static_cast<WgRootPanel*>(pObject.rawPtr()) );

	return 0;
}


//____ setGfxDevice() _________________________________________________________

bool WgRootPanel::setGfxDevice( const WgGfxDevice_p& pDevice )
{
	m_pGfxDevice = pDevice;

	if( m_pGfxDevice && !m_bHasGeo && m_hook._widget() )
		m_hook._widget()->_onNewSize( m_pGfxDevice->canvasSize() );

	return true;
}

//_____ setGeo() _____________________________________________________________

bool WgRootPanel::setGeo( const WgRect& geo )
{
	if( geo.x == 0 && geo.y == 0 && geo.w == 0 && geo.h == 0 )
		m_bHasGeo = false;
	else
		m_bHasGeo = true;

	m_geo = geo;
	return true;
}

//____ geo() __________________________________________________________________

WgRect WgRootPanel::geo() const
{
	if( m_bHasGeo )
		return m_geo;
	else if( m_pGfxDevice )
	{
		WgRect r( WgCoord(0,0), m_pGfxDevice->canvasSize() );
		if( r.w == 0 || r.h == 0 )
			int x = 0;
		return r;
	}
	else
		return WgRect(0,0,0,0);
}


//____ setWidget() _____________________________________________________________

WgHook_p WgRootPanel::setWidget( const WgWidget_p& pWidget )
{
	if( !pWidget )
		return 0;

	m_hook._setWidget(pWidget.rawPtr());
	m_hook._widget()->_onNewSize(m_geo.size());

	m_hook._widget()->_onCollectPatches( m_dirtyPatches, geo(), geo() );

	return &m_hook;
}

//____ removeWidget() _________________________________________________________

bool WgRootPanel::removeWidget()
{
	if( !m_hook._widget() )
		return false;

	m_hook._setWidget(0);
	m_dirtyPatches.add(m_geo);
	return true;
}

//____ clear() ______________________________________________________

bool WgRootPanel::clear()
{
	return removeWidget();
}


//____ setVisible() ___________________________________________________________

bool WgRootPanel::setVisible( bool bVisible )
{
	if( bVisible != m_bVisible )
	{
		m_bVisible = bVisible;
		addDirtyPatch( geo() );
	}
	return true;
}


//____ render() _______________________________________________________________

bool WgRootPanel::render()
{
	return render( geo() );
}

bool WgRootPanel::render( const WgRect& clip )
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

bool WgRootPanel::beginRender()
{
	if( !m_pGfxDevice || !m_hook._widget() )
		return false;						// No GFX-device or no widgets to render.

	return m_pGfxDevice->beginRender();
}


//____ renderSection() __________________________________________________________

bool WgRootPanel::renderSection( const WgRect& _clip )
{
	if( !m_pGfxDevice || !m_hook._widget() )
		return false;						// No GFX-device or no widgets to render.

	// Make sure we have a vaild clip rectangle (doesn't go outside our geometry and has an area)

	WgRect canvas = geo();
	WgRect clip( _clip, canvas );
	if( clip.w == 0 || clip.h == 0 )
		return false;						// Invalid rect area.

	// Nothing to render if our only child is hidden

	if( !m_bVisible )
		return true;						// Not an error, just hidden.

	// Copy and clip our dirty patches

	WgPatches dirtyPatches( m_dirtyPatches.size() );

	WgRect clipped;
	for( const WgRect * pRect = m_dirtyPatches.begin() ; pRect != m_dirtyPatches.end() ; pRect++ )
	{
		if( clipped.intersection( *pRect, clip ) )
			dirtyPatches.push( clipped );
	}

	// Render the dirty patches recursively

	m_hook._widget()->_renderPatches( m_pGfxDevice.rawPtr(), canvas, canvas, &dirtyPatches );

	return true;
}

//____ endRender() ____________________________________________________________

bool WgRootPanel::endRender( void )
{
	if( !m_pGfxDevice || !m_hook._widget() )
		return false;						// No GFX-device or no widgets to render.

	// Turn dirty patches into update patches
	//TODO: Optimize by just making a swap.

	m_updatedPatches.clear();
	m_updatedPatches.add(&m_dirtyPatches);
	m_dirtyPatches.clear();

	return m_pGfxDevice->endRender();
}


//____ _findWidget() _____________________________________________________________

WgWidget * WgRootPanel::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	if( !geo().contains(ofs) || !m_hook._widget() )
		return 0;

	if( m_hook._widget() && m_hook._widget()->IsContainer() )
		return static_cast<WgContainer*>(m_hook._widget())->_findWidget( ofs, mode );

	return m_hook._widget();
}


//____ _focusRequested() _______________________________________________________

bool WgRootPanel::_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting )
{
	return WgBase::msgRouter()->setKeyboardFocus(pWidgetRequesting);
}

//____ _focusReleased() ________________________________________________________

bool WgRootPanel::_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing )
{
	return WgBase::msgRouter()->setKeyboardFocus(0);
}

///////////////////////////////////////////////////////////////////////////////

WgRootPanel::Hook::~Hook()
{
}

const char * WgRootPanel::Hook::type( void ) const
{
	return classType();
}

const char * WgRootPanel::Hook::classType()
{
	return c_hookType;
}

WgCoord WgRootPanel::Hook::pos() const
{
	return m_pRoot->geo();
}

WgSize WgRootPanel::Hook::size() const
{
	return m_pRoot->geo();
}

WgRect WgRootPanel::Hook::geo() const
{
	return m_pRoot->geo();
}

WgCoord WgRootPanel::Hook::globalPos() const
{
	return m_pRoot->geo();
}

WgRect WgRootPanel::Hook::globalGeo() const
{
	return m_pRoot->geo();
}

WgRootPanel * WgRootPanel::Hook::_root() const
{
	return m_pRoot;
}

void WgRootPanel::Hook::_requestRender()
{
	if( m_pRoot->m_bVisible )
		m_pRoot->addDirtyPatch( geo() );
}

void WgRootPanel::Hook::_requestRender( const WgRect& rect )
{
	if( m_pRoot->m_bVisible )
		m_pRoot->addDirtyPatch( WgRect( pos() + rect.pos(), rect.size() ) );
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

