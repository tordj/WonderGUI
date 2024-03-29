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

#ifndef	WG_GLOW_DOT_H
#define	WG_GLOW_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_component.h>
#include <wg_geo.h>
#include <wg_skin.h>
#include <wg_util.h>

namespace wg
{

	//____ Glow _____________________________________________________________

	class Glow : public Component
	{
	public:
		Glow(Widget* pWidget);
		virtual ~Glow() {};


		//____ Blueprint ______________________________________________________

		struct Blueprint
		{
			bool				active = false;

			int					refreshRate = 30;
			spx					radius = 96;

			SizeI				resolution;

			HiColor				seedTint = HiColor::White;
			BlendMode			seedBlend = BlendMode::Blend;

			HiColor				glowTint = HiColor::White;
			BlendMode			glowBlend = BlendMode::Add;

			Placement			resizePlacement = Placement::NorthWest;
			bool				stretchOnResize = false;
			bool				clearOnResize	= false;
		};

		//.____ Control _____________________________________________

		void				setActive(bool bActive);
		bool				isActive();

		void				clear();

		//.____ Behavior ______________________________________________________

		void				setResizeAction(Placement moveGlow, bool bStretchGlow, bool bClearGlow);

		void				setRefreshRate(int updatesPerSecond);
		int					refreshRate() const;

		//.____ Appearance _____________________________________________

		void				setSampleRadius(spx radius);
		spx					sampleRadius() const;

		void				setMatrices(const float red[9], const float green[9], const float blue[9]);

		void				setRedMatrix(const float matrix[9]);
		const float*		redMatrix() const;

		void				setGreenMatrix(const float matrix[9]);
		const float*		greenMatrix() const;

		void				setBlueMatrix(const float matrix[9]);
		const float*		blueMatrix() const;

		void				setPixelResolution(SizeI pixels);
		SizeI				pixelResolution() const;

		void				setSeedStates(HiColor seedTint, BlendMode seedBlend);
		void				setRenderStates(HiColor glowTint, BlendMode glowBlend);

		//.____ Internal ______________________________________________________

		void				_initFromBlueprint(const Blueprint& blueprint);

		void				_setSize(SizeSPX size, int scale);
		void				_update(int microPassed);
		void				_render(GfxDevice* pDevice, const RectSPX& canvas, Surface_p pSeed, const RectSPX& seedArea );


	protected:

		SizeI				_glowResolution();
		Surface_p			_createGlowCanvas( SurfaceFactory * pFactory );


		float				m_redMtx[9];
		float				m_greenMtx[9];
		float				m_blueMtx[9];

		int					m_refreshRate = 30;
		spx					m_radius = 96;

		Surface_p			m_surface[2];
		SizeI				m_resolution;

		HiColor				m_seedTint = HiColor::White;
		BlendMode			m_seedBlend = BlendMode::Blend;

		HiColor				m_glowTint = HiColor::White;
		BlendMode			m_glowBlend = BlendMode::Add;

		int					m_microSecAccumulator = 0;

		bool				m_bClear = false;

		Placement			m_resizePlacement = Placement::NorthWest;
		bool				m_bStretchOnResize = false;
		bool				m_bClearOnResize = false;

		bool				m_bActive = false;
	};


} // namespace wg
#endif	// WG_GLOW_DOT_H
