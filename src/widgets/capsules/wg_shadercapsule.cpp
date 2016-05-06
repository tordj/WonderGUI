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

namespace wg 
{
	
	const char ShaderCapsule::CLASSNAME[] = {"ShaderCapsule"};
	
	//____ Constructor ____________________________________________________________
	
	ShaderCapsule::ShaderCapsule() : m_tintColor(0xFFFFFFFF), m_tintMode(TintMode::Opaque), m_blendMode(BlendMode::Blend)
	{
	}
	
	//____ Destructor _____________________________________________________________
	
	ShaderCapsule::~ShaderCapsule()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool ShaderCapsule::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Capsule::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * ShaderCapsule::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	ShaderCapsule_p ShaderCapsule::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ShaderCapsule_p( static_cast<ShaderCapsule*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	
	//____ setColor() ______________________________________________________________
	
	void ShaderCapsule::setColor( const Color& color)
	{
		if( color != m_tintColor )
		{
			m_tintColor = color;
			_requestRender();
		}
	}
	
	//____ setTintMode() ___________________________________________________________
	
	void ShaderCapsule::setTintMode( TintMode mode )
	{
		if( mode != m_tintMode )
		{
			m_tintMode = mode;
			_requestRender();
		}
	}
	
	//____ setBlendMode() __________________________________________________________
	
	void ShaderCapsule::setBlendMode( BlendMode mode )
	{
		if( mode != m_blendMode )
		{
			m_blendMode = mode;
			_requestRender();
		}
	}
	
	
	//____ _getBlendMode() _________________________________________________________
	
	BlendMode ShaderCapsule::_getBlendMode() const
	{
		return m_blendMode;
	}
	
	//____ _renderPatches() ________________________________________________________
	
	void ShaderCapsule::_renderPatches( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window, Patches * _pPatches )
	{
		// Set our tint color and blend mode.
	
		BlendMode		oldBM;
		Color			oldTC;
	
	
	//	if( (_layer & m_layer) != 0 )
		{
			oldBM = pDevice->getBlendMode();
			oldTC = pDevice->getTintColor();
	
			pDevice->setBlendMode(m_blendMode);
	
			if( m_tintMode == TintMode::Opaque )
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
	
	//____ _cloneContent() _______________________________________________________
	
	void ShaderCapsule::_cloneContent( const Widget * _pOrg )
	{
		ShaderCapsule * pOrg = (ShaderCapsule*) _pOrg;
	
		m_tintColor		= pOrg->m_tintColor;
		m_blendMode		= pOrg->m_blendMode;
		m_tintMode		= pOrg->m_tintMode;
	}

} // namespace wg
