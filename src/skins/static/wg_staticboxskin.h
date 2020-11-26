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
#ifndef WG_STATICBOXSKIN_DOT_H
#define WG_STATICBOXSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>

namespace wg
{

	class StaticBoxSkin;

	typedef	StrongPtr<StaticBoxSkin>	StaticBoxSkin_p;
	typedef	WeakPtr<StaticBoxSkin>		StaticBoxSkin_wp;


	class StaticBoxSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static StaticBoxSkin_p create(BorderI frame, HiColor fillColor, HiColor frameColor);

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }


		//.____ Geometry _________________________________________________

		Size	minSize() const override;
		Size	preferredSize() const override;

		//.____ Misc ____________________________________________________

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0, 
							float* pStateFractions = nullptr) const override;

	private:
		StaticBoxSkin(BorderI frame, HiColor fillColor, HiColor frameColor);
		~StaticBoxSkin() {};

		void		_updateOpaqueFlag();

		BorderI		m_frame;							// In points
		BlendMode	m_blendMode = BlendMode::Blend;

		HiColor		m_fillColor;
		HiColor		m_frameColor;
	};


} // namespace wg
#endif //WG_STATICBOXSKIN_DOT_H
