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

	StaticColorSkin::StaticColorSkin( const Blueprint& blueprint )
	{
		m_color				= blueprint.color;
		m_bOpaque			= (m_color.a == 4096);
		m_contentPadding	= blueprint.padding;
		m_layer				= blueprint.layer;
		m_markAlpha			= blueprint.markAlpha;
		m_overflow			= blueprint.overflow;
		m_blendMode			= blueprint.blendMode;

		if (m_blendMode == BlendMode::Replace)
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
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		pDevice->fill(canvas, m_color);
	}

	//____ _markTest() _________________________________________________________

	bool StaticColorSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2) const
	{
		return ( canvas.contains(ofs) && m_color.a >= m_markAlpha );
	}


} // namespace wg
