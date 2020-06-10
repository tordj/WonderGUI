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

#include <wg_staticblockskin.h>
#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	const TypeInfo StaticBlockSkin::TYPEINFO = { "StaticBlockSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	StaticBlockSkin_p StaticBlockSkin::create(Surface* pSurface, const BorderI& frame)
	{
		return StaticBlockSkin_p(new StaticBlockSkin(pSurface,pSurface->size(),frame));
	}

	StaticBlockSkin_p StaticBlockSkin::create(Surface* pSurface, const RectI& block, const BorderI& frame)
	{
		return StaticBlockSkin_p(new StaticBlockSkin(pSurface, block, frame));
	}

	//____ constructor ____________________________________________________________

	StaticBlockSkin::StaticBlockSkin(Surface* pSurface, const RectI& block, const BorderI& frame)
	{
		m_pSurface = pSurface;
		m_block = block;
		m_bOpaque = m_pSurface->isOpaque();
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& StaticBlockSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ preferredSize() ______________________________________________________________

	Size StaticBlockSkin::preferredSize() const
	{
		return sizeForContent(m_block.size());
	}

	//____ render() ______________________________________________________________

	void StaticBlockSkin::render( GfxDevice * pDevice, const Rect& canvas, State state, float fraction ) const
	{
		if (!m_pSurface)
			return;

		pDevice->setBlitSource(m_pSurface);
		pDevice->blitNinePatch(canvas.px(), pointsToPixels(m_frame * 4 / m_pSurface->qpixPerPoint()), m_block, m_frame);
	}

	//____ markTest() _________________________________________________________

	bool StaticBlockSkin::markTest( const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction ) const
	{
		return markTestNinePatch(ofs, m_pSurface, m_block, canvas, opacityTreshold, m_frame);
	}


} // namespace wg
