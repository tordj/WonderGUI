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
#include <wg_payload.h>

namespace wg
{

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

	Surface::Surface( int flags )
	{
		memset( &m_pixelDescription, 0, sizeof(PixelDescription) );
        
        if( flags & SurfaceFlag::Scale2X )
            m_pixelQuartersPerPoint = 8;            // TODO: Add error handling if size not divisable.
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

	//____ setScaleMode() __________________________________________________________

	void Surface::setScaleMode( ScaleMode mode )
	{
		m_scaleMode = mode;
	}

	//____ width() ________________________________________________________________
	/**
	 * Get the width of the surface.
	 *
	 * @return The width of the surface, measured in pixels.
	 **/
	int Surface::width() const
	{
		return size().w;
	}

	//____ height() _______________________________________________________________
	/**
	 * Get the height of the surface.
	 *
	 * @return The height of the surface, measured in pixels.
	 **/
	int Surface::height() const
	{
		return size().h;
	}

    //____ setScale() _______________________________________________________________

    bool Surface::setScale( float scale )
    {
        //TODO: Error check, only allow certain factors.
        
        m_pixelQuartersPerPoint = (int)(scale*4);
        return true;
    }

    
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
	uint32_t Surface::colorToPixel( const Color& col ) const
	{
		uint32_t pix;

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
			pix = ((col.r >> m_pixelDescription.R_loss) << m_pixelDescription.R_shift) |
				((col.g >> m_pixelDescription.G_loss) << m_pixelDescription.G_shift) |
				((col.b >> m_pixelDescription.B_loss) << m_pixelDescription.B_shift) |
				((col.a >> m_pixelDescription.A_loss) << m_pixelDescription.A_shift);
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
	Color Surface::pixelToColor( uint32_t pixel ) const
	{
		if (m_pixelDescription.bIndexed)
			return m_pClut[pixel];

		Color col;

		col.r = ((pixel & m_pixelDescription.R_mask) >> m_pixelDescription.R_shift) << m_pixelDescription.R_loss;
		col.g = ((pixel & m_pixelDescription.G_mask) >> m_pixelDescription.G_shift) << m_pixelDescription.G_loss;
		col.b = ((pixel & m_pixelDescription.B_mask) >> m_pixelDescription.B_shift) << m_pixelDescription.B_loss;

		if(m_pixelDescription.A_mask == 0)
			col.a = 255;
		else
			col.a = ((pixel & m_pixelDescription.A_mask) >> m_pixelDescription.A_shift) << m_pixelDescription.A_loss;

		return col;
	}

	//____ _lockAndAdjustRegion() __________________________________________________

	RectI Surface::_lockAndAdjustRegion( AccessMode modeNeeded, const RectI& region )
	{
		if( m_accessMode == AccessMode::None )
		{
			lock( modeNeeded );
			return region;
		}
		else if( m_accessMode != AccessMode::ReadWrite && m_accessMode != modeNeeded )
			return RectI(0,0,0,0);

		if( !m_lockRegion.contains( region ) )
			return RectI(0,0,0,0);

		return region - m_lockRegion.pos();
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

	bool Surface::fill( Color col )
	{
		return fill( col, RectI(0,0,size()) );
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
	bool Surface::fill( Color col, const RectI& region )
	{

		AccessMode oldMode = m_accessMode;
		RectI rect = _lockAndAdjustRegion(AccessMode::WriteOnly,region);

		if( rect.w == 0 )
			return false;

		//


		uint32_t pixel = colorToPixel( col );
		int w = rect.w;
		int h = rect.h;
		int p = pitch();
		uint8_t * pDest = m_pPixels + rect.y * p + rect.x*m_pixelDescription.bits / 8;

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

		if( oldMode == AccessMode::None )
			unlock();

		return ret;
	}

	//_____ copyFrom() _____________________________________________________________
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
	bool Surface::copyFrom( Surface * pSrcSurface, CoordI dst )
	{
		if( !pSrcSurface )
			return false;

		return copyFrom( pSrcSurface, RectI(0,0,pSrcSurface->size()), dst );
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
	bool Surface::copyFrom( Surface * pSrcSurface, const RectI& _srcRect, CoordI _dst )
	{
		if( !pSrcSurface || pSrcSurface->m_pixelDescription.format == PixelFormat::Unknown || m_pixelDescription.format == PixelFormat::Unknown )
			return false;

		// Save old locks and lock the way we want.

		AccessMode 	dstOldMode 		= m_accessMode;
		AccessMode 	srcOldMode 		= pSrcSurface->lockStatus();

		RectI srcRect = pSrcSurface->_lockAndAdjustRegion( AccessMode::ReadOnly, _srcRect );
		RectI dstRect = _lockAndAdjustRegion( AccessMode::WriteOnly, RectI(_dst.x,_dst.y,srcRect.w,srcRect.h) );

		// Do the copying

		bool retVal = _copyFrom( pSrcSurface->pixelDescription(), (uint8_t*) pSrcSurface->pixels(), pSrcSurface->pitch(), srcRect, dstRect, pSrcSurface->m_pClut );

		// Release any temporary locks

		if( dstOldMode == AccessMode::None )
			unlock();

		if( srcOldMode == AccessMode::None )
			pSrcSurface->unlock();

		return retVal;
	}

	//____ setPayload() _______________________________________________________

	void Surface::setPayload(BasicPayload * pPayload)
	{
		m_pPayload = pPayload;
	}

	//_____ payload() _________________________________________________________

	BasicPayload_p Surface::payload() const
	{
		return m_pPayload;
	}



	//____ _copyFrom() _________________________________________________________

	/*
		Copying to I8 is only allowed from other I8 content. No color conversion is then performed, CLUTs are assumed to be identical.
		Copying from A8 to any other surface copies white pixels with alpha (if destination has alpha channel, otherwise just white pixels).
	*/


	bool Surface::_copyFrom( const PixelDescription * pSrcFormat, uint8_t * pSrcPixels, int srcPitch, const RectI& srcRect, const RectI& dstRect, const Color * pCLUT )
	{

		if( srcRect.w <= 0 || dstRect.w <= 0 )
			return false;

		const PixelDescription * pDstFormat 	= &m_pixelDescription;
		int		dstPitch 				= m_pitch;

		uint8_t *	pSrc = pSrcPixels;
		uint8_t *	pDst = m_pPixels;

		pSrc += srcRect.y * srcPitch + srcRect.x * pSrcFormat->bits/8;
		pDst += dstRect.y * dstPitch + dstRect.x * pDstFormat->bits/8;


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
		else if (pDstFormat->format == PixelFormat::I8)
		{
			return false;								// Can't copy to CLUT-based surface unless source is of identical format!
		}
		else if (pSrcFormat->bIndexed)
		{
			// Convert pixels from CLUT-based to normal when copying

			if (pSrcFormat->bits != 8)
				return false;							// Only 8-bit CLUTS are supported for the momment.

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
							Color srcpixel = pCLUT[*pSrc++];
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
							Color srcpixel = pCLUT[*pSrc++];

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
							Color srcpixel = pCLUT[*pSrc++];
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
							Color srcpixel = pCLUT[*pSrc++];
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
					return false;			// Failed to copy
			}
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
	 * @fn virtual uint8_t * Surface::lock(AccessMode mode)
	 *
	 * Locks the surface to gain direct access to the pixel data.
	 *
	 * @param mode	AccessMode for the locked surface, can be AccessMode::ReadOnly, AccessMode::WriteOnly
	 *				or AccessMode::ReadWrite.
	 *
	 * Locking a surface gives the user direct access to the raw pixel data of the surface.
	 * The mode of access can be limited to read-only or write-only, which might yield
	 * performance gains on some platforms as compared to read and write access.
	 *
	 * The first pixel of the first line of raw pixel data starts at the address returned
	 * by this method. The rest of the pixels for the same line follows. There might be some
	 * extra bytes between the end of one line and the beginning of the next which should
	 * neither be read or written. Use the value returned by pitch() to properly calculate
	 * the start adress of each line.
	 *
	 * Once you are done with a surface you should release it by calling unlock() after
	 * which no access of the raw pixel data is allowed until the surface has been locked again.
	 * Be aware that the address and pitch of the raw pixel data might change between calls to
	 * lock().
	 *
	 * You should not use the surface as source or destination in any GfxDevice or
	 * Surface::copyTo() calls while it
	 * is locked as that might result in undefined behavior on certain Surface implementations
	 * and hardware architectures.
	 *
	 * @return Pointer to start of pixel data or null if failed.
	 */

	/**
	 * @fn virtual uint8_t * Surface::lockRegion(AccessMode mode, const RectI& region)
	 *
	 * Locks a limited region of the surface to gain direct access to its pixel data.
	 *
	 * @param mode	AccessMode for the locked surface, can be AccessMode::ReadOnly, AccessMode::WriteOnly
	 *				or AccessMode::ReadWrite.
	 * @param region	The rectangular region of the surface to lock.
	 *
	 * Identical to lock() except that just a specific region of the surface may be accessed
	 * and that the pointer returned is to the first pixel of that region, not the entire surface.
	 *
	 * Use pitch() to get the distance from the start of one pixel row of the region to the next.
	 * Use unlock() to release the surface when you are done with it.
	 *
	 * Just locking a specific region of a surface as opposed to the whole of it might yield
	 * performance gains depending on Surface implementation and hardware architecture in use.
	 *
	 * @return Pointer to start of pixel data for specified region or null if failed.
	 */

	/**
	 * @fn virtual void Surface::unlock()
	 *
	 * Unlocks a surface that has previously been locked by a call to lock() or lockRegion().
	 */

} // namespace wg
