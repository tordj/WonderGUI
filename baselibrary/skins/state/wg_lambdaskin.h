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
#ifndef WG_LAMBDASKIN_DOT_H
#define WG_LAMBDASKIN_DOT_H
#pragma once

#include <wg_stateskin.h>

#include <functional>
#include <initializer_list>
#include <utility>

namespace wg
{

	class LambdaSkin;
	typedef	StrongPtr<LambdaSkin>	LambdaSkin_p;
	typedef	WeakPtr<LambdaSkin>	LambdaSkin_wp;

	class LambdaSkin : public StateSkin
	{
	public:

		struct Blueprint
		{
			std::function<bool(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold)> markTestFunc;
			SizeI	minSize;
			std::function<bool(const Rect& rect, const Size& canvasSize, State state)> opacityTestFunc;
			bool	opaque;
			SizeI	preferredSize;
			std::function<void(GfxDevice * pDevice, const Rect& canvas, State state)> renderFunc;
			std::function<bool(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
				float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
				float* pNewStateFractions, float* pOldStateFractions)>	dirtyRectFunc;
		};

		//.____ Creation __________________________________________

		static LambdaSkin_p	create();
		static LambdaSkin_p	create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		void			setMinSize( SizeI pointSize );
		void			setPreferredSize( SizeI pointSize );

		Size			minSize() const override;
		Size			preferredSize() const override;

		//.____ Control _________________________________________________

		void		setOpaque(bool bOpaque);
//		void		setContentShifting(bool bShifting);
//		void		setIgnoresFraction(bool bIgnoresFraction);

		void		setDirtyRectFunc(const std::function<bool(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
									float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
									float* pNewStateFractions, float* pOldStateFractions)>& function);
		void		setMarkTestFunc(const std::function<bool(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold)>& function);
		void		setOpacityTestFunc(const std::function<bool(const Rect& rect, const Size& canvasSize, State state)>& function);
		void		setRenderFunc(const std::function<void(GfxDevice * pDevice, const Rect& canvas, State state)>& function);

		//.____ Misc ____________________________________________________

		bool		isOpaque(State state) const override;
		bool		isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		Rect	dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
							float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
							float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;


	private:
		LambdaSkin();
		LambdaSkin(const Blueprint& blueprint);
		~LambdaSkin() {};

		SizeI		m_preferredSize;				// Points
		SizeI		m_minSize;						// Points

		std::function<bool(const Rect& canvas, State newState, State oldState, float newValue, float oldValue,
			float newValue2, float oldValue2, int newAnimPos, int oldAnimPos,
			float* pNewStateFractions, float* pOldStateFractions)>									m_dirtyRectFunc = nullptr;
		std::function<bool(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold)>	m_markTestFunc = nullptr;
		std::function<bool(const Rect& rect, const Size& canvasSize, State state)>					m_opacityTestFunc = nullptr;
		std::function<void(GfxDevice * pDevice, const Rect& canvas, State state)>					m_renderFunc = nullptr;
	};


} // namespace wg
#endif //WG_LAMBDASKIN_DOT_H
