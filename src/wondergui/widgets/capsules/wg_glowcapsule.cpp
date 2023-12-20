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

		_startReceiveUpdates();

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

	//____ setBlurMatrices() _____________________________________________________

	void GlowCapsule::setBlurMatrices(spx radius, const float red[9], const float green[9], const float blue[9])
	{
		m_blurRadius = radius;

		for (int i = 0; i < 9; i++)
		{
			m_redBlurMtx[i] = red[i];
			m_greenBlurMtx[i] = green[i];
			m_blueBlurMtx[i] = blue[i];
		}

	}

	//____ setBlurCanvasPixelSize() _______________________________________________

	void GlowCapsule::setBlurCanvasPixelSize(SizeI size)
	{
		if (size != m_blurCanvasPixelSize)
		{
			SizeI usedSize = _blurCanvasPixelSize();
			m_blurCanvasPixelSize = size;

			if (m_blurCanvas[0] && m_blurCanvas[0]->pixelSize() != size)
			{
				m_blurCanvas[0] = nullptr;
				m_blurCanvas[1] = nullptr;
				_requestRender();
			}
		}
	}

	//____ setBlurSeeding() ___________________________________________________

	void GlowCapsule::setBlurSeeding(HiColor tint, BlendMode blendMode)
	{
		m_blurSeedTint = tint;
		m_blurSeedBlend = blendMode;
	}

	//____ setBlurOutput() ____________________________________________________

	void GlowCapsule::setBlurOutput(HiColor tint, BlendMode blendMode)
	{
		m_blurOutTint = tint;
		m_blurOutBlend = blendMode;

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

	//____ setBlurSpeed() _____________________________________________________

	void GlowCapsule::setBlurSpeed(int updatesPerSecond)
	{
		if (updatesPerSecond == m_blurSpeed)
			return;

		if (updatesPerSecond < 0)
		{
			//TODO: Error handling!

			return;
		}

		if (m_blurSpeed == 0 && updatesPerSecond > 0)
			_startReceiveUpdates();

		if (m_blurSpeed > 0 && updatesPerSecond == 0)
			_stopReceiveUpdates();

		m_blurSpeed = updatesPerSecond;
	}


	//____ _update() ___________________________________________________________

	void GlowCapsule::_update(int microPassed, int64_t microsecTimestamp)
	{
		int microsecBetweenUpdates = 1000000 / m_blurSpeed;

		if( m_microSecAccumulator < microsecBetweenUpdates && 
			((m_microSecAccumulator + microsecTimestamp) > microsecBetweenUpdates) )
			_requestRender();

		m_microSecAccumulator += microPassed;
	}

	//____ _render() _____________________________________________________________

	void GlowCapsule::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		// Possibly regenerate the canvas

		if (!m_pCanvas)
		{
			SurfaceFactory* pFactory = m_pFactory ? m_pFactory : Base::defaultSurfaceFactory();
			if (!pFactory)
			{
				//TODO: Error handling!
				return;
			}

			SizeI pixelSize = align(m_size) / 64;
			m_pCanvas = pFactory->createSurface( WGBP(Surface, _.size = pixelSize, _.format = m_canvasFormat, _.canvas = true ));
			m_patches.clear();
			m_patches.add(m_size);

			pDevice->beginCanvasUpdate(m_pCanvas);
			pDevice->setBlendMode(BlendMode::Replace);
			pDevice->fill(HiColor::Transparent);
			pDevice->endCanvasUpdate();

		}

		// Possibly regenerate the blur surfaces

		if (m_blurCanvas[0] == nullptr)
		{
			SurfaceFactory* pFactory = m_pFactory ? m_pFactory : Base::defaultSurfaceFactory();
			if (!pFactory)
			{
				//TODO: Error handling!
				return;
			}

			SizeI pixelSize = _blurCanvasPixelSize();

			for (int i = 0; i < 2; i++)
			{
				m_blurCanvas[i] = pFactory->createSurface(WGBP(Surface, _.size = pixelSize, _.format = PixelFormat::BGRA_8, _.canvas = true));
				pDevice->beginCanvasUpdate(m_blurCanvas[i]);
				pDevice->fill(HiColor::Black);
				pDevice->endCanvasUpdate();
			}

		}

		// Render 

		m_skin.render(pDevice, _canvas.size(), m_scale, m_state);

		pDevice->beginCanvasUpdate(m_pCanvas, m_patches.size(), m_patches.begin(), m_pCanvasLayers, m_renderLayer);
		
		// Render children recursively

		RectSPX canvas = m_skin.contentRect(_canvas, m_scale, m_state);

		if (canvas != _canvas)
		{
			auto savedClipData = Util::limitClipList(pDevice, canvas);
			slot._widget()->_render(pDevice, canvas, canvas);
			Util::popClipList(pDevice, savedClipData);
		}
		else
			slot._widget()->_render(pDevice, canvas, canvas);

		pDevice->endCanvasUpdate();

		m_patches.clear();

		// Update blur canvases

		int microsecBetweenUpdates = 1000000 / m_blurSpeed;

		while (m_microSecAccumulator >= microsecBetweenUpdates)
		{
			pDevice->beginCanvasUpdate(m_blurCanvas[1]);
			pDevice->setBlitSource(m_blurCanvas[0]);
			pDevice->setBlurMatrices(m_blurRadius, m_redBlurMtx, m_greenBlurMtx, m_blueBlurMtx);
			pDevice->blur({ 0,0 });
			pDevice->setTintColor(m_blurSeedTint);
			pDevice->setBlendMode(m_blurSeedBlend);
			pDevice->setBlitSource(m_pCanvas);
			pDevice->stretchBlit(m_blurCanvas[0]->pixelSize() * 64);
			pDevice->endCanvasUpdate();

			std::swap(m_blurCanvas[0], m_blurCanvas[1]);

			m_microSecAccumulator -= microsecBetweenUpdates;

			if (m_microSecAccumulator > microsecBetweenUpdates)
				m_microSecAccumulator = microsecBetweenUpdates;		// Make sure we don't loop more than twice.
		}

		// Blit our canvas

		int		rl = pDevice->renderLayer();
		BlendMode bm = pDevice->blendMode();
		HiColor c = pDevice->tintColor();	


		if (m_renderLayer != -1)
			pDevice->setRenderLayer(m_renderLayer);

		pDevice->setBlendMode(m_blurOutBlend);
		pDevice->setTintColor(m_blurOutTint);
		pDevice->setBlitSource(m_blurCanvas[0]);
		pDevice->stretchBlit(canvas);

		pDevice->setBlendMode(m_canvasOutBlend);
		pDevice->setTintColor(m_canvasOutTint);
		pDevice->setBlitSource(m_pCanvas);
		pDevice->blit(canvas);

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

			if (m_blurCanvasPixelSize.isEmpty())
			{
				m_blurCanvas[0] = nullptr;
				m_blurCanvas[1] = nullptr;
			}

		}
		Capsule::_resize(size, scale);
	}

	//____ _childRequestRender() _________________________________________________

	void GlowCapsule::_childRequestRender(StaticSlot* pSlot)
	{
		m_patches.clear();
		m_patches.push(m_size);
		Capsule::_childRequestRender(pSlot);
	}

	void GlowCapsule::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		RectSPX dirt = rect + _contentRect().pos();

		m_patches.add(dirt);
		_requestRender(dirt);
	}

	//____ _blurCanvasPixelSize() _____________________________________________

	SizeI GlowCapsule::_blurCanvasPixelSize()
	{
		if (! m_blurCanvasPixelSize.isEmpty() )
			return m_blurCanvasPixelSize;

		return (m_size - _contentBorderSize(m_scale)) / 64;				
	}


}
