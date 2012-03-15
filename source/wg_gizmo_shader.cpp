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

#include <wg_gizmo_shader.h>
#include <wg_gfxdevice.h>

static const char	c_gizmoType[] = {"Shader"};

//____ Constructor ____________________________________________________________

WgGizmoShader::WgGizmoShader() : m_tintColor(0xFFFFFFFF), m_tintMode(WG_TINTMODE_OPAQUE), m_blendMode(WG_BLENDMODE_BLEND)
{
}

//____ Destructor _____________________________________________________________

WgGizmoShader::~WgGizmoShader()
{
}

//____ Type() _________________________________________________________________

const char * WgGizmoShader::Type( void ) const
{
	return GetMyType();
}

//____ GetMyType() ____________________________________________________________

const char * WgGizmoShader::GetMyType()
{
	return c_gizmoType;
}

//____ SetColor() ______________________________________________________________

void WgGizmoShader::SetColor( const WgColor& color)
{
	if( color != m_tintColor )
	{
		m_tintColor = color;
		_requestRender();
	}
}

//____ SetTintMode() ___________________________________________________________

void WgGizmoShader::SetTintMode( WgTintMode mode )
{
	if( mode != m_tintMode )
	{
		m_tintMode = mode;
		_requestRender();
	}
}

//____ SetBlendMode() __________________________________________________________

void WgGizmoShader::SetBlendMode( WgBlendMode mode )
{
	if( mode != m_blendMode )
	{
		m_blendMode = mode;
		_requestRender();
	}
}


//____ _getBlendMode() _________________________________________________________

WgBlendMode WgGizmoShader::_getBlendMode() const
{
	return m_blendMode;
}


//____ _onMaskPatches() ________________________________________________________

void WgGizmoShader::_onMaskPatches( WgPatches& patches, const WgRect& geo, const WgRect& clip, WgBlendMode blendMode )
{
	if( !m_hook.Hidden() && m_hook.Gizmo() )
		m_hook.Gizmo()->_onMaskPatches( patches, geo, clip, m_blendMode );
}

//____ _renderPatches() ________________________________________________________

void WgGizmoShader::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches, Uint8 _layer )
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

	if( m_hook.Gizmo() && !m_hook.Hidden() )
		m_hook.Gizmo()->_renderPatches( pDevice, _canvas, _canvas, _pPatches, _layer );

	// Reset old blend mode and tint color

//	if( (_layer & m_layer) != 0 )
	{
		pDevice->SetBlendMode(oldBM);
		pDevice->SetTintColor(oldTC);
	}
}

//____ _onCloneContent() _______________________________________________________

void WgGizmoShader::_onCloneContent( const WgGizmo * _pOrg )
{
	WgGizmoShader * pOrg = (WgGizmoShader*) _pOrg;

	m_tintColor		= pOrg->m_tintColor;
	m_blendMode		= pOrg->m_blendMode;
	m_tintMode		= pOrg->m_tintMode;
}
