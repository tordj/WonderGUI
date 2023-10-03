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

	CustomSkin::CustomSkin( const Blueprint& blueprint )
	{
		m_bOpaque			= blueprint.opaque;
		m_padding	= blueprint.padding;
		m_overflow			= blueprint.overflow;
	
		m_markTestFunc		= blueprint.markTestFunc;
		m_renderFunc		= blueprint.renderFunc;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CustomSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ _render() ______________________________________________________________

	void CustomSkin::_render( GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if( m_renderFunc )
			m_renderFunc(pDevice,canvas,scale,state,value,value2);
	}

	//____ _markTest() _________________________________________________________

	bool CustomSkin::_markTest( const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, float value, float value2, int alphaOverride ) const
	{
		if( m_markTestFunc )
			return m_markTestFunc(ofs,canvas,scale,state,value,value2,alphaOverride);
		else
			return m_bOpaque;
	}


} // namespace wg
