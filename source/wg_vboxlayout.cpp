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

#include <assert.h>
#include <wg_vboxlayout.h>


static const char	c_gizmoType[] = {"VBoxLayout"};


WgVBoxHook::WgVBoxHook( WgGizmo * pGizmo, WgVBoxLayout * pParent ) : WgOrderedHook(pGizmo), m_pParent(pParent)
{
	DoSetGizmo();
}


WgGizmoContainer* WgVBoxHook::_parent() const
{
	return m_pParent;
}

//____ Constructor ____________________________________________________________

WgVBoxLayout::WgVBoxLayout() : m_nBestWidth(0)
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

//____ _castDirtyRect() _______________________________________________________

void WgVBoxLayout::_castDirtyRect( const WgRect& _geo, const WgRect& clip, WgRectLink * pDirtIn, WgRectChain* pDirtOut )
{
	/* FUTURE OPTIMIZATIONS:

		In worst case scenario we might get an awful lot of dirty rectangles if all our children are containers and do masking.
		In theory we could get 2*X+2 dirty rects where X is number of child containers getting dirt, since we might get one
		dirty rect left and right of each child.

		This could be avoided if we somehow could tell WgRectChain to split rects vertically first instead of horizontally OR
		if WgRectChain had some logic to join rects when adding new ones.
	*/

	// We can save the rectangle intact for our own use since no children can be masked by their siblings.

	m_dirt.Add( WgRect( *pDirtIn, clip) );

	// Initialize stuff

	WgVBoxHook * pHook = FirstHook();
	WgRect geo = _hookGeo(pHook) + _geo.pos();

	int y1 = WgMax(clip.y, pDirtIn->y);
	int y2 = WgMin(clip.y+clip.h,pDirtIn->y+pDirtIn->h);
	assert( y1 < y2 );										// Our parent (or earlier) have screwed up, passing us a rectangle outside our clipped geometry.

	// Step forward to first non-hidden container within geometric boundaries.

	while( (geo.y + geo.h < y1 || pHook->Hidden() || !pHook->Gizmo()->IsContainer()) && geo.y < y2 )
	{
		pHook = pHook->NextHook();
		if( !pHook )
		{
			pDirtOut->PushExistingRect(pDirtIn);	// We return the rectangle intact since no children have cut it up.
			return;
		}
		_advanceGeoToHook( geo, pHook );
	}

	// We have found at least one non-hidden container within geometric boundaries
	// and need to call _castDirtyRect() recursively.

	WgRect			orgDirtIn = * pDirtIn;
	WgRectChain		dirtChain1, dirtChain2;
	WgRectChain *	pDirtInChain = &dirtChain1;
	WgRectChain *	pDirtOutChain = &dirtChain2;

	pDirtInChain->PushExistingRect(pDirtIn);

	while( geo.y < y2 )
	{
		// Process dirty rects if child is an unhidden container.

		if( !pHook->Hidden() && pHook->Gizmo()->IsContainer() )
		{
			WgRectLink * pDirt = pDirtInChain->Pop();

			while( pDirt )
			{
				if( pDirt->intersectsWith(geo) )
					pHook->_doCastDirtyRect( geo, WgRect(geo,clip), pDirt, pDirtOutChain );
				else
					pDirtOutChain->PushExistingRect(pDirt);

				pDirt = pDirtInChain->Pop();
			}

			WgSwap( pDirtInChain, pDirtOutChain );			// Our dirt out is next siblings dirt in...
		}

		// Forward to next child

		pHook = pHook->NextHook();
		if( !pHook )
			break;

		_advanceGeoToHook( geo, pHook );
	}

	// pDirtInChain now contains the dirty rects that remains after
	// masking against our children, which we should return.

	if( m_bOpaque )
	{
		pDirtInChain->Clear();
		pDirtInChain->Add(orgDirtIn);
		pDirtInChain->Sub(clip);
	}

	pDirtInChain->Transfer( pDirtOut );
}

//____ _renderDirtyRects() ____________________________________________________

void WgVBoxLayout::_renderDirtyRects( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, Uint8 _layer )
{
	WgVBoxHook * pHook = FirstHook();
	if( !pHook )
		return;

	// Loop through children above window, stop at first visible

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

	WgRectLink * pDirt = m_dirt.pRectList;

	while( pDirt )
	{
		pHook = pFirstVisible;
		geo = firstVisibleGeo;

		// Skip forward to first child whose geo overlaps dirty rect

		while( geo.y + geo.h <= pDirt->y )
		{
			pHook = pHook->NextHook();
			if( !pHook )
				break;
			_advanceGeoToHook( geo, pHook );
		}

		while( geo.y < pDirt->y + pDirt->h )
		{
			if( !pHook->m_bHidden && !pHook->Gizmo()->IsContainer() )
				pHook->DoRender( pDevice, geo, geo, WgRect(geo,*pDirt), _layer );

			pHook = pHook->NextHook();
			if( !pHook )
				break;
			_advanceGeoToHook( geo, pHook );
		}

		pDirt = pDirt->pNext;
	}


	// Loop through our visible container-children, get them to render their dirty rects.

	pHook = pFirstVisible;
	geo = firstVisibleGeo;

	while( geo.y < _window.y + _window.h  )
	{
		if( pHook->Gizmo()->IsContainer() )
			pHook->_doRenderDirtyRects(	pDevice, geo, geo, _layer );

		pHook = pHook->NextHook();
		if( !pHook )
			break;
		_advanceGeoToHook( geo, pHook );
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
	prevHookGeo.h = static_cast<const WgVBoxHook*>(pHook)->m_height;
}

//____ _onResizeRequested() ___________________________________________________

void WgVBoxLayout::_onResizeRequested( WgOrderedHook * _pHook )
{
	WgVBoxHook * pHook = static_cast<WgVBoxHook*>(_pHook);

	// Update BestSize

	WgSize oldBestSize = pHook->m_bestSize;
	pHook->m_bestSize = pHook->Gizmo()->BestSize();

	m_bestSize.h += pHook->m_bestSize.h - oldBestSize.h;

	if( pHook->m_bestSize.w != oldBestSize.w )
	{
		if( pHook->m_bestSize.w > m_bestSize.w )				// Is new best size bigger than previous?
		{
			m_bestSize.w = pHook->m_bestSize.w;
			m_nBestWidth = 1;
		}
		else if( oldBestSize.w == m_bestSize.w )			// Was this our widest child (or one of them)?
		{
			m_nBestWidth--;
			if( m_nBestWidth == 0 )
				_refreshBestWidth();
		}
	}

	// We accept any change to height for current width ourselves

	int newHeight = pHook->Gizmo()->HeightForWidth(m_size.w);

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
	WgVBoxHook * pHook = static_cast<WgVBoxHook*>(pInserted);

	// Update stored BestSize

	pHook->m_bestSize = pHook->Gizmo()->BestSize();

	if( m_bestSize.w == pHook->m_bestSize.w )
	{
		m_nBestWidth++;
	}
	else if( m_bestSize.w < pHook->m_bestSize.w )
	{
		m_bestSize.w = pHook->m_bestSize.w;
		m_nBestWidth = 1;
	}

	// We set Gizmo to same width as ours to start with, our parent will
	// expand us in RequestResize() if it wants to.

	int	height = pHook->Gizmo()->HeightForWidth(m_size.w);
	pHook->DoSetNewSize( WgSize(m_size.w,height) );
	pHook->m_height = height;
	m_size.h = height;

	// Request and handle possible resize.

	RequestResize();

	// Request render on affected part.

	_renderFromChildOnward(pHook);
}

//____ _onGizmoDisappeared() __________________________________________________

void WgVBoxLayout::_onGizmoDisappeared( WgOrderedHook * pToBeRemoved )
{
	WgVBoxHook * pHook = static_cast<WgVBoxHook*>(pToBeRemoved);

	// Update stored BestSize

	m_bestSize.h -= pHook->m_bestSize.h;

	if( m_bestSize.w == pHook->m_bestSize.w )
	{
		m_nBestWidth--;
		if( m_nBestWidth == 0 )
			_refreshBestWidth();
	}

	//

	_renderFromChildOnward(pHook);
	m_size.h -= pHook->m_height;
	pHook->m_height = 0;				// If gizmo is just being hidden it needs to have m_height set to 0.
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
// Updates m_bestSize.w and m_nBestWidth. Relies on m_bestSize of the visible
// hooks to have up-to-date data.

void WgVBoxLayout::_refreshBestWidth()
{
	m_bestSize.w = 0;
	m_nBestWidth = 0;
	WgVBoxHook * pHook = FirstHook();

	while( pHook )
	{
		if( !pHook->m_bHidden && pHook->m_bestSize.w >= m_bestSize.w )
		{
			if( pHook->m_bestSize.w > m_bestSize.w )
			{
				m_bestSize.w = pHook->m_bestSize.w;
				m_nBestWidth = 1;
			}
			else
				m_nBestWidth++;
		}
		pHook = pHook->NextHook();
	}
}

//____ _refreshBestSize() ______________________________________________________
// Refreshes m_bestSize for all visible hooks, m_bestSize and m_nBestWidth with fresh
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
			pHook->m_bestSize = pHook->Gizmo()->BestSize();

			if( pHook->m_bestSize.w > m_bestSize.w )
			{
				m_bestSize.w = pHook->m_bestSize.w;
				m_nBestWidth = 1;
			}
			else if( pHook->m_bestSize.w == m_bestSize.w )
				m_nBestWidth++;

			m_bestSize.h += pHook->m_bestSize.h;
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

WgOrderedHook *  WgVBoxLayout::_newHook()
{
	return new WgVBoxHook( this );
}

