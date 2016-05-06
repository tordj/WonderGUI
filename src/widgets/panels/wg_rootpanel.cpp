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

namespace wg 
{
	
	const char RootPanel::CLASSNAME[] = {"RootPanel"};
	
	static const char	c_hookType[] = {"RootHook"};
	
	
	//____ Constructor ____________________________________________________________
	
	RootPanel::RootPanel()
	{
		m_bVisible = true;
		m_bHasGeo = false;
		m_geo = Rect(0,0,0,0);
		m_hook.m_pRoot = this;
	
	}
	
	
	RootPanel::RootPanel( const GfxDevice_p& pGfxDevice )
	{
		m_bVisible = true;
		m_bHasGeo = false;
		m_geo = pGfxDevice->canvasSize();
		m_pGfxDevice = pGfxDevice;
		m_hook.m_pRoot = this;
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
	
	RootPanel_p RootPanel::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return RootPanel_p( static_cast<RootPanel*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setGfxDevice() _________________________________________________________
	
	bool RootPanel::setGfxDevice( const GfxDevice_p& pDevice )
	{
		m_pGfxDevice = pDevice;
	
		if( m_pGfxDevice && !m_bHasGeo && m_hook._widget() )
			m_hook._widget()->_setSize( m_pGfxDevice->canvasSize() );
	
		return true;
	}
	
	//_____ setGeo() _____________________________________________________________
	
	bool RootPanel::setGeo( const Rect& geo )
	{
		if( geo.x == 0 && geo.y == 0 && geo.w == 0 && geo.h == 0 )
			m_bHasGeo = false;
		else
			m_bHasGeo = true;
	
		m_geo = geo;
		return true;
	}
	
	//____ geo() __________________________________________________________________
	
	Rect RootPanel::geo() const
	{
		if( m_bHasGeo )
			return m_geo;
		else if( m_pGfxDevice )
		{
			Rect r( Coord(0,0), m_pGfxDevice->canvasSize() );
			if( r.w == 0 || r.h == 0 )
				int x = 0;
			return r;
		}
		else
			return Rect(0,0,0,0);
	}
	
	
	//____ setWidget() _____________________________________________________________
	
	Hook_p RootPanel::setWidget( const Widget_p& pWidget )
	{
		if( !pWidget )
			return 0;
	
		m_hook._setWidget(pWidget.rawPtr());
		m_hook._widget()->_setSize(m_geo.size());
	
		m_hook._widget()->_collectPatches( m_dirtyPatches, geo(), geo() );
	
		return &m_hook;
	}
	
	//____ removeWidget() _________________________________________________________
	
	bool RootPanel::removeWidget()
	{
		if( !m_hook._widget() )
			return false;
	
		m_hook._setWidget(0);
		m_dirtyPatches.add(m_geo);
		return true;
	}
	
	//____ clear() ______________________________________________________
	
	bool RootPanel::clear()
	{
		return removeWidget();
	}
	
	
	//____ setVisible() ___________________________________________________________
	
	bool RootPanel::setVisible( bool bVisible )
	{
		if( bVisible != m_bVisible )
		{
			m_bVisible = bVisible;
			addDirtyPatch( geo() );
		}
		return true;
	}
	
	
	//____ render() _______________________________________________________________
	
	bool RootPanel::render()
	{
		return render( geo() );
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
		if( !m_pGfxDevice || !m_hook._widget() )
			return false;						// No GFX-device or no widgets to render.
	
		return m_pGfxDevice->beginRender();
	}
	
	
	//____ renderSection() __________________________________________________________
	
	bool RootPanel::renderSection( const Rect& _clip )
	{
		if( !m_pGfxDevice || !m_hook._widget() )
			return false;						// No GFX-device or no widgets to render.
	
		// Make sure we have a vaild clip rectangle (doesn't go outside our geometry and has an area)
	
		Rect canvas = geo();
		Rect clip( _clip, canvas );
		if( clip.w == 0 || clip.h == 0 )
			return false;						// Invalid rect area.
	
		// Nothing to render if our only child is hidden
	
		if( !m_bVisible )
			return true;						// Not an error, just hidden.
	
		// Copy and clip our dirty patches
	
		Patches dirtyPatches( m_dirtyPatches.size() );
	
		Rect clipped;
		for( const Rect * pRect = m_dirtyPatches.begin() ; pRect != m_dirtyPatches.end() ; pRect++ )
		{
			if( clipped.intersection( *pRect, clip ) )
				dirtyPatches.push( clipped );
		}
	
		// Render the dirty patches recursively
	
		m_hook._widget()->_renderPatches( m_pGfxDevice.rawPtr(), canvas, canvas, &dirtyPatches );
	
		return true;
	}
	
	//____ endRender() ____________________________________________________________
	
	bool RootPanel::endRender( void )
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
	
	Widget * RootPanel::_findWidget( const Coord& ofs, SearchMode mode )
	{
		if( !geo().contains(ofs) || !m_hook._widget() )
			return 0;
	
		if( m_hook._widget() && m_hook._widget()->isContainer() )
			return static_cast<Container*>(m_hook._widget())->_findWidget( ofs, mode );
	
		return m_hook._widget();
	}
	
	
	//____ _focusRequested() _______________________________________________________
	
	bool RootPanel::_focusRequested( Widget * pWidgetRequesting )
	{
		if( pWidgetRequesting == m_pFocusedChild.rawPtr() )
			return true;

		Widget * pOldFocus = m_pFocusedChild.rawPtr();
		m_pFocusedChild = pWidgetRequesting;
		return Base::inputHandler()->_focusChanged( this, pOldFocus, pWidgetRequesting );
	}
	
	//____ _focusReleased() ________________________________________________________
	
	bool RootPanel::_focusReleased( Widget * pWidgetReleasing )
	{
		if( pWidgetReleasing != m_pFocusedChild.rawPtr() )
			return true;					// Never had focus, although widget seems to believe it.

		if( pWidgetReleasing == m_hook._widget() )
			return false;
			
		Widget * pOldFocus = m_pFocusedChild.rawPtr();
		m_pFocusedChild = m_hook._widget();
		return Base::inputHandler()->_focusChanged( this, pOldFocus, m_hook._widget());
	}

	//____ _focusedChild() ______________________________________________________

	Widget * RootPanel::_focusedChild() const
	{ 
		if( !m_pFocusedChild )
			return m_hook._widget();

		return m_pFocusedChild.rawPtr(); 
	}

	
	///////////////////////////////////////////////////////////////////////////////
	
	RootPanel::MyHook::~MyHook()
	{
	}
	
	const char * RootPanel::MyHook::type( void ) const
	{
		return classType();
	}
	
	const char * RootPanel::MyHook::classType()
	{
		return c_hookType;
	}
	
	Coord RootPanel::MyHook::pos() const
	{
		return m_pRoot->geo();
	}
	
	Size RootPanel::MyHook::size() const
	{
		return m_pRoot->geo();
	}
	
	Rect RootPanel::MyHook::geo() const
	{
		return m_pRoot->geo();
	}
	
	Coord RootPanel::MyHook::globalPos() const
	{
		return m_pRoot->geo();
	}
	
	Rect RootPanel::MyHook::globalGeo() const
	{
		return m_pRoot->geo();
	}
	
	RootPanel * RootPanel::MyHook::_root() const
	{
		return m_pRoot;
	}
	
	void RootPanel::MyHook::_requestRender()
	{
		if( m_pRoot->m_bVisible )
			m_pRoot->addDirtyPatch( geo() );
	}
	
	void RootPanel::MyHook::_requestRender( const Rect& rect )
	{
		if( m_pRoot->m_bVisible )
			m_pRoot->addDirtyPatch( Rect( pos() + rect.pos(), rect.size() ) );
	}
	
	void RootPanel::MyHook::_requestResize()
	{
		// Do nothing, root ignores size requests.
	}
	
	bool RootPanel::MyHook::_requestFocus( Widget * pWidget )
	{
		return m_pRoot->_focusRequested( pWidget );
	}
	
	bool RootPanel::MyHook::_releaseFocus( Widget * pWidget )
	{
		return m_pRoot->_focusReleased( pWidget );
	}
	
	
	Hook * RootPanel::MyHook::_prevHook() const
	{
		return 0;
	}
	
	Hook * RootPanel::MyHook::_nextHook() const
	{
		return 0;
	}
	
	Container * RootPanel::MyHook::_parent() const
	{
		return 0;
	}
	

} // namespace wg
