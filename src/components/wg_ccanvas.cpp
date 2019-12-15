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

	CCanvas::CCanvas(GeoComponent::Holder * pHolder) : GeoComponent(pHolder)
	{
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
			m_surfaceLostCallback(this);
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


	//____ setDevice() ________________________________________________________
	/**
	*	@brief Sets the GfxDevice to be used for drawing operations
	*
	*	@param	pDevice		Pointer to the GfxDevice to be used to draw on the Surface.
	*
	*	The specified GfxDevice will be initialized to use the Surface of this component
	*	as its canvas and can be used to draw onto it. To draw onto the Surface you start
	*	with a call to the device's beginRender(), followed by the calls for your drawing operations and endRender().
	*	Finally you need to call the present() method of this CCanvas to make sure that what has been
	*	drawn is copied to the screen.
	*
	*	The device should be reserved for this CCanvas until replaced and not used for anything else. Calling this
	*	method is likely to discard your current Surface. You will need to specify a GfxDevice or SurfaceFactory for
	*	a Surface to be generated and displayed.
	*
	*	@return	True if successful, False if device could not be set.
	**/

	bool CCanvas::setDevice(GfxDevice * pDevice)
	{
		if (pDevice != m_pDevice)
		{
			m_pDevice = pDevice;
			regenSurface();
		}
		return true;
	}

	//____ setFactory() ________________________________________________
	/**
	*	@brief	Sets the SurfaceFactory used to create the Surface
	*
	*	@param	pFactory	Pointer to the SurfaceFactory to be used to create the Surface. If nullptr
	*						the default SurfaceFactory of the GfxDevice will be set.
	*
	*	Specifies the SurfaceFactory used to generate and regenerate the Surface. If different from the
	*   one already set, the current surface will be discarded and a new one created.
	*
	*	The SurfaceFactory only needs to be specified if you want a Surface of a different type than
	*   what is default for your specified GfxDevice or if you don't specify any GfxDevice.
	*
	*	@return	True if successfull. False if pFactory can't be used as a canvas by the current GfxDevice.
	**/

	bool CCanvas::setFactory(SurfaceFactory * pFactory)
	{
		if (pFactory != m_pFactory)
		{
			m_pFactory = pFactory;
			regenSurface();
		}
		return true;
	}


	//____ setLostCallback() ___________________________________________
	/**
	*	@brief	Sets a callback for when the Surface has been discarded.
	*
	*	@param func		Function to be called. Nullptr is allowed and will simply remove
	*					previously set callback.
	*
	*	Sets a function that will be called when the Surface for this component has been discarded.
	*	The purpose is to redraw any graphics that has been lost.
	*
	*	The new Surface has been created, filled with the background color and is ready to be
	*	drawn upon when the function is called.
	*
	**/

	void CCanvas::setLostCallback(std::function<void(CCanvas*)> func)
	{
		m_surfaceLostCallback = func;
	}

	//____ setPixelFormat() _____________________________________________________
	/**
	*	@brief Sets the pixel format for the surface.
	*
	*	@param	type	The pixel format to be used by the surface. This needs to
	*					be one of BGR_8, BGRA_8, BGR_16 or BGRA_16.
	*
	*	Sets the pixel format for the surface.
	*	Changing the pixel format will discard and recreate the surface, destroying all its content.
	*	Default pixel format for CCanvas is BGR_8, which means that no alpha channel is included.
	*
	*	@return True if pixel format was accepted, otherwise false.
	**/

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

	//____ setSize() ___________________________________________________
	/**
	*	@brief Sets a fixed size for the surface.
	*
	*	@param SizeI		The size for the surface in pixels, no smaller than {1,1} and no larger than
	*					allowed by the SurfaceFactory in use, or {0,0} to return the surface to dynamic
	*					resizing.
	*
	*	Sets a fixed size for the surface. When the surface has a fixed size it will not be discarded when
	*	resizing CCanvas. Instead it will be positioned and scaled within the CCanvas area according to the
	*	origo and presentation scaling settings.
	*
	*	Setting a fixed size different from the surfaces current size will discard the current surface.
	*
	*	@return		False if specified size was out of allowed range.
	**/

	bool CCanvas::setSize(SizeI sz)
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


	//____ setBackColor() ______________________________________________
	/**
	*	@brief Sets the back color of the surface.
	*
	*	@param Color	Background color for the surface.
	*
	*	Sets the back color of the the surface, which the surface is filled with when generated. The alpha
	*	value of the color will be respected IF THE SURFACE HAS AN ALPHA CHANNEL, allowing
	*	for transparent and semi-transparent background.
	*
	*	Default back color is White (0xFFFFFFFF). Changing the back color will discard the current surface.
	**/

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

	//____ setPresentationScaling() ___________________________________________
	/**
	*	@brief Sets how a fixed size surface is scaled.
	*
	*	@param	policy	Either Original, Stretch or Scale.
	*
	*	Specifies how a fixed size surface is stretched or scaled when the surface is
	*	of a different size than the CCanvas component.
	*
	*	Default value is SizePolicy2D::Original.
	**/

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

	//____ setOrigo() _________________________________________________________
	/**
	*	@brief Sets origo for a fixed size surface
	*
	*	Sets origo to be used to align the surface when it is of a different size than
	*	the CCanvas, which can only happen if it has been set to a fixed size.
	*
	**/

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
	/**
	*	@brief	Redraw the component with the content of the surface.
	*
	*	Marks the area of the component as dirty, forcing a redraw with the content
	*	of the surface during its next render update.
	*
	*	If only parts of the surface has been changed, you can use one or several calls to
	*	present(RectI) instead to increase performance.
	*
	**/

	void CCanvas::present()
	{
		_requestRender(calcPresentationArea());
	}

	/**
	*	@brief	Redraw part of the component with the content of the surface.
	*
	*	@param	area	The area of the surface in pixels that has been modified and needs to be presented.
	*
	*	Marks the area of the component displaying the specified part of the surface as dirty,
	*	forcing a redraw with the content of the surface during its next render update.
	*
	**/

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

	//____ setComponentSize() ___________________________________________________

	void CCanvas::setComponentSize(SizeI sz)
	{
		if (m_fixedSize.w == 0 && m_fixedSize.h == 0)
			regenSurface();
	}

	//____ preferredSize() ____________________________________________________

	SizeI CCanvas::preferredSize() const
	{
		// We use pointsToRaw here instead of pixelsToRaw since a pixel in our surface
		// should be scaled to a point in UI.

		return m_fixedSize.isEmpty()? SizeI(16*4,16*4) : pointsToRawAligned( m_fixedSize );
	}

} //namespace wg
