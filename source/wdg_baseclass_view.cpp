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

#include <wdg_baseclass_view.h>


using namespace WgSignal;

static const char	Wdg_Type[] = {"TordJ/Unknown View-subclass"};


//____ Init() _________________________________________________________________

void Wdg_Baseclass_View::Init()
{
	m_contentWidth	= 0;
	m_contentHeight	= 0;
	m_stepSizeX		= 1;
	m_stepSizeY		= 1;
	m_jumpSizeX		= 0.75f;
	m_jumpSizeY		= 0.75f;

	m_viewPixOfsX	= 0;
	m_viewPixOfsY	= 0;

	m_pScrollbarX	= 0;
	m_pScrollbarY	= 0;
	
	m_bAutoHideScrollbarX = false;
	m_bAutoHideScrollbarY = false;

	m_bScrollbarBottom	= true;
	m_bScrollbarRight	= true;

	m_bAutoScrollX		= false;
	m_bAutoScrollY		= false;

	m_subclassGeo		= m_geo;
}

//____ ~Wdg_Baseclass_View() __________________________________________________

Wdg_Baseclass_View::~Wdg_Baseclass_View()
{
	if( m_pScrollbarX && m_pScrollbarX->Parent() == this && m_pScrollbarX->IsSetToHide() )
		delete m_pScrollbarX;

	if( m_pScrollbarY && m_pScrollbarY->Parent() == this && m_pScrollbarY->IsSetToHide() )
		delete m_pScrollbarY;
}

//____ Type() _________________________________________________________________

const char * Wdg_Baseclass_View::Type() const
{
	return GetMyType();
}

const char * Wdg_Baseclass_View::GetMyType( void )
{
	return Wdg_Type;
}


//____ StepUp() _______________________________________________________________

bool Wdg_Baseclass_View::StepUp()
{
	Sint32 ofs = m_viewPixOfsY - m_stepSizeY;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( ofs );
}

//____ StepDown() _____________________________________________________________
bool Wdg_Baseclass_View::StepDown()
{ 
	return SetViewPixelOfsY( m_viewPixOfsY + m_stepSizeY );
}

//____ StepLeft() _____________________________________________________________
bool Wdg_Baseclass_View::StepLeft()
{ 
	Sint32 ofs = m_viewPixOfsX - m_stepSizeX;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( ofs );
}

//____ StepRight() ____________________________________________________________

bool Wdg_Baseclass_View::StepRight()
{ 
	return SetViewPixelOfsX( m_viewPixOfsX + m_stepSizeX );
}


//____ JumpUp() _______________________________________________________________

bool Wdg_Baseclass_View::JumpUp()
{
	Sint32 ofs = m_viewPixOfsY - (Uint32)(ViewPixelLenY() * m_jumpSizeY);
	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( (Uint32) ofs );
}

//____ JumpDown() _____________________________________________________________

bool Wdg_Baseclass_View::JumpDown()
{
	return SetViewPixelOfsY( (Uint32)(m_viewPixOfsY + ViewPixelLenY() * m_jumpSizeY) );
}

//____ JumpLeft() _____________________________________________________________

bool Wdg_Baseclass_View::JumpLeft()
{
	Sint32 ofs = m_viewPixOfsX - (Uint32)(ViewPixelLenX() * m_jumpSizeX);
	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsX( (Uint32) ofs );
}

//____ JumpRight() ____________________________________________________________

bool Wdg_Baseclass_View::JumpRight()
{
	return SetViewPixelOfsX( (Uint32)(m_viewPixOfsX + ViewPixelLenX() * m_jumpSizeX) );
}

bool Wdg_Baseclass_View::WheelRoll(int distance)
{
	int ofs = int(m_viewPixOfsY) - int(m_stepSizeY) * distance * 3 / 120;

	if( ofs < 0 )
		ofs = 0;

	return SetViewPixelOfsY( ofs );
}

//____ SetJumpSizeX() _________________________________________________________

void Wdg_Baseclass_View::SetJumpSizeX( float viewFraction )
{
	if( viewFraction < 0.f )
		viewFraction = 0.f;
	if( viewFraction > 1.f )
		viewFraction = 1.f;

	m_jumpSizeX = viewFraction;
}

//____ SetJumpSizeY() _________________________________________________________

void Wdg_Baseclass_View::SetJumpSizeY( float viewFraction )
{
	if( viewFraction < 0.f )
		viewFraction = 0.f;
	if( viewFraction > 1.f )
		viewFraction = 1.f;

	m_jumpSizeY = viewFraction;
}

//____ ScrollbarXVisible() ____________________________________________________

bool Wdg_Baseclass_View::ScrollbarXVisible()
{
	if( m_pScrollbarX )
	{
		Uint32	width = Width();
		if( m_pScrollbarY && m_pScrollbarY->Parent() == this && (!m_bAutoHideScrollbarY || m_contentHeight > (Uint32) Height()) )
			width -= m_pScrollbarY->Width();

		if( !m_bAutoHideScrollbarX || m_contentWidth > width )
			return true;
	}
	return false;
}

//____ ScrollbarYVisible() ____________________________________________________

bool Wdg_Baseclass_View::ScrollbarYVisible()
{
	if( m_pScrollbarY )
	{
		Uint32	height = Height();
		if( m_pScrollbarX && m_pScrollbarX->Parent() == this && (!m_bAutoHideScrollbarX || m_contentWidth > (Uint32) Width()) )
			height -= m_pScrollbarX->Height();

		if( !m_bAutoHideScrollbarY || m_contentHeight > height )
			return true;
	}
	return false;
}


//____ ViewPixelLenX() ________________________________________________________

Uint32 Wdg_Baseclass_View::ViewPixelLenX()
{
	int viewLen = Width();

	if( ScrollbarYVisible() )
		viewLen -= m_pScrollbarY->Width();

	return viewLen;
}

//____ ViewPixelLenY() ________________________________________________________

Uint32 Wdg_Baseclass_View::ViewPixelLenY()
{
	int viewLen = Height();

	if( ScrollbarXVisible() )
		viewLen -= m_pScrollbarX->Height();

	return viewLen;
}

//____ ViewOfsX() _____________________________________________________________

float Wdg_Baseclass_View::ViewOfsX()
{
	Uint32 size = m_contentWidth - ViewPixelLenX();
	if( size <= 0 )
		return 0.f;
	else
		return m_viewPixOfsX / (float) size;

}

//____ ViewOfsY() _____________________________________________________________

float Wdg_Baseclass_View::ViewOfsY()
{
	Uint32 size = m_contentHeight - ViewPixelLenY();
	if( size <= 0 )
		return 0.f;
	else
		return m_viewPixOfsY / (float) size;
}

//____ ViewLenX() _____________________________________________________________

float Wdg_Baseclass_View::ViewLenX()
{
	if( m_contentWidth == 0 )
		return 1.0f;

	float len = ViewPixelLenX()/(float)m_contentWidth;

	if( len > 1.f )
		len = 1.f;

	return len;
}

//____ ViewLenY() _____________________________________________________________

float Wdg_Baseclass_View::ViewLenY()
{
	if( m_contentHeight == 0 )
		return 1.0f;

	float len = ViewPixelLenY()/(float)m_contentHeight;

	if( len > 1.f )
		len = 1.f;

	return len;
}

//____ SetViewPixelOfs() ______________________________________________________

bool Wdg_Baseclass_View::SetViewPixelOfs( Uint32 x, Uint32 y )
{
	if( x == m_viewPixOfsX && y == m_viewPixOfsY )
		return true;

	int		pixLenX = ViewPixelLenX();
	int		pixLenY = ViewPixelLenY();
	bool	bChangedX = false;
	bool	bChangedY = false;

	bool retVal = true;
	if( y > m_contentHeight - pixLenY )
	{
		y = m_contentHeight - pixLenY;
		retVal = false;
	}

	if( x > m_contentWidth - pixLenX )
	{
		x = m_contentWidth - pixLenX;
		retVal = false;
	}

	if( x != m_viewPixOfsX )
	{
		m_viewPixOfsX = x;
		bChangedX = true;
	}

	if( y != m_viewPixOfsY )
	{
		m_viewPixOfsY = y;
		bChangedY = true;
	}

	float	ofsX = ViewOfsX();
	float	ofsY = ViewOfsY();

	if( bChangedX )
	{
		Emit( ViewPosX(), ofsX );
		Emit( ViewPosPixelX(), m_viewPixOfsX );

		Emit( ViewPosSizeX(), ofsX, ViewLenX() );
		Emit( ViewPosSizePixelX(), m_viewPixOfsX, pixLenX );
	}

	if( bChangedY )
	{
		Emit( ViewPosY(), ofsY );
		Emit( ViewPosPixelY(), m_viewPixOfsY );

		Emit( ViewPosSizeY(), ofsY, ViewLenY() );
		Emit( ViewPosSizePixelY(), m_viewPixOfsY, pixLenY );
	}

	if( bChangedX || bChangedY )
	{
		Emit( ViewPos(), ofsX, ViewOfsY() );
		Emit( ViewPosPixel(), m_viewPixOfsX, m_viewPixOfsY );
	}

	RequestRender();
	return retVal;
}

//____ SetViewPixelOfsX() _____________________________________________________

bool Wdg_Baseclass_View::SetViewPixelOfsX( Uint32 x )
{
	if( x == m_viewPixOfsX )
		return true;

	int pixLenX = ViewPixelLenX();

	if( pixLenX > (int)m_contentWidth )
		return false;							// Can't scroll, content is smaller than view.

	bool retVal = true;
	if( x > m_contentWidth - pixLenX )
	{
		x = m_contentWidth - pixLenX;
		retVal = false;
	}

	m_viewPixOfsX = x;

	float	ofsX = ViewOfsX();

	Emit( ViewPosX(), ofsX );
	Emit( ViewPosPixelX(), m_viewPixOfsX );

	Emit( ViewPos(), ofsX, ViewOfsY() );
	Emit( ViewPosPixel(), m_viewPixOfsX, m_viewPixOfsY );

	Emit( ViewPosSizeX(), ofsX, ViewLenX() );
	Emit( ViewPosSizePixelX(), m_viewPixOfsX, pixLenX );

	RequestRender();
	return retVal;
}

//____ SetViewPixelOfsY() _____________________________________________________

bool Wdg_Baseclass_View::SetViewPixelOfsY( Uint32 y )
{

	if( y == m_viewPixOfsY )
		return true;

	int pixLenY = ViewPixelLenY();

	if( pixLenY > (int)m_contentHeight )
		return false;							// Can't scroll, content is smaller than view.

	bool retVal = true;
	if( y > m_contentHeight - pixLenY )
	{
		y = m_contentHeight - pixLenY;
		retVal = false;
	}

	m_viewPixOfsY = y;

	float	ofsY = ViewOfsY();

	Emit( ViewPosY(), ofsY );
	Emit( ViewPosPixelY(), m_viewPixOfsY );

	Emit( ViewPos(), ViewOfsX(), ofsY );
	Emit( ViewPosPixel(), m_viewPixOfsX, m_viewPixOfsY );

	Emit( ViewPosSizeY(), ofsY, ViewLenY() );
	Emit( ViewPosSizePixelY(), m_viewPixOfsY, pixLenY );

	RequestRender();
	return retVal;
}

//____ SetViewOfs() ___________________________________________________________

bool Wdg_Baseclass_View::SetViewOfs( float x, float y )
{
	if( x < 0.f )
		x = 0;
	if( x > 1.f )
		x = 1.f;

	if( y < 0.f )
		y = 0;
	if( y > 1.f )
		y = 1.f;

	int	width = m_contentWidth - ViewPixelLenX();
	int	height = m_contentHeight - ViewPixelLenY();

	if( width < 0 )
		width = 0;

	if( height < 0 )
		height = 0;


	return SetViewPixelOfs( (Uint32)((width*x)+0.5f), (Uint32)((height*y)+0.5f) );
}

//____ SetViewOfsX() __________________________________________________________

bool Wdg_Baseclass_View::SetViewOfsX( float x )
{
	if( x < 0.f )
		x = 0;
	if( x > 1.f )
		x = 1.f;

	int	width = m_contentWidth - ViewPixelLenX();
	if( width < 0 )
		width = 0;

	return SetViewPixelOfsX( (Uint32)((width*x)+0.5f) );
}

//____ SetViewOfsY() __________________________________________________________

bool Wdg_Baseclass_View::SetViewOfsY( float y )
{
	if( y < 0.f )
		y = 0;
	if( y > 1.f )
		y = 1.f;

	int	height = m_contentHeight - ViewPixelLenY();
	if( height < 0 )
		height = 0;

	return SetViewPixelOfsY( (Uint32)((height*y)+0.5f) );
}

//____ SetScrollbarX() ________________________________________________________

bool Wdg_Baseclass_View::SetScrollbarX( Wdg_HDrag * pScrollbar )
{
	// Remove callbacks for previous scrollbar.

	if( m_pScrollbarX )
	{
		m_pScrollbarX->RemoveCallbacks(this);
		RemoveCallbacks(m_pScrollbarX);
	}

	//

	bool	bWasVisible = ScrollbarXVisible();

	m_pScrollbarX = pScrollbar;
	if( 0 == m_pScrollbarX->Parent() )
		m_pScrollbarX->SetParent(this);

	if( m_pScrollbarX )
	{
		if( bWasVisible )
			m_pScrollbarX->ShowBranch();
		else
			m_pScrollbarX->HideBranch();

		int height = m_pScrollbarX->MinHeight();
		m_pScrollbarX->SetGeometry(WgOrigo::bottomLeft(),0,-height,WgOrigo::bottomRight(),0,-height);

		m_pScrollbarX->AddCallback( WgSignal::SliderPos(), Wdg_Baseclass_View::cbSetViewOfsX, this );
		m_pScrollbarX->AddCallback( WgSignal::PrevPage(), Wdg_Baseclass_View::cbJumpLeft, this );
		m_pScrollbarX->AddCallback( WgSignal::NextPage(), Wdg_Baseclass_View::cbJumpRight, this );
		m_pScrollbarX->AddCallback( WgSignal::Forward(), Wdg_Baseclass_View::cbStepRight, this );
		m_pScrollbarX->AddCallback( WgSignal::Back(), Wdg_Baseclass_View::cbStepLeft, this );

		AddCallback( WgSignal::ViewPosSizeX(), Wdg_HDrag::cbSetSlider, m_pScrollbarX );
	}

	// Force a refresh of our subclass if its geometry has been affected.

	if( ScrollbarXVisible() != bWasVisible )
	{
		WgRect subclassNewGeo = WgRect(m_geo.x, m_geo.y, ViewPixelLenX(), ViewPixelLenY() );
		DoMyOwnGeometryChangeSubclass( m_subclassGeo, subclassNewGeo );
		m_subclassGeo = subclassNewGeo;
	}

	return true;
}

//____ SetScrollbarY() ________________________________________________________

bool Wdg_Baseclass_View::SetScrollbarY( Wdg_VDrag * pScrollbar )
{
	// Remove callbacks for previous scrollbar.

	if( m_pScrollbarY )
	{
		m_pScrollbarY->RemoveCallbacks(this);
		RemoveCallbacks(m_pScrollbarY);
	}

	//

	bool	bWasVisible = ScrollbarYVisible();

	m_pScrollbarY = pScrollbar;
	if( 0 == m_pScrollbarY->Parent() )
		m_pScrollbarY->SetParent(this);

	if( m_pScrollbarY )
	{
		if( bWasVisible )
			m_pScrollbarY->ShowBranch();
		else
			m_pScrollbarY->HideBranch();

		int width = m_pScrollbarY->MinWidth();
		m_pScrollbarY->SetGeometry(WgOrigo::topRight(),-width,0,WgOrigo::bottomRight(),-width,0);

		m_pScrollbarY->AddCallback( WgSignal::SliderPos(), Wdg_Baseclass_View::cbSetViewOfsY, this );
		m_pScrollbarY->AddCallback( WgSignal::PrevPage(), Wdg_Baseclass_View::cbJumpUp, this );
		m_pScrollbarY->AddCallback( WgSignal::NextPage(), Wdg_Baseclass_View::cbJumpDown, this );
		m_pScrollbarY->AddCallback( WgSignal::Forward(), Wdg_Baseclass_View::cbStepDown, this );
		m_pScrollbarY->AddCallback( WgSignal::Back(), Wdg_Baseclass_View::cbStepUp, this );
		m_pScrollbarY->AddCallback( WgSignal::WheelRoll(1), Wdg_Baseclass_View::cbWheelRoll, this );

		AddCallback( WgSignal::ViewPosSizeY(), Wdg_VDrag::cbSetSlider, m_pScrollbarY );
		AddCallback( WgSignal::WheelRoll(1), Wdg_Baseclass_View::cbWheelRoll, this );
	}

	// Force a refresh of our subclass if its geometry has been affected.

	if( ScrollbarYVisible() != bWasVisible )
	{
		WgRect subclassNewGeo = WgRect(m_geo.x, m_geo.y, ViewPixelLenX(), ViewPixelLenY() );
		DoMyOwnGeometryChangeSubclass( m_subclassGeo, subclassNewGeo );
		m_subclassGeo = subclassNewGeo;
	}

	return true;
}

//____ SetScrollbarAutoHide() _________________________________________________

void Wdg_Baseclass_View::SetScrollbarAutoHide( bool bHideX, bool bHideY )
{
	if( bHideX == m_bAutoHideScrollbarX && bHideY == m_bAutoHideScrollbarY )
		return;

	bool	bWasVisibleX = ScrollbarXVisible();
	bool	bWasVisibleY = ScrollbarYVisible();

	m_bAutoHideScrollbarX = bHideX;
	m_bAutoHideScrollbarY = bHideY;

	// Force a refresh of our subclass if its geometry has been affected.

	if( ScrollbarXVisible() != bWasVisibleX || ScrollbarYVisible() != bWasVisibleY )
	{
		WgRect subclassNewGeo = WgRect(m_geo.x, m_geo.y, ViewPixelLenX(), ViewPixelLenY() );
		DoMyOwnGeometryChangeSubclass( m_subclassGeo, subclassNewGeo );
		m_subclassGeo = subclassNewGeo;
	}

}

//____ SetScrollbarPositions() ________________________________________________

void Wdg_Baseclass_View::SetScrollbarPositions( bool bBottom, bool bRight )
{
	if( bBottom == m_bScrollbarBottom && bRight == m_bScrollbarRight )
		return;

	m_bScrollbarBottom	= bBottom;
	m_bScrollbarRight	= bRight;

	//TODO: We need to force some kind of refresh if things have changed...
}


//____ DoMyOwnGeometryChange() ________________________________________________

void Wdg_Baseclass_View::DoMyOwnGeometryChange( WgRect& oldGeo, WgRect& newGeo )
{
	if( oldGeo.w == newGeo.w && oldGeo.h == newGeo.h )
		return;

	// TODO : Tord
	// viktorfix:
	// if this is a subcall from SetParent(), 
	// and there was no parent before (meaning width and height was 4096 on the m_pScrollbarY),
	// the code below would be bad. ViewPixelLenX() would return a negative value (new width of Baseclass_View object - "old" width of m_pScrollbarY, i.e. 4096)
	// which would then be used to calculate the new geometry for DoMyOwnGeometryChangeSubclass().
	// this code below fixes the problem. but is it the best way?
	if( m_pParent != 0 )
	{
		if( m_pScrollbarY )
		{
			if( ScrollbarYVisible() )
				m_pScrollbarY->ShowBranch();
			else
				m_pScrollbarY->HideBranch();
		}

		if( m_pScrollbarX )
		{
			if( ScrollbarXVisible() )
				m_pScrollbarX->ShowBranch();
			else
				m_pScrollbarX->HideBranch();
		}
	}

	float	lenX = ViewLenX();
	float	lenY = ViewLenY();
	Uint32	lenPixX = ViewPixelLenX();
	Uint32	lenPixY = ViewPixelLenY();

	Emit( ViewSize(), lenX, lenY );
	Emit( ViewSizePixel(), lenPixX, lenPixY );

	bool	bViewPosChanged = false;

	if( oldGeo.w != newGeo.w )
	{
		// Handle autoscrolling

		if( m_bAutoScrollX )
		{
			int oldLenX = oldGeo.w;
			if( ScrollbarYVisible() )
				oldLenX -= m_pScrollbarY->Width();

			if( m_viewPixOfsX + oldLenX >= m_contentWidth && lenPixX < m_contentWidth )
			{
				m_viewPixOfsX = m_contentWidth - lenPixX;
				bViewPosChanged = true;

				Emit( ViewPosX(), ViewOfsX() );
				Emit( ViewPosPixelX(), m_viewPixOfsX );
			}
		}

		// Handle if view became larger than content

		if( lenPixX >= m_contentWidth && m_viewPixOfsX != 0 )
		{
			m_viewPixOfsX = 0;
			bViewPosChanged = true;

			Emit( ViewPosX(), ViewOfsX() );
			Emit( ViewPosPixelX(), m_viewPixOfsX );
		}

		Emit( ViewSizeX(), lenX );
		Emit( ViewSizePixelX(), lenPixX );
		Emit( ViewPosSizeX(), ViewOfsX(), lenX );
		Emit( ViewPosSizePixelX(), m_viewPixOfsX, lenPixX );
	}

	if( oldGeo.h != newGeo.h )
	{
		// Handle autoscrolling

		if( m_bAutoScrollY )
		{
			int oldLenY = oldGeo.h;
			if( ScrollbarXVisible() )
				oldLenY -= m_pScrollbarX->Height();

			if( m_viewPixOfsY + oldLenY >= m_contentHeight && lenPixY < m_contentHeight )
			{
				m_viewPixOfsY = m_contentHeight - lenPixY;
				bViewPosChanged = true;

				Emit( ViewPosY(), ViewOfsY() );
				Emit( ViewPosPixelY(), m_viewPixOfsY );
			}
		}

		// Handle if view became larger than content

		if( lenPixY >= m_contentHeight && m_viewPixOfsY != 0 )
		{
			m_viewPixOfsY = 0;
			bViewPosChanged = true;

			Emit( ViewPosY(), ViewOfsY() );
			Emit( ViewPosPixelY(), m_viewPixOfsY );
		}

		//

		Emit( ViewSizeY(), lenY );
		Emit( ViewSizePixelY(), lenPixY );
		Emit( ViewPosSizeY(), ViewOfsY(), lenY );
		Emit( ViewPosSizePixelY(), m_viewPixOfsY, lenPixY );
	}


	if( bViewPosChanged )
	{
		Emit( ViewPos(), ViewOfsX(), ViewOfsY() );
		Emit( ViewPosPixel(), m_viewPixOfsX, m_viewPixOfsY );
	}

	WgRect subclassNewGeo = WgRect(newGeo.x, newGeo.y, lenPixX, lenPixY );
	DoMyOwnGeometryChangeSubclass( m_subclassGeo, subclassNewGeo );
	m_subclassGeo = subclassNewGeo;
}

//_____________________________________________________________________________
void Wdg_Baseclass_View::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot, const WgWidget * _pBranchRoot )
{
	WgWidget::DoMyOwnCloning(_pClone, _pCloneRoot, _pBranchRoot);

	Wdg_Baseclass_View* pClone = (Wdg_Baseclass_View*)_pClone;

	pClone->m_contentWidth = m_contentWidth;
	pClone->m_contentHeight = m_contentHeight;
	pClone->m_stepSizeX = m_stepSizeX;
	pClone->m_stepSizeY = m_stepSizeY;
	pClone->m_jumpSizeX = m_jumpSizeX;
	pClone->m_jumpSizeY = m_jumpSizeY;

	pClone->m_viewPixOfsX = m_viewPixOfsX;
	pClone->m_viewPixOfsY = m_viewPixOfsY;
							
	pClone->m_pScrollbarX = 0;
	pClone->m_pScrollbarY = 0;

	if( m_pScrollbarX && _pBranchRoot )
	{
		char map[1024];
		map[0] = 0;
		if( _pBranchRoot->PathFinder(m_pScrollbarX, map ) )
		{
			pClone->m_pScrollbarX = (Wdg_HDrag*)_pCloneRoot->PathFollower( map );
		}
		else
		{
			pClone->m_pScrollbarX = (Wdg_HDrag*)m_pScrollbarX->CloneBranch();
		}
	}

	if( m_pScrollbarY && _pBranchRoot )
	{
		char map[1024];
		map[0] = 0;
		if( _pBranchRoot->PathFinder(m_pScrollbarY, map ) )
		{
			pClone->m_pScrollbarY = (Wdg_VDrag*)_pCloneRoot->PathFollower( map );
		}
		else
		{
			pClone->m_pScrollbarY = (Wdg_VDrag*)m_pScrollbarY->CloneBranch();
		}
	}

	pClone->m_bAutoHideScrollbarX = m_bAutoHideScrollbarX;
	pClone->m_bAutoHideScrollbarY = m_bAutoHideScrollbarY;

	pClone->m_bScrollbarBottom = m_bScrollbarBottom;
	pClone->m_bScrollbarRight = m_bScrollbarRight;

	pClone->m_bAutoScrollX = m_bAutoScrollX;
	pClone->m_bAutoScrollY = m_bAutoScrollY;

	pClone->m_subclassGeo = m_subclassGeo;
}

//_____________________________________________________________________________
void Wdg_Baseclass_View::SetContentSize( Uint32 width, Uint32 height )
{
	bool	bWidthChanged	= false;
	bool	bHeightChanged	= false;
	bool	bMoveToXMost	= false;
	bool	bMoveToYMost	= false;

	if( width != m_contentWidth )
	{
		if( m_bAutoScrollX && m_viewPixOfsX + ViewPixelLenX() >= m_contentWidth )
			bMoveToXMost = true;

		bWidthChanged = true;
		m_contentWidth = width;
	}

	if( height != m_contentHeight )
	{
		if( m_bAutoScrollY && m_viewPixOfsY + ViewPixelLenY() >= m_contentHeight )
			bMoveToYMost = true;

		bHeightChanged = true;
		m_contentHeight = height;
	}

	if( !bWidthChanged && !bHeightChanged )
		return;


	bool bViewChanged = false;

	if( ScrollbarYVisible() )
	{
		if( m_pScrollbarY->IsSetToHide() )
		{
			m_pScrollbarY->ShowBranch();
			bViewChanged = true;
		}
	}
	else if( m_pScrollbarY )
	{
		if( !m_pScrollbarY->IsSetToHide() )
		{
			m_pScrollbarY->HideBranch();
			bViewChanged = true;
		}
	}
	if( ScrollbarXVisible() )
	{
		if( m_pScrollbarX->IsSetToHide() )
		{
			m_pScrollbarX->ShowBranch();
			bViewChanged = true;
		}
	}
	else if( m_pScrollbarX )
	{
		if( !m_pScrollbarX->IsSetToHide() )
		{
			m_pScrollbarX->HideBranch();
			bViewChanged = true;
		}
	}

	if( bViewChanged )
	{
		WgRect subclassNewGeo = WgRect(m_geo.x, m_geo.y, ViewPixelLenX(), ViewPixelLenY() );
		DoMyOwnGeometryChangeSubclass( m_subclassGeo, subclassNewGeo );
		m_subclassGeo = subclassNewGeo;
	}


	float	lenX = ViewLenX();
	float	lenY = ViewLenY();
	Uint32	lenPixX = ViewPixelLenX();
	Uint32	lenPixY = ViewPixelLenY();

	Emit( ViewSize(), lenX, lenY );
	Emit( ViewSizePixel(), lenPixX, lenPixY );

	if( bWidthChanged )
	{
		if( (m_viewPixOfsX + lenPixX > m_contentWidth) || (bMoveToXMost && m_contentWidth > lenPixX) )
		{
			Sint32 x = m_contentWidth - lenPixX;
			if( x < 0 ) 
				x = 0;
			m_viewPixOfsX = (Uint32) x;

			Emit( ViewPosX(), ViewOfsX() );
			Emit( ViewPosPixelX(), m_viewPixOfsX );
		}

		Emit( ViewSizeX(), lenX );
		Emit( ViewSizePixelX(), lenPixX );
		Emit( ViewPosSizeX(), ViewOfsX(), lenX );
		Emit( ViewPosSizePixelX(), m_viewPixOfsX, lenPixX );
	}

	if( bHeightChanged )
	{
		if( (m_viewPixOfsY + lenPixY > m_contentHeight) || (bMoveToYMost && m_contentHeight > lenPixY) )
		{
			Sint32 y = m_contentHeight - lenPixY;
			if( y < 0 ) 
				y = 0;
			m_viewPixOfsY = (Uint32) y;

			Emit( ViewPosY(), ViewOfsY() );
			Emit( ViewPosPixelY(), m_viewPixOfsY );
		}

		Emit( ViewSizeY(), lenY );
		Emit( ViewSizePixelY(), lenPixY );
		Emit( ViewPosSizeY(), ViewOfsY(), lenY );
		Emit( ViewPosSizePixelY(), m_viewPixOfsY, lenPixY );
	}

}

//_____________________________________________________________________________
bool Wdg_Baseclass_View::SetAutoscroll( bool bAutoX, bool bAutoY )
{
	m_bAutoScrollX = bAutoX;
	m_bAutoScrollY = bAutoY;
	return true;
}

