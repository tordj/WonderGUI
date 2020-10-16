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

#include <wg_spinanimskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_msgrouter.h>

#include <cmath>

namespace wg
{

	const TypeInfo SpinAnimSkin::TYPEINFO = { "SpinAnimSkin", &Skin::TYPEINFO };

	using namespace Util;

	//____ create() _______________________________________________________________

	SpinAnimSkin_p SpinAnimSkin::create(	Surface * pSurface, Size preferredSize, int cycleDuration, CoordF srcCenter,	CoordF dstCenter, 
											float fromDegrees, float toDegrees, float zoom, const BorderI& gfxPadding, 
											const BorderI& contentPadding)
	{
		return SpinAnimSkin_p(new SpinAnimSkin(pSurface, preferredSize, cycleDuration, srcCenter, dstCenter, fromDegrees, toDegrees, zoom, gfxPadding, contentPadding));
	}

	//____ constructor ____________________________________________________________

	SpinAnimSkin::SpinAnimSkin(	Surface * pSurface, Size preferredSize, int cycleDuration, CoordF srcCenter, CoordF dstCenter, float fromDegrees, 
									float toDegrees, float zoom, const BorderI& gfxPadding, const BorderI& contentPadding) : 
		m_pSurface(pSurface),
		m_preferredSize(preferredSize),
		m_srcCenter(srcCenter),
		m_dstCenter(dstCenter),
		m_fromDegrees(fromDegrees),
		m_toDegrees(toDegrees),
		m_zoom(zoom),
		m_gfxPadding(gfxPadding),
		m_cycleDuration(cycleDuration)
	{
		//TODO: Also take frame opacity into account.

		m_bOpaque = pSurface->isOpaque();
		m_contentPadding = contentPadding;

		for (int i = 0; i < StateBits_Nb; i++)
			m_animationCycles[i] = cycleDuration;
	}

	//____ destructor _________________________________________________________

	SpinAnimSkin::~SpinAnimSkin()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& SpinAnimSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setCycleDuration() _________________________________________________

	void SpinAnimSkin::setCycleDuration(int millisec)
	{
		m_cycleDuration = millisec;

		for (int i = 0; i < StateBits_Nb; i++)
			m_animationCycles[i] = millisec;
	}

	//____ render() ______________________________________________________________

	void SpinAnimSkin::render(GfxDevice * pDevice, const Rect& _canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
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

		float	degrees = m_fromDegrees + (m_toDegrees - m_fromDegrees)*animPos/(float)m_cycleDuration;

		if (degrees < 0.f)
			degrees = 360.f + (float) fmod(degrees, 360.f);
		else if (degrees >= 360.f)
			degrees = (float) fmod(degrees, 360.f);


		pDevice->setBlitSource(m_pSurface);
		pDevice->rotScaleBlit(_canvas.px(), degrees, zoom, m_srcCenter, m_dstCenter);
	}

	//____ preferredSize() ______________________________________________________________

	Size SpinAnimSkin::preferredSize() const
	{
		return m_preferredSize;
	}

	//____ markTest() _________________________________________________________

	bool SpinAnimSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		if (!canvas.contains(ofs))
			return false;

		if (m_bOpaque)
			return true;

		//TODO: Implement!

		return true;
	}


	//____ dirtyRect() ________________________________________________________

	Rect SpinAnimSkin::dirtyRect(const Rect& _canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		if (newAnimPos == oldAnimPos)
			return Rect();

		return _canvas;
	}

	//____ animationLength() __________________________________________________

	int SpinAnimSkin::animationLength(State state) const
	{
		return m_cycleDuration;
	}



} // namespace wg
