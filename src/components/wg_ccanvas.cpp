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

#include <wg_ccanvas.h>
#include <wg_util.h>

namespace wg
{
	using namespace Util;

	CCanvas::CCanvas(GeoComponent::Holder * pHolder, ICanvas * pInterface) : GeoComponent(pHolder)
	{
		m_pComponent = pInterface;
	}


	//____ render() __________________________________________________________

	void CCanvas::render(GfxDevice * pDevice, const RectI& _canvas)
	{
		//TODO: Support bitmap being of different surface kind than destination (Like GL/Software).

		RectI canvas = rawToPixels( calcPresentationArea() + _canvas.pos() );

		pDevice->setBlitSource(m_pSurface);
		pDevice->stretchBlit(canvas);
	}

	//____ alphaTest() _______________________________________________________

	bool CCanvas::alphaTest(const CoordI& ofs, int markOpacity)
	{
		RectI canvas = calcPresentationArea();
		SizeI bmpSize = m_pSurface->size();

		return Util::markTestStretchRect(ofs, m_pSurface, bmpSize, canvas, markOpacity);
	}


	//____ regenSurface() ______________________________________________

	void CCanvas::regenSurface()
	{
		if (!m_pDevice && !m_pFactory)
			return;

		SizeI sz = m_fixedSize;

		if (sz.w == 0 && sz.h == 0)
			sz = rawToPixels(_size());

		SurfaceFactory * pFactory = m_pFactory ? m_pFactory : m_pDevice->surfaceFactory();
		m_pSurface = pFactory->createSurface(sz, m_pixelFormat);
		m_pSurface->fill(m_backColor);

		if( m_pDevice )
			m_pDevice->setCanvas(m_pSurface);

		if (m_surfaceLostCallback != nullptr)
			m_surfaceLostCallback(m_pComponent);
	}

	//____ calcPresentationArea() _______________________________________________

	RectI CCanvas::calcPresentationArea() const
	{
		SizeI window = rawToPixels(_size());
		SizeI bitmapSize = m_pSurface->size();

		SizeI output;

		switch (m_presentationScaling)
		{
		case SizePolicy2D::Scale:
			output = Util::origoToRect(m_origo, window, Util::scaleToFit(bitmapSize, window));
			break;
		case SizePolicy2D::Stretch:
			output = RectI(0,0,window);
			break;
		default:
			SizeI scaledBitmapSize = pointsToPixels(bitmapSize);
			output = Util::origoToRect(m_origo, window, scaledBitmapSize);
			break;
		}

		return pixelsToRaw(output);
	}


	//____ setDevice() _________________________________________________

	bool CCanvas::setDevice(GfxDevice * pDevice)
	{
		if (pDevice != m_pDevice)
		{
			m_pDevice = pDevice;
			regenSurface();
		}
		return true;
	}

	//____ setSurfaceFactory() ________________________________________________

	bool CCanvas::setSurfaceFactory(SurfaceFactory * pFactory)
	{
		if (pFactory != m_pFactory)
		{
			m_pFactory = pFactory;
			regenSurface();
		}
		return true;
	}


	//____ setLostCallback() ___________________________________________

	void CCanvas::setLostCallback(std::function<void(ICanvas*)> func)
	{
		m_surfaceLostCallback = func;
	}

	//____ setPixelFormat() _________________________________________________

	bool CCanvas::setPixelFormat(PixelFormat format)
	{
		if (format != PixelFormat::BGRA_8 && format != PixelFormat::BGR_8)
			return false;

		if (format != m_pixelFormat)
		{
			m_pixelFormat = format;
			regenSurface();
		}
		return true;
	}

	//____ setSurfaceSize() ___________________________________________________

	bool CCanvas::setSurfaceSize(SizeI sz)
	{
		SizeI max = m_pDevice ? m_pDevice->surfaceFactory()->maxSize() : SizeI(65536,65536);

		if (sz.w > max.w || sz.h > max.h)
			return false;							// Requested size bigger than factory can create.

		if (sz != m_fixedSize)
		{
			m_fixedSize = sz;
			regenSurface();
		}

		return true;
	}

	//____ setComponentSize() ___________________________________________________

	void CCanvas::setComponentSize(SizeI sz)
	{
		if (m_fixedSize.w == 0 && m_fixedSize.h == 0)
			regenSurface();
	}

	//____ setBackColor() ______________________________________________

	void CCanvas::setBackColor(Color color)
	{
		if (color != m_backColor)
		{
			m_backColor = color;
			regenSurface();
		}
	}

	//____ clear() _____________________________________________________

	void CCanvas::clear()
	{
		//TODO: Use GfxDevice instead of slow Surface::fill();

		if (m_pSurface)
			m_pSurface->fill(m_backColor);
	}

	//____ setPresentationScaling() ____________________________________

	void CCanvas::setPresentationScaling(SizePolicy2D policy)
	{
		if (m_presentationScaling != policy)
		{
			RectI oldArea = calcPresentationArea();
			m_presentationScaling = policy;
			RectI newArea = calcPresentationArea();

			if (oldArea != newArea)
			{
				_requestRender(oldArea);
				_requestRender(newArea);
			}
		}
	}

	//____ setOrigo() __________________________________________________

	void CCanvas::setOrigo(Origo origo)
	{
		if (m_origo != origo)
		{
			RectI oldArea = calcPresentationArea();
			m_origo = origo;
			RectI newArea = calcPresentationArea();

			if (oldArea != newArea)
			{
				_requestRender(oldArea);
				_requestRender(newArea);
			}
		}
	}

	//____ present() ___________________________________________________

	void CCanvas::present()
	{
		_requestRender(calcPresentationArea());
	}

	void CCanvas::present(RectI area)
	{
		RectI dest = rawToPixels(calcPresentationArea());
		SizeI bitmapSize = m_pSurface->size();

		int x1 = area.x * dest.w / bitmapSize.w;
		int x2 = (area.x + area.w) * dest.w / bitmapSize.w + 1;
		int y1 = area.y * dest.h / bitmapSize.h;
		int y2 = (area.y + area.h) * dest.h / bitmapSize.h + 1;

		RectI a2 = RectI::getUnion(dest, { x1,y1,x2 - x1,y2 - y1 });

		_requestRender(pixelsToRaw(a2));
	}

	//____ preferredSize() ____________________________________________________

	SizeI CCanvas::preferredSize() const
	{
		// We use pointsToRaw here instead of pixelsToRaw since a pixel in our surface
		// should be scaled to a point in UI.

		return m_fixedSize.isEmpty()? SizeI(16*4,16*4) : pointsToRawAligned( m_fixedSize );
	}

} //namespace wg
