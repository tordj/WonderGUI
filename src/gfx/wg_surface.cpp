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

		Util::pixelFormatToDescription(format, m_pixelDescription);

		m_size			= bp.size;
		m_scale			= bp.scale;
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

	//____ setScale() _________________________________________________________

	void Surface::setScale(int scale)
	{
		m_scale = scale;
	}

	//____ scale() ____________________________________________________________

	int Surface::scale() const
	{
		return m_scale;
	}

	//____ setTiling() ________________________________________________________
	/**
	 * @brief Set whether Surface should be tiling.
	 * 
	 * Set whether Surface should be tiling, e.g. if reads outside the Surface
	 * should "loop back over the surface" or result in transparency.
	 * 
	 * @param bTiling	True if Surface should be tiling, otherwise false.
	 * 
	 * @return True if successful.
	 * 
	 * A Surface needs to be tiling in order for GfxDevice::tile() and similar functions
	 * to work properly.
	 * 
	 * It also affects other blit methods such as rotScaleBlit() which will either
	 * fill the destination with a tiled version or just produce one transformed copy with 
	 * transparency around.
	 * 
	 * A Surface set to tiling can be used in normal blit operatons as well, but 
	 * colors from one edge tend to bleed to the one across when using SampleMethod::Bilinear
	 * since they are considered neighbors.
	 * 
	 * Also worth noting is that SoftSurface puts extra restrictions on tiling Surfaces -
	 * their length and height must be a power of two. Calling this method to enable tiling for 
	 * a SoftSurface where not both dimension are a power of two will fail.
	 */

	//____ colorToPixel() ____________________________________________________________
	/**
	 * Convert specified color to a pixel in surface's native format.
	 *
	 * @param col	Color to be converted to pixel
	 *
	 * Converts the specified color to a raw pixel in surface's native format. If the native
	 * pixel format is less than 32 bits, the upper bits of the return value are cleared.
	 * The pixel value returned is the one closes resembling the specified color, given the
	 * limitations of the surfaces native format.
	 *
	 * The alpha channel of the color value is ignored if surface does not contain an alpha channel.
	 *
	 * Note: This call is very slow on indexed surfaces (PixelFormat::I8), since the whole clut
	 * needs to be searched for the closest color with quite some math on every entry.
	 *
	 * @return Pixel value in surface's native format that closest resembles specified color.
	 *
	 **/
	uint32_t Surface::colorToPixel( const HiColor& color ) const
	{
		uint32_t pix;

		uint8_t* pPackTab = m_pixelDescription.bLinear ? HiColor::packLinearTab : HiColor::packSRGBTab;
		Color8 col(pPackTab[color.r], pPackTab[color.g], pPackTab[color.b], HiColor::packLinearTab[color.a]);

		if (m_pixelDescription.bIndexed )
		{
			pix = 0;                                                    // Just to avoid compiler warning.
			int closestValue = std::numeric_limits<int>::max();
			for (int i = 0; i < 255; i++)
			{
				int rDiff = m_pClut[i].r - col.r;
				int gDiff = m_pClut[i].g - col.g;
				int bDiff = m_pClut[i].b - col.b;
				int aDiff = m_pClut[i].a - col.a;
				int value = rDiff*rDiff + gDiff*gDiff + bDiff*bDiff + aDiff*aDiff;
				if (value < closestValue)
				{
					closestValue = value;
					pix = i;
					if (closestValue == 0)
						break;
				}
			}
		}
		else
		{
			pix =	((col.r >> m_pixelDescription.R_loss) << m_pixelDescription.R_shift) |
					((col.g >> m_pixelDescription.G_loss) << m_pixelDescription.G_shift) |
					((col.b >> m_pixelDescription.B_loss) << m_pixelDescription.B_shift) |
					((col.a >> m_pixelDescription.A_loss) << m_pixelDescription.A_shift);

			if( m_pixelDescription.bBigEndian != isSystemBigEndian() )
			{

				if( m_pixelDescription.bits == 16 )
					pix = endianSwap( (uint16_t) pix );
				else if( m_pixelDescription.bits == 16 )
					pix = endianSwap( pix );
			}
		}
		return pix;
	}

	//____ pixelToColor() ____________________________________________________________
	/**
	 * Get the color and alpha values of a pixel
	 *
	 * @param pixel Pixel value in surface's native format to get the RGBA values of
	 *
	 * Converts the specified pixel value to a Color object with RGBA values.
	 * If the native pixel format is less than 32 bits, the upper bits of the parameter
	 * needs to be cleared.
	 * If the surface doesn't have an alpha channel, the alpha value of the Color structure is
	 * set to 255 (opaque).
	 *
	 * @return Color structure with RGBA values for the specified pixel value.
	 *
	 **/
	HiColor Surface::pixelToColor( uint32_t pixel ) const
	{
		if (m_pixelDescription.bIndexed)
			return m_pClut[pixel];

		if( m_pixelDescription.bBigEndian != isSystemBigEndian() )
		{

			if( m_pixelDescription.bits == 16 )
				pixel = endianSwap( (uint16_t) pixel );
			else if( m_pixelDescription.bits == 16 )
				pixel = endianSwap( pixel );
		}

		Color8 col;

        const uint8_t *    pConvTab_R = s_pixelConvTabs[m_pixelDescription.R_bits];
        const uint8_t *    pConvTab_G = s_pixelConvTabs[m_pixelDescription.G_bits];
        const uint8_t *    pConvTab_B = s_pixelConvTabs[m_pixelDescription.B_bits];
        const uint8_t *    pConvTab_A = s_pixelConvTabs[m_pixelDescription.A_bits];
        
        col.r = pConvTab_R[(pixel & m_pixelDescription.R_mask) >> m_pixelDescription.R_shift];
        col.g = pConvTab_G[(pixel & m_pixelDescription.G_mask) >> m_pixelDescription.G_shift];
        col.b = pConvTab_B[(pixel & m_pixelDescription.B_mask) >> m_pixelDescription.B_shift];
        col.a = pConvTab_A[(pixel & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift];

		int16_t* pUnpackTab = m_pixelDescription.bLinear ? HiColor::unpackLinearTab : HiColor::unpackSRGBTab;
		return HiColor(pUnpackTab[col.r], pUnpackTab[col.g], pUnpackTab[col.b], HiColor::unpackLinearTab[col.a]);
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


		uint32_t pixel = colorToPixel( color );
		int w = pixbuf.rect.w;
		int h = pixbuf.rect.h;
		int p = pixbuf.pitch;
		uint8_t * pDest = pixbuf.pPixels;

		bool ret = true;
		switch( m_pixelDescription.bits )
		{
			case 8:
				for( int y = 0 ; y < h ; y++ )
				{
					for( int x = 0 ; x < w ; x++ )
						pDest[x] = (uint8_t) pixel;
					pDest += p;
				}
				break;
			case 16:
				for( int y = 0 ; y < h ; y++ )
				{
					for( int x = 0 ; x < w ; x++ )
						((uint16_t*)pDest)[x] = (uint16_t) pixel;
					pDest += p;
				}
				break;
			case 24:
			{
				uint8_t one = (uint8_t) pixel;
				uint8_t two = (uint8_t) (pixel>>8);
				uint8_t three = (uint8_t) (pixel>>16);

				for( int y = 0 ; y < h ; y++ )
				{
					for( int x = 0 ; x < w*3 ;  )
					{
						pDest[x++] = one;
						pDest[x++] = two;
						pDest[x++] = three;
					}
					pDest += p;
				}
				break;
			}
			case 32:
				for( int y = 0 ; y < h ; y++ )
				{
					for( int x = 0 ; x < w ; x++ )
						((uint32_t*)pDest)[x] = pixel;
					pDest += p;
				}
				break;
			default:
				ret = false;
		}

		//

		pullPixels(pixbuf);
		freePixelBuffer(pixbuf);

		return ret;
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
		bp.clut = m_pClut;
		bp.dynamic = m_bDynamic;
		bp.format = m_pixelDescription.format;
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
		if( !pSrcSurface || pSrcSurface->pixelFormat() == PixelFormat::Undefined || m_pixelDescription.format == PixelFormat::Undefined )
			return false;


		auto srcbuf = pSrcSurface->allocPixelBuffer(_srcRect);
		bool bPushed = pSrcSurface->pushPixels(srcbuf);
		if (!bPushed)
		{
			//TODO: Error handling.
		}


		bool retVal = _copy({ _dest, _srcRect.size() }, pSrcSurface->pixelDescription(), srcbuf.pPixels, srcbuf.pitch, { 0,0,_srcRect.size() }, srcbuf.pClut);

		pSrcSurface->freePixelBuffer(srcbuf);


		return retVal;
	}


	//____ _copy() _________________________________________________________

	/*
		Copying to I8 is only allowed from other I8 content. No color conversion is then performed, CLUTs are assumed to be identical.
		Copying from A8 to any other surface copies white pixels with alpha (if destination has alpha channel, otherwise just white pixels).
	*/


	bool Surface::_copy( const RectI& destRect, const PixelDescription * pSrcFormat, uint8_t * pSrcPixels, int srcPitch, const RectI& srcRect, const Color8 * pCLUT )
	{
		//TODO: Support endian-swap.


		if( srcRect.w <= 0 || destRect.w <= 0 )
			return false;

		auto pixbuf = allocPixelBuffer(destRect);


		const PixelDescription * pDstFormat 	= &m_pixelDescription;
		int		dstPitch = pixbuf.pitch;

		uint8_t *	pSrc = pSrcPixels;
		uint8_t *	pDst = pixbuf.pPixels;

		pSrc += srcRect.y * srcPitch + srcRect.x * pSrcFormat->bits/8;
//		pDst += dstRect.y * dstPitch + dstRect.x * pDstFormat->bits/8;


		if( pSrcFormat->bits == pDstFormat->bits && pSrcFormat->R_mask == pDstFormat->R_mask &&
			pSrcFormat->G_mask == pDstFormat->G_mask && pSrcFormat->B_mask == pDstFormat->B_mask &&
			pSrcFormat->A_mask == pDstFormat->A_mask)
		{
			// We have identical formats so we can do a fast straight copy

			int lineLength = srcRect.w * pSrcFormat->bits/8;
			for( int y = 0 ; y < srcRect.h ; y++ )
			{
				memcpy( pDst, pSrc, lineLength );
				pSrc += srcPitch;
				pDst += dstPitch;
			}
		}
		else if (pSrcFormat->format == PixelFormat::BGRA_8 && pDstFormat->format == PixelFormat::BGR_8)
		{
			// Standard 8-bit RGB values, we just need to lose the alpha channel

			int		srcLineInc = srcPitch - 4 * srcRect.w;
			int		dstLineInc = dstPitch - 3 * srcRect.w;

			for (int y = 0; y < srcRect.h; y++)
			{
				for (int x = 0; x < srcRect.w; x++)
				{
					* pDst++ = * pSrc++;
					* pDst++ = * pSrc++;
					*pDst++ = *pSrc++;
					pSrc++;
				}
				pSrc += srcLineInc;
				pDst += dstLineInc;
			}
		}
		else if(pSrcFormat->format == PixelFormat::BGR_8 && pDstFormat->format == PixelFormat::BGRA_8)
		{
			// Standard 8-bit RGB values, We just needs to add an alpha channel

			int		srcLineInc = srcPitch - 3 * srcRect.w;
			int		dstLineInc = dstPitch - 4 * srcRect.w;

			for (int y = 0; y < srcRect.h; y++)
			{
				for (int x = 0; x < srcRect.w; x++)
				{
					*pDst++ = *pSrc++;
					*pDst++ = *pSrc++;
					*pDst++ = *pSrc++;
					*pDst++ = 255;
				}
				pSrc += srcLineInc;
				pDst += dstLineInc;
			}

		}
		else if (pDstFormat->format == PixelFormat::CLUT_8_sRGB || pDstFormat->format == PixelFormat::CLUT_8_linear)
		{
			freePixelBuffer(pixbuf);
			return false;								// Can't copy to CLUT-based surface unless source is of identical format!
		}
		else if (pSrcFormat->bIndexed)
		{
			// Convert pixels from CLUT-based to normal when copying

			if (pSrcFormat->bits != 8)
			{
				freePixelBuffer(pixbuf);
				return false;							// Only 8-bit CLUTS are supported for the momment.
			}

			int		srcInc = pSrcFormat->bits / 8;
			int		dstInc = pDstFormat->bits / 8;

			int		srcLineInc = srcPitch - srcInc * srcRect.w;
			int		dstLineInc = dstPitch - dstInc * srcRect.w;

			switch (pDstFormat->bits)
			{
				case 0x08:										// This can only be A8, so we simplify and optimize a bit....
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							Color8 srcpixel = pCLUT[*pSrc++];
							*pDst++ = srcpixel.a;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
					break;

				case 0x10:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							Color8 srcpixel = pCLUT[*pSrc++];

							unsigned int dstpixel = ((((uint32_t)srcpixel.r >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)srcpixel.g >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)srcpixel.b >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)srcpixel.a >> pDstFormat->A_loss) << pDstFormat->A_shift));
							*((unsigned short*)pDst) = (unsigned short)dstpixel; pDst += 2;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
					break;

				case 0x18:										// This can only be BGR_8, so we simplify and optimize a bit....
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							Color8 srcpixel = pCLUT[*pSrc++];
							pDst[0] = srcpixel.b;
							pDst[1] = srcpixel.g;
							pDst[2] = srcpixel.r;
							pDst += 3;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
					break;

				case 0x20:										// This can only be BGRA_8, so we simplify and optimize a bit....
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							Color8 srcpixel = pCLUT[*pSrc++];
							pDst[0] = srcpixel.b;
							pDst[1] = srcpixel.g;
							pDst[2] = srcpixel.r;
							pDst[3] = srcpixel.a;
							pDst += 4;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
					break;
				default:
					freePixelBuffer(pixbuf);
					return false;								// Unsupported destination format!

			}
		}
		else
		{
			// We need to fully convert pixels when copying

			int		srcInc = pSrcFormat->bits/8;
			int		dstInc = pDstFormat->bits/8;

			int		srcLineInc = srcPitch - srcInc * srcRect.w;
			int		dstLineInc = dstPitch - dstInc * srcRect.w;

			const uint8_t *	pConvTab_R = s_pixelConvTabs[pSrcFormat->R_bits];
			const uint8_t *	pConvTab_G = s_pixelConvTabs[pSrcFormat->G_bits];
			const uint8_t *	pConvTab_B = s_pixelConvTabs[pSrcFormat->B_bits];
			const uint8_t *	pConvTab_A = s_pixelConvTabs[pSrcFormat->A_bits];


			switch( (pSrcFormat->bits << 8) + pDstFormat->bits )
			{
				case 0x0808:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							unsigned int srcpixel = *((unsigned char*)pSrc); pSrc += 1;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							*((unsigned char*)pDst) = (unsigned char)dstpixel; pDst += 1;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x1008:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							unsigned int srcpixel = *((unsigned short*)pSrc); pSrc += 2;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							*((unsigned char*)pDst) = (unsigned char)dstpixel; pDst += 1;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x1808:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							unsigned int srcpixel = pSrc[0] + (((unsigned int)pSrc[1]) << 8) + (((unsigned int)pSrc[2]) << 16); pSrc += 3;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							*((unsigned char*)pDst) = (unsigned char)dstpixel; pDst += 1;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x2008:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							unsigned int srcpixel = *((unsigned int*)pSrc); pSrc += 4;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							*((unsigned char*)pDst) = (unsigned char)dstpixel; pDst += 1;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x0810:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							unsigned int srcpixel = *((unsigned char*)pSrc); pSrc += 1;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							*((unsigned short*)pDst) = (unsigned char)dstpixel; pDst += 2;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x0818:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							unsigned int srcpixel = *((unsigned char*)pSrc); pSrc += 1;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							pDst[0] = (unsigned char)dstpixel;
							pDst[1] = (unsigned char)(dstpixel >> 8);
							pDst[2] = (unsigned char)(dstpixel >> 16);
							pDst += 3;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
					break;

				case 0x0820:
					for (int y = 0; y < srcRect.h; y++)
					{
						for (int x = 0; x < srcRect.w; x++)
						{
							unsigned int srcpixel = *((unsigned char*)pSrc); pSrc += 1;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
								(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
								(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
								(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							*((unsigned int*)pDst) = (unsigned int)dstpixel; pDst += 4;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
					break;

				case 0x1010:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = * ((unsigned short*)pSrc); pSrc+=2;
							unsigned int dstpixel = ( (((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													  (((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													  (((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													  (((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							* ((unsigned short*)pDst) = (unsigned short) dstpixel; pDst+=2;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x1810:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = pSrc[0] + (((unsigned int)pSrc[1])<<8) + (((unsigned int)pSrc[2])<<16); pSrc+=3;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							* ((unsigned short*)pDst) = (unsigned short) dstpixel; pDst+=2;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x2010:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = * ((unsigned int*)pSrc); pSrc+=4;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							* ((unsigned short*)pDst) = (unsigned short) dstpixel; pDst+=2;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x1018:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = * ((unsigned short*)pSrc); pSrc+=2;
							unsigned int dstpixel = ( (((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													  (((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													  (((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													  (((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							pDst[0] = (unsigned char) dstpixel;
							pDst[1] = (unsigned char) (dstpixel >> 8);
							pDst[2] = (unsigned char) (dstpixel >> 16);
							pDst+=3;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
						pDst+=3;
					}
				break;
				case 0x1818:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = pSrc[0] + (((unsigned int)pSrc[1]) << 8) + (((unsigned int)pSrc[2]) << 16); pSrc+=3;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							pDst[0] = (unsigned char) dstpixel;
							pDst[1] = (unsigned char) (dstpixel >> 8);
							pDst[2] = (unsigned char) (dstpixel >> 16);
							pDst+=3;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x2018:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = * ((unsigned int*)pSrc); pSrc+=4;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							pDst[0] = (unsigned char) dstpixel;
							pDst[1] = (unsigned char) (dstpixel >> 8);
							pDst[2] = (unsigned char) (dstpixel >> 16);
							pDst+=3;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x1020:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = * ((unsigned short*)pSrc); pSrc+=2;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							* ((unsigned int*)pDst) = dstpixel; pDst+=4;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x1820:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = pSrc[0] + (((unsigned int)pSrc[1]) << 8) + (((unsigned int)pSrc[2]) << 16); pSrc+=3;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							* ((unsigned int*)pDst) = dstpixel; pDst+=4;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;
				case 0x2020:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = * ((unsigned int*)pSrc); pSrc+=4;
							unsigned int dstpixel = ((((uint32_t)pConvTab_R[(srcpixel & pSrcFormat->R_mask) >> pSrcFormat->R_shift] >> pDstFormat->R_loss) << pDstFormat->R_shift) |
													(((uint32_t)pConvTab_G[(srcpixel & pSrcFormat->G_mask) >> pSrcFormat->G_shift] >> pDstFormat->G_loss) << pDstFormat->G_shift) |
													(((uint32_t)pConvTab_B[(srcpixel & pSrcFormat->B_mask) >> pSrcFormat->B_shift] >> pDstFormat->B_loss) << pDstFormat->B_shift) |
													(((uint32_t)pConvTab_A[(srcpixel & pSrcFormat->A_mask) >> pSrcFormat->A_shift] >> pDstFormat->A_loss) << pDstFormat->A_shift));
							* ((unsigned int*)pDst) = dstpixel; pDst+=4;
						}
						pSrc += srcLineInc;
						pDst += dstLineInc;
					}
				break;

				default:
					freePixelBuffer(pixbuf);
					return false;			// Failed to copy
			}
		}

		pullPixels(pixbuf);
		freePixelBuffer(pixbuf);
		return true;
	}

	//____ convert() _________________________________________________________

	Surface_p Surface::convert(PixelFormat format, SurfaceFactory* pFactory)
	{
		if (!pFactory)
			pFactory = GfxBase::defaultSurfaceFactory();

		if (!pFactory)
		{
			//TODO: Error handling!
			return Surface_p();
		}

		if ( (format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_linear || format == PixelFormat::CLUT_8_sRGB) &&
			( m_pixelDescription.format != PixelFormat::CLUT_8_sRGB && m_pixelDescription.format != PixelFormat::CLUT_8_linear ))
		{
			// We only allow conversion to indexed if surface has 256 colors at most!

			uint32_t* pCLUT = (uint32_t*) GfxBase::memStackAlloc(1024);
			int nColors = 0;

			auto pBlob = Blob::create(m_size.w * m_size.h);
			uint8_t* pPixels = (uint8_t*) pBlob->data();

			auto pixbuf = allocPixelBuffer();
			pushPixels(pixbuf);


			switch (m_pixelDescription.format)
			{
				case PixelFormat::BGRA_8_sRGB:
				case PixelFormat::BGRA_8_linear:
				{
					// Generate CLUT (kind of) and indexed pixels
						
					uint8_t* pDest = pPixels;
					for (int y = 0; y < m_size.h; y++)
					{
						uint32_t* pLine = (uint32_t*) (pixbuf.pPixels + pixbuf.pitch * y);

						for (int x = 0; x < m_size.w; x++)
						{
							uint32_t pixel = pLine[x];
							int ofs = 0;
							while( ofs < nColors && pCLUT[ofs] != pixel )
								ofs++;
							if( ofs == nColors )
							{
								if( nColors == 256 )
								{
									nColors++;
									goto end;
								}
								
								pCLUT[ofs] = pixel;
								nColors++;
							}
							* pDest++ = (uint8_t) ofs;
						}
					}

					// Convert CLUT from pixels to colors
					
					Color8 * pColor = (Color8*) pCLUT;
					for( int i = 0 ; i < nColors ; i++ )
					{
						uint8_t * pBGRA = (uint8_t*) &pCLUT[i];
						Color8 col( pBGRA[2], pBGRA[1], pBGRA[0], pBGRA[3] );
						pColor[i] = col;
					}
					break;
				}
					
				default:
					GfxBase::handleError(ErrorSeverity::Serious, ErrorCode::FailedPrerequisite, "Conversion of this kind of surface to CLUT_8 has not been implemented (sorry!)", this, &TYPEINFO, __func__, __FILE__, __LINE__);
					
					freePixelBuffer(pixbuf);
					GfxBase::memStackRelease(1024);
					return nullptr;
			}
end:
			
			if( nColors > 256 )
			{
				GfxBase::handleError(ErrorSeverity::Serious, ErrorCode::FailedPrerequisite, "Can't convert surface with more than 256 colors to CLUT_8", this, &TYPEINFO, __func__, __FILE__, __LINE__);
				
				freePixelBuffer(pixbuf);
				GfxBase::memStackRelease(1024);
				return nullptr;
			}
					
			// Fill out clut with zeroes
				
			for( int i = nColors ; i < 256 ; i++ )
				pCLUT[i] = 0;

			// Create surface

			auto bp = blueprint();
			bp.format = format;
			bp.clut = (Color8*) pCLUT;
			auto pSurface = pFactory->createSurface(bp, pBlob);

			// Cleanup and return
			
			freePixelBuffer(pixbuf);
			GfxBase::memStackRelease(1024);
			return pSurface;
		}
		else
		{
			auto bp = blueprint();
			bp.format = format;

			auto pSurface = pFactory->createSurface(bp);
			pSurface->copy({ 0,0 }, this);
			return pSurface;
		}
	}


	//____ _alpha() ___________________________________________________________

	int Surface::_alpha(CoordSPX _coord, const PixelBuffer& buffer)
	{
		//TODO: Take endianess into account.
		//TODO: Take advantage of subpixel precision and interpolate alpha value if surface set to interpolate.

		CoordI coord(((_coord.x + 32) / 64) % m_size.w, ((_coord.y + 32) / 64) % m_size.h);

		switch (m_pixelDescription.format)
		{
		case PixelFormat::CLUT_8_sRGB:
		case PixelFormat::CLUT_8_linear:
		{
			uint8_t index = buffer.pPixels[buffer.pitch * coord.y + coord.x];
			return HiColor::unpackLinearTab[buffer.pClut[index].a];
		}
		case PixelFormat::A_8:
		{
			uint8_t* pPixel = buffer.pPixels + buffer.pitch * coord.y + coord.x;
			return HiColor::unpackLinearTab[pPixel[0]];
		}
		case PixelFormat::BGRA_4_linear:
		{
			uint16_t pixel = *(uint16_t*)(buffer.pPixels + buffer.pitch * coord.y + coord.x * 2);
			const uint8_t* pConvTab = s_pixelConvTabs[4];

			return HiColor::unpackLinearTab[((pConvTab[(pixel & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift] >> m_pixelDescription.A_loss) << m_pixelDescription.A_shift)];
		}
		case PixelFormat::BGRA_8_sRGB:
		case PixelFormat::BGRA_8_linear:
		{
			uint8_t* pPixel = buffer.pPixels + buffer.pitch * coord.y + coord.x * 4;
			return HiColor::unpackLinearTab[pPixel[3]];
		}
		default:
			return 4096;
		}
	}

    //____ _isBlueprintValid() ________________________________________________

    bool Surface::_isBlueprintValid(const Blueprint& bp, SizeI maxSize, Surface * pOther)
    {
        SizeI size = pOther ? pOther->pixelSize() :bp.size;

        PixelFormat format = bp.format;
        if( format == PixelFormat::Undefined )
            format = pOther ? pOther->pixelFormat() : PixelFormat::BGRA_8;

        if (size.w > maxSize.w || size.h > maxSize.h)
            return false;

        bool bIsIndexed = (format == PixelFormat::CLUT_8 || format == PixelFormat::CLUT_8_sRGB || format == PixelFormat::CLUT_8_linear);

        if (format < PixelFormat_min || format > PixelFormat_max || (bIsIndexed && bp.clut == nullptr))
            return false;

        if (bp.canvas && bIsIndexed)
            return false;

        if( bIsIndexed )
        {
            if( bp.mipmap )
                return false;            // Indexed can't be mipmapped.

            if ( pOther && !pOther->pixelDescription()->bIndexed)
                return false;            // Can't create indexed from non-indexed source.
            
            if( bp.clut == nullptr && ( !pOther || pOther->clut() == nullptr ) )
                return false;           // Indexed but clut is missing.
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
