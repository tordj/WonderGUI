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

#include <wg_spinmeterskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_context.h>

namespace wg
{

	const TypeInfo SpinMeterSkin::TYPEINFO = { "SpinMeterSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	SpinMeterSkin_p SpinMeterSkin::create(	Surface * pSurface, Size preferredSize, CoordF srcCenter,	CoordF dstCenter, 
											float fromDegrees, float toDegrees, float zoom, const BorderI& gfxPadding, 
											const BorderI& contentPadding)
	{
		return SpinMeterSkin_p(new SpinMeterSkin(pSurface, preferredSize, srcCenter, dstCenter, fromDegrees, toDegrees, zoom, gfxPadding, contentPadding));
	}

	//____ constructor ____________________________________________________________

	SpinMeterSkin::SpinMeterSkin(	Surface * pSurface, Size preferredSize, CoordF srcCenter, CoordF dstCenter, float fromDegrees, 
									float toDegrees, float zoom, const BorderI& gfxPadding, const BorderI& contentPadding) : 
		m_pSurface(pSurface),
		m_preferredSize(preferredSize),
		m_srcCenter(srcCenter),
		m_dstCenter(dstCenter),
		m_fromDegrees(fromDegrees),
		m_toDegrees(toDegrees),
		m_zoom(zoom),
		m_gfxPadding(gfxPadding),
		m_contentPadding(contentPadding)
	{

		//TODO: Also take frame opacity into account.

		m_bOpaque = pSurface->isOpaque();
		m_bIgnoresFraction = false;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SpinMeterSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ isStateIdentical() _________________________________________________

	bool SpinMeterSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		return true;
	}

	//____ isOpaque() ______________________________________________________________

	bool SpinMeterSkin::isOpaque() const
	{
		return m_bOpaque;
	}

	bool SpinMeterSkin::isOpaque(State state) const
	{
		return m_bOpaque;
	}

	//____ isOpaque() ______________________________________________________________

	bool SpinMeterSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		return m_bOpaque;
	}

	//____ render() ______________________________________________________________

	void SpinMeterSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float fraction) const
	{
		float	zoom = m_zoom * Base::activeContext()->scale();

		// Scale zoom to fit content of preferred size into canvas size.

		Rect canvas = _canvas;
		if (!m_preferredSize.isEmpty() && canvas.size() != m_preferredSize)
		{
			float xScale = float(_canvas.w) / float(m_preferredSize.w);
			float yScale = float(_canvas.h) / float(m_preferredSize.h);
			float scale = min(xScale, yScale);

			MU w = m_preferredSize.w * scale;
			MU h = m_preferredSize.h * scale;

			canvas.x += (w - canvas.w) / 2;
			canvas.y += (h - canvas.h) / 2;
			canvas.w = w;
			canvas.h = h;

			zoom *= scale;
		}

		//

		canvas -= m_gfxPadding;

		float	degrees = m_fromDegrees + (m_toDegrees - m_fromDegrees)*fraction;

		if (degrees < 0.f)
			degrees = 360.f + fmod(degrees, 360.f);
		else if (degrees >= 360.f)
			degrees = fmod(degrees, 360.f);


		SizeI srcSize = m_pSurface->size();

		pDevice->setBlitSource(m_pSurface);
		pDevice->rotScaleBlit(_canvas.px(), degrees, zoom, m_srcCenter, m_dstCenter);
	}

	//____ minSize() ______________________________________________________________

	Size SpinMeterSkin::minSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ preferredSize() ______________________________________________________________

	Size SpinMeterSkin::preferredSize() const
	{
		return m_preferredSize;
	}

	//____ setContentPadding() ________________________________________________

	void SpinMeterSkin::setContentPadding(const BorderI& padding)
	{
		m_contentPadding = padding;
	}

	//____ contentPadding() ______________________________________________________________

	Border SpinMeterSkin::contentPadding(State state) const
	{
		return Border(m_contentPadding).aligned();
	}

	//____ contentPaddingSize() ______________________________________________________________

	Size SpinMeterSkin::contentPaddingSize() const
	{
		return Size(Border(m_contentPadding).aligned());
	}

	//____ contentOfs() ______________________________________________________________

	Coord SpinMeterSkin::contentOfs(State state) const
	{
		return Coord(m_contentPadding.left, m_contentPadding.top).aligned();
	}

	//____ sizeForContent() ___________________________________________________

	Size SpinMeterSkin::sizeForContent(const Size& contentSize) const
	{
		return contentSize + Size(Border(m_contentPadding).aligned());
	}

	//____ contentRect() ______________________________________________________

	Rect SpinMeterSkin::contentRect(const Rect& canvas, State state) const
	{
		return (canvas - Border(m_contentPadding).aligned()).aligned();
	}


	//____ markTest() _________________________________________________________

	bool SpinMeterSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		if (!canvas.contains(ofs))
			return false;

		if (m_bOpaque)
			return true;

		//TODO: Implement!

		return true;
	}

	//____ fractionChangeRect() ______________________________________

	Rect SpinMeterSkin::fractionChangeRect(const Rect& _canvas, State state, float oldFraction, float newFraction) const
	{
		return _canvas;
	}

} // namespace wg
