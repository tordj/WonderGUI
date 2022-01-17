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
#include <wg_gradient.h>
#include <vector>

namespace wg
{

	class FrameMeterSkin;

	typedef	StrongPtr<FrameMeterSkin>	FrameMeterSkin_p;
	typedef	WeakPtr<FrameMeterSkin>		FrameMeterSkin_wp;


	class FrameMeterSkin : public Skin
	{
	public:
		//____ Blueprint ______________________________________________________

		struct FrameBP
		{
			Coord	coord;
			int		duration;					// Millisec
			GfxFlip	flip = GfxFlip::Normal;
		};

		struct Blueprint
		{
			BlendMode				blendMode = BlendMode::Undefined;
			HiColor					color = HiColor::Undefined;
			Border					contentPadding;
			std::vector<FrameBP>	frames;			// Mandatory
			Border					gfxPadding;
			Gradient				gradient;
			int						layer = -1;
			int						markAlpha = 1;
			Border					overflow;

			Size					size;			// Mandatory
			Surface_p				surface;		// Mandatory

		};

		//.____ Creation __________________________________________

		static FrameMeterSkin_p create( const Blueprint& blueprint );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ____________________________________________________

		SizeSPX		_preferredSize(int scale) const override;
		SizeSPX		_minSize(int scale) const override;

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, 
								float value = 1.f, float value2 = -1.f) const override;

		void 		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
								float value = 1.f, float value2 = -1.f, int animPos = 0,
								float* pStateFractions = nullptr) const override;

		RectSPX		_dirtyRect(	const RectSPX& canvas, int scale, State newState, State oldState, float newValue = 1.f, float oldValue = 1.f,
								float newValue2 = -1.f, float oldValue2 = -1.f, int newAnimPos = 0, int oldAnimPos = 0,
								float* pNewStateFractions = nullptr, float* pOldStateFractions = nullptr) const override;

	private:
		FrameMeterSkin( const Blueprint& bp );
		~FrameMeterSkin() {};

		class Frame
		{
        public:
            Frame( Coord _coord, int _duration, int _timestamp, GfxFlip _flip )
            : coord(_coord), duration(_duration), timestamp(_timestamp), flip(_flip) {}
            
			Coord	coord;
			int		duration;					// Millisec
			int		timestamp;					// Millisec
			GfxFlip	flip = GfxFlip::Normal;

		};



		void		_updateOpacityFlag();

		const Frame * _valueToFrame(float value) const;

		Border			m_gfxPadding;
		bool			m_bAllFramesOpaque = false;

		BlendMode		m_blendMode;
		HiColor			m_color;
		Gradient		m_gradient;

		int					m_duration;		// Millisec
		Size				m_size;
		Surface_p			m_pSurface;
		std::vector<Frame>	m_frames;
	};


} // namespace wg
#endif //WG_FRAMEMETERSKIN_DOT_H
