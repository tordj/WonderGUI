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
#ifndef WG_FRAMEMETERSKIN_DOT_H
#define WG_FRAMEMETERSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_canimframes.h>

namespace wg
{

	class FrameMeterSkin;

	typedef	StrongPtr<FrameMeterSkin>	FrameMeterSkin_p;
	typedef	WeakPtr<FrameMeterSkin>		FrameMeterSkin_wp;


	class FrameMeterSkin : public Skin, protected CAnimFrames::Holder
	{
	public:
		//.____ Creation __________________________________________

		static FrameMeterSkin_p create();

		//.____ Components _______________________________________

		CAnimFrames		frames;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size		preferredSize() const override;
		Size		minSize() const override;

		//.____ Appearance ____________________________________________________

		void		setGfxPadding(BorderI padding);
		Border		gfxPadding() const { return m_gfxPadding; }

		//.____ Misc ____________________________________________________

		bool		markTest(const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, float fraction = 1.f) const override;
		void 		render(GfxDevice * pDevice, const Rect& canvas, State state, float fraction = 1.f) const override;

		Rect		fractionChangeRect(const Rect& canvas, State state, float oldFraction, float newFraction) const override;

	private:
		FrameMeterSkin();
		~FrameMeterSkin() {};

		const AnimFrame * _fractionToFrame(float fraction) const;

		void		_didAddEntries(AnimFrame* pEntry, int nb) override;
		void		_didMoveEntries(AnimFrame* pFrom, AnimFrame* pTo, int nb) override;
		void		_willEraseEntries(AnimFrame* pEntry, int nb) override;
		void		_didSetAnimFrameSize(CAnimFrames* pComponent) override;
		void		_didSetAnimSurface(CAnimFrames* pComponent) override;

		Object*		_object() override;

		BorderI		m_gfxPadding;
	};


} // namespace wg
#endif //WG_FRAMEMETERSKIN_DOT_H
