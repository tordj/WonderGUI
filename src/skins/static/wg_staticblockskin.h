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

		static StaticBlockSkin_p create(Surface* pSurface, const BorderI& frame = { 0 });
		static StaticBlockSkin_p create(Surface* pSurface, const RectI& block, const BorderI& frame = { 0 });

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		Size	preferredSize() const override;

		//.____ Appearance _________________________________________________

		void		setBlendMode(BlendMode mode);
		BlendMode	blendMode() const { return m_blendMode; }

		//.____ Misc ____________________________________________________

		bool		markTest(	const Coord& ofs, const Rect& canvas, State state, int opacityTreshold, 
								float value = 1.f, float value2 = -1.f) const override;

		void		render(	GfxDevice * pDevice, const Rect& canvas, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

	private:
		StaticBlockSkin(Surface* pSurface, const RectI& block, const BorderI& frame = { 0 });
		~StaticBlockSkin() {};

		Surface_p		m_pSurface;
		RectI			m_block;
		BorderI			m_frame;
		BlendMode		m_blendMode = BlendMode::Blend;
	};


} // namespace wg
#endif //WG_STATICBLOCKSKIN_DOT_H
