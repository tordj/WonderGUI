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
#ifndef WG_STATICBLOCKSKIN_DOT_H
#define WG_STATICBLOCKSKIN_DOT_H
#pragma once

#include <wg_skin.h>
#include <wg_color.h>
#include <wg_gradient.h>
#include <wg_surface.h>

namespace wg
{

	class StaticBlockSkin;

	typedef	StrongPtr<StaticBlockSkin>	StaticBlockSkin_p;
	typedef	WeakPtr<StaticBlockSkin>	StaticBlockSkin_wp;


	class StaticBlockSkin : public Skin
	{
	public:
		//.____ Creation __________________________________________

		static StaticBlockSkin_p create(Surface* pSurface, const Border& frame = { 0 });
		static StaticBlockSkin_p create(Surface* pSurface, const Rect& block, const Border& frame = { 0 });

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		void		setColor(HiColor color);
		void		setGradient(const Gradient& gradient);

		HiColor		color() const { return m_color; }
		Gradient	gradient() const { return m_gradient; }

		bool		setRigidPartX(pts ofs, pts length, YSections sections);
		bool		setRigidPartY(pts ofs, pts length, XSections sections);


		//.____ Internal ____________________________________________________

		SizeSPX		_preferredSize(int scale) const override;

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

	private:
		StaticBlockSkin(Surface* pSurface, const Rect& block, const Border& frame = { 0 });
		~StaticBlockSkin() {};

		void _updateOpacityFlag();

		Surface_p		m_pSurface;
		NinePatch		m_ninePatch;
		Border			m_gfxFrame;

		BlendMode		m_blendMode = BlendMode::Undefined;
		HiColor			m_color = Color::White;
		Gradient		m_gradient;
		bool			m_bGradient = false;

	};


} // namespace wg
#endif //WG_STATICBLOCKSKIN_DOT_H
