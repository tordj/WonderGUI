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
	CCanvas::CCanvas(ComponentHolder * pHolder, ICanvas * pInterface) : Component(pHolder)
	{
		m_pInterface = pInterface;
	}


	//____ render() __________________________________________________________

	void CCanvas::render(GfxDevice * pDevice, const Rect& _canvas)
	{
		//TODO: Support bitmap being of different surface kind than destination (Like GL/Software).
	
		Rect canvas = calcPresentationArea() + _canvas.pos();

		pDevice->setBlitSource(m_pSurface);
		pDevice->stretchBlit(canvas);
	}

	//____ alphaTest() _______________________________________________________

	bool CCanvas::alphaTest(const Coord& ofs, int markOpacity)
	{
		Rect canvas = calcPresentationArea();
		Size bmpSize = m_pSurface->size();

		return Util::markTestStretchRect(ofs, m_pSurface, bmpSize, canvas, markOpacity);
	}


	//____ regenSurface() ______________________________________________

	void CCanvas::regenSurface()
	{
		if (!m_pDevice && !m_pFactory)
			return;

		Size sz = m_fixedSize;

		if (sz.w == 0 && sz.h == 0)
			sz = _size();

		bool bSurfaceLost = m_pSurface;

		SurfaceFactory * pFactory = m_pFactory ? m_pFactory : m_pDevice->surfaceFactory();
		m_pSurface = pFactory->createSurface(sz, m_pixelFormat);
		m_pSurface->fill(m_backColor);

		if( m_pDevice )
			m_pDevice->setCanvas(m_pSurface);

		if (m_surfaceLostCallback != nullptr)
			m_surfaceLostCallback(m_pInterface);
	}

	//____ calcPresentationArea() _______________________________________________

	Rect CCanvas::calcPresentationArea() const
	{
		Size window = _size();
		Size bitmapSize = m_pSurface->size();

		switch (m_presentationScaling)
		{
		case SizePolicy2D::Scale:
			return Util::origoToRect(m_origo, window, Util::scaleToFit(bitmapSize, window));

		case SizePolicy2D::Stretch:
			return Rect(0,0,window);

		default:
			return Util::origoToRect(m_origo, window, bitmapSize);
		}
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

	bool CCanvas::setSurfaceSize(Size sz)
	{
		Size max = m_pDevice ? m_pDevice->surfaceFactory()->maxSize() : Size(65536,65536);

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

	void CCanvas::setComponentSize(Size sz)
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
		if (m_pSurface)
			m_pSurface->fill(m_backColor);
	}

	//____ setPresentationScaling() ____________________________________

	void CCanvas::setPresentationScaling(SizePolicy2D policy)
	{
		if (m_presentationScaling != policy)
		{
			Rect oldArea = calcPresentationArea();
			m_presentationScaling = policy;
			Rect newArea = calcPresentationArea();

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
			Rect oldArea = calcPresentationArea();
			m_origo = origo;
			Rect newArea = calcPresentationArea();

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

	void CCanvas::present(Rect area)
	{
		Rect dest = calcPresentationArea();
		Size bitmapSize = m_pSurface->size();

		int x1 = area.x * dest.w / bitmapSize.w;
		int x2 = (area.x + area.w) * dest.w / bitmapSize.w + 1;
		int y1 = area.y * dest.h / bitmapSize.h;
		int y2 = (area.y + area.h) * dest.h / bitmapSize.h + 1;

		Rect a2 = Rect::getUnion(dest, { x1,y1,x2 - x1,y2 - y1 });

		_requestRender(a2);
	}

	//____ preferredSize() ____________________________________________________

	Size CCanvas::preferredSize() const
	{
		return m_fixedSize == Size()? Size(16,16) : m_fixedSize;
	}

} //namespace wg
