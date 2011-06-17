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

#include <wg_vboxlayout.h>

static const char	c_gizmoType[] = {"VBoxLayout"};


WgVBoxHook::WgVBoxHook( WgGizmo * pGizmo, WgVBoxLayout * pParent ) : WgOrderedHook(pGizmo), m_pParent(pParent)
{
}


WgGizmoContainer* WgVBoxHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgVBoxLayout::WgVBoxLayout()
{
}

//____ Destructor _____________________________________________________________

WgVBoxLayout::~WgVBoxLayout()
{
}

//____ Type() _________________________________________________________________

const char * WgVBoxLayout::Type() const
{
	return c_gizmoType;
}

//____ GetMyType() ____________________________________________________________

const char * WgVBoxLayout::GetMyType()
{
	return c_gizmoType;
}

//____ HeightForWidth() _______________________________________________________

int WgVBoxLayout::HeightForWidth( int width ) const
{
	if( width == m_size.w )
		return m_size.h;

	int height = 0;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		height += pHook->Gizmo()->HeightForWidth(width);
		pHook = pHook->NextHook();
	}

	return height;
}

//____ WidthForHeight() _______________________________________________________

int WgVBoxLayout::WidthForHeight( int height ) const
{
	return -1; // No recommendation. Should this maybe be BestSize().w instead?
}

//____ MinSize() ______________________________________________________________

WgSize WgVBoxLayout::MinSize() const
{
	WgSize	minBox;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		WgSize minGizmo = pHook->Gizmo()->MinSize();

		minBox.h += minGizmo.h;
		if( minBox.w < minGizmo.w )
			minBox.w = minGizmo.w;

		pHook = pHook->NextHook();
	}

	return minBox;
}

//____ BestSize() _____________________________________________________________

WgSize WgVBoxLayout::BestSize() const
{
	return m_bestSize;
}

//____ MaxSize() ______________________________________________________________

WgSize WgVBoxLayout::MaxSize() const
{
	WgSize	maxBox;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		WgSize maxGizmo = pHook->Gizmo()->BestSize();

		maxBox.h += maxGizmo.h;
		if( maxBox.w < maxGizmo.w )
			maxBox.w = maxGizmo.w;

		pHook = pHook->NextHook();
	}

	return maxBox;
}

//____ _onNewSize() ___________________________________________________________

void WgVBoxLayout::_onNewSize( const WgSize& size )
{
	if( size.w != m_size.w )
	{
		_adaptChildrenToWidth( size.w );
		RequestRender();
	}
}

//____ _onAction() ____________________________________________________________

void WgVBoxLayout::_onAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
}


void WgVBoxLayout::_castDirtyRect( const WgRect& geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
}

//____ _renderDirtyRects() ____________________________________________________

void WgVBoxLayout::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{
	WgVBoxHook * pHook = m_hooks.First();
	if( !pHook )
		return;

	// Loop through dirty rects above window, stop at first visible

	WgRect	geo = _hookGeo(pHook) + _canvas.pos();
	while( geo.y + geo.h < _window.y )
	{
		pHook = pHook->NextHook();
		if( !pHook )
			return;							// 

		_advanceGeoToHook( geo, pHook );
	}

	// Save info about first visible, so we can resume from here

	WgVBoxHook* pFirstVisible	= pHook;
	WgRect		firstVisibleGeo = geo;

	// Loop through our dirty rects and render our (non-container) children.

	WgRectLink * pDirt = m_dirt.First();

	while( pDirt )
	{
		WgVBoxHook* pHook = pFirstVisible;
		WgRect		geo = firstVisibleGeo;

		// Skip forward to first child whose geo 

		while( geo.y + geo.h <= pDirt->y )
		{
			pHook = pHook->NextHook();
			if( !pHook )
				break;
			_advanceGeoToHook( geo, pHook );
		}

		while( geo.y < pDirt->y + pDirt->h )
		{
			if( !pHook->m_bHidden  )


			pHook = pHook->NextHook();
			if( !pHook )
				break;
			_advanceGeoToHook( geo, pHook );
		}

		pDirt = pDirt->Next();
	}


	// Loop through our container-children, get them to render their dirty rects.

	pHook = m_hooks.First();



	while( pHook )
	{
		if( pHook->Gizmo()->IsContainer() )
		{
			pHook->_doRenderDirtyRects(	pDevice, 
		}
		pHook = pHook->NextHook();
	}


}

//____ _hookGeo() _____________________________________________________________

WgRect WgVBoxLayout::_hookGeo( const WgOrderedHook * pHook )
{
	WgVBoxHook * p = FirstHook();
	int ofs = 0;

	while( p != pHook )
	{
		ofs += p->m_height;
		p = p->NextHook();
	}

	return WgRect(0,ofs,m_size.w,p->m_height);
}

//____ _advanceGeoToHook() ____________________________________________________

void  WgVBoxLayout::_advanceGeoToHook( WgRect& prevHookGeo, const WgOrderedHook * pHook )
{
	prevHookGeo.y += prevHookGeo.h;
	prevHookGeo.h = pHook->m_height;
}

//____ _onResizeRequested() ___________________________________________________

void WgVBoxLayout::_onResizeRequested( WgOrderedHook * pHook )
{
	// Update BestSize

	WgSize oldBestSize = pHook->m_bestSize();
	pHook->m_bestSize = pHook->Gizmo()->BestSize();

	m_bestSize.h += pHook->BestSize.h - oldBestSize.h;

	if( pHook->BestSize.w != oldBestSize.w )
	{
		if( pHook->BestSize.w > m_bestSize.w )				// Is new best size bigger than previous?
		{
			m_bestSize.w = pHook->BestSize.w;
			m_bestWidthCounter = 1;
		}
		else if( oldBestSize.w == m_bestSize.w )			// Was this our widest child (or one of them)?
		{
			m_bestWidthCounter--;
			if( m_bestWidthCounter == 0 )
				_recalcBestWidth();
		}
	}

	// We accept any change to height for current width ourselves

	int newHeight = pHook->Gizmo()->HeightForWidth();

	bool bRequestRender = false;
	if( newHeight != m_size.h )
		bRequestRender = true;			// Request render once we have resized.

	m_size.h += newHeight - pHook->m_height;
	pHook->m_height = newHeight;

	// Now we have up-to-date data for HeightForWidth() and BestSize() requests,
	// we notify our parent that we might need a resize.

	RequestResize();

	// Force a render if since we can't know for sure if RequestResize() got us one.

	_renderFromChildOnward( pHook );
}

//____ _onRenderRequested() ___________________________________________________

void  WgVBoxLayout::_onRenderRequested( WgOrderedHook * pHook )
{
	WgRect rect = _hookGeo(pHook);
	if( !rect.empty() )
		RequestRender(rect);
}

void  WgVBoxLayout::_onRenderRequested( WgOrderedHook * pHook, const WgRect& rect )
{
	WgRect hookGeo = _hookGeo(pHook);
	WgRect clippedRect( hookGeo, rect + hookGeo.pos() );

	if( !clippedRect.empty() )
		RequestRender(clippedRect);
}

//____ _onGizmoAppeared() _____________________________________________________

void  WgVBoxLayout::_onGizmoAppeared( WgOrderedHook * pInserted )
{
	// Update stored BestSize

	WgSize bestSize = pInserted->Gizmo()->BestSize();
	pInserted->m_bestWidth = bestSize.w;
	m_bestSize.h += bestSize.h;

	if( m_bestSize.w == bestSize.w )
	{
		m_bestWidthCounter++;
	}
	else if( m_bestSize.w < bestSize.w )
	{
		m_bestSize.w = bestSize.w;
		m_bestWidthCounter = 1;
	}

	// We set Gizmo to same width as ours to start with, our parent will
	// expand us in RequestResize() if it wants to.

	int	height = pInserted->Gizmo()->HeightForWidth(m_size.w);
	pInserted->DoSetNewSize( WgSize(m_size.w,height) );
	pInserted->m_height = height;
	m_size.h = height;

	// Request and handle possible resize.

	RequestResize();

	// Request render on affected part.

	_renderFromChildOnward(pInserted);
}

//____ _onGizmoDisappeared() __________________________________________________

void WgVBoxLayout::_onGizmoDisappeared( WgOrderedHook * pToBeRemoved )
{
	// Update stored BestSize

	WgSize bestSize = pInserted->Gizmo()->BestSize();
	m_bestSize.h -= bestSize.h;

	if( m_bestSize.w == bestSize.w )
	{
		m_bestWidthCounter--;
		if( m_bestWidthCounter == 0 )
			_refreshBestWidth();
	}

	//

	_renderFromChildOnward(pToBeRemoved);
	m_size.h -= pToBeRemoved->m_height;
	pToBeRemoved->m_height = 0;				// If gizmo is just being hidden it needs to have m_height set to 0.
	RequestResize();
}

//____ _onGizmosReordered() ___________________________________________________

void  WgVBoxLayout::_onGizmosReordered()
{
	RequestRender();
}

//____ _adaptChildrenToWidth() ________________________________________________
// Adapts all non-hidden gizmos to our width and updates m_size.

void WgVBoxLayout::_adaptChildrenToWidth( int width )
{
	m_size.h = 0;
	m_size.w = width;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		if( !pHook->m_bHidden )
		{
			int height = pHook->Gizmo()->HeightForWidth( width );
			pHook->DoSetNewSize( WgSize(width,height) );
			pHook->m_height = height;
			m_size.h += height;
		}
		pHook = pHook->NextHook();
	}
}

//____ _refreshBestWidth() ______________________________________________________
// Updates m_bestSize.w and m_nBestWidth. Relies on m_bestWidth of the visible 
// hooks to have up-to-date data.

void WgVBoxLayout::_refreshBestWidth()
{
	m_bestSize.w = 0;
	m_nBestWidth = 0;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		if( !pHook->m_bHidden && pHook->m_bestWidth >= m_bestSize.w )
		{
			if( pHook->m_bestWidth > m_bestSize.w )
			{
				m_bestSize.w = pHook->m_bestWidth;
				m_BestWidth = 1;
			}
			else
				m_nBestWidth++;
		}
		pHook = pHook->NextHook();
	}
}

//____ _refreshBestSize() ______________________________________________________
// Refreshes m_bestWidth for all visible hooks, m_bestSize and m_nBestWidth with fresh
// info straight from the children. 

void WgVBoxLayout::_refreshBestSize()
{
	m_bestSize.clear();
	m_nBestWidth = 0;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		if( !pHook->m_bHidden )
		{
			WgSize best = pHook->Gizmo()->BestSize();

			pHook->m_bestWidth = best.w;

			if( best.w > m_bestSize.w )
			{
				m_bestSize.w = best.w;
				m_BestWidth = 1;
			}
			else if( best.w == m_bestSize.w )
				m_nBestWidth++;

			m_bestSize.h += best.h;
		}
		pHook = pHook->NextHook();
	}
}



//____ _refreshAllGizmos() ____________________________________________________

void  WgVBoxLayout::_refreshAllGizmos()
{
	_refreshBestSize();
	_adaptChildrenToWidth( m_size.w );
	RequestResize();
	RequestRender();
}

//____ _renderFromChildOnward() _______________________________________________

void WgVBoxLayout::_renderFromChildOnward( WgOrderedHook * pHook )
{
	WgRect geo = _hookGeo(pHook);
	geo.h = m_size.h - geo.y;
	RequestRender( geo );
}


//____ _newHook() _____________________________________________________________

WgOrderedHook *  WgVBoxLayout::_newHook(WgGizmo * pGizmo)
{
	return new WgVBoxHook( pGizmo, this );
}

