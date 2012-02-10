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

#include <new>

#include <memory.h>

#include <wg_surface.h>
#include <wg_geo.h>

#include <wg_blockset.h>
#include <wg_mempool.h>

WgMemPool * WgSurface::g_pBlockSetMemPool = 0;

//____ WgSurface() ____________________________________________________________

WgSurface::WgSurface()
{
	m_accessMode	= WG_NO_ACCESS;
	m_pPixels		= 0;

	if( !g_pBlockSetMemPool )
		g_pBlockSetMemPool = new WgMemPool( 16, sizeof(WgBlockSet) );


	memset( &m_pixelFormat, 0, sizeof(WgPixelFormat) );
}

//____ ~WgSurface() ____________________________________________________________

WgSurface::~WgSurface()
{
}

//____ Width() ________________________________________________________________

int WgSurface::Width() const
{
	return Size().w;
}

//____ Height() _______________________________________________________________

int WgSurface::Height() const
{
	return Size().h;
}


//____ Col2Pixel() ____________________________________________________________

Uint32 WgSurface::Col2Pixel( const WgColor& col ) const
{
	Uint32 pix = ((col.r << m_pixelFormat.R_shift) & m_pixelFormat.R_mask) |
				 ((col.g << m_pixelFormat.G_shift) & m_pixelFormat.G_mask) |
				 ((col.b << m_pixelFormat.B_shift) & m_pixelFormat.B_mask) |
				 ((col.a << m_pixelFormat.A_shift) & m_pixelFormat.A_mask);

	return pix;
}

//____ Pixel2Col() ____________________________________________________________

WgColor WgSurface::Pixel2Col( Uint32 pixel ) const
{
	WgColor col( (pixel & m_pixelFormat.R_mask) >> m_pixelFormat.R_shift,
				 (pixel & m_pixelFormat.G_mask) >> m_pixelFormat.G_shift,
				 (pixel & m_pixelFormat.B_mask) >> m_pixelFormat.B_shift,
				 (pixel & m_pixelFormat.A_mask) >> m_pixelFormat.A_shift );

	return col;
}

//____ _lockAndAdjustRegion() __________________________________________________

WgRect WgSurface::_lockAndAdjustRegion( WgAccessMode modeNeeded, const WgRect& region )
{
	if( m_accessMode == WG_NO_ACCESS )
	{
		Lock( modeNeeded );
		return region;
	}
	else if( m_accessMode != WG_READ_WRITE && m_accessMode != modeNeeded )
		return WgRect(0,0,0,0);

	if( !m_lockRegion.Contains( region ) )
		return WgRect(0,0,0,0);

	return region - m_lockRegion.Pos();
}

//____ Fill() _________________________________________________________________

bool WgSurface::Fill( WgColor col )
{
	return Fill( col, WgRect(0,0,Size()) );
}

bool WgSurface::Fill( WgColor col, const WgRect& _rect )
{

	WgAccessMode oldMode = m_accessMode;
	WgRect rect = _lockAndAdjustRegion(WG_WRITE_ONLY,_rect);

	if( rect.w == 0 )
		return false;

	//


	Uint32 pixel = Col2Pixel( col );
	int width = Width();
	int height = Height();
	int pitch = Pitch();
	Uint8 * pDest = m_pPixels;

	bool ret = true;
	switch( m_pixelFormat.bits )
	{
		case 8:
			for( int y = 0 ; y < height ; y++ )
			{
				for( int x = 0 ; x < width ; x++ )
					pDest[x] = (Uint8) pixel;
				pDest += pitch;
			}
			break;
		case 16:
			for( int y = 0 ; y < height ; y++ )
			{
				for( int x = 0 ; x < width ; x++ )
					((Uint16*)pDest)[x] = (Uint16) pixel;
				pDest += pitch;
			}
			break;
		case 24:
		{
			Uint8 one = (Uint8) pixel;
			Uint8 two = (Uint8) (pixel>>8);
			Uint8 three = (Uint8) (pixel>>16);

			for( int y = 0 ; y < height ; y++ )
			{
				for( int x = 0 ; x < width ; x++ )
				{
					pDest[x++] = one;
					pDest[x++] = two;
					pDest[x++] = three;
				}
				pDest += pitch - width*3;
			}
			break;
		}
		case 32:
			for( int y = 0 ; y < height ; y++ )
			{
				for( int x = 0 ; x < width ; x++ )
					((Uint32*)pDest)[x] = pixel;
				pDest += pitch;
			}
			break;
		default:
			ret = false;
	}

	//

	if( oldMode == WG_NO_ACCESS )
		Unlock();

	return ret;
}

//_____ CopyFrom() _____________________________________________________________

bool WgSurface::CopyFrom( WgSurface * pSrcSurface, WgCoord dst )
{
	if( pSrcSurface == 0 )
		return false;

	return CopyFrom( pSrcSurface, WgRect(0,0,pSrcSurface->Size()), dst );
}

bool WgSurface::CopyFrom( WgSurface * pSrcSurface, const WgRect& _srcRect, WgCoord _dst )
{
	if( pSrcSurface == 0 || pSrcSurface->m_pixelFormat.type == WG_PIXEL_UNKNOWN || m_pixelFormat.type == WG_PIXEL_UNKNOWN )
		return false;

	// Save old locks and lock the way we want.

	WgAccessMode 	dstOldMode 		= m_accessMode;
	WgAccessMode 	srcOldMode 		= pSrcSurface->GetLockStatus();

	WgRect srcRect = pSrcSurface->_lockAndAdjustRegion( WG_READ_ONLY, _srcRect );
	WgRect dstRect = _lockAndAdjustRegion( WG_WRITE_ONLY, WgRect(_dst.x,_dst.y,srcRect.w,srcRect.h) );

	// Do the copying

	if( srcRect.w > 0 && dstRect.w > 0 )
	{
		const WgPixelFormat * pSrcFormat = pSrcSurface->PixelFormat();
		const WgPixelFormat * pDstFormat = &m_pixelFormat;

		int		srcPitch = pSrcSurface->Pitch();
		int		dstPitch = m_pitch;

		unsigned char *	pSrc = (unsigned char*) pSrcSurface->Pixels();
		unsigned char *	pDst = (unsigned char*) m_pPixels;

		pSrc += srcRect.y * srcPitch + srcRect.x * pSrcFormat->bits/8;
		pDst += dstRect.y * dstPitch + dstRect.x * pDstFormat->bits/8;


		if( (pSrcFormat->type == pDstFormat->type && pSrcFormat->type != WG_PIXEL_CUSTOM) ||
			(pSrcFormat->bits == pDstFormat->bits && pSrcFormat->R_mask == pDstFormat->R_mask &&
			 pSrcFormat->G_mask == pDstFormat->G_mask && pSrcFormat->B_mask == pDstFormat->B_mask &&
			 pSrcFormat->A_mask == pDstFormat->A_mask) )
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
		else if( (pSrcFormat->type == WG_PIXEL_RGBA_8 || pSrcFormat->type == WG_PIXEL_RGB_8) &&
				 (pDstFormat->type == WG_PIXEL_RGBA_8 || pDstFormat->type == WG_PIXEL_RGB_8) )
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

			//TODO: Optimize by making different loops for all 16 combinations of source and destination pixel-bits.

			int		srcInc = pSrcFormat->bits/8;
			int		dstInc = pDstFormat->bits/8;

			int		srcLineInc = srcPitch - srcInc * srcRect.w;
			int		dstLineInc = dstPitch - dstInc * srcRect.w;

			unsigned int	R_mask = (((0xFFFFFFFF & pSrcFormat->R_mask) >> pSrcFormat->R_shift) << pDstFormat->R_shift) & pDstFormat->R_mask;
			unsigned int	G_mask = (((0xFFFFFFFF & pSrcFormat->G_mask) >> pSrcFormat->G_shift) << pDstFormat->G_shift) & pDstFormat->G_mask;
			unsigned int	B_mask = (((0xFFFFFFFF & pSrcFormat->B_mask) >> pSrcFormat->B_shift) << pDstFormat->B_shift) & pDstFormat->B_mask;
			unsigned int	A_mask = (((0xFFFFFFFF & pSrcFormat->A_mask) >> pSrcFormat->A_shift) << pDstFormat->A_shift) & pDstFormat->A_mask;

			int		R_shift = pSrcFormat->R_shift - pDstFormat->R_shift;
			int		G_shift = pSrcFormat->G_shift - pDstFormat->G_shift;
			int		B_shift = pSrcFormat->B_shift - pDstFormat->B_shift;
			int		A_shift = pSrcFormat->A_shift - pDstFormat->A_shift;

			for( int y = 0 ; y < srcRect.h ; y++ )
			{
				for( int x = 0 ; x < srcRect.w ; x++ )
				{
					unsigned int srcpixel;

					switch( pSrcFormat->bits )
					{
						case 8:
							srcpixel = pSrc[0];
						break;
						case 16:
							srcpixel = * ((unsigned short*)pSrc);
						case 24:
							srcpixel = pSrc[0] + ((unsigned int)pSrc[1]) << 8 + ((unsigned int)pSrc[2]) << 16;
						case 32:
							srcpixel = * ((unsigned int*)pSrc);
					}

					unsigned int dstpixel = ((srcpixel >> R_shift) & R_mask) |
											((srcpixel >> G_shift) & G_mask) |
											((srcpixel >> B_shift) & B_mask) |
											((srcpixel >> A_shift) & A_mask);

					switch( pDstFormat->bits )
					{
						case 8:
							pDst[0] = (unsigned char) dstpixel;
						break;
						case 16:
							* ((unsigned short*)pDst) = (unsigned short) dstpixel;
						case 24:
							pDst[0] = (unsigned char) dstpixel;
							pDst[1] = (unsigned char) (dstpixel >> 8);
							pDst[2] = (unsigned char) (dstpixel >> 16);
						case 32:
							* ((unsigned int*)pDst) = dstpixel;
					}

					pSrc += srcInc;
					pDst += dstInc;
				}
				pSrc += srcLineInc;
				pDst += dstLineInc;
			}
		}
	}

	// Release any temporary locks

	if( dstOldMode == WG_NO_ACCESS )
		Unlock();

	if( srcOldMode == WG_NO_ACCESS )
		pSrcSurface->Unlock();
}





//____ defineBlockSet() ________________________________________________________

WgBlockSetPtr WgSurface::defineBlockSet(	const WgRect& normal, const WgRect& marked,
											const WgRect& selected, const WgRect& disabled,
											const WgRect& special, const WgBorders& gfxBorders,
											const WgBorders& contentBorders, const WgColorSetPtr& pTextColors, Uint32 flags ) const
{

	WgBlockSet * p =new(g_pBlockSetMemPool->allocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, marked, selected, disabled, special, gfxBorders, contentBorders, pTextColors, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet(	const WgRect& normal, const WgRect& marked,
											const WgRect& selected, const WgRect& disabled,
											const WgBorders& gfxBorders, const WgBorders& contentBorders,
											const WgColorSetPtr& pTextColors, Uint32 flags ) const
{

	WgBlockSet * p =new(g_pBlockSetMemPool->allocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, marked, selected, disabled, normal, gfxBorders, contentBorders, pTextColors, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet(	const WgRect& normal, const WgRect& marked,
										const WgRect& selected, const WgBorders& gfxBorders,
										const WgBorders& contentBorders, const WgColorSetPtr& pTextColors,
										Uint32 flags ) const
{
	WgBlockSet * p = new( g_pBlockSetMemPool->allocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, marked, selected, normal, normal, gfxBorders, contentBorders, pTextColors, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgRect& normal, const WgRect& disabled,
										const WgBorders& gfxBorders, const WgBorders& contentBorders,
										const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	WgBlockSet * p = new( g_pBlockSetMemPool->allocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, normal, normal, disabled, normal, gfxBorders, contentBorders, pTextColors, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgRect& normal, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	WgBlockSet * p = new( g_pBlockSetMemPool->allocEntry())
							WgBlockSet(g_pBlockSetMemPool, this, normal, normal, normal, normal, normal, gfxBorders, contentBorders, pTextColors, flags );

//	m_blockSets.push_back( p );
	return WgBlockSetPtr(p);
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile5& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip*4) / 5;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							WgRect(tile.x+skip*2, tile.y, w, h),
							WgRect(tile.x+skip*3, tile.y, w, h),
							WgRect(tile.x+skip*4, tile.y, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile4& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip*3) / 4;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							WgRect(tile.x+skip*2, tile.y, w, h),
							WgRect(tile.x+skip*3, tile.y, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile3& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip*2) / 3;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							WgRect(tile.x+skip*2, tile.y, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgHorrTile2& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = (tile.w - tile.skip) / 2;
	int h = tile.h;
	int	skip = w + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x+skip, tile.y, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile5& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = tile.w;
	int h = (tile.h - tile.skip*4) / 5;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							WgRect(tile.x, tile.y+skip*2, w, h),
							WgRect(tile.x, tile.y+skip*3, w, h),
							WgRect(tile.x, tile.y+skip*4, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile4& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = tile.w;
	int h = (tile.h - tile.skip*3) / 4;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							WgRect(tile.x, tile.y+skip*2, w, h),
							WgRect(tile.x, tile.y+skip*3, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile3& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = tile.w;
	int h = (tile.h - tile.skip*2) / 3;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							WgRect(tile.x, tile.y+skip*2, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

WgBlockSetPtr WgSurface::defineBlockSet( const WgVertTile2& tile, const WgBorders& gfxBorders, const WgBorders& contentBorders,
										 const WgColorSetPtr& pTextColors, Uint32 flags ) const
{
	int	w = tile.w;
	int h = tile.h / 2;
	int	skip = h + tile.skip;

	return defineBlockSet(	WgRect(tile.x, tile.y, w, h),
							WgRect(tile.x, tile.y+skip, w, h),
							gfxBorders, contentBorders, pTextColors, flags );
}

