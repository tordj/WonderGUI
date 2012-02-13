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

#include <wdg_root.h>
#include <wg_input.h>
#include <wg_gfx.h>


static const char	Wdg_Type[] = {"TordJ/Root"};

//____ NewWidgetOfMyType() __________________________________________________________

WgWidget * Wdg_Root::NewWidgetOfMyType() const
{
	return new Wdg_Root;
}


//____ Init() _________________________________________________________________

void Wdg_Root::Init( void )
{
	m_input.setRootWidget( this );
}

//____ ~Wdg_Root() ____________________________________________________________

Wdg_Root::~Wdg_Root( void )
{
}	

//____ Type() _________________________________________________________________

const char * Wdg_Root::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_Root::GetMyType( void )
{
	return Wdg_Type;
}

//____ Update() _______________________________________________________________

void	Wdg_Root::Update( Uint32 msTime )
{
	
	if( m_info.msTime == 0 )
		m_info.msDiff = 0;
	else
		m_info.msDiff = msTime - m_info.msTime;
		
	m_info.msTime = msTime;
	

  UpdateX( m_info );
}


//____ DoMyOwnMarkTest() ___________________________________________________

bool Wdg_Root::DoMyOwnMarkTest( int _x, int _y )
{
	return true;
}

//____ BeginRender() __________________________________________________________

int Wdg_Root::BeginRender( WgRect * _paDirtyRects, int _nRects, int _maxRects, WgRect * _pClip )
{
	// Clip all dirty rectangles against their widgets visible geometry.

	if( _pClip )
		ClipDirtAgainstVisibleGeometry( * _pClip );
	else
		ClipDirtAgainstVisibleGeometry( m_geo );

	// Make all dirty rectangle cordinates absolute and clip them if we have a clipping rectangle.

	FreezeBranch( 0, 0 );

	// Adding any user supplied dirty rectangles (where he has drawn something
	// upon our widgets). Doing this after the freeze since he sends them in as
	// global cordinates and root-widget doesn't necessarily has to start on 0,0.

	for( int i = 0 ; i < _nRects ; i++ )
		m_dirtyRects.Add( _paDirtyRects[i].x, _paDirtyRects[i].y,
										_paDirtyRects[i].w, _paDirtyRects[i].h );


	// Mask widgets against everything above them except their own children.
	// Hopefully we get rid off (or make smaller) a lot of dirty rectangles.
	// Also generates extra dirty rectangles for !m_bOpaque objects where necessary.

	PreRenderMasking( 0, 0 );		

	// Gather the rectangles for the exported list now before the rectangles get
	// split up too much.

	WgRectChain	dirtObj;
	CollectDirtyRectangles( &dirtObj );

	// Distribute any dirty rectangles left to our topmost children, thus
	// eliminate unnecessary overdraw.

	PushDirt( 0, 0 );

	// Prepare device for rendering...

	WgGfx::BeginRender();

	// Export the array of dirty rectangles
	
	return ExportDirtyRects( &dirtObj, _paDirtyRects, _maxRects );
}

//____ RenderLayer() __________________________________________________________

void Wdg_Root::RenderLayer( Uint8 _layer )
{
	// Do the actual rendering. Take the lowest one first and the upmost last.
	// That way we can handle bRenderAll and bRenderOne objects.

	RenderRecursively(_layer, m_geo );
}

//____ EndRender() ____________________________________________________________

void Wdg_Root::EndRender( void )
{
	WgGfx::EndRender();
	ClearBranchFromDirt();
}


//____ Render() _______________________________________________________________

int	Wdg_Root::Render( WgRect * _paDirtyRects, int _nRects, int _maxRects, WgRect * _pClip )
{
	int retVal = BeginRender( _paDirtyRects, _nRects, _maxRects, _pClip );
	RenderLayer(0xFF);
	EndRender();
	return retVal;
}




//____ ExportDirtyRects() ___________________________________________________

int	Wdg_Root::ExportDirtyRects( WgRectChain * _pDirtObj, WgRect * _wpaDirtyRects,
															int _maxRects )
{
	int		i = 0;
	WgRectLink * pDirt = _pDirtObj->pRectList;

	// Fill in the array.

	while( pDirt && i < _maxRects )
	{
		_wpaDirtyRects[i++] = * pDirt;
		pDirt = pDirt->pNext;
	}

	// Check so the array wasn't too small, go for plan B if it was.

	if( pDirt )
	{
		_pDirtObj->OneForAll( _wpaDirtyRects );
		return 1;
	}

	return	i;
}



//____ SetSize() ______________________________________________________________

bool Wdg_Root::SetSize( int _w, int _h, bool bAllowMoveChildren )
{
  if( _w == m_geo.w && _h == m_geo.h )
    return  true;

	m_sizeLimit_user.minW = _w;
	m_sizeLimit_user.minH = _h;
	m_sizeLimit_user.maxW = _w;
	m_sizeLimit_user.maxH = _h;
 	RefreshSizeLimit();


	m_x2 = _w;
	m_y2 = _h;

	m_geo.w = _w;
	m_geo.h = _h;

	RequestRender( 0, 0, m_geo.w, m_geo.h, 0, false );

	WgWidget * pChild = m_pFirstChild;
	while( pChild != 0 )
	{
		pChild->UpdateGeometry(false, true);
		pChild = pChild->m_pNextSibling;
	}

	Emit( WgSignal::GeometryChange() );

  return true;
}

