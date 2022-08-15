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

#include <wg_cabi.h>
#include <wg_cabi_surface.h>


namespace wg
{
	const TypeInfo CABISurface::TYPEINFO = { "CABISurface", &Surface::TYPEINFO };


	//____ create ______________________________________________________________

	CABISurface_p CABISurface::create( wg_obj object)
	{
		return CABISurface_p(new CABISurface(object));
	}

	//____ constructor _____________________________________________________________

	CABISurface::CABISurface(wg_obj object) : Surface( Blueprint(), PixelFormat::BGRA_8, SampleMethod::Bilinear )
	{
		CABI::object->retain(object);

		m_cSurface = object;

		// We have initialized with an empty Blueprint. Now lets set correct values.

		m_id				= CABI::surface->getSurfaceIdentity(object);
		m_scale				= CABI::surface->getSurfaceScale(object);
		m_pixelDescription	= * (PixelDescription*)CABI::surface->surfacePixelDescription(object);
		
		wg_sizeI pixSize 	= CABI::surface->surfacePixelSize(object);
		m_size				= * (SizeI*) &pixSize;
		m_sampleMethod		= (SampleMethod)CABI::surface->surfaceSampleMethod(object);
		m_bMipmapped		= CABI::surface->surfaceIsMipmapped(object);
		m_bTiling			= CABI::surface->surfaceIsTiling(object);
		m_bCanvas			= CABI::surface->surfaceCanBeCanvas(object);
		m_pClut				= (Color8*)CABI::surface->surfaceClut(object);
		m_pBaggage			= reinterpret_cast<Object*>(CABI::surface->getSurfaceBaggage(object));
	}



	//____ Destructor ______________________________________________________________

	CABISurface::~CABISurface()
	{
		if (m_cObserverId != 0)
			CABI::surface->removeSurfaceObserver(m_cSurface,m_cObserverId);

		CABI::object->release(m_cSurface);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CABISurface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setScale() _________________________________________________________

	void CABISurface::setScale(int scale)
	{
		CABI::surface->setSurfaceScale(m_cSurface, scale);
	}

	//____ scale() ____________________________________________________________

	int CABISurface::scale() const
	{
		return CABI::surface->getSurfaceScale(m_cSurface);
	}

	//____ allocPixelBuffer() _________________________________________________

	const PixelBuffer CABISurface::allocPixelBuffer(const RectI& rect)
	{
		auto pixbuf = CABI::surface->allocPixelBufferFromRect(m_cSurface, (const wg_rectI*)&rect);

		return { (PixelFormat) pixbuf.format, pixbuf.pPixels, (Color8*) pixbuf.pClut, * (RectI*) &pixbuf.rect, pixbuf.pitch };
	}

	//____ pushPixels() _______________________________________________________

	bool CABISurface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		wg_pixelBuffer pixbuf = { (wg_pixelFormat)buffer.format, buffer.pPixels, (wg_color8*)buffer.pClut, *(wg_rectI*)&buffer.rect, buffer.pitch };

		return CABI::surface->pushPixelsFromRect(m_cSurface, &pixbuf, (const wg_rectI*) &bufferRect);
	}

	//____ pullPixels() _______________________________________________________

	void CABISurface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		wg_pixelBuffer pixbuf = { (wg_pixelFormat)buffer.format, buffer.pPixels, (wg_color8*)buffer.pClut, *(wg_rectI*)&buffer.rect, buffer.pitch };

		CABI::surface->pullPixelsFromRect(m_cSurface, &pixbuf, (const wg_rectI*)&bufferRect);
	}

	//____ freePixelBuffer() __________________________________________________

	void CABISurface::freePixelBuffer(const PixelBuffer& buffer)
	{
		wg_pixelBuffer pixbuf = { (wg_pixelFormat)buffer.format, buffer.pPixels, (wg_color8*)buffer.pClut, *(wg_rectI*)&buffer.rect, buffer.pitch };

		CABI::surface->freePixelBuffer(m_cSurface, &pixbuf);
	}

	//____ alpha() ____________________________________________________________

	int CABISurface::alpha( CoordSPX coord )
	{
		return CABI::surface->surfaceAlpha(m_cSurface, { coord.x, coord.y });
	}

	//____ fill() _____________________________________________________________

	bool CABISurface::fill(HiColor color)
	{
		return CABI::surface->fillSurface(m_cSurface, *(wg_color*)&color);
	}

	bool CABISurface::fill(const RectI& region, HiColor color )
	{
		return CABI::surface->fillSurfaceRect( m_cSurface, (const wg_rectI*) &region, * (wg_color*)&color );
	}

	//____ addObserver() ______________________________________________________

	int CABISurface::addObserver(const std::function<void(int nRects, const RectSPX* pRects)>& func)
	{
		if( m_pObserver == nullptr )
			m_cObserverId = CABI::surface->addSurfaceObserver(m_cSurface, [](int nRects, const wg_rectSPX* pRects, void* pData, int data) { ((CABISurface*)pData)->_notifyObservers(nRects, (RectSPX*)pRects); }, this, 0);

		return Surface::addObserver(func);
	}

	//____ removeObserver() ___________________________________________________

	bool CABISurface::removeObserver(int observerId)
	{
		Surface::removeObserver(observerId);

		if (m_pObserver == nullptr);
		CABI::surface->removeSurfaceObserver(m_cSurface, m_cObserverId);

		return true;
	}



} // namespace wg
