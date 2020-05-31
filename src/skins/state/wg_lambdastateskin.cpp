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

#include <wg_lambdastateskin.h>
#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>

namespace wg
{

	using namespace Util;

	const TypeInfo LambdaStateSkin::TYPEINFO = { "LambdaStateSkin", &StateSkin::TYPEINFO };

	//____ create() ___________________________________________________________

	LambdaStateSkin_p LambdaStateSkin::create()
	{
		return LambdaStateSkin_p(new LambdaStateSkin());
	}

	LambdaStateSkin_p LambdaStateSkin::create(const Blueprint& blueprint)
	{
		return LambdaStateSkin_p(new LambdaStateSkin(blueprint));
	}


	//____ constructor ________________________________________________________

	LambdaStateSkin::LambdaStateSkin()
	{
	}

	LambdaStateSkin::LambdaStateSkin(const Blueprint& blueprint)
	{
		m_markTestFunc		= blueprint.markTestFunc;
		m_minSize			= blueprint.minSize;
		m_opacityTestFunc	= blueprint.opacityTestFunc;
		m_bOpaque			= blueprint.opaque;
		m_preferredSize		= blueprint.preferredSize;
		m_renderFunc		= blueprint.renderFunc;
		m_stateCompareFunc	= blueprint.stateCompareFunc;
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& LambdaStateSkin::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setMinSize() _______________________________________________________

	void LambdaStateSkin::setMinSize(SizeI pointSize)
	{
		m_minSize = pointSize;
	}

	//____ setPreferredSize() _________________________________________________

	void LambdaStateSkin::setPreferredSize(SizeI pointSize)
	{
		m_preferredSize = pointSize;
	}

	//____ setOpaque() ________________________________________________________

	void LambdaStateSkin::setOpaque(bool bOpaque)
	{
		m_bOpaque = bOpaque;
	}

	//____ setStateCompareFunc() ______________________________________________

	void LambdaStateSkin::setStateCompareFunc(const std::function<bool(State state1, State state2)>& function)
	{
		m_stateCompareFunc = function;
	}

	//____ setMarkTestFunc() __________________________________________________

	void LambdaStateSkin::setMarkTestFunc(const std::function<bool(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold)>& function)
	{
		m_markTestFunc = function;
	}

	//____ setOpacityTestFunc() _______________________________________________

	void LambdaStateSkin::setOpacityTestFunc(const std::function<bool(const Rect& rect, const Size& canvasSize, State state)>& function)
	{
		m_opacityTestFunc = function;
	}

	//____ setRenderFunc() ____________________________________________________

	void LambdaStateSkin::setRenderFunc(const std::function<void(GfxDevice * pDevice, const Rect& canvas, State state)>& function)
	{
		m_renderFunc = function;
	}

	//____ isOpaque() _________________________________________________________

	bool LambdaStateSkin::isOpaque(State state) const
	{
		return m_bOpaque;
	}

	bool LambdaStateSkin::isOpaque(const Rect& rect, const Size& canvasSize, State state) const
	{
		if (m_bOpaque)
			return true;
		else if (m_opacityTestFunc)
			return m_opacityTestFunc(rect, canvasSize, state);
		else
			return false;
	}

	//____ isStateIdentical() _________________________________________________

	bool LambdaStateSkin::isStateIdentical(State state, State comparedTo, float fraction) const
	{
		if (!StateSkin::isStateIdentical(state, comparedTo))
			return false;
		else if (m_stateCompareFunc)
			return m_stateCompareFunc(state, comparedTo);
		else
			return false;
	}

	//____ markTest() ________________________________________________________

	bool LambdaStateSkin::markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction) const
	{
		if (m_markTestFunc)
			return m_markTestFunc(ofs, canvas, state, opacityTreshold);
		else
			return (opacityTreshold <= 0 || (m_bOpaque && opacityTreshold < 256));
	}

	//____ render() __________________________________________________________

	void LambdaStateSkin::render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction) const
	{
		if (m_renderFunc)
			m_renderFunc(pDevice, canvas, state);
	}

	//____ minSize() _________________________________________________________

	Size LambdaStateSkin::minSize() const
	{
		return m_minSize;
	}

	//____ preferredSize() ___________________________________________________

	Size LambdaStateSkin::preferredSize() const
	{
		return m_preferredSize;
	}
}
