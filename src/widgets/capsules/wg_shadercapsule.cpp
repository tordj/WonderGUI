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
#include <wg_internal.h>

namespace wg
{
	using namespace Util;

	const TypeInfo ShaderCapsule::TYPEINFO = { "ShaderCapsule", &Capsule::TYPEINFO };

	//____ constructor ____________________________________________________________

	ShaderCapsule::ShaderCapsule() : m_tintColor(0xFFFFFFFF), m_tintMode(BlendMode::Replace), m_renderMode(BlendMode::Blend)
	{
	}

	//____ Destructor _____________________________________________________________

	ShaderCapsule::~ShaderCapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ShaderCapsule::typeInfo(void) const
	{
		return TYPEINFO;
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

	//____ _render() ________________________________________________________

	void ShaderCapsule::_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window )
	{
		// Render our skin

		OO(skin)._render( pDevice, _canvas, m_state );

		if (!slot._widget())
			return;


		// Set our tint color and blend mode.

		BlendMode		oldBM;
		Color			oldTC;
		Color			newTC;

		oldBM = pDevice->blendMode();
		oldTC = pDevice->tintColor();

		pDevice->setBlendMode(m_renderMode);
		pDevice->setTintColor( Color::blend(oldTC, m_tintColor, m_tintMode) );

		// Render children recursively

		Rect canvas = OO(skin)._contentRect(_canvas, m_state);

		if (canvas != _canvas)
		{
			auto savedClipData = limitClipList(pDevice, canvas );
			OO(slot._widget())->_render(pDevice, canvas, canvas );
			popClipList(pDevice, savedClipData);
		}
		else
			OO(slot._widget())->_render( pDevice, canvas, canvas );

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
