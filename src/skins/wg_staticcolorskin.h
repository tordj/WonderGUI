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
#ifndef WG_STATICStaticColorSkin_DOT_H
#define WG_STATICStaticColorSkin_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>

namespace wg
{

	class StaticColorSkin;

	typedef	StrongPtr<StaticColorSkin>	StaticColorSkin_p;


	class StaticColorSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static StaticColorSkin_p create( Color col );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size	minSize() const override;
		Size	preferredSize() const override;

		Size	sizeForContent(const Size& contentSize) const override;
		Border	contentPadding() const override;
		Size	contentPaddingSize() const override;
		Coord	contentOfs(State state) const override;
		Rect	contentRect(const Rect& canvas, State state) const override;

		//.____ Misc ____________________________________________________

		bool	isOpaque() const override;
		bool	isOpaque(State state) const override;
		bool	isOpaque(const Rect& rect, const Size& canvasSize, State state) const override;

		bool	isStateIdentical(State state, State comparedTo) const override;

		bool	markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold) const override;
		void 	render(GfxDevice * pDevice, const Rect& canvas, State state) const override;

	private:
		StaticColorSkin( Color col );
		~StaticColorSkin() {};

		Color		m_color;

	};


} // namespace wg
#endif //WG_STATICCOLORSKIN_DOT_H
