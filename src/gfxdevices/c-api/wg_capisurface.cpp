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


#include <wg_capisurface.h>
#include <wg_c_object.h>
#include <wg_c_surface.h>


namespace wg
{
	const TypeInfo CAPISurface::TYPEINFO = { "CAPISurface", &Surface::TYPEINFO };


	//____ create ______________________________________________________________

	CAPISurface_p CAPISurface::create( wg_obj object)
	{
		return CAPISurface_p(new CAPISurface(object));
	}

	//____ constructor _____________________________________________________________

	CAPISurface::CAPISurface(wg_obj object) : Surface( Blueprint(), PixelFormat::BGRA_8, SampleMethod::Bilinear )
	{
		wg_retain(object);

		m_cSurface = object;

		// We have initialized with an empty Blueprint. Now lets set correct values.

		m_id				= wg_getSurfaceIdentity(object);
		m_scale				= wg_getSurfaceScale(object);
		m_pixelDescription	= * (PixelDescription*) wg_surfacePixelDescription(object);
		
		wg_sizeI pixSize 	= wg_surfacePixelSize(object);
		m_size				= * (SizeI*) &pixSize;
		m_sampleMethod		= (SampleMethod)wg_surfaceSampleMethod(object);
		m_bMipmapped		= wg_surfaceIsMipmapped(object);
		m_bTiling			= wg_surfaceIsTiling(object);
		m_bCanvas			= wg_surfaceCanBeCanvas(object);
		m_pClut				= (Color8*)wg_surfaceClut(object);
		m_pBaggage			= reinterpret_cast<Object*>(wg_getSurfaceBaggage(object));
	}



	//____ Destructor ______________________________________________________________

	CAPISurface::~CAPISurface()
	{
		if (m_cObserverId != 0)
			wg_removeSurfaceObserver(m_cSurface,m_cObserverId);

		wg_release(m_cSurface);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CAPISurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setScale() _________________________________________________________

	void CAPISurface::setScale(int scale)
	{
		wg_setSurfaceScale(m_cSurface, scale);
	}

	//____ scale() ____________________________________________________________

	int CAPISurface::scale() const
	{
		return wg_getSurfaceScale(m_cSurface);
	}

	//____ allocPixelBuffer() _________________________________________________

	const PixelBuffer CAPISurface::allocPixelBuffer(const RectI& rect)
	{
		auto pixbuf = wg_allocPixelBufferFromRect(m_cSurface, (const wg_rectI*)&rect);

		return { (PixelFormat) pixbuf.format, pixbuf.pPixels, (Color8*) pixbuf.pClut, * (RectI*) &pixbuf.rect, pixbuf.pitch };
	}

	//____ pushPixels() _______________________________________________________

	bool CAPISurface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		wg_pixelBuffer pixbuf = { (wg_pixelFormat)buffer.format, buffer.pPixels, (wg_color8*)buffer.pClut, *(wg_rectI*)&buffer.rect, buffer.pitch };

		return wg_pushPixelsFromRect(m_cSurface, &pixbuf, (const wg_rectI*) &bufferRect);
	}

	//____ pullPixels() _______________________________________________________

	void CAPISurface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		wg_pixelBuffer pixbuf = { (wg_pixelFormat)buffer.format, buffer.pPixels, (wg_color8*)buffer.pClut, *(wg_rectI*)&buffer.rect, buffer.pitch };

		wg_pullPixelsFromRect(m_cSurface, &pixbuf, (const wg_rectI*)&bufferRect);
	}

	//____ freePixelBuffer() __________________________________________________

	void CAPISurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		wg_pixelBuffer pixbuf = { (wg_pixelFormat)buffer.format, buffer.pPixels, (wg_color8*)buffer.pClut, *(wg_rectI*)&buffer.rect, buffer.pitch };

		wg_freePixelBuffer(m_cSurface, &pixbuf);
	}

	//____ alpha() ____________________________________________________________

	int CAPISurface::alpha( CoordSPX coord )
	{
		return wg_surfaceAlpha(m_cSurface, { coord.x, coord.y });
	}

	//____ fill() _____________________________________________________________

	bool CAPISurface::fill(HiColor color)
	{
		return wg_fillSurface(m_cSurface, *(wg_color*)&color);
	}

	bool CAPISurface::fill(const RectI& region, HiColor color )
	{
		return wg_fillSurfaceRect( m_cSurface, (const wg_rectI*) &region, * (wg_color*)&color );
	}

	//____ addObserver() ______________________________________________________

	int CAPISurface::addObserver(const std::function<void(int nRects, const RectSPX* pRects)>& func)
	{
		if( m_pObserver == nullptr )
			m_cObserverId = wg_addSurfaceObserver(m_cSurface, [](int nRects, const wg_rectSPX* pRects, void* pData, int data) { ((CAPISurface*)pData)->_notifyObservers(nRects, (RectSPX*)pRects); }, this, 0);

		return Surface::addObserver(func);
	}

	//____ removeObserver() ___________________________________________________

	bool CAPISurface::removeObserver(int observerId)
	{
		Surface::removeObserver(observerId);

		if (m_pObserver == nullptr);
			wg_removeSurfaceObserver(m_cSurface, m_cObserverId);

		return true;
	}



} // namespace wg
