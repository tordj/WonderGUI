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
#include <wg_patches.h>

namespace wg
{

	const char ShaderCapsule::CLASSNAME[] = {"ShaderCapsule"};

	//____ Constructor ____________________________________________________________

	ShaderCapsule::ShaderCapsule() : m_tintColor(0xFFFFFFFF), m_tintMode(BlendMode::Replace), m_renderMode(BlendMode::Blend)
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

	ShaderCapsule_p ShaderCapsule::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return ShaderCapsule_p( static_cast<ShaderCapsule*>(pObject) );

		return 0;
	}


	//____ setTintColor() ______________________________________________________

	void ShaderCapsule::setTintColor( Color color, BlendMode mode )
	{
		if( color != m_tintColor || mode != m_tintMode )
		{
			m_tintMode = mode;
			m_tintColor = color;
			_requestRender();
		}
	}

	//____ setRenderMode() __________________________________________________________

	void ShaderCapsule::setRenderMode( BlendMode mode )
	{
		if( mode != m_renderMode )
		{
			m_renderMode = mode;
			_requestRender();
		}
	}


	//____ _getRenderMode() _________________________________________________________

	BlendMode ShaderCapsule::_getRenderMode() const
	{
		return m_renderMode;
	}

	//____ _renderPatches() ________________________________________________________

	void ShaderCapsule::_renderPatches( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window, const Patches& _patches )
	{
		// Render our skin

		if( m_pSkin )
			Capsule::_renderPatches( pDevice, _canvas, _window, _patches );

		if (!m_child.pWidget)
			return;

		//



		// Set our tint color and blend mode.

		BlendMode		oldBM;
		Color			oldTC;
		Color			newTC;

		oldBM = pDevice->blendMode();
		oldTC = pDevice->tintColor();

		pDevice->setBlendMode(m_renderMode);
		pDevice->setTintColor( Color::blend(oldTC, m_tintColor, m_tintMode) );

		// Render children recursively

		RectI canvas = m_pSkin ? m_pSkin->contentRect(_canvas, m_state) : _canvas;

		if (canvas != _canvas)
		{
			Patches trimmedPatches( _patches, canvas );
			m_child.pWidget->_renderPatches(pDevice, canvas, canvas, trimmedPatches );
		}
		else
			m_child.pWidget->_renderPatches( pDevice, canvas, canvas, _patches );

		// Reset old blend mode and tint color

		pDevice->setBlendMode(oldBM);
		pDevice->setTintColor(oldTC);
	}

	//____ _cloneContent() _______________________________________________________

	void ShaderCapsule::_cloneContent( const Widget * _pOrg )
	{
		Capsule::_cloneContent( _pOrg );

		ShaderCapsule * pOrg = (ShaderCapsule*) _pOrg;

		m_tintColor		= pOrg->m_tintColor;
		m_renderMode	= pOrg->m_renderMode;
		m_tintMode		= pOrg->m_tintMode;
	}

} // namespace wg
