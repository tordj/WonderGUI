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

#include <wg_filtercapsule.h>
#include <wg_gfxdevice.h>
#include <wg_base.h>
#include <wg_util.h>

#include <array>

static constexpr std::pair<std::array<uint32_t, 9>, const uint32_t> GenerateFilter(const float lambda, const std::array<float, 9> &Af)
{
    std::array<uint32_t, 9> A{0};
    std::array<float, 9>  Afn{0};

    // Sum
    float Asum = 0.0f;
    for(int i=0; i<9; i++)
        Asum += Af[i];

    // Normalize
    for(int i=0; i<9; i++)
        Afn[i] = lambda*Af[i]/Asum;

    const float Bf = 1.0f - lambda;

    // Convert to int
    for(int i=0; i<9; i++)
        A[i] = 65535*Afn[i];

    uint32_t B = 65535*Bf;

    return std::make_pair(A, B);
}

namespace wg
{
	const TypeInfo FilterCapsule::TYPEINFO = { "FilterCapsule", &Capsule::TYPEINFO };

	using namespace Util;

	//____ constructor ____________________________________________________________

	FilterCapsule::FilterCapsule()
	{
	}

	//____ destructor _____________________________________________________________

	FilterCapsule::~FilterCapsule()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& FilterCapsule::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setTintColor() ______________________________________________________

	void FilterCapsule::setTintColor(HiColor color, BlendMode mode)
	{
		if (color != m_tintColor || mode != m_tintMode)
		{
			m_tintMode = mode;
			m_tintColor = color;
			_requestRender();
		}
	}

	//____ setRenderMode() __________________________________________________________

	void FilterCapsule::setRenderMode(BlendMode mode)
	{
		if (mode != m_renderMode)
		{
			m_renderMode = mode;
			_requestRender();
		}
	}

	//____ setSurfaceFactory() ________________________________________________

	void FilterCapsule::setSurfaceFactory(SurfaceFactory* pFactory)
	{
		if (pFactory != m_pFactory)
		{
			m_pFactory = pFactory;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasFormat() _____________________________________________________

	void FilterCapsule::setCanvasFormat(PixelFormat format)
	{
		if (format != m_canvasFormat)
		{
			m_canvasFormat = format;
			m_pCanvas = nullptr;
			_requestRender();
		}
	}

	//____ setCanvasLayers() ___________________________________________________

	void FilterCapsule::setCanvasLayers(CanvasLayers * pLayers)
	{
		if( pLayers != m_pCanvasLayers )
		{
			m_pCanvasLayers = pLayers;
			_requestRender();
		}
	}

	//____ setRenderLayer() ___________________________________________________

	void FilterCapsule::setRenderLayer(int renderLayer)
	{
		if (renderLayer != m_renderLayer)
		{
			m_renderLayer = renderLayer;
			_requestRender();
		}
	}

	//____ _render() _____________________________________________________________

	void FilterCapsule::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
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
			m_pCanvas      = pFactory->createSurface( WGBP(Surface, _.size = pixelSize, _.format = m_canvasFormat, _.canvas = true ));
            m_pStateCanvas = pFactory->createSurface( WGBP(Surface, _.size = pixelSize, _.format = m_canvasFormat, _.canvas = true ));
            m_pTmpCanvas   = pFactory->createSurface( WGBP(Surface, _.size = pixelSize, _.format = m_canvasFormat, _.canvas = true ));
			m_patches.clear();
			m_patches.add(m_size);
		}

		// Make sure we have right scale

//		if (m_pCanvas->scale() != m_scale)
//			m_pCanvas->setScale(m_scale);

		// Render 

		pDevice->beginCanvasUpdate(m_pCanvas, m_patches.size(), m_patches.begin(), m_pCanvasLayers, m_renderLayer);
        pDevice->setBlendMode(BlendMode::Replace);
        pDevice->fill(HiColor::Transparent);
        pDevice->setBlendMode(BlendMode::Blend);

		m_skin.render(pDevice, _canvas.size(), m_scale, m_state);
		
		// Render children recursively

		RectSPX canvas = m_skin.contentRect(_canvas.size(), m_scale, m_state);

		if (canvas != _canvas)
		{
			auto savedClipData = Util::limitClipList(pDevice, canvas);
			slot._widget()->_render(pDevice, canvas, canvas);
			Util::popClipList(pDevice, savedClipData);
		}
		else
			slot._widget()->_render(pDevice, canvas, canvas);

		pDevice->endCanvasUpdate();

        // Filter!
        std::array<uint32_t, 9> Ar;
        std::array<uint32_t, 9> Ag;
        std::array<uint32_t, 9> Ab;
        std::array<uint32_t, 9> Aa;
        uint32_t B;
        
        // Radius
        const int R = 4;

        const float rnd1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        const float rnd2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

        std::tie(Ar, B) = GenerateFilter(0.93f,
                                        {
                                            1.54f, 0.83f, 2.992f,
                                            0.4f, 0.01f, 0.5f,
                                            0.2f, 0.16f, 0.03f
                                        });
        Ag = Ar;
        Ab = Ar;
        Aa = Ar;

        Ag[0] = Ag[0] - (0.5f - 0.5f*rnd1)*560;
        Ab[1] = Ab[1] - 60;

        const std::array<std::array<uint32_t, 9>, 4> A = {Ar, Ag, Ab, Aa};

        auto pbSource  = m_pCanvas->allocPixelBuffer();
        auto pbState   = m_pStateCanvas->allocPixelBuffer();
        auto pbTmp     = m_pTmpCanvas->allocPixelBuffer();
        int pixelBytes = Util::pixelFormatToDescription(pbState.format).bits / 8;

        m_pCanvas->pushPixels(pbSource);

        // Filter algorithm:
        // x: state surface (and output)
        // tmp: scratch surface
        // u: input surface
        // A: spread matrix
        // B: input gain

        // tmp = A * x
        // x = tmp + B * u

        // Spread state to tmp
        // tmp = A * x
        for( int y = R ; y < pbTmp.rect.h-R ; y++ )
        {
            uint8_t * pDstCurr = pbTmp.pixels   + (y)   * pbTmp.pitch;

            uint8_t * pSrcPrev = pbState.pixels + (y-R) * pbState.pitch;
            uint8_t * pSrcCurr = pbState.pixels + (y)   * pbState.pitch;
            uint8_t * pSrcNext = pbState.pixels + (y+R) * pbState.pitch;

            for( int x = R ; x < pbTmp.rect.w-R; x++ )
            {
                // Loop over channels
                for(int p=0; p<4; p++)
                {
                    pDstCurr[p + x*pixelBytes] = (uint8_t)(std::min((
                                        (uint32_t)pSrcPrev[p + (x-R)*pixelBytes]*A[p][0] + (uint32_t)pSrcPrev[p + (x+0)*pixelBytes]*A[p][1] + (uint32_t)pSrcPrev[p + (x+R)*pixelBytes]*A[p][2] +
                                        (uint32_t)pSrcCurr[p + (x-R)*pixelBytes]*A[p][3] + (uint32_t)pSrcCurr[p + (x+0)*pixelBytes]*A[p][4] + (uint32_t)pSrcCurr[p + (x+R)*pixelBytes]*A[p][5] +
                                        (uint32_t)pSrcNext[p + (x-R)*pixelBytes]*A[p][6] + (uint32_t)pSrcNext[p + (x+0)*pixelBytes]*A[p][7] + (uint32_t)pSrcNext[p + (x+R)*pixelBytes]*A[p][8]
                                )/(65535), (uint32_t)255));
                }
            }
        }

        // Blend source to tmp, store in state
        // x = tmp + B * u
        for( int y = 0 ; y < pbSource.rect.h ; y++ )
        {
            uint8_t * pSrc1 = pbTmp.pixels + (y)      * pbTmp.pitch;
            uint8_t * pSrc2 = pbSource.pixels + (y)   * pbSource.pitch;
            uint8_t * pDst  = pbState.pixels    + (y) * pbState.pitch;

            for( int x = 0 ; x < pbSource.rect.w; x++ )
            {
                for(int p=0; p<4; p++)
                    pDst[p + x*pixelBytes] = (uint8_t)std::min(((uint32_t)pSrc1[p + x*pixelBytes]*(65535) + (uint32_t)pSrc2[p + x*pixelBytes] * (B))/(65535), (uint32_t)255);
            }
        }

        m_pStateCanvas->pullPixels(pbState);
        
        m_pCanvas->freePixelBuffer(pbSource);
        m_pStateCanvas->freePixelBuffer(pbState);
        m_pTmpCanvas->freePixelBuffer(pbTmp);

        // Filter done!
        ///////

		m_patches.clear();

		// Blit our canvas

		int		rl = pDevice->renderLayer();
		BlendMode bm = pDevice->blendMode();
		HiColor c = pDevice->tintColor();	

		if (m_renderLayer != -1)
			pDevice->setRenderLayer(m_renderLayer);

		//pDevice->setBlendMode(m_renderMode);
        pDevice->setBlendMode(BlendMode::Blend);

		if (m_tintColor != HiColor::Undefined)
		{
			HiColor newCol = HiColor::blend(c, m_tintColor, m_tintMode);
			pDevice->setTintColor(newCol);
		}

		pDevice->setBlitSource(m_pStateCanvas);
		pDevice->blit(_canvas);

		pDevice->setTintColor(c);
		pDevice->setBlendMode(bm);
		pDevice->setRenderLayer(rl);
	}

	//____ _resize() _____________________________________________________________

	void FilterCapsule::_resize(const SizeSPX& size, int scale)
	{
		if( size != m_size) {
			m_pCanvas = nullptr;
            m_pStateCanvas = nullptr;
            m_pTmpCanvas = nullptr;
        }

		Capsule::_resize(size, scale);
	}

	//____ _childRequestRender() _________________________________________________

	void FilterCapsule::_childRequestRender(StaticSlot* pSlot)
	{
		m_patches.clear();
		m_patches.push(m_size);
		Capsule::_childRequestRender(pSlot);
	}

	void FilterCapsule::_childRequestRender(StaticSlot* pSlot, const RectSPX& rect)
	{
		RectSPX dirt = rect + _contentRect().pos();

		m_patches.add(dirt);
		_requestRender(dirt);
	}
}
