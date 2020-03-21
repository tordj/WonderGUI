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

	const TypeInfo LambdaSkin::TYPEINFO = { "LambdaSkin", &ExtendedSkin::TYPEINFO };

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
		m_stateCompareFunc	= blueprint.stateCompareFunc;
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

	//____ setStateCompareFunc() ______________________________________________

	void LambdaSkin::setStateCompareFunc(const std::function<bool(State state1, State state2)>& function)
	{
		m_stateCompareFunc = function;
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

	void LambdaSkin::setRenderFunc(const std::function<void(GfxDevice * pDevice, const Rect& _canvas, State state)>& function)
	{
		m_renderFunc = function;
	}

	//____ isOpaque() _________________________________________________________

	bool LambdaSkin::isOpaque() const
	{
		return m_bOpaque;
	}

	bool LambdaSkin::isOpaque(State state) const
	{
		return m_bOpaque;
	}

	//____ isStateIdentical() _________________________________________________

	bool LambdaSkin::isStateIdentical(State state, State comparedTo) const
	{
		if (!ExtendedSkin::isStateIdentical(state, comparedTo))
			return false;
		else if (m_stateCompareFunc)
			return m_stateCompareFunc(state, comparedTo);
		else
			return false;
	}

	//____ _markTest() ________________________________________________________

	bool LambdaSkin::_markTest(const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold) const
	{
		if (m_markTestFunc)
			return m_markTestFunc(reinterpret_cast<const Coord&>(ofs), reinterpret_cast<const Rect&>(canvas), state, opacityTreshold);
		else
			return (opacityTreshold <= 0 || (m_bOpaque && opacityTreshold < 256));
	}

	//____ _isOpaque() ________________________________________________________

	bool LambdaSkin::_isOpaque(const RectI& rect, const SizeI& canvasSize, State state) const
	{
		if (m_bOpaque)
			return true;
		else if (m_opacityTestFunc)
			return m_opacityTestFunc(reinterpret_cast<const Rect&>(rect), reinterpret_cast<const Size&>(canvasSize), state);
		else
			return false;
	}

	//____ _render() __________________________________________________________

	void LambdaSkin::_render(GfxDevice * pDevice, const RectI& canvas, State state) const
	{
		if (m_renderFunc)
			m_renderFunc(pDevice, reinterpret_cast<const Rect&>(canvas), state);
	}

	//____ _minSize() _________________________________________________________

	SizeI LambdaSkin::_minSize() const
	{
		return pointsToRawAligned(m_minSize);
	}

	//____ _preferredSize() ___________________________________________________

	SizeI LambdaSkin::_preferredSize() const
	{
		return pointsToRawAligned(m_preferredSize);
	}
}