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

#include <wg_shadercapsule.h>
#include <wg_gfxdevice.h>

const char WgShaderCapsule::CLASSNAME[] = {"ShaderCapsule"};

//____ Constructor ____________________________________________________________

WgShaderCapsule::WgShaderCapsule() : m_tintColor(0xFFFFFFFF), m_tintMode(WG_TINTMODE_OPAQUE), m_blendMode(WG_BLENDMODE_BLEND)
{
}

//____ Destructor _____________________________________________________________

WgShaderCapsule::~WgShaderCapsule()
{
}

//____ isInstanceOf() _________________________________________________________

bool WgShaderCapsule::isInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgCapsule::isInstanceOf(pClassName);
}

//____ className() ____________________________________________________________

const char * WgShaderCapsule::className( void ) const
{ 
	return CLASSNAME; 
}

//____ cast() _________________________________________________________________

WgShaderCapsulePtr WgShaderCapsule::cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->isInstanceOf(CLASSNAME) )
		return WgShaderCapsulePtr( static_cast<WgShaderCapsule*>(pObject.rawPtr()) );

	return 0;
}


//____ setColor() ______________________________________________________________

void WgShaderCapsule::setColor( const WgColor& color)
{
	if( color != m_tintColor )
	{
		m_tintColor = color;
		_requestRender();
	}
}

//____ setTintMode() ___________________________________________________________

void WgShaderCapsule::setTintMode( WgTintMode mode )
{
	if( mode != m_tintMode )
	{
		m_tintMode = mode;
		_requestRender();
	}
}

//____ setBlendMode() __________________________________________________________

void WgShaderCapsule::setBlendMode( WgBlendMode mode )
{
	if( mode != m_blendMode )
	{
		m_blendMode = mode;
		_requestRender();
	}
}


//____ _getBlendMode() _________________________________________________________

WgBlendMode WgShaderCapsule::_getBlendMode() const
{
	return m_blendMode;
}

//____ _renderPatches() ________________________________________________________

void WgShaderCapsule::_renderPatches( WgGfxDevice * pDevice, const WgRect& _canvas, const WgRect& _window, WgPatches * _pPatches )
{
	// Set our tint color and blend mode.

	WgBlendMode		oldBM;
	WgColor			oldTC;


//	if( (_layer & m_layer) != 0 )
	{
		oldBM = pDevice->getBlendMode();
		oldTC = pDevice->getTintColor();

		pDevice->setBlendMode(m_blendMode);

		if( m_tintMode == WG_TINTMODE_OPAQUE )
			pDevice->setTintColor(m_tintColor);
		else	// MULTIPLY
			pDevice->setTintColor(m_tintColor*oldTC);
	}

	// Render children recursively

	if( m_hook._widget() )
		m_hook._widget()->_renderPatches( pDevice, _canvas, _canvas, _pPatches );

	// Reset old blend mode and tint color

//	if( (_layer & m_layer) != 0 )
	{
		pDevice->setBlendMode(oldBM);
		pDevice->setTintColor(oldTC);
	}
}

//____ _onCloneContent() _______________________________________________________

void WgShaderCapsule::_onCloneContent( const WgWidget * _pOrg )
{
	WgShaderCapsule * pOrg = (WgShaderCapsule*) _pOrg;

	m_tintColor		= pOrg->m_tintColor;
	m_blendMode		= pOrg->m_blendMode;
	m_tintMode		= pOrg->m_tintMode;
}
