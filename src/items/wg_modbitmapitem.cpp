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

#include <wg_modbitmapitem.h>
#include <wg_util.h>

namespace wg
{
	ModBitmapItem::ModBitmapItem(ItemHolder * pHolder, ModBitmap * pInterface) : Item(pHolder)
	{
		m_pInterface = pInterface;
	}


	//____ render() __________________________________________________________

	void ModBitmapItem::render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _clip)
	{
		//TODO: Support bitmap being of different surface kind than destination.
	
		Rect canvas = calcPresentationArea();

		pDevice->clipStretchBlit(Rect(canvas, _clip), m_pSurface, canvas);
	}

	//____ alphaTest() _______________________________________________________

	bool ModBitmapItem::alphaTest(const Coord& ofs, int markOpacity)
	{
		Rect canvas = calcPresentationArea();
		Size bmpSize = m_pSurface->size();

		return Util::markTestStretchRect(ofs, m_pSurface, bmpSize, canvas, markOpacity);
	}


	//____ regenSurface() ______________________________________________

	void ModBitmapItem::regenSurface()
	{
		if (!m_pDevice)
			return;

		Size sz = m_fixedSize;

		if (sz.w == 0 && sz.h == 0)
			sz = _size();

		bool bSurfaceLost = m_pSurface;
		m_pSurface = m_pDevice->surfaceFactory()->createSurface(sz, m_pixelType);
		m_pSurface->fill(m_backColor);

		m_pDevice->setCanvas(m_pSurface);

		if (m_bitmapLostCallback != nullptr)
			m_bitmapLostCallback(m_pInterface);
	}

	//____ calcPresentationArea() _______________________________________________

	Rect ModBitmapItem::calcPresentationArea() const
	{
		Rect window = _geo();
		Size bitmapSize = m_pSurface->size();

		switch (m_presentationScaling)
		{
		case SizePolicy2D::Scale:
			return Util::origoToRect(m_origo, window, Util::scaleToFit(bitmapSize, window.size()));

		case SizePolicy2D::Stretch:
			return window;

		default:
			return Util::origoToRect(m_origo, window, bitmapSize);
		}
	}


	//____ setDevice() _________________________________________________

	bool ModBitmapItem::setDevice(GfxDevice * pDevice)
	{
		m_pDevice = pDevice;
		regenSurface();
		return true;
	}

	//____ setLostCallback() ___________________________________________

	void ModBitmapItem::setLostCallback(std::function<void(ModBitmap*)> func)
	{
		m_bitmapLostCallback = func;
	}

	//____ setPixelType() _________________________________________________

	bool ModBitmapItem::setPixelType(PixelType format)
	{
		if (format != PixelType::BGRA_8 && format != PixelType::BGR_8)
			return false;

		if (format != m_pixelType)
		{
			m_pixelType = format;
			regenSurface();
		}
		return true;
	}

	//____ setBitmapSize() ___________________________________________________

	bool ModBitmapItem::setBitmapSize(Size sz)
	{
		Size max = m_pDevice->surfaceFactory()->maxSize();

		if (sz.w > max.w || sz.h > max.h)
			return false;							// Requested size bigger than factory can create.

		if (sz != m_fixedSize)
		{
			m_fixedSize = sz;
			regenSurface();
		}

		return true;
	}

	//____ setItemSize() ___________________________________________________

	void ModBitmapItem::setItemSize(Size sz)
	{
		if (m_fixedSize.w == 0 && m_fixedSize.h == 0)
			regenSurface();
	}

	//____ setBackColor() ______________________________________________

	void ModBitmapItem::setBackColor(Color color)
	{
		if (color != m_backColor)
		{
			m_backColor = color;
			regenSurface();
		}
	}

	//____ clear() _____________________________________________________

	void ModBitmapItem::clear()
	{
		if (m_pSurface)
			m_pSurface->fill(m_backColor);
	}

	//____ setPresentationScaling() ____________________________________

	void ModBitmapItem::setPresentationScaling(SizePolicy2D policy)
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

	void ModBitmapItem::setOrigo(Origo origo)
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

	void ModBitmapItem::present()
	{
		_requestRender(calcPresentationArea());
	}

	void ModBitmapItem::present(Rect area)
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




} //namespace wg