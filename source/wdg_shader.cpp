/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@swipnet.se].

                            -----------

  The WonderGUI Graphics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------
	
  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@swipnet.se] for details.

=========================================================================*/


#include	<wdg_shader.h>
#include	<wg_gfx.h>

static const char	Wdg_Type[] = {"TordJ/Shader"};

//____ NewOfMyType() __________________________________________________________

WgWidget * Wdg_Shader::NewOfMyType() const
{
	return new Wdg_Shader;
}


//____ Init() _________________________________________________________________

void Wdg_Shader::Init( void )
{
	m_tintColor = WgColor(255,255,255);
	m_blendMode = WG_BLENDMODE_BLEND;
	m_tintMode	= TINTMODE_OPAQUE;
}

//____ Destructor _____________________________________________________________

Wdg_Shader::~Wdg_Shader( void )
{
}


//____ Type() _________________________________________________________________

const char * Wdg_Shader::Type( void ) const
{
	return GetMyType();
}

const char * Wdg_Shader::GetMyType( void )
{
	return Wdg_Type;
}

//____ SetColor() _____________________________________________________________

void Wdg_Shader::SetColor( const WgColor& col )
{
	if( m_tintColor != col )
	{
		m_tintColor = col;
		RequestRender();
	}
}

//____ SetBlendMode() _________________________________________________________

void Wdg_Shader::SetBlendMode( WgBlendMode mode )
{
	if( m_blendMode != mode )
	{
		m_blendMode = mode;
		RequestRender();
	}
}

//____ SetTintMode() __________________________________________________________

void Wdg_Shader::SetTintMode( WgTintMode mode )
{
	if( m_tintMode != mode )
	{
		m_tintMode = mode;
		RequestRender();
	}
}



//____ DoMyOwnCloning() _______________________________________________________

void Wdg_Shader::DoMyOwnCloning( WgWidget * _pClone, const WgWidget * _pCloneRoot,
																const WgWidget * _pBranchRoot )
{
	Wdg_Shader * pClone = (Wdg_Shader *) _pClone;

	pClone->m_tintColor = m_tintColor;
	pClone->m_blendMode = m_blendMode;
	pClone->m_tintMode  = m_tintMode;
}

//____ DoMyOwnMarkTest() ___________________________________________________

bool Wdg_Shader::DoMyOwnMarkTest( int _x, int _y )
{
	return false;
}


//____ RenderRecursively() ____________________________________________________

void Wdg_Shader::RenderRecursively(Uint8 _layer, const WgRect& clip )
{
	// Skip my own rendering

//	if( m_dirtyRects.pRectList )
//	{
		m_dirtyRects.Clear();
	  	m_bRenderedHere = true;
//	}

	// Set our tint color and blend mode.

	WgBlendMode		oldBM;
	WgColor			oldTC;
	

	if( (_layer & m_layer) != 0 )
	{
		oldBM = WgGfx::blendMode();
		oldTC = WgGfx::tintColor();

		WgGfx::setBlendMode(m_blendMode);


		if( m_tintMode == TINTMODE_OPAQUE )
			WgGfx::setTintColor(m_tintColor);
		else	// MULTIPLY
			WgGfx::setTintColor(m_tintColor*oldTC);
	}

	// Call base method.

	WgWidget::RenderRecursively( _layer, clip );

	// Reset old blend mode and tint color

	if( (_layer & m_layer) != 0 )
	{
		WgGfx::setBlendMode(oldBM);
		WgGfx::setTintColor(oldTC);
	}
}

