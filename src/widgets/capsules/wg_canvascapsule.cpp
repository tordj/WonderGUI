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
#include <wg_context.h>
#include <wg_internal.h>
#include <wg_util.h>

namespace wg
{
	const TypeInfo CanvasCapsule::TYPEINFO = { "CanvasCapsule", &Capsule::TYPEINFO };

	//____ constructor ____________________________________________________________

	CanvasCapsule::CanvasCapsule()
	{
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

	//____ _render() _____________________________________________________________

	void CanvasCapsule::_render(GfxDevice* pDevice, const Rect& _canvas, const Rect& _window)
	{
		// Possibly regenerate the canvas

		if (!m_pCanvas)
		{
			SurfaceFactory* pFactory = m_pFactory ? m_pFactory : Base::activeContext()->surfaceFactory();
			if (!pFactory)
			{
				//TODO: Error handling!
				return;
			}

			m_pCanvas = pFactory->createSurface(m_size.px(), m_canvasFormat, SurfaceFlag::Canvas);
			m_patches.clear();
			m_patches.add(m_size);
		}

		// Generate the clip buffer

		int allocSize = m_patches.size() * sizeof(RectI);
		char * pClipBuffer = Base::memStackAlloc(allocSize);

		RectI* pDst = (RectI*) pClipBuffer;
		const Rect* pSrc = m_patches.begin();
		while (pSrc != m_patches.end())
			*pDst++ = (*pSrc++).px();

		// Render 

		pDevice->beginCanvasUpdate(m_pCanvas, m_patches.size(), (RectI*)pClipBuffer);

		OO(skin)._render(pDevice, _canvas.size(), m_state);

		// Render children recursively

		Rect canvas = OO(skin)._contentRect(_canvas, m_state);

		if (canvas != _canvas)
		{
			auto savedClipData = Util::limitClipList(pDevice, canvas);
			OO(slot._widget())->_render(pDevice, canvas, canvas);
			Util::popClipList(pDevice, savedClipData);
		}
		else
			OO(slot._widget())->_render(pDevice, canvas, canvas);

		pDevice->endCanvasUpdate();
		Base::memStackRelease(allocSize);

		// Blit our canvas

		pDevice->setBlitSource(m_pCanvas);
		pDevice->blit(_canvas.px());
	}

	//____ _cloneContent() _______________________________________________________

	void CanvasCapsule::_cloneContent(const Widget* _pOrg)
	{

	}

	//____ _resize() _____________________________________________________________

	void CanvasCapsule::_resize(const Size& size)
	{
		m_pCanvas = nullptr;
		Capsule::_resize(size);
	}

	//____ _childRequestRender() _________________________________________________

	void CanvasCapsule::_childRequestRender(StaticSlot* pSlot)
	{
		m_patches.clear();
		m_patches.push(m_size);
		Capsule::_childRequestRender(pSlot);
	}

	void CanvasCapsule::_childRequestRender(StaticSlot* pSlot, const Rect& rect)
	{
		Rect dirt = rect + _contentRect().pos();

		m_patches.add(dirt);
		_requestRender(dirt);
	}
}