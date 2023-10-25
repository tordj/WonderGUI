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

#include <wg_lambdaskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo LambdaSkin::TYPEINFO = { "LambdaSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	LambdaSkin_p LambdaSkin::create(const Blueprint& blueprint)
	{
		return LambdaSkin_p(new LambdaSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	LambdaSkin::LambdaSkin( const Blueprint& bp ) : Skin(bp)
	{
		m_defaultSize		= bp.defaultSize;
		m_bOpaque			= bp.opaque;
	
		m_markTestFunc		= bp.markTestFunc;
		m_renderFunc		= bp.renderFunc;
		
		m_bIgnoresState		= bp.ignoreState;
		m_bIgnoresValue		= bp.ignoreValue;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& LambdaSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() ______________________________________________________________

	void LambdaSkin::_render( GfxDevice * pDevice, const RectSPX& _canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
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

	bool LambdaSkin::_markTest( const CoordSPX& ofs, const RectSPX& _canvas, int scale, State state, float value, float value2, int alphaOverride ) const
	{
		RectSPX canvas = _canvas - align(ptsToSpx(m_margin, scale));

		int alpha = alphaOverride != -1 ? alphaOverride : m_markAlpha;
		
		if( alpha == 0 )
			return true;
		else if( alpha > 4096 )
			return true;
		
		if( m_markTestFunc )
			return m_markTestFunc(ofs,canvas,scale,state,value,value2,alpha);
		else
		{
			if (canvas.contains(ofs))
				return m_bOpaque;
			else
				return false;
		}
	}

	//____ _defaultSize() ______________________________________________________________

	SizeSPX LambdaSkin::_defaultSize(int scale) const
	{
		if( m_defaultSize.isEmpty() )
			return SizeSPX(align(ptsToSpx(m_margin,scale)) + align(ptsToSpx(m_padding,scale)));
		else
			return align(ptsToSpx(m_defaultSize,scale));
	}

	//____ _dirtyRect() ________________________________________________________

	RectSPX LambdaSkin::_dirtyRect(const RectSPX& canvas, int scale, State newState, State oldState, float newValue, float oldValue,
								   float newValue2, float oldValue2, int newAnimPos, int oldAnimPos, float* pNewStateFractions,
								   float* pOldStateFractions) const
	{
		if( newState != oldState && !m_bIgnoresState )
			return canvas;

		if( (newValue != oldValue || newValue2 != oldValue2) && !m_bIgnoresValue )
			return canvas;

		return RectSPX();
	}

} // namespace wg
