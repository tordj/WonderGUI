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

#include <memory.h>
#include <wg_surface.h>

namespace wg 
{
	
	const char Surface::CLASSNAME[] = {"Surface"};
	
	
	//____ Surface() ____________________________________________________________
	
	Surface::Surface()
	{
		m_accessMode	= AccessMode::None;
		m_pPixels		= 0;
		m_scaleMode		= ScaleMode::Nearest;
		
		memset( &m_pixelFormat, 0, sizeof(PixelFormat) );
	}
	
	//____ ~Surface() ____________________________________________________________
	
	Surface::~Surface()
	{
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool Surface::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * Surface::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	Surface_p Surface::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return Surface_p( static_cast<Surface*>(pObject) );
	
		return 0;
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
	 * @return Pixel value in surface's native format that closest resembles specified color.
	 *
	 **/
	uint32_t Surface::colorToPixel( const Color& col ) const
	{
		uint32_t pix = ((col.r << m_pixelFormat.R_shift) & m_pixelFormat.R_mask) |
					 ((col.g << m_pixelFormat.G_shift) & m_pixelFormat.G_mask) |
					 ((col.b << m_pixelFormat.B_shift) & m_pixelFormat.B_mask) |
					 ((col.a << m_pixelFormat.A_shift) & m_pixelFormat.A_mask);
	
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
		Color col( (pixel & m_pixelFormat.R_mask) >> m_pixelFormat.R_shift,
					 (pixel & m_pixelFormat.G_mask) >> m_pixelFormat.G_shift,
					 (pixel & m_pixelFormat.B_mask) >> m_pixelFormat.B_shift,
					 (pixel & m_pixelFormat.A_mask) >> m_pixelFormat.A_shift );
	
		return col;
	}
	
	//____ _lockAndAdjustRegion() __________________________________________________
	
	Rect Surface::_lockAndAdjustRegion( AccessMode modeNeeded, const Rect& region )
	{
		if( m_accessMode == AccessMode::None )
		{
			lock( modeNeeded );
			return region;
		}
		else if( m_accessMode != AccessMode::ReadWrite && m_accessMode != modeNeeded )
			return Rect(0,0,0,0);
	
		if( !m_lockRegion.contains( region ) )
			return Rect(0,0,0,0);
	
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
		return fill( col, Rect(0,0,size()) );
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
	bool Surface::fill( Color col, const Rect& region )
	{
	
		AccessMode oldMode = m_accessMode;
		Rect rect = _lockAndAdjustRegion(AccessMode::WriteOnly,region);
	
		if( rect.w == 0 )
			return false;
	
		//
	
	
		uint32_t pixel = colorToPixel( col );
		int w = width();
		int h = height();
		int p = pitch();
		uint8_t * pDest = m_pPixels;
	
		bool ret = true;
		switch( m_pixelFormat.bits )
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
					for( int x = 0 ; x < w ; x++ )
					{
						pDest[x++] = one;
						pDest[x++] = two;
						pDest[x++] = three;
					}
					pDest += p - w*3;
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
	bool Surface::copyFrom( Surface * pSrcSurface, Coord dst )
	{
		if( !pSrcSurface )
			return false;
	
		return copyFrom( pSrcSurface, Rect(0,0,pSrcSurface->size()), dst );
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
	bool Surface::copyFrom( Surface * pSrcSurface, const Rect& _srcRect, Coord _dst )
	{
		if( !pSrcSurface || pSrcSurface->m_pixelFormat.type == PixelType::Unknown || m_pixelFormat.type == PixelType::Unknown )
			return false;
	
		// Save old locks and lock the way we want.
	
		AccessMode 	dstOldMode 		= m_accessMode;
		AccessMode 	srcOldMode 		= pSrcSurface->lockStatus();
	
		Rect srcRect = pSrcSurface->_lockAndAdjustRegion( AccessMode::ReadOnly, _srcRect );
		Rect dstRect = _lockAndAdjustRegion( AccessMode::WriteOnly, Rect(_dst.x,_dst.y,srcRect.w,srcRect.h) );
	
		// Do the copying

		bool retVal = _copyFrom( pSrcSurface->pixelFormat(), (uint8_t*) pSrcSurface->pixels(), pSrcSurface->pitch(), srcRect, dstRect );
	
		// Release any temporary locks
	
		if( dstOldMode == AccessMode::None )
			unlock();
	
		if( srcOldMode == AccessMode::None )
			pSrcSurface->unlock();
	
		return retVal;
	}


	//____ _copyFrom() _________________________________________________________

	bool Surface::_copyFrom( const PixelFormat * pSrcFormat, uint8_t * pSrcPixels, int srcPitch, const Rect& srcRect, const Rect& dstRect )
	{
			
		if( srcRect.w <= 0 || dstRect.w <= 0 )
			return false;

		const PixelFormat * pDstFormat 	= &m_pixelFormat;
		int		dstPitch 				= m_pitch;

		unsigned char *	pSrc = (unsigned char*) pSrcPixels;
		unsigned char *	pDst = (unsigned char*) m_pPixels;

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
		else if( (pSrcFormat->type == PixelType::BGRA_8 || pSrcFormat->type == PixelType::BGR_8) &&
				 (pDstFormat->type == PixelType::BGRA_8 || pDstFormat->type == PixelType::BGR_8) )
		{
			// We are just switching between RGBA_8 and RGB_8, just copy RGB components and skip alpha

			int		srcInc = pSrcFormat->bits/8;
			int		dstInc = pDstFormat->bits/8;

			int		srcLineInc = srcPitch - srcInc * srcRect.w;
			int		dstLineInc = dstPitch - dstInc * srcRect.w;

			for( int y = 0 ; y < srcRect.h ; y++ )
			{
				for( int x = 0 ; x < srcRect.w ; x++ )
				{
					pDst[0] = pSrc[0];
					pDst[1] = pSrc[1];
					pDst[2] = pSrc[2];
					pSrc += srcInc;
					pDst += dstInc;
				}
				pSrc += srcLineInc;
				pDst += dstLineInc;
			}

		}
		else
		{
			// We need to fully convert pixels when copying

			int		srcInc = pSrcFormat->bits/8;
			int		dstInc = pDstFormat->bits/8;

			int		srcLineInc = srcPitch - srcInc * srcRect.w;
			int		dstLineInc = dstPitch - dstInc * srcRect.w;

			unsigned int	R_mask = (((0xFFFFFFFF & pSrcFormat->R_mask) >> pSrcFormat->R_shift) << pDstFormat->R_shift) & pDstFormat->R_mask;
			unsigned int	G_mask = (((0xFFFFFFFF & pSrcFormat->G_mask) >> pSrcFormat->G_shift) << pDstFormat->G_shift) & pDstFormat->G_mask;
			unsigned int	B_mask = (((0xFFFFFFFF & pSrcFormat->B_mask) >> pSrcFormat->B_shift) << pDstFormat->B_shift) & pDstFormat->B_mask;
			unsigned int	A_mask = (((0xFFFFFFFF & pSrcFormat->A_mask) >> pSrcFormat->A_shift) << pDstFormat->A_shift) & pDstFormat->A_mask;


			switch( (pSrcFormat->bits << 8) + pDstFormat->bits )
			{
				case 0x1010:
					for( int y = 0 ; y < srcRect.h ; y++ )
					{
						for( int x = 0 ; x < srcRect.w ; x++ )
						{
							unsigned int srcpixel = * ((unsigned short*)pSrc); pSrc+=2;
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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
							unsigned int dstpixel = (((srcpixel >> pSrcFormat->R_shift) << pDstFormat->R_shift) & R_mask) |
													(((srcpixel >> pSrcFormat->G_shift) << pDstFormat->G_shift) & G_mask) |
													(((srcpixel >> pSrcFormat->B_shift) << pDstFormat->B_shift) & B_mask) |
													(((srcpixel >> pSrcFormat->A_shift) << pDstFormat->A_shift) & A_mask);
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



} // namespace wg
