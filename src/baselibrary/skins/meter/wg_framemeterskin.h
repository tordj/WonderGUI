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
#include <wg_gradient.h>

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

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setColor(HiColor tintColor);
		HiColor		color() const { return m_color; }

		void		setGradient(const Gradient& gradient);
		Gradient	gradient() const { return m_gradient; }

		void		setGfxPadding(BorderI padding);
		Border		gfxPadding() const { return m_gfxPadding; }

		//.____ Misc ____________________________________________________

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void 		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

		Rect		dirtyRect(	const Rect& canvas, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
								float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
								float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		FrameMeterSkin();
		~FrameMeterSkin() {};

		void		_updateOpacityFlag();

		const AnimFrame * _valueToFrame(float value) const;

		void		_didAddEntries(AnimFrame* pEntry, int nb) override;
		void		_didMoveEntries(AnimFrame* pFrom, AnimFrame* pTo, int nb) override;
		void		_willEraseEntries(AnimFrame* pEntry, int nb) override;
		void		_didSetAnimFrameSize(CAnimFrames* pComponent) override;
		void		_didSetAnimSurface(CAnimFrames* pComponent) override;

		Object*		_object() override;

		BorderI		m_gfxPadding;
		bool		m_bAllFramesOpaque = false;

		BlendMode		m_blendMode = BlendMode::Undefined;
		HiColor			m_color = Color::White;
		Gradient		m_gradient;
		bool			m_bGradient = false;
	};


} // namespace wg
#endif //WG_FRAMEMETERSKIN_DOT_H
