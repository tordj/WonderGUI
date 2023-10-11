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

#include <wg_staticcolorskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_skin.impl.h>

namespace wg
{

	const TypeInfo StaticColorSkin::TYPEINFO = { "StaticColorSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticColorSkin_p StaticColorSkin::create( HiColor col )
	{
		if (!col.isValid())
			return nullptr;

		Blueprint blueprint;
		blueprint.color = col;
		return StaticColorSkin_p(new StaticColorSkin(blueprint));
	}

	StaticColorSkin_p StaticColorSkin::create(const Blueprint& blueprint)
	{
		if (!blueprint.color.isValid())
			return nullptr;

		return StaticColorSkin_p(new StaticColorSkin(blueprint));
	}

	//____ constructor ____________________________________________________________

	StaticColorSkin::StaticColorSkin( const Blueprint& bp ) : Skin(bp)
	{
		m_color				= bp.color;
		m_bOpaque			= (m_color.a == 4096);
		m_blendMode			= bp.blendMode;

		if(!m_margin.isEmpty())
			m_bOpaque = false;
		else if (m_blendMode == BlendMode::Replace)
			m_bOpaque = true;
		else if (m_blendMode == BlendMode::Blend)
			m_bOpaque = (m_color.a == 4096);
		else
			m_bOpaque = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticColorSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() ______________________________________________________________

	void StaticColorSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RectSPX canvasWithoutMargin = canvas - align(ptsToSpx(m_margin,scale));
		
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		pDevice->fill(canvasWithoutMargin, m_color);
	}

	//____ _markTest() _________________________________________________________

	bool StaticColorSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2, int alphaOverride ) const
	{
		int alpha = alphaOverride == -1 ? m_markAlpha : alphaOverride;

		RectSPX canvasWithoutMargin = canvas - align(ptsToSpx(m_margin,scale));

		return ( canvasWithoutMargin.contains(ofs) && m_color.a >= alpha );
	}


} // namespace wg
