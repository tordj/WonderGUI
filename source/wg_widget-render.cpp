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

#include	<wg_widget.h>

/*
	This file contains all rendering-related widget-stuff.

	Widgets request rendering by calling RequestRender() and supplying
	doMyOwnRender(). They and the optional ClearRenderQueue(), provide
	the only interfaces to rendering and everything else is handled by the
	functions in this file.

	Some control/hints/optimizations are allowed by setting the following
	variables in the widget:

	m_bOpaque
	m_bRendersAll
	m_bRenderOne

*/


//____ WgWidget::RequestRender() _________________________________________________

void WgWidget::RequestRender( int _x, int _y, int _w, int _h, WgWidget * _pMaskBranch, bool _bRenderParentToo )
{
	WgDirtyRectObj	dirtObj;

	dirtObj.Add( _x, _y, _w, _h );

	// Possibly mask...

	if( _pMaskBranch && dirtObj.pRectList )
	{
		int	myAbsX = 0, myAbsY = 0;
		int	maskAbsX = -_pMaskBranch->m_geo.x, maskAbsY = -_pMaskBranch->m_geo.y;

		Local2abs( &myAbsX, &myAbsY );
		_pMaskBranch->Local2abs( &maskAbsX, &maskAbsY );

		myAbsX -= maskAbsX;
		myAbsY -= maskAbsY;
		_pMaskBranch->MaskAgainstBranch( &dirtObj, -myAbsX, -myAbsY );
	}

	// Add the dirty rectangles

	if( m_bRendersAll )
	{
		// Render all of us unless rectangle is totally covered...

		if( dirtObj.pRectList )
		{
			m_dirtyRects.Add( 0, 0, m_geo.w, m_geo.h );

			// Mark dirty rectangle on parent recursively if we aren't completely opaque and
			// _bRenderParentToo isn't set to false (true is default).
			// Also checks m_bRenderedHere since there's no need to update what's behind
			// if it hasn't been covered by us yet.

			if( _bRenderParentToo && !m_bOpaque && m_bRenderedHere && m_pParent != 0 )
				m_pParent->RequestRender( m_geo.x, m_geo.y, m_geo.w, m_geo.h );
		}
	}
	else
	{
		// Mark dirty rectangles on parent recursively if we aren't completely opaque and
		// _bRenderParentToo isn't set to false (true is default).
		// Also checks m_bRenderedHere since there's no need to update what's behind
		// if it hasn't been covered by us yet.

		if( _bRenderParentToo && !m_bOpaque && m_bRenderedHere && m_pParent != 0 )
		{
			WgDirtyRect	* pRect = dirtObj.pRectList;
			while( pRect )
			{
				m_pParent->RequestRender( m_geo.x + pRect->x, m_geo.y + pRect->y, pRect->w, pRect->h );
				pRect = pRect->pNext;
			}
		}

		// Transfer these rectangles to our real list.

		dirtObj.Transfer( &m_dirtyRects );
	}
}

//____ WgWidget::MaskAgainstBranch() ____________________________________________

void WgWidget::MaskAgainstBranch( WgDirtyRectObj * _pDirtObj, int _ofsX, int _ofsY )
{
	if( m_bRenderedHere && !m_bHidden )
	{
  		if( m_bOpaque )
  			_pDirtObj->Sub( m_geo.x + _ofsX, m_geo.y + _ofsY, m_geo.w, m_geo.h );
  		else
  		{
				// We can't mask since we're not opaque, but we might have children that are...

  			if( m_pFirstChild )
  				m_pFirstChild->MaskAgainstBranch( _pDirtObj, _ofsX + m_geo.x, _ofsY + m_geo.y );
  		}
	}

	// Call siblings recursively, if there are dirty rectangles left.

	if( m_pNextSibling && _pDirtObj->pRectList )
		m_pNextSibling->MaskAgainstBranch( _pDirtObj, _ofsX, _ofsY );

	return;
}


//____ WgWidget::RenderRecursively() ____________________________________________

void WgWidget::RenderRecursively(Uint8 _layer)
{

	// Do my own rendering


	if( m_dirtyRects.pRectList && (_layer & m_layer) != 0)
	{
   	WgRect		window;

   	window.x = 0;
   	window.y = 0;
   	window.w = m_geo.w;
   	window.h = m_geo.h;
   	Local2abs( &window.x, &window.y );

		m_dirtyRects.Clip( &window );

		WgDirtyRect * pDirty = m_dirtyRects.pRectList;
 		while( pDirty != 0 )
 		{
			DoMyOwnRender( window, *pDirty, _layer );
 			pDirty = pDirty->pNext;
 		}
	}
  	m_bRenderedHere = true;


	// Render children recursively

	for( WgWidget * pTmp = m_pFirstChild ; pTmp != 0 ; pTmp = pTmp->m_pNextSibling )
	{
		if( !pTmp->m_bHidden )
			pTmp->RenderRecursively(_layer);
	}
}



//____ ClipDirtAgainstVisibleGeometry() ______________________________________

void WgWidget::ClipDirtAgainstVisibleGeometry( const WgRect& visible )
{

	WgRect clip( visible, m_geo );

	clip.x -= m_geo.x;
	clip.y -= m_geo.y;

	if( clip.w == 0 || clip.h == 0 )
		ClearBranchFromDirt();
	else
	{
		m_dirtyRects.Clip( &clip );

		// Call children recursively

		for( WgWidget * pChi = m_pFirstChild ; pChi != 0 ; pChi = pChi->m_pNextSibling )
		{
			if( !pChi->m_bHidden )
				pChi->ClipDirtAgainstVisibleGeometry( clip );
		}
	}
}


//____ WgWidget::FreezeBranch() _________________________________________________

void WgWidget::FreezeBranch( int _ofsX, int _ofsY )
{

	_ofsX += m_geo.x;
	_ofsY += m_geo.y;

	// Freeze my rectangles

	WgDirtyRect	* pDirt = m_dirtyRects.pRectList;
	while( pDirt )
	{
		pDirt->x += _ofsX;
		pDirt->y += _ofsY;
		pDirt = pDirt->pNext;
	}

	// Call children recursively

	for( WgWidget * pChi = m_pFirstChild ; pChi != 0 ; pChi = pChi->m_pNextSibling )
	{
		if( !pChi->m_bHidden )
		    pChi->FreezeBranch( _ofsX, _ofsY );
	}

}


//____ WgWidget::ClearBranchFromDirt() __________________________________________

void	WgWidget::ClearBranchFromDirt( void )
{
	m_dirtyRects.Clear();

	// Call children recursively

	for( WgWidget * pChi = m_pFirstChild ; pChi != 0 ; pChi = pChi->m_pNextSibling )
	{
		if( !pChi->m_bHidden )
		    pChi->ClearBranchFromDirt();
	}
}


//____ WgWidget::PreRenderMasking() _____________________________________________

// Masks against covering widgets recursively through the whole tree and also
// adds dirt to covering non-opaque widgets along the way.

void	WgWidget::PreRenderMasking( int _ofsX, int _ofsY )
{

	// Mask any dirty rectangles against our own upper siblings, parents upper
	// siblings,grandparents upper siblings etc.

	WgWidget	* pTmp = this;

	int		ofsX = _ofsX, ofsY = _ofsY;
	bool	bNonOpaqueCovering = false;

	while( pTmp && m_dirtyRects.pRectList )
	{
		if( pTmp->m_pNextSibling )
			if( pTmp->m_pNextSibling->MaskAgainstFrozenBranch( &m_dirtyRects, ofsX, ofsY ) )
				bNonOpaqueCovering = true;
		pTmp = pTmp->m_pParent;
		if( pTmp != 0 )
		{
			ofsX -= pTmp->m_geo.x;
			ofsY -= pTmp->m_geo.y;
		}
	}

	// Add dirty rectangles to any non-opaque widgets still covering.

	if( bNonOpaqueCovering && m_dirtyRects.pRectList )
	{
		WgWidget	* pTmp = this;

		int		ofsX = _ofsX, ofsY = _ofsY;

		while( pTmp )
		{
			if( pTmp->m_pNextSibling )
				pTmp->m_pNextSibling->AddDirtToNonOpaqueCovering( &m_dirtyRects, ofsX, ofsY );
			pTmp = pTmp->m_pParent;

			if( pTmp != 0 )
			{
				ofsX -= pTmp->m_geo.x;
				ofsY -= pTmp->m_geo.y;
			}
		}
	}

	// Call children recursively

	for( WgWidget * pChi = m_pFirstChild ; pChi != 0 ; pChi = pChi->m_pNextSibling )
	{
		if( !pChi->m_bHidden )
			pChi->PreRenderMasking( _ofsX + m_geo.x, _ofsY + m_geo.y );
	}
}

//____ WgWidget::AddDirtToNonOpaqueCovering() ___________________________________

void WgWidget::AddDirtToNonOpaqueCovering( WgDirtyRectObj * _pDirtObj, int _ofsX, int _ofsY )
{
 	if( !m_bOpaque && !m_bHidden )
 	{
	 	WgRect	clip;

	 	clip.x = _ofsX + m_geo.x;
	 	clip.y = _ofsY + m_geo.y;
	 	clip.w = m_geo.w;
	 	clip.h = m_geo.h;

		WgDirtyRect * pTmp = _pDirtObj->pRectList;
		WgRect		clipped;

		while( pTmp )
		{
			if( clipped.Intersection( *pTmp, clip ) )
				m_dirtyRects.Add( clipped.x, clipped.y, clipped.w, clipped.h );
			pTmp = pTmp->pNext;
		}

 	}

	// Call siblings recursively.

	if( m_pNextSibling )
		m_pNextSibling->AddDirtToNonOpaqueCovering( _pDirtObj, _ofsX, _ofsY );
}


//____ WgWidget::MaskAgainstFrozenBranch() ____________________________________________

// Somewhat different than WgWidget::MaskAgainstBranch(), mainly we never have to
// check m_bRenderedHere and we return weather we encountered a non-opaque widget
// along the way.

bool WgWidget::MaskAgainstFrozenBranch( WgDirtyRectObj * _pDirtObj, int _ofsX, int _ofsY )
{

	if( !m_bHidden )
	{
 		if( m_bOpaque )
 			_pDirtObj->Sub( m_geo.x + _ofsX, m_geo.y + _ofsY, m_geo.w, m_geo.h );
 		else
 		{
 			// We can't mask since we're not opaque, but we might have children that are...

 			if( m_pFirstChild )
 				m_pFirstChild->MaskAgainstFrozenBranch( _pDirtObj, _ofsX + m_geo.x, _ofsY + m_geo.y );

			// Call siblings recursively, if there are dirty rectangles left.

			if( m_pNextSibling && _pDirtObj->pRectList )
				m_pNextSibling->MaskAgainstFrozenBranch( _pDirtObj, _ofsX, _ofsY );

			return	true;
 		}
	}

	// Call siblings recursively, if there are dirty rectangles left.

	if( m_pNextSibling && _pDirtObj->pRectList )
		return m_pNextSibling->MaskAgainstFrozenBranch( _pDirtObj, _ofsX, _ofsY );

	return false;
}



//____ WgWidget::PushDirt() _______________________________________________

// Push dirty rectangles out to our children, thus reducing overdraw.

void	WgWidget::PushDirt( int _ofsX, int _ofsY )
{
//	if( !m_pFirstChild )
//		return;

	if( m_bHidden )
		return;

	// Push all dirty rectangles (or parts thereof) that we can onto our children.

	if( m_pFirstChild && m_dirtyRects.pRectList )
		m_pFirstChild->PushDirtSub( &m_dirtyRects, _ofsX + m_geo.x, _ofsY + m_geo.y );

	// Special actions needs to be taken if we have m_bRendersAll or m_bRenderOne
	// set and we still have dirty rectangles left.

	if( m_dirtyRects.pRectList && (m_bRendersAll || m_bRenderOne) )
	{
		// Set our new rectangle

		WgRect	rect;

		if( m_bRendersAll )
		{
			rect.x = _ofsX + m_geo.x;
			rect.y = _ofsY + m_geo.y;
			rect.w = m_geo.w;
			rect.h = m_geo.h;
		}
		else
			m_dirtyRects.OneForAll( &rect );

		// Make it our only one

		m_dirtyRects.Clear();
		m_dirtyRects.Add( rect.x, rect.y, rect.w, rect.h );

		// Make another copy to work with when pushing

		WgDirtyRectObj	dirtObj;
		dirtObj.Add( rect.x, rect.y, rect.w, rect.h );

		// Push against covering siblings, parent-siblings etc, begin with topmost.

		if( m_pParent )
		{
			int		x = 0, y = 0;

			Local2abs( &x, &y );
			PushDirtSubCovering( &dirtObj, x, y );

		}

		// Push what's left to our children (again).

		if( m_pFirstChild && dirtObj.pRectList )
			m_pFirstChild->PushDirtSub( &dirtObj, _ofsX + m_geo.x, _ofsY + m_geo.y );

	}

	// Call children recursively, making they distribute their rectangles.

	for( WgWidget * pChi = m_pFirstChild ; pChi != 0 ; pChi = pChi->m_pNextSibling )
			pChi->PushDirt( _ofsX + m_geo.x, _ofsY + m_geo.y );
}


//____ WgWidget::PushDirtSubCovering() __________________________________________

// Subroutine for PushDirt(). Pushes dirt towards siblings and parents siblings etc
// which are covering. Only used for m_bRendersAll and m_bRenderOne widgets.

void	WgWidget::PushDirtSubCovering( WgDirtyRectObj * _pDirtObj, int _ofsX, int _ofsY )
{
	_ofsX -= m_geo.x;
	_ofsY -= m_geo.y;

	if( m_pParent )
		m_pParent->PushDirtSubCovering( _pDirtObj, _ofsX, _ofsY );

	if( m_pNextSibling )
		m_pNextSibling->PushDirtSub( _pDirtObj, _ofsX, _ofsY );
}


//____ WgWidget::PushDirtSub() __________________________________________________

// Subroutine for PushDirt(), calling its siblings recursively and moving dirt
// to top-sibling first, then following through with the rest.

void	WgWidget::PushDirtSub( WgDirtyRectObj * _pDirtObj, int _ofsX, int _ofsY )
{
	if( m_id == 1 )
	{
		m_id = 1;			// Testcode, just so we can put a breakpoint. Delete if committed to repository...

	}


	// Call siblings recursively first, so we start with the top-one

	if( m_pNextSibling )
		m_pNextSibling->PushDirtSub( _pDirtObj, _ofsX, _ofsY );

	// Skip here if we are hidden

	if( m_bHidden )
		return;


	// Clip and transfer dirt to us

 	WgRect	clip;

 	clip.x = _ofsX + m_geo.x;
 	clip.y = _ofsY + m_geo.y;
 	clip.w = m_geo.w;
 	clip.h = m_geo.h;

	if( m_bOpaque )
	{
		// We're opaque, so we just take over the dirt we cover.

  	_pDirtObj->ClipTransfer( &m_dirtyRects, &clip );
	}
	else
	{
		// We can't mask since we aren't opaque, but let's see if we have children that can.

		if( m_pFirstChild )
			m_pFirstChild->PushDirtSub( _pDirtObj, _ofsX + m_geo.x, _ofsY + m_geo.y );

		// Whatever we cover we must update too since we're not opaque :(

		WgDirtyRect * pTmp = _pDirtObj->pRectList;
		WgRect		clipped;

		while( pTmp )
		{
			if( clipped.Intersection( *pTmp, clip ) )
				m_dirtyRects.Add( clipped.x, clipped.y, clipped.w, clipped.h );
			pTmp = pTmp->pNext;
		}
	}
}


//____ WgWidget::CollectDirtyRectangles() _______________________________________

void WgWidget::CollectDirtyRectangles( WgDirtyRectObj * _pDirtObj )
{
	WgDirtyRect	* pDirt = m_dirtyRects.pRectList;

	while( pDirt )
	{
		_pDirtObj->Add( pDirt->x, pDirt->y, pDirt->w, pDirt->h );
		pDirt = pDirt->pNext;
	}

	// Handle children recursively

	for( WgWidget * pTmp = m_pFirstChild ; pTmp != 0 ; pTmp = pTmp->m_pNextSibling )
	{
		if( !pTmp->m_bHidden )
			pTmp->CollectDirtyRectangles( _pDirtObj );
	}

}
