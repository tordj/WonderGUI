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
#ifndef WG_GLOWCAPSULE_DOT_H
#define WG_GLOWCAPSULE_DOT_H
#pragma once

#include <wg_capsule.h>
#include <wg_surfacefactory.h>
#include <wg_patches.h>
#include <wg_canvaslayers.h>

namespace wg
{

	class GlowCapsule;
	typedef	StrongPtr<GlowCapsule>	GlowCapsule_p;
	typedef	WeakPtr<GlowCapsule>		GlowCapsule_wp;


	//____ GlowCapsule ______________________________________________________
	/**
	* @brief	A widget that provides a separate canvas for its descendants.
	*
	* The GlowCapsule provides a separate canvas for its descendants into which they
	* are rendered. This then acts as a bitmap cache for fast blits of content when
	* it needs to be re-rendered with none or small changes.
	*
	*/

	class GlowCapsule : public Capsule
	{
	public:

		//____ Blueprint __________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			BlendMode		blendMode		= BlendMode::Blend;
			Widget_p		child;
			bool			disabled		= false;
			bool			dropTarget		= false;
			Finalizer_p		finalizer		= nullptr;
			int				id = 0;
			CanvasLayers_p	layers			= nullptr;
			MarkPolicy		markPolicy		= MarkPolicy::AlphaTest;
			bool			pickable		= false;
			int				pickCategory	= 0;
			PixelFormat		pixelFormat		= PixelFormat::BGRA_8;
			PointerStyle	pointer			= PointerStyle::Undefined;
			int				renderLayer		= -1;
			bool			selectable		= true;
			Skin_p			skin;
			bool			stickyFocus		= false;
			SurfaceFactory_p surfaceFactory;
			bool			tabLock			= false;
			HiColor			tintColor		= HiColor::Undefined;
			BlendMode		tintColorBlend	= BlendMode::Replace;
			String			tooltip;
		};

		//.____ Creation __________________________________________

		static GlowCapsule_p	create() { return GlowCapsule_p(new GlowCapsule()); }
		static GlowCapsule_p	create( const Blueprint& bp ) { return GlowCapsule_p(new GlowCapsule(bp)); }

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _________________________________________________

		void				setMatrices(spx radius, const float red[9], const float green[9], const float blue[9]);

		void				setResolution(SizeI pixels);

		void				setRefreshRate(int updatesPerSecond);

		void				setGlowSeeding(HiColor tint, BlendMode blendMode);
		void				setGlowOutput(HiColor tint, BlendMode blendMode);
		void				setCanvasOutput(HiColor tint, BlendMode blendMode);


		void				setSurfaceFactory(SurfaceFactory* pFactory);
		SurfaceFactory_p	surfaceFactory() const { return m_pFactory; }
		
		void				setCanvasFormat(PixelFormat format);
		PixelFormat			canvasFormat() const { return m_canvasFormat; }
		
		void				setCanvasLayers(CanvasLayers * pLayers);
		CanvasLayers_p		canvasLayers() const { return m_pCanvasLayers; }

		void				setRenderLayer(int layer);
		int					renderLayer() const { return m_renderLayer; }

	protected:
		GlowCapsule();
		
		template<class BP> GlowCapsule(const BP& bp) : Capsule(bp)
		{
			m_pFactory		= bp.surfaceFactory;
			m_canvasFormat	= bp.pixelFormat;
			m_pCanvasLayers = bp.layers;
			m_renderLayer	= bp.renderLayer;

			if( bp.child )
				slot.setWidget(bp.child);
		}
		
		virtual ~GlowCapsule();

		void				_update(int microPassed, int64_t microsecTimestamp) override;
		void				_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window) override;
		void				_resize(const SizeSPX& size, int scale) override;

		void				_childRequestRender(StaticSlot* pSlot) override;
		void				_childRequestRender(StaticSlot* pSlot, const RectSPX& rect) override;

		SizeI				_glowResolution();



		Surface_p			m_pCanvas;
		CanvasLayers_p		m_pCanvasLayers;
		SurfaceFactory_p	m_pFactory;
		PixelFormat			m_canvasFormat = PixelFormat::BGRA_8;
		int					m_renderLayer = -1;

		PatchesSPX			m_patches;

		float				m_redGlowMtx[9];
		float				m_greenGlowMtx[9];
		float				m_blueGlowMtx[9];

		int					m_glowRefreshRate = 30;
		spx					m_glowRadius = 96;

		Surface_p			m_glowCanvas[2];
		SizeI				m_glowResolution;

		HiColor				m_glowSeedTint = HiColor::White;
		BlendMode			m_glowSeedBlend = BlendMode::Blend;

		HiColor				m_glowOutTint = HiColor::White;
		BlendMode			m_glowOutBlend = BlendMode::Add;

		HiColor				m_canvasOutTint = HiColor::White;
		BlendMode			m_canvasOutBlend = BlendMode::Blend;

		int					m_microSecAccumulator = 0;
	};


} // namespace wg
#endif //WG_GLOWCAPSULE_DOT_H
