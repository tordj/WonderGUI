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

#include <wg_gizmo_container.h>
#include <wg_rectchain.h>

//____ Constructor _____________________________________________________________

WgGizmoContainer::WgGizmoContainer() : m_bFocusGroup(false), m_bRadioGroup(false), m_bTooltipGroup(false), m_maskOp(WG_MASKOP_RECURSE)
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


//____ CastToRoot() ___________________________________________________________

WgRoot * WgGizmoContainer::CastToRoot()
{
	return 0;
}

//____ SetMaskOp() _____________________________________________________________

void WgGizmoContainer::SetMaskOp( WgMaskOp operation )
{
	if( operation != m_maskOp )
	{
		m_maskOp = operation;
		CastToGizmo()->RequestRender();
	}
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

//____ _onRender() ____________________________________________________________

void WgGizmoContainer::_onRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{
	// Do nothing, we have nothing to display ourselves.
}

//____ _onAlphaTest() _________________________________________________________

bool WgGizmoContainer::_onAlphaTest( const WgCoord& ofs )
{
	return false;		// We are a cointainer, we have nothing displayed ourselves.
}


//____ _onCollectRects() _______________________________________________________

void WgGizmoContainer::_onCollectRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	WgRect childGeo;
	WgHook * p = _firstHookWithGeo( childGeo );

	while(p)
	{
		p->Gizmo()->_onCollectRects( rects, childGeo, clip );
		p = _nextHookWithGeo( childGeo, p );
	}	
}

//____ _onMaskRects() __________________________________________________________

void WgGizmoContainer::_onMaskRects( WgRectChain& rects, const WgRect& geo, const WgRect& clip )
{
	switch( m_maskOp )
	{
		case WG_MASKOP_RECURSE:
		{
			WgRect childGeo;
			WgHook * p = _firstHookWithGeo( childGeo );

			while(p)
			{
				p->Gizmo()->_onMaskRects( rects, childGeo, clip );
				p = _nextHookWithGeo( childGeo, p );
			}
			break;
		}	
		case WG_MASKOP_SKIP:
			break;
		case WG_MASKOP_MASK:
			rects.Sub( WgRect(geo,clip) );
			break;
	}
}
