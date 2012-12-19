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

#include <wg_shaderpanel.h>
#include <wg_gfxdevice.h>

static const char	c_widgetType[] = {"ShaderPanel"};

//____ Constructor ____________________________________________________________

WgShaderPanel::WgShaderPanel() : m_tintColor(0xFFFFFFFF), m_tintMode(WG_TINTMODE_OPAQUE), m_blendMode(WG_BLENDMODE_BLEND)
{
}

//____ Destructor _____________________________________________________________

WgShaderPanel::~WgShaderPanel()
{
}

//____ Type() _________________________________________________________________

const char * WgShaderPanel::Type( void ) const
{
	return GetClass();
}

//____ GetClass() ____________________________________________________________

const char * WgShaderPanel::GetClass()
{
	return c_widgetType;
}

//____ SetColor() ______________________________________________________________

void WgShaderPanel::SetColor( const WgColor& color)
{
	if( color != m_tintColor )
	{
		m_tintColor = color;
		_requestRender();
	}
}

//____ SetTintMode() ___________________________________________________________

void WgShaderPanel::SetTintMode( WgTintMode mode )
{
	if( mode != m_tintMode )
	{
		m_tintMode = mode;
		_requestRender();
	}
}

//____ SetBlendMode() __________________________________________________________

void WgShaderPanel::SetBlendMode( WgBlendMode mode )
{
	if( mode != m_blendMode )
	{
		m_blendMode = mode;
		_requestRender();
	}
}


//____ _getBlendMode() _________________________________________________________

WgBlendMode WgShaderPanel::_getBlendMode() const
{
	return m_blendMode;
}


//____ _onMaskPatches() ________________________________________________________

void WgShaderPanel::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( m_hook.IsVisible() && m_hook.Widget() )
		m_hook.Widget()->_onMaskPatches( patches, geo, clip, m_blendMode );
}

//____ _renderPatches() ________________________________________________________

void WgShaderPanel::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
{
	// Set our tint color and blend mode.

	WgBlendMode		oldBM;
	WgColor			oldTC;


//	if( (_layer & m_layer) != 0 )
	{
		oldBM = pDevice->GetBlendMode();
		oldTC = pDevice->GetTintColor();

		pDevice->SetBlendMode(m_blendMode);

		if( m_tintMode == WG_TINTMODE_OPAQUE )
			pDevice->SetTintColor(m_tintColor);
		else	// MULTIPLY
			pDevice->SetTintColor(m_tintColor*oldTC);
	}

	// Render children recursively

	if( m_hook.Widget() && m_hook.IsVisible() )
		m_hook.Widget()->_renderPatches( pDevice, _canvas, _canvas, _pPatches, _layer );

	// Reset old blend mode and tint color

//	if( (_layer & m_layer) != 0 )
	{
		pDevice->SetBlendMode(oldBM);
		pDevice->SetTintColor(oldTC);
	}
}

//____ _onCloneContent() _______________________________________________________

void WgShaderPanel::_onCloneContent( const WgWidget * _pOrg )
{
	WgShaderPanel * pOrg = (WgShaderPanel*) _pOrg;

	m_tintColor		= pOrg->m_tintColor;
	m_blendMode		= pOrg->m_blendMode;
	m_tintMode		= pOrg->m_tintMode;
}
