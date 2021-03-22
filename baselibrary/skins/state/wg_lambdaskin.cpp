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

namespace wg
{

	using namespace Util;

	const TypeInfo LambdaSkin::TYPEINFO = { "LambdaSkin", &StateSkin::TYPEINFO };

	//____ create() ___________________________________________________________

	LambdaSkin_p LambdaSkin::create()
	{
		return LambdaSkin_p(new LambdaSkin());
	}

	LambdaSkin_p LambdaSkin::create(const Blueprint& blueprint)
	{
		return LambdaSkin_p(new LambdaSkin(blueprint));
	}


	//____ constructor ________________________________________________________

	LambdaSkin::LambdaSkin()
	{
	}

	LambdaSkin::LambdaSkin(const Blueprint& blueprint)
	{
		m_markTestFunc		= blueprint.markTestFunc;
		m_minSize			= blueprint.minSize;
		m_opacityTestFunc	= blueprint.opacityTestFunc;
		m_bOpaque			= blueprint.opaque;
		m_preferredSize		= blueprint.preferredSize;
		m_renderFunc		= blueprint.renderFunc;
		m_dirtyRectFunc		= blueprint.dirtyRectFunc;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& LambdaSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setMinSize() _______________________________________________________

	void LambdaSkin::setMinSize(SizeI pointSize)
	{
		m_minSize = pointSize;
	}

	//____ setPreferredSize() _________________________________________________

	void LambdaSkin::setPreferredSize(SizeI pointSize)
	{
		m_preferredSize = pointSize;
	}

	//____ setOpaque() ________________________________________________________

	void LambdaSkin::setOpaque(bool bOpaque)
	{
		m_bOpaque = bOpaque;
	}

	//____ setDirtyRectFunc() ______________________________________________

	void LambdaSkin::setDirtyRectFunc(const std::function<bool(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
																float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
																float* pNewStateFractions, float* pOldStateFractions)>& function)
	{
		m_dirtyRectFunc = function;
	}

	//____ setMarkTestFunc() __________________________________________________

	void LambdaSkin::setMarkTestFunc(const std::function<bool(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold)>& function)
	{
		m_markTestFunc = function;
	}

	//____ setOpacityTestFunc() _______________________________________________

	void LambdaSkin::setOpacityTestFunc(const std::function<bool(const Rect& rect, const Size& canvasSize, State state)>& function)
	{
		m_opacityTestFunc = function;
	}

	//____ setRenderFunc() ____________________________________________________

	void LambdaSkin::setRenderFunc(const std::function<void(GfxDevice * pDevice, const Rect& canvas, State state)>& function)
	{
		m_renderFunc = function;
	}

	//____ isOpaque() _________________________________________________________

	bool LambdaSkin::isOpaque(State state) const
	{
		return m_bOpaque;
	}

	bool LambdaSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		if (m_bOpaque)
			return true;
		else if (m_opacityTestFunc)
			return m_opacityTestFunc(rect, canvasSize, state);
		else
			return false;
	}

	//____ dirtyRect() ______________________________________________________

	Rect LambdaSkin::dirtyRect(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
		float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
		float* pNewStateFractions, float* pOldStateFractions) const
	{
		Rect r = StateSkin::dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2,
			newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);

		if (!r.isEmpty())
			return r;
		else if (m_dirtyRectFunc)
			return StateSkin::dirtyRect(canvas, newState, oldState, newValue, oldValue, newValue2, oldValue2,
				newAnimPos, oldAnimPos, pNewStateFractions, pOldStateFractions);
		else return canvas;
	}


	//____ markTest() ________________________________________________________

	bool LambdaSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float value, float value2) const
	{
		if (m_markTestFunc)
			return m_markTestFunc(ofs, canvas, state, opacityTreshold);
		else
			return (opacityTreshold <= 0 || (m_bOpaque && opacityTreshold < 256));
	}

	//____ render() __________________________________________________________

	void LambdaSkin::render(GfxDevice * pDevice, const Rect& canvas, State state, float value, float value2, int animPos, float* pStateFractions) const
	{
		if (m_renderFunc)
			m_renderFunc(pDevice, canvas, state);
	}

	//____ minSize() _________________________________________________________

	Size LambdaSkin::minSize() const
	{
		return m_minSize;
	}

	//____ preferredSize() ___________________________________________________

	Size LambdaSkin::preferredSize() const
	{
		return m_preferredSize;
	}
}
