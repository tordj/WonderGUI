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

#include <math.h>

#include <wg_gizmo_circlemeter.h>
#include <wg_font.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>

static const char	c_gizmoType[] = {"TordJ/CircleMeter"};

using namespace WgSignal;



//____ Constructor _________________________________________________________________

WgGizmoCirclemeter::WgGizmoCirclemeter( void )
{
	WgUCord16			clean(0,0);

	m_pSurfBg			= 0;
	m_normalBg			= clean;
	m_overBg			= clean;
	m_pressBg			= clean;
	m_disableBg			= clean;

	m_pSurfSlices		= 0;
	m_nBlocksPerSlice	= 1;
	m_startSlice		= 0;

	for( int i = 0 ; i < 4 ; i++ )
		m_sliceOfs[i]	= clean;

	m_slicePitch		= 0;
	m_bAscendingBlocks	= true;

	m_bRegenText		= true;			// So text is generated at first render.
	m_bPressed			= false;
	m_renderStyle		= 0;

	m_numberBorders		= WgBorders(0);
	m_text.setAlignment( WgOrigo::midCenter() );
	m_text.SetWrap(false);
	m_fullRangeDistance	= 0;
	m_valueAtPress		= 0;
}

//____ ~WgGizmoCirclemeter() _____________________________________________________

WgGizmoCirclemeter::~WgGizmoCirclemeter( void )
{
}


//____ Type() _________________________________________________________________

const char * WgGizmoCirclemeter::Type( void ) const
{
	return GetMyType();
}

const char * WgGizmoCirclemeter::GetMyType( void )
{
	return c_gizmoType;
}

//____ SetBackgroundSource() __________________________________________________

void WgGizmoCirclemeter::SetBackgroundSource(	WgSurface * pSurf, Uint32 xNormal, Uint32 yNormal,
	Uint32 xOver, Uint32 yOver, Uint32 xPress, Uint32 yPress, Uint32 xDisable, Uint32 yDisable )
{
		m_pSurfBg		= pSurf;
		m_normalBg.x	= xNormal;
		m_normalBg.y	= yNormal;
		m_overBg.x		= xOver;
		m_overBg.y		= yOver;

		m_pressBg.x		= xPress;
		m_pressBg.y		= yPress;

		m_disableBg.x	= xDisable;
		m_disableBg.y	= yDisable;

		OnRefresh();
}

//____ SetForegroundSource() __________________________________________________

void WgGizmoCirclemeter::SetForegroundSource( WgSurface * pSurf, WgRect& src, Uint32 destOfsX, Uint32 destOfsY )
{
	m_pSurfFg		= pSurf;
	m_srcFg			= src;
	m_destOfsFg.x	= destOfsX;
	m_destOfsFg.y	= destOfsY;

	OnRefresh();
}


//____ SetSliceSource() _______________________________________________________

void WgGizmoCirclemeter::SetSliceSource( WgSurface * pSurf, Uint8 blocksPerSlice, Uint8 slicePitch, bool bAscending,
	 Uint32 xOfsSlice1Blocks, Uint32 yOfsSlice1Blocks, Uint32 xOfsSlice2Blocks,
	 Uint32 yOfsSlice2Blocks, Uint32 xOfsSlice3Blocks, Uint32 yOfsSlice3Blocks,
	Uint32 xOfsSlice4Blocks, Uint32 yOfsSlice4Blocks )
{
		m_pSurfSlices		= pSurf;
		m_nBlocksPerSlice	= blocksPerSlice;
		m_slicePitch		= slicePitch;
		m_bAscendingBlocks	= bAscending;

		m_sliceOfs[0].x		= xOfsSlice1Blocks;
		m_sliceOfs[0].y		= yOfsSlice1Blocks;
		m_sliceOfs[1].x		= xOfsSlice2Blocks;
		m_sliceOfs[1].y		= yOfsSlice2Blocks;
		m_sliceOfs[2].x		= xOfsSlice3Blocks;
		m_sliceOfs[2].y		= yOfsSlice3Blocks;
		m_sliceOfs[3].x		= xOfsSlice4Blocks;
		m_sliceOfs[3].y		= yOfsSlice4Blocks;

		RequestRender();
}

//____ SetStartSlice() ________________________________________________________

bool WgGizmoCirclemeter::SetStartSlice( Uint8 slice )
{
	if( slice > 3 )
		return false;

	m_startSlice = slice;
	return true;
}

//____ SetNumberPosition() ____________________________________________________

void WgGizmoCirclemeter::SetNumberPosition( WgBorders numberBorders, WgOrigo numberAlignment )
{
	m_numberBorders			= numberBorders;
	m_text.setAlignment( numberAlignment );

	RequestRender();
}

//____ SetFullRangeDistance() _________________________________________________

void WgGizmoCirclemeter::SetFullRangeDistance(Uint32 pixels)
{
	if( m_fullRangeDistance != pixels )
	{
		m_fullRangeDistance = pixels;
		RequestRender();
	}
}


//____ SetFonts() _____________________________________________________________

bool WgGizmoCirclemeter::SetFonts( WgFont * _pFonts )
{
	if( _pFonts != m_text.getFont() )
	{
		m_text.setFont(_pFonts);
		m_bRegenText	= true;
		RequestRender();
	}

	return true;
}


//____ SetFormat() ____________________________________________________________

void WgGizmoCirclemeter::SetFormat( const WgValueFormat& format )
{
	m_format = format;
	RequestRender();
}



//____ OnAction() _________________________________________________

void WgGizmoCirclemeter::OnAction( WgInput::UserAction action, int button_key, const WgActionDetails& info, const WgInput& inputObj )
{
	int newRenderStyle = m_renderStyle;

	switch( action )
	{
		case	WgInput::POINTER_ENTER:
			if( !m_bPressed )
				newRenderStyle = 1;
			break;

		case	WgInput::POINTER_EXIT:
			if( !m_bPressed )
				newRenderStyle = 0;
			break;

		case WgInput::BUTTON_PRESS:
			if( button_key == 1 )
			{
				m_valueAtPress = m_value;
				m_bPressed = true;
				newRenderStyle = 2;
			}
			break;

		case WgInput::BUTTON_RELEASE:
			if( m_bPressed && button_key == 1 )
			{
				m_bPressed = false;
				newRenderStyle = 1;
			}
			break;

		case WgInput::BUTTON_DOWN:
			if( m_bPressed && button_key == 1 && m_fullRangeDistance != 0 )
			{
				const WgActionDetails * pPressInfo = inputObj.getPressDetails(1);

				Sint32 dragX = info.x - pPressInfo->x;
				float	incPerPixel = (m_rangeMax - m_rangeMin)/(float)m_fullRangeDistance;
				SetValue((int)(dragX*incPerPixel));
			}
			break;

        default:
            break;

	}

	if( newRenderStyle != m_renderStyle )
	{
		m_renderStyle = newRenderStyle;
		RequestRender();
	}
}

//____ OnRender() ________________________________________________________

void WgGizmoCirclemeter::OnRender( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, const WgRect& _clip, Uint8 _layer )
{

	// Render the background

	WgUCord16 * pTopLeft = &m_normalBg;

	switch( m_renderStyle )
	{
		case 1:
			pTopLeft = &m_overBg;
			break;
		case 2:
			pTopLeft = &m_pressBg;
			break;
		case 3:
			pTopLeft = &m_disableBg;
			break;
	}

	if( m_pSurfBg )
	{
		pDevice->ClipBlit( _clip, m_pSurfBg, WgRect(pTopLeft->x, pTopLeft->y, _canvas.w, _canvas.h), _canvas.x, _canvas.y );
	}

	// Render the slices

	float		frac		= FractionalValue();
	int			slice		= (int)(frac*4);								// What slice is fractional.
	int			block		= (int)(((frac*4)-slice)*m_nBlocksPerSlice);
	int			sliceWidth	= _canvas.w / 2;
	int			sliceHeight	= _canvas.w / 2;
	WgUCord16	ofs;


	// Adds on startSlice and takes modulo, but only if not whole is filled in which case slice must be 5.

	if( frac != 1.f )
	{
		slice += m_startSlice;
		slice %= 4;
	}

	const static int slicePlacement[4][2] = { {1,0}, {1,1}, {0,1}, {0,0} };

	for( int i = 0 ; i < 4 ; i++ )
	{
		ofs = m_sliceOfs[i];

		int xOfs = 0;
		if( i == slice )
		{
			xOfs = block;
		}
		else if( (m_startSlice <= slice && (i >= m_startSlice && i < slice)) ||
				 (m_startSlice > slice && (i >= m_startSlice || i < slice )) )
			xOfs = (m_nBlocksPerSlice-1);											// This slice should be filled.

		if(!m_bAscendingBlocks)
			xOfs = (m_nBlocksPerSlice-1) - xOfs;

		ofs.x += xOfs*m_slicePitch;

		pDevice->ClipBlit(_clip,m_pSurfSlices, WgRect(ofs.x, ofs.y, sliceWidth, sliceHeight), _canvas.x + slicePlacement[i][0]*sliceWidth, _canvas.y + slicePlacement[i][1]*sliceHeight );
	}

	// Render the foreground (if any)

	if( m_pSurfFg )
	{
		pDevice->ClipBlit( _clip, m_pSurfFg, m_srcFg, _canvas.x + m_destOfsFg.x, _canvas.y + m_destOfsFg.y );
	}


	// Render the value

	if( m_text.getFont() )
	{
		// We regenerate the text for the value at render if needed and not before,
		// so we never end up regenerating text more than once for each render.

		if( m_bRegenText )
		{
			m_text.setScaledValue( m_value, (Uint32) pow(10.f,m_format.decimals), m_format );
			m_bRegenText = false;
		}

		// Choose right style

		WgMode style = WG_MODE_NORMAL;
		switch( m_renderStyle )
		{
			case 1:
				style = WG_MODE_MARKED;
				break;
			case 2:
				style = WG_MODE_MARKED;
				break;
			case 3:
				style = WG_MODE_DISABLED;
				break;
		}

		// Print the text

		WgRect	textArea = _canvas;
			textArea.Shrink(m_numberBorders);

		m_text.setMode(style);
		pDevice->PrintText( _clip, &m_text, textArea );
	}
}

//____ OnCloneContent() _______________________________________________________

void WgGizmoCirclemeter::OnCloneContent( const WgGizmo * _pOrg )
{
	//TODO: Implement!
}

//____ OnAlphaTest() ______________________________________________________

bool WgGizmoCirclemeter::OnAlphaTest( const WgCord& ofs )
{
	//TODO: Implement!


	return true;
}

//____ OnEnable() ___________________________________________________

void WgGizmoCirclemeter::OnEnable( void )
{
	m_renderStyle = 0;
	RequestRender();
}

//____ OnDisable() ___________________________________________________

void WgGizmoCirclemeter::OnDisable( void )
{
	m_renderStyle = 3;
	RequestRender();
}

//____ ValueModified() ________________________________________________________

void WgGizmoCirclemeter::ValueModified()
{
	Emit( IntegerChanged(), m_value );
	Emit( Fraction(), FractionalValue() );

	m_bRegenText = true;
	RequestRender();
}

//____ RangeModified() ________________________________________________________

void WgGizmoCirclemeter::RangeModified()
{
	Emit( Fraction(), FractionalValue() );
	RequestRender();		// Since the indicator will move when range is modified...
}

//____ RepresentationModified() _______________________________________________

void WgGizmoCirclemeter::RepresentationModified()
{
	m_bRegenText = true;
	RequestRender();
}

//____ OnRefresh() _______________________________________________________

void WgGizmoCirclemeter::OnRefresh( void )
{
	if( m_pSurfBg )
	{
		if( m_pSurfBg->IsOpaque() )
			m_bOpaque = true;
		else
			m_bOpaque = false;

		RequestRender();
	}
}
