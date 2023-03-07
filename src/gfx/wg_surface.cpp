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

#include <limits>
#include <memory.h>
#include <wg_surface.h>
#include <wg_dataset.h>
#include <wg_gfxutil.h>
#include <wg_gfxbase.h>
#include <wg_pixeltools.h>

namespace wg
{
	using namespace Util;


	const TypeInfo Surface::TYPEINFO = { "Surface", &Object::TYPEINFO };

	const uint8_t pixelConvTab_0[] = { 0xff };				// If a channel is missing it is assumed to have full value (alpha in RGB_8 is 255, RGB in A8 are all 255)
	const uint8_t pixelConvTab_2[] = { 0, 0xff };
	const uint8_t pixelConvTab_4[] = { 0, 0x55, 0xaa, 0xff };
	const uint8_t pixelConvTab_8[] = { 0, 0x24, 0x48, 0x6d, 0x91, 0xb6, 0xda, 0xff };
	const uint8_t pixelConvTab_16[] = { 0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
	const uint8_t pixelConvTab_32[] = {	0x00, 0x08, 0x10, 0x18, 0x20, 0x29, 0x31, 0x39, 0x41, 0x4a, 0x52, 0x5a, 0x62, 0x6a, 0x73, 0x7b,
												0x83, 0x8b, 0x94, 0x9c, 0xa4, 0xac, 0xb4, 0xbd, 0xc5, 0xcd, 0xd5, 0xde, 0xe6, 0xee, 0xf6, 0xff };
	const uint8_t pixelConvTab_64[] = {	0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c,
												0x40, 0x44, 0x48, 0x4c, 0x50, 0x55, 0x59, 0x5d, 0x61, 0x65, 0x69, 0x6d, 0x71, 0x75, 0x79, 0x7d,
												0x81, 0x85, 0x89, 0x8d, 0x91, 0x95, 0x99, 0x9d, 0xa1, 0xa5, 0xaa, 0xae, 0xb2, 0xb6, 0xba, 0xbe,
												0xc2, 0xc6, 0xca, 0xce, 0xd2, 0xd6, 0xda, 0xde, 0xe2, 0xe6, 0xea, 0xee, 0xf2, 0xf6, 0xfa, 0xff };
	const uint8_t pixelConvTab_128[] = { 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e,
												0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e,
												0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,
												0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e,
												0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e,
												0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,
												0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,
												0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xff };
	const uint8_t pixelConvTab_256[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
												0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
												0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
												0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
												0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f,
												0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f,
												0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f,
												0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
												0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f,
												0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f,
												0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
												0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf,
												0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
												0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
												0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef,
												0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };

	const uint8_t * Surface::s_pixelConvTabs[9] = { pixelConvTab_0, pixelConvTab_2, pixelConvTab_4, pixelConvTab_8, pixelConvTab_16, pixelConvTab_32, pixelConvTab_64,
													pixelConvTab_128, pixelConvTab_256 };

	//____ Surface() ____________________________________________________________

	Surface::Surface(const Blueprint& bp, PixelFormat defaultPixelFormat, SampleMethod defaultSampleMethod )
	{
		PixelFormat format = bp.format == PixelFormat::Undefined ? defaultPixelFormat : bp.format;
		SampleMethod method = bp.sampleMethod == SampleMethod::Undefined ? defaultSampleMethod : bp.sampleMethod;

		format = Util::translatePixelFormat(format);

		m_pixelFormat	= format;
		m_pPixelDescription = &Util::pixelFormatToDescription(format);

		m_size			= bp.size;
		m_scale			= bp.scale == 0 ? 64 : bp.scale;
		m_sampleMethod	= method;
		m_bTiling		= bp.tiling;
        m_bCanvas		= bp.canvas;
		m_bBuffered		= bp.buffered;
		m_bDynamic		= bp.dynamic;
		m_id			= bp.identity;
	}


	//____ ~Surface() ____________________________________________________________

	Surface::~Surface()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& Surface::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ scale() ____________________________________________________________

	int Surface::scale() const
	{
		return m_scale;
	}

	//____ fill() _________________________________________________________________
	/**
	 * Fill the surface with the specified color.
	 *
	 * @param col	Color value with which to fill the surface.
	 *
	 * The fill instruction replaces the surface content with the pixel values so
	 * no blending operations will take place. If the specified color has an alpha
	 * value it will replace the alpha channel of the surface (if it has any), not blend the color
	 * onto the surface.
	 *
	 * If the surface has less than 24-bits or color information, it will be filled with the
	 * pixel value most closely resembling the one specified.
	 *
	 **/

	bool Surface::fill( HiColor color )
	{
		return fill(RectI(0, 0, pixelSize()), color );
	}

	/**
	 * Fill the specied region of the surface with the specified color.
	 *
	 * @param col		Color value with which to fill the region.
	 * @param region	Region of surface to fill.
	 *
	 * The fill instruction replaces the surface content with the pixel values so
	 * no blending operations will take place. If the specified color has an alpha
	 * value it will replace the alpha channel of the surface (if it has any), not blend the color
	 * onto the surface.
	 *
	 * If the surface has less than 24-bits or color information, it will be filled with the
	 * pixel value most closely resembling the one specified.
	 *
	 **/
	bool Surface::fill(const RectI& region, HiColor color  )
	{
		auto pixbuf = allocPixelBuffer(region);

		PixelTools::fillBitmap(pixbuf.pixels, pixbuf.format, pixbuf.pitch, pixbuf.rect.size(), color);
		
		pullPixels(pixbuf);
		freePixelBuffer(pixbuf);

		return true;
	}

	//____ pullPixels() _______________________________________________________
	/**
	 * @brief Partial copy of pixels from PixelBuffer to Surface.
	 *
	 * Partial copy of pixels from PixelBuffer to Surface.
	 *
	 * @param buffer		PixelBuffer to copy pixels from.
	 * @param bufferRect	The source rectangle withing the PixelBuffer.
	 * @return 				True if operation could be performed.
	 *
	 * Only the specified rectangle within the PixelBuffer is copied to the Surface,
	 * the rest of the surface is unaffected. To copy the whole PixelBuffer to Surface, use pullPixels(const PixelBuffer& buffer) instead.
	 *
	 * Please note that the rectangle specified is within the PixelBuffer, not the Surface. You should therefore not add
	 * the offset of the PixelBuffer to bufferRect.
	 *
	 */
	void Surface::pullPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	{
		RectSPX rect = (bufferRect + buffer.rect.pos())*64;

		_notifyObservers(1, &rect);
	}


	//____ addObserver() ______________________________________________________

	int Surface::addObserver(const std::function<void(int nRects, const RectSPX* pRects)>& func)
	{
		int id = m_pObserver ? m_pObserver->id + 1 : 1;
		auto p = new Observer();

		p->id = id;
		p->func = func;
		p->pNext = m_pObserver;

		m_pObserver = p;
		return id;
	}

	//____ removeObserver() ___________________________________________________

	bool Surface::removeObserver(int observerId)
	{
		Observer** pPointer = &m_pObserver;

		while (*pPointer != nullptr)
		{
			if ((*pPointer)->id == observerId)
			{
				auto p = (*pPointer);
				(*pPointer) = p->pNext;
				delete p;
				return true;
			}

			pPointer = &(*pPointer)->pNext;
		}

		return false;
	}

	//____ _notifyObservers() _________________________________________________

	void Surface::_notifyObservers(int nRects, const RectSPX* pRects)
	{
		Observer* p = m_pObserver;
		while (p)
		{
			p->func(nRects, pRects);
			p = p->pNext;
		}
	}

	//____ blueprint() ________________________________________________________

	Surface::Blueprint Surface::blueprint() const
	{
		Blueprint bp;

		bp.buffered = m_bBuffered;
		bp.canvas = m_bCanvas;
		bp.palette = m_pPalette;
		bp.dynamic = m_bDynamic;
		bp.format = m_pixelFormat;
		bp.identity = m_id;
		bp.mipmap = m_bMipmapped;
		bp.sampleMethod = m_sampleMethod;
		bp.scale = m_scale;
		bp.size = m_size;
		bp.tiling = m_bTiling;

		return bp;
	}


	//_____ copy() _____________________________________________________________
	/**
	 * Copy the content of the specified surface to given coordinate of this surface
	 *
	 * @param	pSrcSurface		The surface from which to copy the graphics
	 * @param	dst				Destination coordinate for top-left corner of copied graphics.
	 *
	 * The entire content of pSrcSurface is considered a rectangular graphics block that
	 * will be copied to the coordinate specified. The content is copied as is, with color
	 * values and alpha channel, no blend operation is applied. Pixel format is converted
	 * as needed. If the source surface doesn't have an alpha channel, the alpha values of
	 * copied pixels are set to 255. Clipping is applied if needed. The destination coordinate
	 * must be within the bounds of the destination area.
	 *
	 * @return True if successful, otherwise false.
	 **/
	bool Surface::copy(CoordI dest, Surface * pSrcSurface )
	{
		if( !pSrcSurface )
			return false;

		return copy( dest, pSrcSurface, RectI(0,0,pSrcSurface->pixelSize()) );
	}

	/**
	 * Copy the specified rectangular area of the source surface to given coordinate of this surface.
	 *
	 * @param	pSrcSurface		The surface from which to copy the graphics block
	 * @param 	srcRect			Rectangular section of source surface to copy. Must be fully within
	 * 							the bounds of pSrcSurface.
	 * @param	dst				Destination coordinate for top-left corner of copied graphics.
	 *
	 * A rectangular graphics block is copied from the source surface to the coordinate specified.
	 * The content is copied as is, with color values and alpha channel, no blend operation is applied.
	 * Pixel format is converted as needed. If the source surface doesn't have an alpha channel, the alpha values of
	 * copied pixels are set to 255. Clipping is applied if needed. The destination coordinate must be within the
	 * bounds of the destination surface.
	 *
	 * @return True if successful, otherwise false.
	 **/
	bool Surface::copy(CoordI _dest, Surface * pSrcSurface, const RectI& _srcRect )
	{
		if( !pSrcSurface || pSrcSurface->pixelFormat() == PixelFormat::Undefined || m_pixelFormat == PixelFormat::Undefined )
			return false;

		// Clip as needed to get source and dest rectangles.
		
		RectI srcRect = RectI::overlap(_srcRect, pSrcSurface->m_size );
		if( srcRect.isEmpty() )
			return false;
			
		RectI destRect = RectI::overlap( { _dest, srcRect.size() }, m_size );
		if( destRect.isEmpty() )
			return false;
		
		srcRect.setSize( destRect.size() );
		
		// Allocate our buffers, push source pixels
		
		
		auto srcbuf = pSrcSurface->allocPixelBuffer(srcRect);
		bool bPushed = pSrcSurface->pushPixels(srcbuf);
		if (!bPushed)
		{
			//TODO: Error handling.
		}
		
		auto dstbuf = allocPixelBuffer( destRect );
		
		//
		
		auto& srcDesc = Util::pixelFormatToDescription(srcbuf.format);
		auto& dstDesc = Util::pixelFormatToDescription(m_pixelFormat);
		
		int dstPaletteEntries = 256;
		
		bool retVal = PixelTools::copyPixels(srcbuf.rect.w, srcbuf.rect.h, srcbuf.pixels, srcbuf.format, srcbuf.pitch - srcbuf.rect.w*srcDesc.bits/8,
								dstbuf.pixels, dstbuf.format, dstbuf.pitch - dstbuf.rect.w*dstDesc.bits/8, srcbuf.palette,
								m_pPalette, 256, dstPaletteEntries, 256);

		pullPixels(dstbuf);
		freePixelBuffer(dstbuf);
		
		pSrcSurface->freePixelBuffer(srcbuf);

		return retVal;
	}

	//____ convert() _________________________________________________________

	Surface_p Surface::convert( const Surface::Blueprint& _bp, SurfaceFactory* pFactory)
	{
		if (!pFactory)
			pFactory = GfxBase::defaultSurfaceFactory();

		if (!pFactory)
		{
			//TODO: Error handling!
			return Surface_p();
		}

		auto bp = _bp;
		
		bp.size = m_size;
		
		if( bp.scale == 0 )
			bp.scale = m_scale;
		
		if( bp.sampleMethod == SampleMethod::Undefined )
			bp.sampleMethod = m_sampleMethod;
		
		if( bp.format == PixelFormat::Undefined )
			bp.format = m_pixelFormat;
		
		auto pSurface = pFactory->createSurface(bp);
		if( pSurface->copy({ 0,0 }, this) )
			return pSurface;
		
		return nullptr;
	}


	//____ _alpha() ___________________________________________________________

	int Surface::_alpha(CoordSPX _coord, const PixelBuffer& buffer)
	{
		//TODO: Take endianess into account.
		//TODO: Take advantage of subpixel precision and interpolate alpha value if surface set to interpolate.

		CoordI coord(((_coord.x + 32) / 64) % m_size.w, ((_coord.y + 32) / 64) % m_size.h);

		switch (buffer.format)
		{
		case PixelFormat::Index_8_sRGB:
		case PixelFormat::Index_8_linear:
		{
			uint8_t index = buffer.pixels[buffer.pitch * coord.y + coord.x];
			return HiColor::unpackLinearTab[buffer.palette[index].a];
		}
		case PixelFormat::Alpha_8:
		{
			uint8_t* pPixel = buffer.pixels + buffer.pitch * coord.y + coord.x;
			return HiColor::unpackLinearTab[pPixel[0]];
		}
		case PixelFormat::BGRA_4_linear:
		{
			uint16_t pixel = *(uint16_t*)(buffer.pixels + buffer.pitch * coord.y + coord.x * 2);
			const uint8_t* pConvTab = s_pixelConvTabs[4];

			return HiColor::unpackLinearTab[pConvTab[pixel >> 12]];
		}
		case PixelFormat::BGRA_8_sRGB:
		case PixelFormat::BGRA_8_linear:
		{
			uint8_t* pPixel = buffer.pixels + buffer.pitch * coord.y + coord.x * 4;
			return HiColor::unpackLinearTab[pPixel[3]];
		}
		default:
			return 4096;
		}
	}

    //____ _isBlueprintValid() ________________________________________________

    bool Surface::_isBlueprintValid(const Blueprint& bp, SizeI maxSize)
    {
        SizeI size = bp.size;

        PixelFormat format = bp.format;
        if( format == PixelFormat::Undefined )
            format = PixelFormat::BGRA_8;

		if( size.isEmpty() )
			return false;
		
        if (size.w > maxSize.w || size.h > maxSize.h)
            return false;

        bool bIsIndexed = (format == PixelFormat::Index_8 || format == PixelFormat::Index_8_sRGB || format == PixelFormat::Index_8_linear);

        if (format < PixelFormat_min || format > PixelFormat_max || (bIsIndexed && bp.palette == nullptr))
            return false;

        if (bp.canvas && bIsIndexed)
            return false;

        if( bIsIndexed )
        {
            if( bp.mipmap )
                return false;            // Indexed can't be mipmapped.

            if( bp.palette == nullptr )
                return false;           // Indexed but palette is missing.
        }
                
        return true;
    }


	/**
	 * @fn virtual uint32_t	Surface::pixel( CoordI coord ) const
	 *
	 * Get the raw pixel value from the specified coordinate of the surface.
	 *
	 * @param coord Coordinate of pixel to get.
	 *
	 * The coordinate must be within the surface boundaries. A coordinate outside
	 * the surface will result in undefined behavior.
	 *
	 * If pixel is less than 32 bits in size, it is packed into the lower
	 * bits of the return value with the rest of the bits cleared.
	 *
	 * @return Pixel value in surface's native format.
	 *
	 */


	/**
	 * @fn virtual uint8_t Surface::alpha(CoordI coord) const
	 *
	 * Get the alpha value from the specified coordinate of the surface.
	 *
	 * @param coord Coordinate of pixel.
	 *
	 * Gets the alpha value of the pixel at the specified coordinate in the range of 0-255.
	 * The alpha value is by default used for opacity level,
	 * where 0 is a fully transparent pixel and 255 is opaque.
	 *
	 * The coordinate specified must be within the surface boundaries. A coordinate outside
	 * the surface will result in undefined behavior.
	 *
	 * @return Alpha value of pixel at coordinate.
	 */

	/**
	 * @fn virtual bool Surface::isOpaque() const
	 *
	 * Check if surface is entirely opaque.
	 *
	 * A surface is opaque if it contains no transparent or semi-transparent
	 * pixels at all and therefore is guaranteed to completely cover any graphics
	 * behind it. This method is called regularly in time-critical sections of
	 * the code to optimize rendering operations and therefore should not do any
	 * time consuming operations.
	 *
	 * @return True if surface is guaranteed to only contain completely opaque pixels. False if
	 * if it does contain (semi)transparent pixels or we simply just don't know.
	 */


	/**
	 * @fn virtual const PixelBuffer Surface::allocPixelBuffer(const RectI& rect)
	 * 
	 * @brief 	Allocate a buffer to access part of the surface
	 * 
	 * @param rect	A rectangular area (measured in pixels) of the Surface that should be
	 * 				mapped to this PixelBuffer.
	 * 
	 * @return 	A PixelBuffer providing info about the buffer, as well as working as a handle for
	 * 			future references.
	 * 
	 * Allocates a buffer that provides access to a rectangular part of the surface. First pixel
	 * in the buffer is the topmost pixel of the mapped rectangle. Mapping only part of the surface
	 * and not the whole surface can allow for significantly faster push/pull operations, as well as
	 * save memory.
	 * 
	 * The buffer should be expected to be uninitialized, but might already contain the
	 * content of the Surface rectangle depending on Surface implementation. For example is the
	 * buffer of a SoftSurface just a pointer into the pixels of the surface, providing
	 * direct access, while the OpenGL and Metal surfaces are uninitialized since we don't
	 * have direct access to the texture pixels.
	 * 
	 * To fill the buffer with the content of the surface rectangle, call pushPixels(). To copy the
	 * content of the buffer to the surface rectangle, use pullPixels(). These calls will do the actual
	 * pixel copying for an OpenGL and Metal surface, while just immediately return for a SoftSurface.
	 * 
	 * Once you are done with your buffer, call freePixelBuffer() to return the memory allocated.
	 */

	/**
	 * @fn virtual void freePixelBuffer(const PixelBuffer& buffer)
	 * 
	 * @brief 	Free a previously allocated PixelBuffer.
	 * 
	 * Free a previously allocated PixelBuffer.
	 * 
	 */

	/**
	 * @fn virtual bool	Surface::pushPixels(const PixelBuffer& buffer, const RectI& bufferRect)
	 * 
	 * @brief Partial copy of pixels from Surface to PixelBuffer.
	 * 
	 * Partial copy of pixels from Surface to PixelBuffer.
	 * 
	 * @param buffer		PixelBuffer to copy pixels to.
	 * @param bufferRect	The destination rectangle withing the PixelBuffer. 
	 * @return 				True if operation could be performed.
	 * 
	 * Only the specified rectangle within the PixelBuffer is updated with pixels from the Surface,
	 * the rest is unaffected. To update the whole PixelBuffer, you can use pushPixels(const PixelBuffer& buffer) instead.
	 * 
	 * Please note that the rectangle specified is within the PixelBuffer, not the Surface. You should therefore not add
	 * the offset of the PixelBuffers mapping to bufferRect.
	 * 
	 */

	 //____ alpha() ____________________________________________________________
	 /**
	  * @fn virtual int	Surface::alpha( CoordSPX coord )
	  *
	  * Get the alpha value from the specified coordinate of the surface.
	  *
	  * @param coord Coordinate, specified in subpixels.
	  *
	  * Gets the alpha value of the pixel at the specified coordinate in the range of 0-4096.
	  * The alpha value is by default used for opacity level,
	  * where 0 is a fully transparent pixel and 4096 is opaque.
	  * The coordinate specified must be within the surface boundaries. A coordinate outside
	  * the surface will result in undefined behavior.
	  *
	  * @return Alpha value of pixel at coordinate.
	  */


} // namespace wg
