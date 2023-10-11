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

		//____ Blueprint ______________________________________________________

		struct RigidPartXBlueprint
		{
			pts			ofs = 0;
			pts			length = 0;
			YSections	sections = YSections::None;
		};

		struct RigidPartYBlueprint
		{
			pts			ofs = 0;
			pts			length = 0;
			XSections	sections = XSections::None;
		};

		struct Blueprint
		{
			BlendMode	blendMode = BlendMode::Undefined;
			Rect		block;
			HiColor		color = HiColor::Undefined;
			Finalizer_p	finalizer = nullptr;
			Border		frame;
			Gradient	gradient;
			int			layer = -1;
			Border		margin;
			int			markAlpha = 1;
			Border		overflow;
			Border		padding;

			RigidPartXBlueprint	rigidPartX;
			RigidPartYBlueprint rigidPartY;

			Surface_p	surface;
		};

		//.____ Creation __________________________________________

		static StaticBlockSkin_p create(Surface* pSurface, const Border& frame = { 0 });
		static StaticBlockSkin_p create(Surface* pSurface, const Rect& block, const Border& frame = { 0 });
		static StaticBlockSkin_p create( const Blueprint& blueprint );


		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Internal ____________________________________________________

		SizeSPX		_defaultSize(int scale) const override;

		bool		_markTest(	const CoordSPX& ofs, const RectSPX& canvas, int scale, State state, 
								float value = 1.f, float value2 = -1.f, int alphaOverride = -1 ) const override;

		void		_render(	GfxDevice * pDevice, const RectSPX& canvas, int scale, State state, 
							float value = 1.f, float value2 = -1.f, int animPos = 0,
							float* pStateFractions = nullptr) const override;

	private:
		StaticBlockSkin( const Blueprint& blueprint );
		~StaticBlockSkin() {};

		void _updateOpacityFlag();
		bool _setRigidPartY(pts _ofs, pts _length, XSections sections);
		bool _setRigidPartX(pts _ofs, pts _length, YSections sections);


		Surface_p		m_pSurface;
		NinePatch		m_ninePatch;
		Border			m_gfxFrame;

		BlendMode		m_blendMode;
		HiColor			m_color;
		Gradient		m_gradient;

	};


} // namespace wg
#endif //WG_STATICBLOCKSKIN_DOT_H
