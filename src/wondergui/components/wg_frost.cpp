/*=========================================================================

                             >>> WonderGUI <<<

  This file is part of Tord Bärnfors' WonderGUI UI Toolkit and copyright
  Tord Bärnfors, Sweden [mail: first name AT barnfors DOT c_o_m].

                                -----------

  The WonderGUI UI Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                                -----------

  The WonderGUI UI Toolkit is also available for use in commercial
  closed source projects under a separate license. Interested parties
  should contact Bärnfors Technology AB [www.barnfors.com] for details.

=========================================================================*/
#include <wg_frost.h>
#include <wg_gfxdevice.h>
#include <wg_util.h>
#include <wg_base.h>

#include <algorithm>

namespace wg
{
	using namespace Util;

	const TypeInfo	Frost::TYPEINFO = { "Frost", &Component::TYPEINFO };

	//____ Pane::clear() ______________________________________________________

	void Frost::Pane::clear()
	{
		m_pBlur[0] = nullptr;
		m_pBlur[1] = nullptr;
		m_pMasked = nullptr;
		m_bValid = false;
	}

	//____ constructor ________________________________________________________

	Frost::Frost(Widget* pWidget) : Component(pWidget)
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Frost::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setColor() _________________________________________________________

	void Frost::setColor(HiColor color)
	{
		if (color != m_color)
		{
			m_color = color;
			_settingsChanged();
		}
	}

	//____ setDownscale() _____________________________________________________

	void Frost::setDownscale(int factor)
	{
		if (factor < 1 || factor > 16)
		{
			Base::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Downscale must be in the range 1-16.",
				m_pWidget, &TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		if (factor != m_downscale)
		{
			m_downscale = factor;
			_settingsChanged();
		}
	}

	//____ setMask() __________________________________________________________

	void Frost::setMask(Skin* pSkin)
	{
		if (pSkin != m_pMask)
		{
			m_pMask = pSkin;
			_settingsChanged();
		}
	}

	//____ setPasses() ________________________________________________________

	void Frost::setPasses(int passes)
	{
		if (passes < 0 || passes > 8)
		{
			Base::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Passes must be in the range 0-8.",
				m_pWidget, &TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		if (passes != m_passes)
		{
			m_passes = passes;
			_settingsChanged();
		}
	}

	//____ setRadius() ________________________________________________________

	void Frost::setRadius(pts radius)
	{
		if (radius < 0)
		{
			Base::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Radius must be >= 0.",
				m_pWidget, &TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		if (radius != m_radius)
		{
			m_radius = radius;
			_settingsChanged();
		}
	}

	//____ setTint() __________________________________________________________

	void Frost::setTint(HiColor tint)
	{
		if (tint != m_tint)
		{
			m_tint = tint;
			_requestRender();			// Applied when rendering, the frost itself is still valid.
		}
	}

	//____ _initFromBlueprint() _______________________________________________

	void Frost::_initFromBlueprint(const Blueprint& bp)
	{
		m_color		= bp.color;
		m_downscale = std::clamp(bp.downscale, 1, 16);
		m_pMask		= bp.mask;
		m_passes	= std::clamp(bp.passes, 0, 8);
		m_radius	= std::max(bp.radius, pts(0));
		m_tint		= bp.tint;

		m_bBrushesDirty = true;
	}

	//____ _setBackdropSize() _________________________________________________

	void Frost::_setBackdropSize(SizeSPX size, int scale)
	{
		if (size == m_backdropSize && scale == m_scale)
			return;

		if (scale != m_scale)
		{
			m_scale = scale;
			m_bBrushesDirty = true;
			m_pBackdrop = nullptr;		// Content needs to be rendered in the new scale.
		}

		m_backdropSize = size;

		// A backdrop of the wrong size is recreated, and all panes with it,
		// when next updated.
	}

	//____ _invalidateBackdrop() ______________________________________________

	void Frost::_invalidateBackdrop(const RectSPX& rect)
	{
		m_backdropPatches.add(RectSPX::overlap(rect, RectSPX(0, 0, m_backdropSize)));
	}

	void Frost::_invalidateBackdrop()
	{
		m_backdropPatches.clear();
		m_backdropPatches.add(RectSPX(0, 0, m_backdropSize));
	}

	//____ _updateBackdrop() __________________________________________________
	/**
	 * Renders the dirty parts of the backdrop into its cache. The renderFunc
	 * is called within a canvas update of the cache, clipped to what is dirty.
	 */

	void Frost::_updateBackdrop(GfxDevice* pDevice, const std::function<void(GfxDevice* pDevice, const RectSPX& canvas)>& renderFunc)
	{
		if (m_backdropSize.isEmpty())
			return;

		// The backdrop has the color space of the canvas we are rendered onto. If that
		// has changed, we start over. Same if our size has changed.

		ColorSpace colorSpace = pDevice->canvas().colorSpace;
		SizeI pixelSize = alignUp(m_backdropSize) / 64;

		if (m_pBackdrop && (m_pBackdrop->colorSpace() != colorSpace || m_pBackdrop->pixelSize() != pixelSize))
			m_pBackdrop = nullptr;

		if (!m_pBackdrop)
		{
			m_pBackdrop = _createSurface(pDevice, pixelSize);
			_invalidateBackdrop();
			m_version++;
		}

		if (m_backdropPatches.isEmpty())
			return;

		// The device holds on to the update rects until the canvas update has ended,
		// so we hand it a copy that won't change if more of the backdrop is invalidated.

		PatchesSPX patches(m_backdropPatches);
		m_backdropPatches.clear();

		pDevice->beginCanvasUpdate(m_pBackdrop, patches.size(), patches.begin());

		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->fill(HiColor::Transparent);
		pDevice->setBlendMode(BlendMode::Blend);

		renderFunc(pDevice, RectSPX(0, 0, m_backdropSize));

		pDevice->endCanvasUpdate();
	}

	//____ _renderBackdrop() __________________________________________________
	/**
	 * Copies the cached backdrop onto the canvas, within the clip list of the device.
	 */

	void Frost::_renderBackdrop(GfxDevice* pDevice, const RectSPX& canvas)
	{
		if (!m_pBackdrop)
			return;

		BlendMode	oldBlendMode = pDevice->blendMode();
		Surface_p	pOldSource = pDevice->blitSource();

		pDevice->setBlendMode(BlendMode::Blend);
		pDevice->setBlitSource(m_pBackdrop);
		pDevice->blit(canvas.pos());

		pDevice->setBlitSource(pOldSource);
		pDevice->setBlendMode(oldBlendMode);
	}

	//____ _reach() ___________________________________________________________
	/**
	 * How far outside a pane the backdrop affects its frost, in spx.
	 */

	spx Frost::_reach() const
	{
		int ds = m_downscale;
		int radius = _firstPassRadius();

		int reach = ds / 2;					// Prefilter when shrinking.

		for (int i = 0; i < m_passes; i++)
			reach += std::max(1, radius >> i) * ds;

		reach += ds;						// Bilinear sampling and rounding of the shrunk backdrop.

		return reach * 64;
	}

	//____ _paneDamage() ______________________________________________________
	/**
	 * Returns the part of a pane whose frost is changed by a change of the backdrop
	 * within backdropChange, or an empty rectangle if the frost of the pane is unaffected.
	 */

	RectSPX Frost::_paneDamage(const RectSPX& backdropChange, const RectSPX& paneGeo) const
	{
		return RectSPX::overlap(backdropChange + BorderSPX(_reach()), paneGeo);
	}

	//____ _renderPane() ______________________________________________________
	/**
	 * Renders the frost of a pane onto the canvas, recalculating it first if needed.
	 *
	 * paneGeo is the geometry of the pane in the backdrop, canvas where it is on the
	 * canvas. The state is passed on to the mask skin.
	 */

	void Frost::_renderPane(GfxDevice* pDevice, Pane& pane, const RectSPX& paneGeo, const RectSPX& canvas, State state)
	{
		if (!m_pBackdrop)
			return;

		if (!pane.m_bValid || pane.m_version != m_version || pane.m_geo != paneGeo)
			_updatePane(pDevice, pane, paneGeo, state);

		if (!pane.m_bValid)
			return;

		BlendMode	oldBlendMode = pDevice->blendMode();
		bool		bHadTint = pDevice->hasTintColor();
		HiColor		oldTint = pDevice->tintColor();
		Surface_p	pOldSource = pDevice->blitSource();

		pDevice->setBlendMode(BlendMode::Blend);

		if (m_tint != HiColor::White)
			pDevice->setTintColor(oldTint * m_tint);

		if (pane.m_pMasked)
		{
			pDevice->setBlitSource(pane.m_pMasked);
			pDevice->blit(canvas.pos());
		}
		else
		{
			// Only the part of the pane that is on the backdrop has frost.

			RectSPX area = RectSPX::overlap(paneGeo, RectSPX(0, 0, m_backdropSize));

			pDevice->setBlitSource(pane.m_pBlur[pane.m_result]);
			pDevice->stretchBlit(area - paneGeo.pos() + canvas.pos(), _blurSource(pane, area));
		}

		pDevice->setBlitSource(pOldSource);

		if (bHadTint)
			pDevice->setTintColor(oldTint);
		else
			pDevice->clearTintColor();

		pDevice->setBlendMode(oldBlendMode);
	}

	//____ _settingsChanged() _________________________________________________

	void Frost::_settingsChanged()
	{
		m_version++;
		m_bBrushesDirty = true;
		_requestRender();
	}

	//____ _updatePane() ______________________________________________________

	void Frost::_updatePane(GfxDevice* pDevice, Pane& pane, const RectSPX& paneGeo, State state)
	{
		pane.m_bValid = false;

		// The area of the backdrop that the frost depends on.

		RectSPX region = RectSPX::overlap(paneGeo + BorderSPX(_reach()), RectSPX(0, 0, m_backdropSize));
		if (region.isEmpty())
			return;

		if (m_bBrushesDirty)
			_updateBrushes();

		int ds = m_downscale;
		SizeI blurSize = { std::max(1, (region.w / 64 + ds - 1) / ds), std::max(1, (region.h / 64 + ds - 1) / ds) };

		for (int i = 0; i < 2; i++)
		{
			if (!pane.m_pBlur[i] || pane.m_pBlur[i]->pixelSize() != blurSize || pane.m_pBlur[i]->colorSpace() != m_pBackdrop->colorSpace())
				pane.m_pBlur[i] = _createSurface(pDevice, blurSize);
		}

		// Shrink the backdrop. We blur it on the way so details don't
		// come and go as the pane moves.

		pDevice->beginCanvasUpdate(pane.m_pBlur[0]);
		pDevice->setBlendMode(BlendMode::Replace);
		pDevice->setBlitSource(m_pBackdrop);

		if (ds > 1)
		{
			pDevice->setBlurbrush(m_pPrefilter);
			pDevice->stretchBlur(RectSPX(0, 0, blurSize * 64), region);
		}
		else
			pDevice->blit({ 0,0 }, region);

		if (m_passes == 0 && m_color.a > 0)
		{
			pDevice->setBlendMode(BlendMode::Blend);
			pDevice->fill(m_color);
		}

		pDevice->endCanvasUpdate();

		// Blur passes, back and forth between our two surfaces.

		int src = 0;
		for (int i = 0; i < m_passes; i++)
		{
			int dst = src ^ 1;

			pDevice->beginCanvasUpdate(pane.m_pBlur[dst]);
			pDevice->setBlendMode(BlendMode::Replace);
			pDevice->setBlitSource(pane.m_pBlur[src]);
			pDevice->setBlurbrush(m_passBrushes[i]);
			pDevice->blur({ 0,0 });

			if (i == m_passes - 1 && m_color.a > 0)
			{
				pDevice->setBlendMode(BlendMode::Blend);
				pDevice->fill(m_color);
			}

			pDevice->endCanvasUpdate();
			src = dst;
		}

		pane.m_result = src;
		pane.m_region = region;

		// Shape the frost with the mask. The mask skin is rendered onto transparent white,
		// so its color is kept where it is partly transparent, and the frost is then
		// multiplied into it. Multiply keeps the alpha of the destination.

		if (m_pMask)
		{
			SizeI maskSize = alignUp(paneGeo.size()) / 64;

			if (!pane.m_pMasked || pane.m_pMasked->pixelSize() != maskSize || pane.m_pMasked->colorSpace() != m_pBackdrop->colorSpace())
				pane.m_pMasked = _createSurface(pDevice, maskSize);

			RectSPX area = RectSPX::overlap(paneGeo, RectSPX(0, 0, m_backdropSize));

			pDevice->beginCanvasUpdate(pane.m_pMasked);
			pDevice->setBlendMode(BlendMode::Replace);
			pDevice->fill(HiColor(4096, 4096, 4096, 0));
			pDevice->setBlendMode(BlendMode::Blend);

			m_pMask->_render(pDevice, RectSPX(0, 0, paneGeo.size()), m_scale, state);

			pDevice->setBlendMode(BlendMode::Multiply);
			pDevice->setBlitSource(pane.m_pBlur[src]);
			pDevice->stretchBlit(area - paneGeo.pos(), _blurSource(pane, area));
			pDevice->endCanvasUpdate();
		}
		else
			pane.m_pMasked = nullptr;

		pane.m_geo = paneGeo;
		pane.m_version = m_version;
		pane.m_bValid = true;
	}

	//____ _updateBrushes() ___________________________________________________

	void Frost::_updateBrushes()
	{
		int ds = m_downscale;

		// Prefilter covers the pixels that are shrunk into one, in pixels of the backdrop.

		Blurbrush::Blueprint bp;
		bp.size = std::max(1, ds / 2) * 64;
		m_pPrefilter = Blurbrush::create(bp);

		// Each pass has half the radius of the previous one, in pixels of the shrunk backdrop.

		int radius = _firstPassRadius();

		m_passBrushes.clear();
		for (int i = 0; i < m_passes; i++)
		{
			bp.size = std::max(1, radius >> i) * 64;
			m_passBrushes.push_back(Blurbrush::create(bp));
		}

		m_bBrushesDirty = false;
	}

	//____ _blurSource() ______________________________________________________
	/**
	 * Maps an area of the backdrop to the blurred surface of a pane, in spx of its pixels.
	 */

	RectSPX Frost::_blurSource(const Pane& pane, const RectSPX& area) const
	{
		Surface* pBlur = pane.m_pBlur[pane.m_result];

		float scaleX = pBlur->pixelWidth() * 64 / float(pane.m_region.w);
		float scaleY = pBlur->pixelHeight() * 64 / float(pane.m_region.h);

		return RectSPX(	spx((area.x - pane.m_region.x) * scaleX), spx((area.y - pane.m_region.y) * scaleY),
						spx(area.w * scaleX), spx(area.h * scaleY));
	}

	//____ _firstPassRadius() _________________________________________________
	/**
	 * Radius of the first blur pass, in pixels of the shrunk backdrop.
	 */

	int Frost::_firstPassRadius() const
	{
		int pixels = ptsToSpx(m_radius, m_scale) / 64;
		return std::max(1, (pixels + m_downscale / 2) / m_downscale);
	}

	//____ _createSurface() ___________________________________________________

	Surface_p Frost::_createSurface(GfxDevice* pDevice, SizeI pixelSize)
	{
		return pDevice->surfaceFactory()->createSurface(WGBP(Surface,
			_.canvas = true,
			_.colorSpace = pDevice->canvas().colorSpace,
			_.format = PixelFormat::ARGB_8,
			_.sampleMethod = SampleMethod::Bilinear,
			_.scale = m_scale,
			_.size = pixelSize));
	}

} // namespace wg
