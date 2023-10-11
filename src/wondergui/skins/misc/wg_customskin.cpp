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

#include <wg_customskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo CustomSkin::TYPEINFO = { "CustomSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	CustomSkin_p CustomSkin::create(const Blueprint& blueprint)
	{
		return CustomSkin_p(new CustomSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	CustomSkin::CustomSkin( const Blueprint& bp ) : Skin(bp)
	{
		m_bOpaque			= bp.opaque;
	
		m_markTestFunc		= bp.markTestFunc;
		m_renderFunc		= bp.renderFunc;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CustomSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() ______________________________________________________________

	void CustomSkin::_render( GfxDevice * pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if (m_renderFunc)
		{
			RectSPX canvas = _canvas - align(ptsToSpx(m_margin, scale));

			int prevLayer = -1;
			if (m_layer != -1 && pDevice->renderLayer() != m_layer)
			{
				prevLayer = pDevice->renderLayer();
				pDevice->setRenderLayer(m_layer);
			}

			m_renderFunc(pDevice, canvas, scale, state, value, value2);

			if (prevLayer != -1)
				pDevice->setRenderLayer(prevLayer);
		}
	}

	//____ _markTest() _________________________________________________________

	bool CustomSkin::_markTest( const CoordSPX& ofs, const RectSPX& _canvas, int scale, State state, float value, float value2, int alphaOverride ) const
	{
		RectSPX canvas = _canvas - align(ptsToSpx(m_margin, scale));

		if( m_markTestFunc )
			return m_markTestFunc(ofs,canvas,scale,state,value,value2,alphaOverride != -1 ? alphaOverride : m_markAlpha);
		else
		{
			if (canvas.contains(ofs))
				return m_bOpaque;
			else
				return false;
		}
	}

} // namespace wg
