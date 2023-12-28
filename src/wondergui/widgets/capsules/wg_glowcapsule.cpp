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

#include <wg_glowcapsule.h>
#include <wg_gfxdevice.h>
#include <wg_base.h>
#include <wg_util.h>

namespace wg
{
	const TypeInfo GlowCapsule::TYPEINFO = { "GlowCapsule", &Capsule::TYPEINFO };

	using namespace Util;

	//____ constructor ____________________________________________________________

	GlowCapsule::GlowCapsule()
	{
		_init();

	}

	//____ destructor _____________________________________________________________

	GlowCapsule::~GlowCapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GlowCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ clear() ____________________________________________________________

	void GlowCapsule::clear()
	{
		m_bClear = true;
		_requestRender();
	}


	//____ setMatrices() _____________________________________________________

	void GlowCapsule::setMatrices(spx radius, const float red[9], const float green[9], const float blue[9])
	{
		m_glowRadius = radius;

		for (int i = 0; i < 9; i++)
		{
			m_redGlowMtx[i] = red[i];
			m_greenGlowMtx[i] = green[i];
			m_blueGlowMtx[i] = blue[i];
		}

	}

	//____ setResolution() _______________________________________________

	void GlowCapsule::setResolution(SizeI size)
	{
		if (size != m_glowResolution)
		{
			SizeI usedSize = _glowResolution();
			m_glowResolution = size;

			if (m_glowCanvas[0] && m_glowCanvas[0]->pixelSize() != size)
			{
				m_glowCanvas[0] = nullptr;
				m_glowCanvas[1] = nullptr;
				_requestRender();
			}
		}
	}

	//____ setResizeAction() _________________________________________________

	void GlowCapsule::setResizeAction(Placement moveGlow, bool bStretchGlow, bool bClearGlow)
	{
		m_glowResizePlacement = moveGlow;
		m_bStretchGlowOnResize = bStretchGlow;
		m_bClearGlowOnResize = bClearGlow;
	}


	//____ setGlowSeed() ___________________________________________________

	void GlowCapsule::setGlowSeed(HiColor tint, BlendMode blendMode)
	{
		m_glowSeedTint = tint;
		m_glowSeedBlend = blendMode;
	}

	//____ setGlowOutput() ____________________________________________________

	void GlowCapsule::setGlowOutput(HiColor tint, BlendMode blendMode)
	{
		m_glowOutTint = tint;
		m_glowOutBlend = blendMode;

	}

	//____ setCanvasOutput() __________________________________________________

	void GlowCapsule::setCanvasOutput(HiColor tint, BlendMode blendMode)
	{
		m_canvasOutTint = tint;
		m_canvasOutBlend = blendMode;
	}

	//____ setSurfaceFactory() ________________________________________________

	void GlowCapsule::setSurfaceFactory(SurfaceFactory* pFactory)
	{
		if (pFactory != m_pFactory)
		{
			m_pFactory = pFactory;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasFormat() _____________________________________________________

	void GlowCapsule::setCanvasFormat(PixelFormat format)
	{
		if (format != m_canvasFormat)
		{
			m_canvasFormat = format;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasLayers() ___________________________________________________

	void GlowCapsule::setCanvasLayers(CanvasLayers * pLayers)
	{
		if( pLayers != m_pCanvasLayers )
		{
			m_pCanvasLayers = pLayers;
			_requestRender();
		}
	}

	//____ setRenderLayer() ___________________________________________________

	void GlowCapsule::setRenderLayer(int renderLayer)
	{
		if (renderLayer != m_renderLayer)
		{
			m_renderLayer = renderLayer;
			_requestRender();
		}
	}

	//____ setRefreshRate() _____________________________________________________

	void GlowCapsule::setRefreshRate(int updatesPerSecond)
	{
		if (updatesPerSecond == m_glowRefreshRate)
			return;

		if (updatesPerSecond < 0)
		{
			//TODO: Error handling!

			return;
		}

		if (m_glowRefreshRate == 0 && updatesPerSecond > 0)
			_startReceiveUpdates();

		if (m_glowRefreshRate > 0 && updatesPerSecond == 0)
			_stopReceiveUpdates();

		m_glowRefreshRate = updatesPerSecond;
	}


	//____ _update() ___________________________________________________________

	void GlowCapsule::_update(int microPassed, int64_t microsecTimestamp)
	{
		int microsecBetweenUpdates = 1000000 / m_glowRefreshRate;

		if( m_microSecAccumulator < microsecBetweenUpdates && 
			((m_microSecAccumulator + microPassed) > microsecBetweenUpdates) )
			_requestRender();

		m_microSecAccumulator += microPassed;
	}

	//____ _render() _____________________________________________________________

	void GlowCapsule::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		auto contentRect = _contentRect( _canvas );


		// Possibly regenerate the canvas

		if (!m_pCanvas)
		{
			SurfaceFactory* pFactory = m_pFactory ? m_pFactory : Base::defaultSurfaceFactory();
			if (!pFactory)
			{
				//TODO: Error handling!
				return;
			}

			SizeI pixelSize = contentRect.size() / 64;
			m_pCanvas = pFactory->createSurface( WGBP(Surface, _.size = pixelSize, _.format = m_canvasFormat, _.canvas = true ));
			m_patches.clear();
			m_patches.add(contentRect.size());

		}

		// Possibly regenerate the glow surfaces

		if (m_glowCanvas[1] == nullptr)
		{
			m_glowCanvas[1] = _createGlowCanvas();

			if (m_glowCanvas[0] == nullptr)
			{
				// Both are missing, we just recreate and clear them.

				m_glowCanvas[0] = _createGlowCanvas();
				m_bClear = true;
			}
			else
			{
				pDevice->beginCanvasUpdate(m_glowCanvas[1]);
				pDevice->fill(Color::Black);

				pDevice->setBlitSource(m_glowCanvas[0]);
				SizeSPX destSize = (m_glowCanvas[1]->pixelSize() * 64) - SizeSPX(128, 128);
				SizeSPX sourceSize = (m_glowCanvas[0]->pixelSize() * 64) - SizeSPX(128, 128);

				if (m_bStretchGlowOnResize)
				{
					pDevice->stretchBlit({ 64,64,destSize}, { 64,64,sourceSize});
				}
				else
				{
					RectSPX dest = Util::placementToRect(m_glowResizePlacement,destSize, sourceSize);

					pDevice->blit({ dest.x + 64,dest.y + 64 }, { 64,64,sourceSize });
				}

				pDevice->endCanvasUpdate();

				m_glowCanvas[0] = _createGlowCanvas();
				pDevice->beginCanvasUpdate(m_glowCanvas[0]);
				pDevice->fill(Color::Black);
				pDevice->endCanvasUpdate();

				std::swap(m_glowCanvas[0], m_glowCanvas[1]);
			}
		}

		// Render
		
		m_skin.render(pDevice, _canvas, m_scale, m_state);

		// Render children recursively

		if (!m_patches.isEmpty())
		{
			pDevice->beginCanvasUpdate(m_pCanvas, m_patches.size(), m_patches.begin(), m_pCanvasLayers, m_renderLayer);

			pDevice->setBlendMode(BlendMode::Replace);
			pDevice->fill(HiColor::Transparent);
			pDevice->setBlendMode(BlendMode::Blend);

			RectSPX canvas = { 0,0, m_pCanvas->pixelSize()*64 };
			slot._widget()->_render(pDevice, canvas, canvas);

			pDevice->endCanvasUpdate();

			m_patches.clear();
		}
		
		// Possibly clear glow canvas

		if (m_bClear)
		{
			pDevice->beginCanvasUpdate(m_glowCanvas[0]);
			pDevice->fill(HiColor::Black);
			pDevice->endCanvasUpdate();

			m_bClear = false;
		}

		// Update glow canvases

		int microsecBetweenUpdates = 1000000 / m_glowRefreshRate;

		RectSPX srcRect = m_glowCanvas[0]->pixelSize() * 64;
		srcRect -= BorderSPX(64);

		RectSPX destRect = m_glowCanvas[1]->pixelSize() * 64;
		destRect -= BorderSPX(64);


		while (m_microSecAccumulator >= microsecBetweenUpdates)
		{
			pDevice->beginCanvasUpdate(m_glowCanvas[1]);
			pDevice->setBlitSource(m_glowCanvas[0]);
			pDevice->setBlurMatrices(m_glowRadius, m_redGlowMtx, m_greenGlowMtx, m_blueGlowMtx);
			pDevice->blur( destRect.pos(), srcRect);
			pDevice->setTintColor(m_glowSeedTint);
			pDevice->setBlendMode(m_glowSeedBlend);
			pDevice->setBlitSource(m_pCanvas);
			pDevice->stretchBlit( destRect );
			pDevice->endCanvasUpdate();

			std::swap(m_glowCanvas[0], m_glowCanvas[1]);

			m_microSecAccumulator -= microsecBetweenUpdates;

			if (m_microSecAccumulator > microsecBetweenUpdates)
				m_microSecAccumulator = microsecBetweenUpdates;		// Make sure we don't loop more than twice.
		}

		// Blit to destination, first updated blur, then local canvas.

		int		rl = pDevice->renderLayer();
		BlendMode bm = pDevice->blendMode();
		HiColor c = pDevice->tintColor();	


		if (m_renderLayer != -1)
			pDevice->setRenderLayer(m_renderLayer);

		pDevice->setBlendMode(m_glowOutBlend);
		pDevice->setTintColor(m_glowOutTint);
		pDevice->setBlitSource(m_glowCanvas[0]);
		pDevice->stretchBlit(contentRect, destRect );

		pDevice->setBlendMode(m_canvasOutBlend);
		pDevice->setTintColor(m_canvasOutTint);
		pDevice->setBlitSource(m_pCanvas);
		pDevice->blit(contentRect.pos());

		pDevice->setTintColor(c);
		pDevice->setBlendMode(bm);
		pDevice->setRenderLayer(rl);
	}

	//____ _resize() _____________________________________________________________

	void GlowCapsule::_resize(const SizeSPX& size, int scale)
	{
		if (size != m_size)
		{
			m_pCanvas = nullptr;

			if (m_glowResolution.isEmpty())
			{
				m_glowCanvas[1] = nullptr;

				if (m_bClearGlowOnResize)
					m_glowCanvas[0] = nullptr;
			}

		}
		Capsule::_resize(size, scale);
	}

	//____ _childRequestRender() _________________________________________________

	void GlowCapsule::_childRequestRender(StaticSlot* pSlot)
	{
		m_patches.clear();
		m_patches.push(m_size-_contentBorderSize(m_scale));
		Capsule::_childRequestRender(pSlot);
	}

	void GlowCapsule::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		m_patches.add(rect);

		RectSPX dirt = rect + _contentRect().pos();
		_requestRender(dirt);
	}

	//____ _glowResolution() _____________________________________________

	SizeI GlowCapsule::_glowResolution()
	{
		if (! m_glowResolution.isEmpty() )
			return m_glowResolution;

		return (m_size - _contentBorderSize(m_scale)) / 64;				
	}

	//____ _createGlowCanvas() ________________________________________________

	Surface_p GlowCapsule::_createGlowCanvas()
	{
		SurfaceFactory* pFactory = m_pFactory ? m_pFactory : Base::defaultSurfaceFactory();
		if (!pFactory)
		{
			//TODO: Error handling!
			return nullptr;
		}

		// Glow surfaces have a border that is not used to make sampling at corners
		// work better.

		SizeI pixelSize = _glowResolution() + SizeI(2,2);

		return pFactory->createSurface(WGBP(Surface, _.size = pixelSize, _.format = PixelFormat::BGRX_8, _.canvas = true));
	}

	//____ _init() ____________________________________________________________

	void GlowCapsule::_init()
	{
		for (int i = 0; i < 9; i++)
		{
			m_redGlowMtx[i] = 0.1f;
			m_greenGlowMtx[i] = 0.1f;
			m_blueGlowMtx[i] = 0.1f;
		}

		_startReceiveUpdates();
	}

}
