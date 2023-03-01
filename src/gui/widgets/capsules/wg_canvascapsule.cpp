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

#include <wg_canvascapsule.h>
#include <wg_gfxdevice.h>
#include <wg_base.h>
#include <wg_util.h>

namespace wg
{
	const TypeInfo CanvasCapsule::TYPEINFO = { "CanvasCapsule", &Capsule::TYPEINFO };

	using namespace Util;

	//____ constructor ____________________________________________________________

	CanvasCapsule::CanvasCapsule()
	{
	}

	CanvasCapsule::CanvasCapsule(const Blueprint& bp)
	{
		_initFromBlueprint(bp);

		m_pFactory		= bp.surfaceFactory;
		m_canvasFormat	= bp.pixelFormat;
		m_pCanvasLayers = bp.layers;
		m_renderLayer	= bp.renderLayer;

		m_renderMode	= bp.blendMode;
		m_tintColor		= bp.tintColor;
		m_tintMode		= bp.tintColorBlend;

		if( bp.child )
			slot.setWidget(bp.child);
	}


	//____ Destructor _____________________________________________________________

	CanvasCapsule::~CanvasCapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CanvasCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setTintColor() ______________________________________________________

	void CanvasCapsule::setTintColor(HiColor color, BlendMode mode)
	{
		if (color != m_tintColor || mode != m_tintMode)
		{
			m_tintMode = mode;
			m_tintColor = color;
			_requestRender();
		}
	}

	//____ setRenderMode() __________________________________________________________

	void CanvasCapsule::setRenderMode(BlendMode mode)
	{
		if (mode != m_renderMode)
		{
			m_renderMode = mode;
			_requestRender();
		}
	}

	//____ setSurfaceFactory() ________________________________________________

	void CanvasCapsule::setSurfaceFactory(SurfaceFactory* pFactory)
	{
		if (pFactory != m_pFactory)
		{
			m_pFactory = pFactory;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasFormat() _____________________________________________________

	void CanvasCapsule::setCanvasFormat(PixelFormat format)
	{
		if (format != m_canvasFormat)
		{
			m_canvasFormat = format;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasLayers() ___________________________________________________

	void CanvasCapsule::setCanvasLayers(CanvasLayers * pLayers)
	{
		if( pLayers != m_pCanvasLayers )
		{
			m_pCanvasLayers = pLayers;
			_requestRender();
		}
	}

	//____ setRenderLayer() ___________________________________________________

	void CanvasCapsule::setRenderLayer(int renderLayer)
	{
		if (renderLayer != m_renderLayer)
		{
			m_renderLayer = renderLayer;
			_requestRender();
		}
	}

	//____ _render() _____________________________________________________________

	void CanvasCapsule::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
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
		}

		// Make sure we have right scale

		if (m_pCanvas->scale() != m_scale)
			m_pCanvas->setScale(m_scale);

		// Generate the clip buffer

		int allocSize = m_patches.size() * sizeof(RectSPX);
		RectSPX * pClipBuffer = (RectSPX*) Base::memStackAlloc(allocSize);

		RectSPX* pDst = pClipBuffer;
		const RectSPX* pSrc = m_patches.begin();
		while (pSrc != m_patches.end())
			*pDst++ = (*pSrc++);

		// Render 

		pDevice->beginCanvasUpdate(m_pCanvas, m_patches.size(), pClipBuffer, m_pCanvasLayers, m_renderLayer);

		m_skin.render(pDevice, _canvas.size(), m_scale, m_state);
		
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
		Base::memStackFree(allocSize);
		
		// Blit our canvas

		int		rl = pDevice->renderLayer();
		BlendMode bm = pDevice->blendMode();
		HiColor c = pDevice->tintColor();	

		if (m_renderLayer != -1)
			pDevice->setRenderLayer(m_renderLayer);

		pDevice->setBlendMode(m_renderMode);

		if (m_tintColor != HiColor::Undefined)
		{
			HiColor newCol = HiColor::blend(c, m_tintColor, m_tintMode);
			pDevice->setTintColor(newCol);
		}

		pDevice->setBlitSource(m_pCanvas);
		pDevice->blit(_canvas);

		pDevice->setTintColor(c);
		pDevice->setBlendMode(bm);
		pDevice->setRenderLayer(rl);
	}

	//____ _resize() _____________________________________________________________

	void CanvasCapsule::_resize(const SizeSPX& size, int scale)
	{
		if( size != m_size)
			m_pCanvas = nullptr;
		Capsule::_resize(size, scale);
	}

	//____ _childRequestRender() _________________________________________________

	void CanvasCapsule::_childRequestRender(StaticSlot* pSlot)
	{
		m_patches.clear();
		m_patches.push(m_size);
		Capsule::_childRequestRender(pSlot);
	}

	void CanvasCapsule::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		RectSPX dirt = rect + _contentRect().pos();

		m_patches.add(dirt);
		_requestRender(dirt);
	}
}
