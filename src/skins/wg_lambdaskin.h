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
#ifndef WG_LAMDASKIN_DOT_H
#define WG_LAMDASKIN_DOT_H
#pragma once

#include <wg_extendedskin.h>

#include <functional>
#include <initializer_list>
#include <utility>

namespace wg
{




	class LambdaSkin;
	typedef	StrongPtr<LambdaSkin>	LambdaSkin_p;

	class LambdaSkin : public ExtendedSkin
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
			std::function<bool(State state1, State state2)>	stateCompareFunc;
		};

		//.____ Creation __________________________________________

		static LambdaSkin_p	create();
		static LambdaSkin_p	create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		bool				isInstanceOf(const char * pClassName) const override;
		const char *		className(void) const override;
		static const char	CLASSNAME[];
		static LambdaSkin_p	cast(Object * pObject);

		//.____ Geometry _________________________________________________

		void			setMinSize( SizeI pointSize );
		void			setPreferredSize( SizeI pointSize );

		//.____ Control _________________________________________________

		void		setOpaque(bool bOpaque);

		void		setStateCompareFunc(const std::function<bool(State state1, State state2)>& function);
		void		setMarkTestFunc(const std::function<bool(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold)>& function);
		void		setOpacityTestFunc(const std::function<bool(const Rect& rect, const Size& canvasSize, State state)>& function);
		void		setRenderFunc(const std::function<void(GfxDevice * pDevice, const Rect& canvas, State state)>& function);

		//.____ Misc ____________________________________________________

		bool		isOpaque() const override;
		bool		isOpaque(State state) const override;

		bool		isStateIdentical(State state, State comparedTo) const override;

		//.____ Internal ______________________________________________________

		bool		_markTest(const CoordI& ofs, const RectI& canvas, State state, int opacityTreshold) const override;

		bool		_isOpaque(const RectI& rect, const SizeI& canvasSize, State state) const override;

		void		_render(GfxDevice * pDevice, const RectI& _canvas, State state) const override;

		SizeI		_minSize() const override;
		SizeI		_preferredSize() const override;


	private:
		LambdaSkin();
		LambdaSkin(const Blueprint& blueprint);
		~LambdaSkin() {};

		bool		m_bOpaque = false;
		SizeI		m_preferredSize;				// Points
		SizeI		m_minSize;						// Points

		std::function<bool(State state, State comparedTo)>											m_stateCompareFunc = nullptr;
		std::function<bool(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold)>	m_markTestFunc = nullptr;
		std::function<bool(const Rect& rect, const Size& canvasSize, State state)>					m_opacityTestFunc = nullptr;
		std::function<void(GfxDevice * pDevice, const Rect& _canvas, State state)>					m_renderFunc = nullptr;
	};


} // namespace wg
#endif //WG_LAMDASKIN_DOT_H


