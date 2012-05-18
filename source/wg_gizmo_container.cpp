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
#include <wg_gizmo_container.h>

#ifdef WG_TNG
#	include <wg_patches.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

//____ Constructor _____________________________________________________________

WgGizmoContainer::WgGizmoContainer() : m_bFocusGroup(false), m_bRadioGroup(false), m_bTooltipGroup(false), m_maskOp(WG_MASKOP_RECURSE),
	m_bSiblingsOverlap(true)
{
}



//____ IsGizmo() ______________________________________________________________

bool WgGizmoContainer::IsGizmo() const
{
	return true;
}

//____ IsRoot() _______________________________________________________________

bool WgGizmoContainer::IsRoot() const
{
	return false;
}

//____ CastToContainer() _______________________________________________________

WgGizmoContainer * WgGizmoContainer::CastToContainer()
{
	return this;
}

const WgGizmoContainer * WgGizmoContainer::CastToContainer() const
{
	return this;
}


//____ CastToRoot() ___________________________________________________________

WgRoot * WgGizmoContainer::CastToRoot()
{
	return 0;
}

const WgRoot * WgGizmoContainer::CastToRoot() const
{
	return 0;
}

//____ SetMaskOp() _____________________________________________________________

void WgGizmoContainer::SetMaskOp( WgMaskOp operation )
{
	if( operation != m_maskOp )
	{
		m_maskOp = operation;
		CastToGizmo()->_requestRender();
	}
}

//____ FindGizmo() ____________________________________________________________

WgGizmo * WgGizmoContainer::FindGizmo( const WgCoord& ofs, WgSearchMode mode )
{
	WgRect childGeo;
	WgHook * pHook = _lastHookWithGeo( childGeo );
	WgGizmo * pResult = 0;

	while( pHook && !pResult )
	{
		if( !pHook->Hidden() && childGeo.Contains( ofs ) )
		{
			if( pHook->Gizmo()->IsContainer() )
			{
				pResult = pHook->Gizmo()->CastToContainer()->FindGizmo( ofs - childGeo.Pos(), mode );
			}
			else
			{
				switch( mode )
				{
					case WG_SEARCH_ACTION_TARGET:
					case WG_SEARCH_MARKPOLICY:
						if( pHook->Gizmo()->MarkTest( ofs - childGeo.Pos() ) )
							pResult = pHook->Gizmo();
						break;
					case WG_SEARCH_GEOMETRY:
						pResult = pHook->Gizmo();
						break;
				}
			}
		}
		pHook = _prevHookWithGeo( childGeo, pHook );
	}

	// Return us if search mode is GEOMETRY

	if( !pResult && mode == WG_SEARCH_GEOMETRY )
		pResult = CastToGizmo();

	return pResult;
}

//____ _focusRequested() _______________________________________________________

bool WgGizmoContainer::_focusRequested( WgHook * pBranch, WgGizmo * pGizmoRequesting )
{
	WgHook * p = CastToGizmo()->Hook();
	if( p )
		return p->Parent()->_focusRequested( p, pGizmoRequesting );
	else
		return false;
}

//____ _focusReleased() ________________________________________________________

bool WgGizmoContainer::_focusReleased( WgHook * pBranch, WgGizmo * pGizmoReleasing )
{
	WgHook * p = CastToGizmo()->Hook();
	if( p )
		return p->Parent()->_focusReleased( p, pGizmoReleasing );
	else
		return false;
}


WgGizmoModalLayer *  WgGizmoContainer::_getModalLayer() const
{
	const WgGizmoParent * p = CastToGizmo()->ParentX();

	if( p )
		return p->_getModalLayer();
	else
		return 0;
}

WgGizmoMenuLayer * WgGizmoContainer::_getMenuLayer() const
{
	const WgGizmoParent * p = CastToGizmo()->ParentX();

	if( p )
		return p->_getMenuLayer();
	else
		return 0;
}



//____ _onEnable() _____________________________________________________________

void WgGizmoContainer::_onEnable()
{
	WgGizmo * p = FirstGizmo();
	while( p )
	{
		p->Enable();
		p = p->NextSibling();
	}
}

//____ _onDisable() ____________________________________________________________

void WgGizmoContainer::_onDisable()
{
	WgGizmo * p = FirstGizmo();
	while( p )
	{
		p->Disable();
		p = p->NextSibling();
	}
}

//____ _renderPatches() _____________________________________________________
// Default implementation for container rendering patches.
#ifdef WG_TNG
class GizmoRenderContext
{
public:
	GizmoRenderContext() : pGizmo(0) {}
	GizmoRenderContext( WgGizmo * pGizmo, const WgRect& geo ) : pGizmo(pGizmo), geo(geo) {}

	WgGizmo *	pGizmo;
	WgRect		geo;
	WgPatches	patches;
};

void WgGizmoContainer::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
{

	// We start by eliminating dirt outside our geometry

	WgPatches 	patches( _pPatches->Size() );								// TODO: Optimize by pre-allocating?

	for( const WgRect * pRect = _pPatches->Begin() ; pRect != _pPatches->End() ; pRect++ )
	{
		if( _canvas.IntersectsWith( *pRect ) )
			patches.Push( WgRect(*pRect,_canvas) );
	}

	//

	WgRect	dirtBounds = patches.Union();

	if( m_bSiblingsOverlap )
	{

		// Create GizmoRenderContext's for siblings that might get dirty patches

		std::vector<GizmoRenderContext> renderList;

		WgRect childGeo;
		WgHook * p = _firstHookWithGeo( childGeo );
		while(p)
		{
			WgRect geo = childGeo + _canvas.Pos();

			if( !p->Hidden() && geo.IntersectsWith( dirtBounds ) )
				renderList.push_back( GizmoRenderContext(p->Gizmo(), geo ) );

			p = _nextHookWithGeo( childGeo, p );
		}

		// Go through GizmoRenderContexts in reverse order (topmost first), push and mask dirt

		for( int i = renderList.size()-1 ; i >= 0 ; i-- )
		{
			GizmoRenderContext * p = &renderList[i];

			p->patches.Push( &patches );

			p->pGizmo->_onMaskPatches( patches, p->geo, p->geo, pDevice->GetBlendMode() );		//TODO: Need some optimizations here, grandchildren can be called repeatedly! Expensive!

			if( patches.IsEmpty() )
				break;
		}

		// Go through GizmoRenderContexts and render the patches

		for( int i = 0 ; i < renderList.size() ; i++ )
		{
			GizmoRenderContext * p = &renderList[i];
			p->pGizmo->_renderPatches( pDevice, p->geo, p->geo, &p->patches, _layer );
		}

	}
	else
	{
		WgRect childGeo;
		WgHook * p = _firstHookWithGeo( childGeo );

		while(p)
		{
			WgRect canvas = childGeo + _canvas.Pos();
			if( !p->Hidden() && canvas.IntersectsWith( dirtBounds ) )
				p->Gizmo()->_renderPatches( pDevice, canvas, canvas, &patches, _layer );
			p = _nextHookWithGeo( childGeo, p );
		}

	}
}
#endif


//____ _onAlphaTest() _________________________________________________________

bool WgGizmoContainer::_onAlphaTest( const WgCoord& ofs )
{
	return false;		// By default cointainers have nothing to display themselves.
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoContainer::_onCloneContent( const WgGizmoContainer * _pOrg )
{
	m_bFocusGroup 		= _pOrg->m_bFocusGroup;
	m_bRadioGroup 		= _pOrg->m_bRadioGroup;
	m_bTooltipGroup 	= _pOrg->m_bTooltipGroup;
	m_bSiblingsOverlap 	= _pOrg->m_bSiblingsOverlap;
	m_maskOp 			= _pOrg->m_maskOp;
}

//____ _onCollectPatches() _______________________________________________________
#ifdef WG_TNG
void WgGizmoContainer::_onCollectPatches( WgPatches& container, const WgRect& geo, const WgRect& clip )
{
	WgRect childGeo;
	WgHook * p = _firstHookWithGeo( childGeo );

	while(p)
	{
		if( !p->Hidden() )
			p->Gizmo()->_onCollectPatches( container, childGeo + geo.Pos(), clip );
		p = _nextHookWithGeo( childGeo, p );
	}
}
#endif

//____ _onMaskPatches() __________________________________________________________
#ifdef WG_TNG
void WgGizmoContainer::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	switch( m_maskOp )
	{
		case WG_MASKOP_RECURSE:
		{
			WgRect childGeo;
			WgHook * p = _firstHookWithGeo( childGeo );

			while(p)
			{
				if( !p->Hidden() )
					p->Gizmo()->_onMaskPatches( patches, childGeo + geo.Pos(), clip, blendMode );
				p = _nextHookWithGeo( childGeo, p );
			}
			break;
		}
		case WG_MASKOP_SKIP:
			break;
		case WG_MASKOP_MASK:
			patches.Sub( WgRect(geo,clip) );
			break;
	}
}
#endif
