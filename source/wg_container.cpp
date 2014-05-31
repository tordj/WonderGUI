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

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

const char WgContainer::CLASSNAME[] = {"Container"};

//____ Constructor _____________________________________________________________

WgContainer::WgContainer() : m_bSiblingsOverlap(true)
{
}

//____ IsInstanceOf() _________________________________________________________

bool WgContainer::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgWidget::IsInstanceOf(pClassName);	
}

//____ ClassName() ____________________________________________________________

const char * WgContainer::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgContainerPtr WgContainer::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgContainerPtr( static_cast<WgContainer*>(pObject.GetRealPtr()) );

	return 0;
}

//____ IsContainer() ______________________________________________________________

bool WgContainer::IsContainer() const
{
	return true;
}

//____ _isPanel() ______________________________________________________________

bool WgContainer::_isPanel() const
{
	return false;
}

//____ _firstWidget() __________________________________________________________

WgWidget * WgContainer::_firstWidget() const 
{ 
	WgHook * p = _firstHook(); 
	if( p ) 
		return p->_widget(); 
	else 
		return 0;
}

//____ _lastWidget() ___________________________________________________________

WgWidget * WgContainer::_lastWidget() const 
{ 
	WgHook * p = _lastHook(); 
	if( p ) 
		return p->_widget(); 
	else 
		return 0; 
}

//____ _findWidget() ____________________________________________________________

WgWidget * WgContainer::_findWidget( const WgCoord& ofs, WgSearchMode mode )
{
	WgRect childGeo;
	WgHook * pHook = _lastHookWithGeo( childGeo );
	WgWidget * pResult = 0;

	while( pHook && !pResult )
	{
		if( pHook->_isVisible() && childGeo.Contains( ofs ) )
		{
			if( pHook->_widget()->IsContainer() )
			{
				pResult = static_cast<WgContainer*>(pHook->_widget())->_findWidget( ofs - childGeo.Pos(), mode );
			}
			else if( mode == WG_SEARCH_GEOMETRY || pHook->_widget()->MarkTest( ofs - childGeo.Pos() ) )
			{
					pResult = pHook->_widget();
			}
		}
		pHook = _prevHookWithGeo( childGeo, pHook );
	}

	// Check against ourselves

	if( !pResult && ( mode == WG_SEARCH_GEOMETRY || MarkTest(ofs)) )
		pResult = this;
		
	return pResult;
}

//____ _focusRequested() _______________________________________________________

bool WgContainer::_focusRequested( WgHook * pBranch, WgWidget * pWidgetRequesting )
{
	WgHook * p = _hook();
	if( p )
	{
		WgRootPanel * r = p->_root();
		if( r )
			return r->_focusRequested( p, pWidgetRequesting );
	}
	return false;
}

//____ _focusReleased() ________________________________________________________

bool WgContainer::_focusReleased( WgHook * pBranch, WgWidget * pWidgetReleasing )
{
	WgHook * p = _hook();
	if( p )
	{
		WgRootPanel * r = p->_root();
		if( r )
			return r->_focusReleased( p, pWidgetReleasing );
	}
	return false;
}


WgModalLayer *  WgContainer::_getModalLayer() const
{
	const WgContainer * p = _parent();

	if( p )
		return p->_getModalLayer();
	else
		return 0;
}

WgPopupLayer * WgContainer::_getPopupLayer() const
{
	const WgContainer * p = _parent();

	if( p )
		return p->_getPopupLayer();
	else
		return 0;
}

//____ _onStateChanged() ______________________________________________________

void WgContainer::_onStateChanged( WgState oldState )
{
	WgWidget::_onStateChanged(oldState);

	if( oldState.IsEnabled() != m_state.IsEnabled() )
	{
		bool bEnabled = m_state.IsEnabled();
		WgWidget * p = _firstWidget();
		while( p )
		{
			p->SetEnabled(bEnabled);
			p = p->_nextSibling();
		}
	}

	if( oldState.IsSelected() != m_state.IsSelected() )
	{
		bool bSelected = m_state.IsSelected();
		WgWidget * p = _firstWidget();
		while( p )
		{
			WgState old = p->m_state;
			p->m_state.SetSelected(bSelected);
			p->_onStateChanged( old );
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
	WidgetRenderContext( WgWidget * pWidget, const WgRect& geo ) : pWidget(pWidget), geo(geo) {}

	WgWidget *	pWidget;
	WgRect		geo;
	WgPatches	patches;
};

void WgContainer::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{

	// We start by eliminating dirt outside our geometry

	WgPatches 	patches( _pPatches->Size() );								// TODO: Optimize by pre-allocating?

	for( const WgRect * pRect = _pPatches->Begin() ; pRect != _pPatches->End() ; pRect++ )
	{
		if( _canvas.IntersectsWith( *pRect ) )
			patches.Push( WgRect(*pRect,_canvas) );
	}


	// Render container itself
	
	for( const WgRect * pRect = patches.Begin() ; pRect != patches.End() ; pRect++ )
		_onRender(pDevice, _canvas, _window, *pRect );
		
	
	// Render children

	WgRect	dirtBounds = patches.Union();
	
	if( m_bSiblingsOverlap )
	{

		// Create WidgetRenderContext's for siblings that might get dirty patches

		std::vector<WidgetRenderContext> renderList;

		WgRect childGeo;
		WgHook * p = _firstHookWithGeo( childGeo );
		while(p)
		{
			WgRect geo = childGeo + _canvas.Pos();

			if( p->_isVisible() && geo.IntersectsWith( dirtBounds ) )
				renderList.push_back( WidgetRenderContext(p->_widget(), geo ) );

			p = _nextHookWithGeo( childGeo, p );
		}

		// Go through WidgetRenderContexts in reverse order (topmost first), push and mask dirt

		for( int i = renderList.size()-1 ; i >= 0 ; i-- )
		{
			WidgetRenderContext * p = &renderList[i];

			p->patches.Push( &patches );

			p->pWidget->_onMaskPatches( patches, p->geo, p->geo, pDevice->GetBlendMode() );		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

			if( patches.IsEmpty() )
				break;
		}

		// Go through WidgetRenderContexts and render the patches

		for( int i = 0 ; i < (int) renderList.size() ; i++ )
		{
			WidgetRenderContext * p = &renderList[i];
			p->pWidget->_renderPatches( pDevice, p->geo, p->geo, &p->patches );
		}

	}
	else
	{
		WgRect childGeo;
		WgHook * p = _firstHookWithGeo( childGeo );

		while(p)
		{
			WgRect canvas = childGeo + _canvas.Pos();
			if( p->_isVisible() && canvas.IntersectsWith( dirtBounds ) )
				p->_widget()->_renderPatches( pDevice, canvas, canvas, &patches );
			p = _nextHookWithGeo( childGeo, p );
		}

	}
}

//____ _onCloneContent() _______________________________________________________

void WgContainer::_onCloneContent( const WgContainer * _pOrg )
{
	m_bSiblingsOverlap 	= _pOrg->m_bSiblingsOverlap;
}

//____ _onCollectPatches() _______________________________________________________

void WgContainer::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	if( m_pSkin )
		container.Add( WgRect( geo, clip ) );
	else
	{
		WgRect childGeo;
		WgHook * p = _firstHookWithGeo( childGeo );

		while(p)
		{
			if( p->_isVisible() )
				p->_widget()->_onCollectPatches( container, childGeo + geo.Pos(), clip );
			p = _nextHookWithGeo( childGeo, p );
		}
	}
}

//____ _onMaskPatches() __________________________________________________________

void WgContainer::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	//TODO: Don't just check IsOpaque() globally, check rect by rect.
	if( (m_bOpaque && blendMode == WG_BLENDMODE_BLEND) || blendMode == WG_BLENDMODE_OPAQUE)
		patches.Sub( WgRect(geo,clip) );
	else
	{
		WgRect childGeo;
		WgHook * p = _firstHookWithGeo( childGeo );

		while(p)
		{
			if( p->_isVisible() )
				p->_widget()->_onMaskPatches( patches, childGeo + geo.Pos(), clip, blendMode );
			p = _nextHookWithGeo( childGeo, p );
		}
	}
}
