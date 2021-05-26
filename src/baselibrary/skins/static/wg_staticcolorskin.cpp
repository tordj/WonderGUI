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
		return StaticColorSkin_p(new StaticColorSkin(col));
	}

	//____ constructor ____________________________________________________________

	StaticColorSkin::StaticColorSkin( HiColor col )
	{
		m_color = col;
		m_bOpaque = (m_color.a == 4096);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticColorSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setBlendMode() _____________________________________________________

	void StaticColorSkin::setBlendMode(BlendMode mode)
	{
		m_blendMode = mode;
		if (mode == BlendMode::Replace)
			m_bOpaque = true;
		else if (mode == BlendMode::Blend)
			m_bOpaque = (m_color.a == 4096);
		else
			m_bOpaque = false;
	}

	//____ _render() ______________________________________________________________

	void StaticColorSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		RenderSettings settings(pDevice, m_layer, m_blendMode);

		pDevice->fill(canvas, m_color);
	}

	//____ _markTest() _________________________________________________________

	bool StaticColorSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, float value, float value2) const
	{
		return ( canvas.contains(ofs) && m_color.a >= opacityTreshold );
	}


} // namespace wg