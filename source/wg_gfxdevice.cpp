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

#include <wg_gfxdevice.h>

#include <wg_gfxdevice.h>
#include <wg_surface.h>
#include <wg_geo.h>
#include <wg_blockset.h>
#include <wg_text.h>
#include <wg_cursorinstance.h>
#include <wg_font.h>
#include <wg_gfxanim.h>
#include <wg_util.h>
#include <wg_pen.h>


//____ Constructor _____________________________________________________________

WgGfxDevice::WgGfxDevice()
{
	m_tintColor 		= WgColor(255,255,255);
	m_blendMode 		= WG_BLENDMODE_BLEND;
	m_bSaveDirtyRects 	= false;
	m_renderFlags		= 0;
}

//____ SetSaveDirtyRects() _____________________________________________________

bool WgGfxDevice::SetSaveDirtyRects( bool bSave )
{
	m_bSaveDirtyRects = bSave;
	return true;
}

//____ SetTintColor() __________________________________________________________

void WgGfxDevice::SetTintColor( WgColor color )
{
	m_tintColor = color;
}

//____ SetBlendMode() __________________________________________________________

bool WgGfxDevice::SetBlendMode( WgBlendMode blendMode )
{
	m_blendMode = blendMode;
	return false;				// Not implemented.
}

//______________________________________________________________________________
Uint32 WgGfxDevice::SetRenderFlags( Uint32 flags )
{
	Uint32 oldFlags = m_renderFlags;
	m_renderFlags = flags;
	return oldFlags;
}

//____ BeginRender() ___________________________________________________________

bool WgGfxDevice::BeginRender()
{
	return true;	// Assumed to be ok if device doesn't have its own method.
}

//____ EndRender() _____________________________________________________________

bool WgGfxDevice::EndRender()
{
	return true;	// Assumed to be ok if device doesn't have its own method.
}

//____ Blit() __________________________________________________________________

void WgGfxDevice::Blit( const WgSurface* pSrc )
{
	Blit( pSrc, WgRect( 0, 0, pSrc->GetWidth(), pSrc->GetHeight() ), 0, 0 );
}

void WgGfxDevice::Blit( const WgSurface* pSrc, Sint32 dx, Sint32 dy )
{
	Blit( pSrc, WgRect( 0, 0, pSrc->GetWidth(), pSrc->GetHeight() ), dx, dy );
}

//____ StretchBlit() ___________________________________________________________

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, bool bTriLinear, float mipmapBias )
{
	StretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->GetWidth(), (float) pSrc->GetHeight(), 0, 0, (float) m_canvasWidth, (float) m_canvasHeight, bTriLinear, mipmapBias );
}

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, const WgRect& dest, bool bTriLinear, float mipmapBias )
{
	StretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->GetWidth(), (float) pSrc->GetHeight(), (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
}

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear, float mipmapBias )
{
	StretchBlitSubPixel( pSrc, (float) src.x, (float) src.y, (float) src.w, (float) src.h, (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
}

//____ TileBlit() ______________________________________________________________

void WgGfxDevice::TileBlit( const WgSurface* _pSrc )
{
	TileBlit( _pSrc, WgRect( 0, 0, _pSrc->GetWidth(), _pSrc->GetHeight() ), WgRect(0,0,m_canvasWidth,m_canvasHeight) );
}

void WgGfxDevice::TileBlit( const WgSurface* _pSrc, const WgRect& _dest )
{
	TileBlit( _pSrc, WgRect( 0, 0, _pSrc->GetWidth(), _pSrc->GetHeight() ), _dest );
}

void WgGfxDevice::TileBlit( const WgSurface* _pSrc, const WgRect& _src, const WgRect& _dest )
{
	if( !_pSrc || _dest.h == 0 || _dest.w == 0 )
		return;

	WgRect	r = _src;
	WgRect	r2 = _src;

	int nCol = _dest.w / _src.w;
	r2.w = _dest.w % _src.w;

	int nRow = (_dest.h+(_src.h-1))/ _src.h;	// Including any cut row....

	Sint32		destX = _dest.x;
	Sint32		destY = _dest.y;

	for( int row = 1 ; row <= nRow ; row++ )
	{
		// Possibly cut the height if this is the last row...

		if( row == nRow )
		{
			r.h = _dest.y + _dest.h - destY;
			r2.h = r.h;
		}

		// Blit a row.

		for( int col = 0 ; col < nCol ; col++ )
		{
			Blit( _pSrc, r, destX, destY );
			destX += r.w;
		}

		// Blit any left over part at end of row.

		if( r2.w > 0 )
			Blit( _pSrc, r2, destX, destY );

		destX = _dest.x;
		destY += _src.h;
	}
	return;
}


//____ ClipFill() ______________________________________________________________

void WgGfxDevice::ClipFill( const WgRect& _clip, const WgRect& _rect, const WgColor& _col )
{
	Fill( WgRect( _clip, _rect ), _col );
}

//____ ClipBlit() ______________________________________________________________

void WgGfxDevice::ClipBlit( const WgRect& clip, const WgSurface* pSrc )
{
	ClipBlit( clip, pSrc, WgRect(0,0,pSrc->GetWidth(),pSrc->GetHeight()), 0, 0 );
}

void WgGfxDevice::ClipBlit( const WgRect& clip, const WgSurface* pSrc, Sint32 dx, Sint32 dy  )
{
	ClipBlit( clip, pSrc, WgRect(0,0,pSrc->GetWidth(),pSrc->GetHeight()), dx, dy );
}

void WgGfxDevice::ClipBlit( const WgRect& clip, const WgSurface* pSrc, const WgRect& srcRect, Sint32 dx, Sint32 dy  )
{
	if( (clip.x <= dx) && (clip.x + clip.w > dx + srcRect.w) &&
      (clip.y <= dy) && (clip.y + clip.h > dy + srcRect.h) )
	{
		Blit( pSrc, srcRect, dx, dy );														// Totally inside clip-rect.
		return;
	}

	if( (clip.x > dx + srcRect.w) || (clip.x + clip.w < dx) ||
      (clip.y > dy + srcRect.h) || (clip.y + clip.h < dy) )
		return;																						// Totally outside clip-rect.

	// Do Clipping

	WgRect	newSrc = srcRect;

	if( dx < clip.x )
	{
		newSrc.w -= clip.x - dx;
		newSrc.x += clip.x - dx;
		dx = clip.x;
	}

	if( dy < clip.y )
	{
		newSrc.h -= clip.y - dy;
		newSrc.y += clip.y - dy;
		dy = clip.y;
	}

	if( dx + newSrc.w > clip.x + clip.w )
		newSrc.w = (clip.x + clip.w) - dx;

	if( dy + newSrc.h > clip.y + clip.h )
		newSrc.h = (clip.y + clip.h) - dy;


	Blit( pSrc, newSrc, dx, dy );
}

//____ ClipStretchBlit() _______________________________________________________

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, bool bTriLinear, float mipBias )
{
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->GetWidth(), pSrc->GetHeight()), WgRect( 0,0,m_canvasWidth,m_canvasHeight), bTriLinear, mipBias );
}

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& dest, bool bTriLinear, float mipBias )
{
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->GetWidth(), pSrc->GetHeight()), dest, bTriLinear, mipBias );
}

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear, float mipBias )
{
	ClipStretchBlit( clip, pSrc, (float)src.x, (float)src.y, (float)src.w, (float)src.h, (float)dest.x, (float)dest.y, (float)dest.w, (float)dest.h, false );
}

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias)
{
	float cx = std::max(float(clip.x), dx);
	float cy = std::max(float(clip.y), dy);
	float cw = std::min(float(clip.x + clip.w), dx + dw) - cx;
	float ch = std::min(float(clip.y + clip.h), dy + dh) - cy;

	if(cw <= 0 || ch <= 0)
		return;

	if( dw > cw )
	{
		float	sdxr = sw / dw;			// Source/Destination X Ratio.

		sw = sdxr * cw;

		if( dx < cx )
			sx += sdxr * (cx - dx);
	}

	if( dh > ch )
	{
		float	sdyr = sh / dh;			// Source/Destination Y Ratio.

		sh = sdyr * ch;

		if( dy < cy )
			sy += sdyr * (cy - dy);
	}

	StretchBlitSubPixel( pSrc, sx, sy, sw, sh, cx, cy, cw, ch, bTriLinear, mipBias );
}

//____ ClipTileBlit() __________________________________________________________

void WgGfxDevice::ClipTileBlit( const WgRect& clip, const WgSurface* pSrc )
{
	ClipTileBlit( clip, pSrc, WgRect( 0,0,pSrc->GetWidth(),pSrc->GetHeight() ),
				  WgRect(0,0,m_canvasWidth,m_canvasHeight) );
}

void WgGfxDevice::ClipTileBlit( const WgRect& clip, const WgSurface* pSrc,
								  const WgRect& dest )
{
	ClipTileBlit( clip, pSrc, WgRect(0,0,pSrc->GetWidth(),pSrc->GetHeight()), dest );
}


void WgGfxDevice::ClipTileBlit( const WgRect& _clip, const WgSurface* _pSrc, const WgRect& _src, const WgRect& _dest )
{
	if( !_pSrc )
		return;

	WgRect	myRect;

	WgRect	clip;
	if( !clip.Intersection( _dest, _clip ) )
		return;

	// Take care of start-offset change caused by clipping.

	Sint32		xStart = (clip.x - _dest.x) % _src.w;
	if( xStart < 0 )
		xStart += _src.w;
	xStart += _src.x;

	Sint32		yStart = (clip.y - _dest.y) % _src.h;
	if( yStart < 0 )
		yStart += _src.h;
	yStart += _src.y;


	Sint32		destY = clip.y, destX;

	myRect.y = yStart;
	myRect.h =_src.y + _src.h - yStart;

	while( destY < clip.y + clip.h )
	{
		if( myRect.h > clip.y + clip.h - destY )
			myRect.h = clip.y + clip.h - destY;

		myRect.x = xStart;
		myRect.w = _src.x + _src.w - xStart;
		if( myRect.w > clip.w )
			myRect.w = clip.w;


		// Blit a row.

		destX = clip.x;
		Blit( _pSrc, myRect, destX, destY );
		destX += myRect.w;
		myRect.x = _src.x;
		myRect.w = _src.w;

		while( destX <= clip.x + clip.w - _src.w )
		{
			Blit( _pSrc, myRect, destX, destY );
			destX += myRect.w;
		}
		myRect.w = clip.x + clip.w - destX;
		if( myRect.w > 0 )
			Blit( _pSrc, myRect, destX, destY );

		destY += myRect.h;
		myRect.y = _src.y;
		myRect.h = _src.h;
	}
	return;
}

//____ BlitBlock() ____________________________________________________________

void WgGfxDevice::BlitBlock( const WgBlock& _block, const WgRect& _dest2, bool bTriLinear, float mipmapbias )
{
	if( !_block.GetSurface() )
		return;

	if( _block.IsSkipable() )
		return;

	WgRect _dest = _dest2;
	WgUtil::AdjustScaledArea(_block, _dest);

	// Shortcuts & optimizations for common special cases.

	const WgRect& src = _block.GetRect();
	const WgSurface * pSurf = _block.GetSurface();

	if( src.w == _dest.w && src.h == _dest.h )
	{
		Blit( pSurf, src, _dest.x, _dest.y );
		return;
	}

	if( !_block.HasBorders() )
	{
		if( _block.HasTiledCenter() )
			TileBlit( pSurf, src, _dest );
		else
			StretchBlit( pSurf, src, _dest, bTriLinear, mipmapbias );
		return;
	}

	if( src.w == _dest.w )
	{
		BlitVertBar( pSurf, src, _block.GetBorders(),
					 _block.HasTiledCenter(), _dest.x, _dest.y, _dest.h );
		return;
	}

	if( src.h == _dest.h )
	{
		BlitHorrBar( pSurf, src, _block.GetBorders(),
					 _block.HasTiledCenter(), _dest.x, _dest.y, _dest.w );
		return;
	}

	const WgBorders& borders = _block.GetBorders();

	// Render upper row (top-left corner, top stretch area and top-right corner)

	if( borders.top > 0 )
	{
		WgRect rect( src.x, src.y, src.w, borders.top );

		BlitHorrBar( pSurf, rect, borders, _block.HasTiledTopBorder(),
					_dest.x, _dest.y, _dest.w );
	}

	// Render lowest row (bottom-left corner, bottom stretch area and bottom-right corner)

	if( borders.bottom > 0 )
	{
		WgRect rect( src.x, src.y + src.h - borders.bottom, src.w, borders.bottom );

		BlitHorrBar( pSurf, rect, borders, _block.HasTiledBottomBorder(),
					_dest.x, _dest.y + _dest.h - borders.bottom, _dest.w );
	}

	// Render left and right stretch areas

	if( _dest.h > (Sint32) borders.GetHeight() )
	{
		if( borders.left > 0 )
		{
			WgRect sr( src.x, src.y + borders.top, borders.left, src.h - borders.GetHeight() );
			WgRect dr( _dest.x, _dest.y + borders.top, borders.left, _dest.h - borders.GetHeight() );

			if( _block.HasTiledLeftBorder() )
				TileBlit( pSurf, sr, dr );
			else
				StretchBlit( pSurf, sr, dr );
		}

		if( borders.right > 0 )
		{
			WgRect sr(	src.x + src.w - borders.right, src.y + borders.top,
						borders.right, src.h - borders.GetHeight() );
			WgRect dr(	_dest.x + _dest.w - borders.right, _dest.y + borders.top,
						borders.right, _dest.h - borders.GetHeight() );

			if( _block.HasTiledRightBorder() )
				TileBlit( pSurf, sr, dr );
			else
				StretchBlit( pSurf, sr, dr );
		}
	}


	// Render middle stretch area

	if( (_dest.h > borders.top + borders.bottom) && (_dest.w > borders.left + borders.right ) )
	{
		WgRect sr(	src.x + borders.left, src.y + borders.top,
					src.w - borders.GetWidth(), src.h - borders.GetHeight() );

		WgRect dr(	_dest.x + borders.left, _dest.y + borders.top,
					_dest.w - borders.GetWidth(), _dest.h - borders.GetHeight() );

		if( _block.HasTiledCenter() )
			TileBlit( pSurf, sr, dr );
		else
			StretchBlit( pSurf, sr, dr, bTriLinear, mipmapbias );
	}


}


//____ ClipBlitBlock() ________________________________________________________

void WgGfxDevice::ClipBlitBlock( const WgRect& _clip, const WgBlock& _block, const WgRect& _dest2, bool bTriLinear, float mipmapbias )
{
	if( !_block.GetSurface() )
		return;

	if( _block.IsSkipable() )
		return;

	// Shortcuts & optimizations for common special cases.

	if( _clip.Contains( _dest2 ) )
	{
		BlitBlock( _block, _dest2, bTriLinear, mipmapbias );
		return;
	}

	WgRect _dest = _dest2;
	WgUtil::AdjustScaledArea(_block, _dest);

	const WgRect& src = _block.GetRect();
	const WgSurface * pSurf = _block.GetSurface();

	if( src.w == _dest.w && src.h == _dest.h )
	{
		ClipBlit( _clip, pSurf, src, _dest.x, _dest.y );
		return;
	}

	if( !_block.HasBorders() )
	{
		if( _block.HasTiledCenter() )
			ClipTileBlit( _clip, pSurf, src, _dest );
		else
			ClipStretchBlit( _clip, pSurf, src, _dest, bTriLinear, mipmapbias );
		return;
	}

	if( src.w == _dest.w )
	{
		ClipBlitVertBar( _clip, pSurf, src, _block.GetBorders(),
						 _block.HasTiledCenter(), _dest.x, _dest.y, _dest.h );
		return;
	}

	if( src.h == _dest.h )
	{
		ClipBlitHorrBar( _clip, pSurf, src, _block.GetBorders(),
						 _block.HasTiledCenter(), _dest.x, _dest.y, _dest.w );
		return;
	}

	const WgBorders& borders = _block.GetBorders();

	// Render upper row (top-left corner, top stretch area and top-right corner)

	if( borders.top > 0 )
	{
		WgRect rect( src.x, src.y, src.w, borders.top );

		ClipBlitHorrBar( _clip, pSurf, rect, borders, _block.HasTiledTopBorder(),
								_dest.x, _dest.y, _dest.w );
	}

	// Render lowest row (bottom-left corner, bottom stretch area and bottom-right corner)

	if( borders.bottom > 0 )
	{
		WgRect rect( src.x, src.y + src.h - borders.bottom, src.w, borders.bottom );

		ClipBlitHorrBar( _clip, pSurf, rect, borders, _block.HasTiledBottomBorder(),
								_dest.x, _dest.y + _dest.h - borders.bottom, _dest.w );
	}

	// Render left and right stretch areas

	if( _dest.h > (Sint32) borders.GetHeight() )
	{
		if( borders.left > 0 )
		{
			WgRect sr( src.x, src.y + borders.top, borders.left, src.h - borders.GetHeight() );
			WgRect dr( _dest.x, _dest.y + borders.top, borders.left, _dest.h - borders.GetHeight() );

			if( _block.HasTiledLeftBorder() )
				ClipTileBlit( _clip, pSurf, sr, dr );
			else
				ClipStretchBlit( _clip, pSurf, sr, dr );
		}

		if( borders.right > 0 )
		{
			WgRect sr(	src.x + src.w - borders.right, src.y + borders.top,
						borders.right, src.h - borders.GetHeight() );
			WgRect dr(	_dest.x + _dest.w - borders.right, _dest.y + borders.top,
						borders.right, _dest.h - borders.GetHeight() );

			if( _block.HasTiledRightBorder() )
				ClipTileBlit( _clip, pSurf, sr, dr );
			else
				ClipStretchBlit( _clip, pSurf, sr, dr );
		}
	}


	// Render middle stretch area

	if( (_dest.h > borders.top + borders.bottom) && (_dest.w > borders.left + borders.right ) )
	{
		WgRect sr(	src.x + borders.left, src.y + borders.top,
					src.w - borders.GetWidth(), src.h - borders.GetHeight() );

		WgRect dr(	_dest.x + borders.left, _dest.y + borders.top,
					_dest.w - borders.GetWidth(), _dest.h - borders.GetHeight() );

		if( _block.HasTiledCenter() )
			ClipTileBlit( _clip, pSurf, sr, dr );
		else
			ClipStretchBlit( _clip, pSurf, sr, dr, bTriLinear, mipmapbias );
	}


}


//____ ClipBlitHorrBar() ______________________________________________________

void WgGfxDevice::ClipBlitHorrBar(	const WgRect& _clip, const WgSurface * _pSurf,
								  	const WgRect& _src, const WgBorders& _borders,
								  	bool _bTile, Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	/*
		This can be optimized by handling clipping directly instead of calling clipBlit().
	*/

	// Blit left edge

	WgRect	r( _src.x, _src.y, _borders.left, _src.h );
	ClipBlit( _clip, _pSurf, r, _dx, _dy );

	_len -= _borders.GetWidth();			// Remove left and right edges from len.
	_dx += _borders.left;

	// Blit tiling part

	r.x += _borders.left;
	r.w = _src.w - _borders.GetWidth();

	if( _bTile )
	{
		while( _len > r.w )
		{
			ClipBlit( _clip, _pSurf, r, _dx, _dy );
			_len -= r.w;
			_dx += r.w;
		}
		if( _len != 0 )
		{
			r.w = _len;
			ClipBlit( _clip, _pSurf, r, _dx, _dy );
			_dx += _len;
		}
	}
	else
	{
		ClipStretchBlit( _clip, _pSurf, r, WgRect( _dx, _dy, _len, r.h ) );
		_dx += _len;
	}

	// Blit right edge

	r.x = _src.x + _src.w - _borders.right;
	r.w = _borders.right;
	ClipBlit( _clip, _pSurf, r, _dx, _dy );
}

//____ ClipBlitVertBar() ______________________________________________________

void WgGfxDevice::ClipBlitVertBar(	const WgRect& _clip, const WgSurface * _pSurf,
								  	const WgRect& _src, const WgBorders& _borders,
								  	bool _bTile, Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	/*
		This can be optimized by handling clipping directly instead of calling clipBlit().
	*/

	// Blit top edge

	WgRect	r( _src.x, _src.y, _src.w, _borders.top );
	ClipBlit( _clip, _pSurf, r, _dx, _dy );

	_len -= _borders.GetHeight();			// Remove top and bottom edges from len.
	_dy += _borders.top;

	// Blit tiling part

	r.y += _borders.top;
	r.h = _src.h - _borders.GetHeight();

	if( _bTile )
	{
		while( _len > r.h )
		{
			ClipBlit( _clip, _pSurf, r, _dx, _dy );
			_len -= r.h;
			_dy += r.h;
		}
		if( _len != 0 )
		{
			r.h = _len;
			ClipBlit( _clip, _pSurf, r, _dx, _dy );
			_dy += _len;
		}
	}
	else
	{
		ClipStretchBlit( _clip, _pSurf, r, WgRect( _dx, _dy, r.w, _len ) );
		_dy += _len;
	}

	// Blit bottom edge

	r.y = _src.y + _src.h - _borders.bottom;
	r.h = _borders.bottom;
	ClipBlit( _clip, _pSurf, r, _dx, _dy );
}


//____ BlitHorrBar() __________________________________________________________

void WgGfxDevice::BlitHorrBar(	const WgSurface * _pSurf, const WgRect& _src,
								const WgBorders& _borders, bool _bTile,
								Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	// Blit left edge

	WgRect	r( _src.x, _src.y, _borders.left, _src.h );
	Blit( _pSurf, r, _dx, _dy );

	_len -= _borders.GetWidth();			// Remove left and right edges from len.
	_dx += _borders.left;

	// Blit tiling part

	r.x += _borders.left;
	r.w = _src.w - _borders.GetWidth();

	if( _bTile )
	{
		while( _len > r.w )
		{
			Blit( _pSurf, r, _dx, _dy );
			_len -= r.w;
			_dx += r.w;
		}
		if( _len != 0 )
		{
			r.w = _len;
			Blit( _pSurf, r, _dx, _dy );
			_dx += _len;
		}
	}
	else
	{
		StretchBlit( _pSurf, r, WgRect( _dx, _dy, _len, r.h ) );
		_dx += _len;
	}

	// Blit right edge

	r.x = _src.x + _src.w - _borders.right;
	r.w = _borders.right;
	Blit( _pSurf, r, _dx, _dy );
}

//____ BlitVertBar() __________________________________________________________

void WgGfxDevice::BlitVertBar(	const WgSurface * _pSurf, const WgRect& _src,
								const WgBorders& _borders, bool _bTile,
								Sint32 _dx, Sint32 _dy, Sint32 _len )
{
	// Blit top edge

	WgRect	r( _src.x, _src.y, _src.w, _borders.top );
	Blit( _pSurf, r, _dx, _dy );

	_len -= _borders.GetHeight();			// Remove top and bottom borders from len.
	_dy += _borders.top;

	// Blit tiling part

	r.y += _borders.top;
	r.h = _src.h - _borders.GetHeight();

	if( _bTile )
	{
		while( _len > r.h )
		{
			Blit( _pSurf, r, _dx, _dy );
			_len -= r.h;
			_dy += r.h;
		}
		if( _len != 0 )
		{
			r.h = _len;
			Blit( _pSurf, r, _dx, _dy );
			_dy += _len;
		}
	}
	else
	{
		StretchBlit( _pSurf, r, WgRect( _dx, _dy, r.w, _len ) );
		_dy += _len;
	}

	// Blit bottom edge

	r.y = _src.y + _src.h - _borders.bottom;
	r.h = _borders.bottom;
	Blit( _pSurf, r, _dx, _dy );
}


void WgGfxDevice::PrintText( const WgRect& clip, const WgText * pText, const WgCursorInstance* pCursor, const WgRect& dest, WgPen* pPen )
{
	const WgTextPropPtr	pDefProp = pText->getProperties();
	const WgOrigo& origo	= pText->alignment();

	Uint32				nLines = pText->nbSoftLines();
	const WgTextLine *	pLines = pText->getSoftLines();
	const WgChar *		pChars = pText->getText();

	pPen->SetTextNode( pText->getNode() );
	pPen->SetTextProp( pDefProp, WgTextPropPtr(), pText->mode() );

	PrintTextSelection(clip, pText, pCursor, dest, pPen);

	WgCord	pos;
	pos.x = dest.x;
	pos.y = (int) dest.y + origo.calcOfsY( dest.h, pText->height() ) + pPen->GetBaseline();
	//pPen->SetOrigo( pos );

	Uint32 cursLine = -1, cursCol = -1;
	if(pCursor)
		pCursor->getSoftPos( cursLine, cursCol );

	for( unsigned int i = 0 ; i < nLines ; i++ )
	{
		if( cursLine == i )
		{
			pos.x = dest.x;
			if( origo.anchorX() != 0 && origo.hotspotX() != 0 )
			{

				// TODO: should take textprop for cursors position into account...
				int linewidth = pText->getSoftLineWidthPart(i, 0, cursCol ) /*+ pPen->AdvancePosCursor( *pCursor )*/ + pText->getSoftLineWidthPart(i, cursCol );
				pos.x += origo.calcOfsX( dest.w, linewidth );
				if( pos.x < dest.x )
					pos.x = dest.x;
				pPen->SetOrigo( pos );	// So tab positions will start counting from start of line.
			}

			const WgFont * pFont = pPen->GetFont();
			WgCursor::Mode cursMode = pCursor->cursorMode();
			if( cursCol == 0 && pFont->GetCursor()->bearingX(cursMode) < 0 )
				pos.x -= pFont->GetCursor()->bearingX(cursMode);

			pPen->SetPos( pos );
			PrintLine( pPen, pDefProp, pText->mode(), pChars + pLines[i].ofs, cursCol, false);
			pPen->BlitCursor( *pCursor );
			pPen->AdvancePosCursor( *pCursor );
			pPen->FlushChar();				// Avoid kerning against glyph before cursor.
			PrintLine( pPen, pDefProp, pText->mode(), pChars + pLines[i].ofs + cursCol, pLines[i].nChars - cursCol, true );
		}
		else
		{
			pos.x = dest.x;
			if( origo.anchorX() != 0 && origo.hotspotX() != 0 )
			{
				pos.x += origo.calcOfsX( dest.w, pText->getSoftLineWidth(i) );
				if( pos.x < dest.x )
					pos.x = dest.x;
				pPen->SetOrigo( pos );		// So tab positions will start counting from start of line.
			}

			pPen->SetPos( pos );
			PrintLine( pPen, pDefProp, pText->mode(), pChars + pLines[i].ofs, pLines[i].nChars, true );
		}


		pos.y += pPen->GetLineSpacing() + pText->lineSpaceAdjustment();
	}
}

void WgGfxDevice::PrintTextWithCursor( const WgRect& clip, const WgText * pText, const WgCursorInstance& ci, const WgRect& dest )
{
	if( !pText || !pText->getFont()  )
		return;

	WgPen	pen( this, dest, clip );
	PrintText(clip, pText, &ci, dest, &pen);
}

//____ PrintText() ____________________________________________________________

void WgGfxDevice::PrintText( const WgRect& clip, const WgText * pText, const WgRect& dest )
{
	if( !pText || !pText->getFont()  )
		return;

	WgPen pen;
	pen.SetDevice( this );

	if( dest.h < (int) pText->height() || dest.w < (int) pText->width() || !clip.Contains( dest ) )
		pen.SetClipRect( clip );

	PrintText(clip, pText, 0, dest, &pen);
}


//___________________________________________________________________________________________________
void WgGfxDevice::PrintTextSelection( const WgRect& clip, const WgText * pText, const WgCursorInstance* pCursor, const WgRect& dstRect, WgPen* pPen )
{
	const WgTextPropPtr	pDefProp = pText->getProperties();
	const WgOrigo& origo	= pText->alignment();

	Uint32				nLines = pText->nbSoftLines();
	const WgTextLine *	pLines = pText->getSoftLines();
	const WgChar *		pChars = pText->getText();

	pPen->SetTextNode( pText->getNode() );
	pPen->SetTextProp( pDefProp, WgTextPropPtr(), pText->mode() );

	Uint32 iSelStartLine, iSelEndLine, iSelStartCol, iSelEndCol;
	if(!pText->getSelection(iSelStartLine, iSelStartCol, iSelEndLine, iSelEndCol))
		return;

	pText->posHard2Soft(iSelStartLine, iSelStartCol);
	pText->posHard2Soft(iSelEndLine, iSelEndCol);

	pPen->SetPos(WgCord(0, 0));
	int xs = CalcCharOffset(pPen, pDefProp, pChars + pLines[iSelStartLine].ofs, iSelStartCol, pText->mode() );

	pPen->SetPos(WgCord(0, 0));
	int xe = CalcCharOffset(pPen, pDefProp, pChars + pLines[iSelEndLine].ofs, iSelEndCol, pText->mode() );

	WgCord dstPos;
	dstPos.x = dstRect.x;
	dstPos.y = (int) dstRect.y + origo.calcOfsY( dstRect.h, pText->height() );

	WgRect r;

	WgColor col = pText->getSelectionColor();

	int lineH = pPen->GetLineSpacing() + pText->lineSpaceAdjustment();
	if(iSelStartLine == iSelEndLine)
	{
		r.x = dstPos.x + xs;
		r.y = dstPos.y + iSelStartLine * lineH;
		r.w = xe - xs;
		r.h = lineH;
		ClipFill(clip, r, col);
	}
	else
	{
		r.x = dstPos.x + xs;
		r.y = dstPos.y + iSelStartLine * lineH;
		r.w = pText->width() - xs;
		r.h = lineH;
		ClipFill(clip, r, col);

		if(iSelEndLine - iSelStartLine >= 2)
		{
			r.x = dstPos.x;
			r.y = r.y + r.h;
			r.w = pText->width();
			r.h = (iSelEndLine - iSelStartLine - 1) * lineH;
			ClipFill(clip, r, col);
		}

		r.x = dstPos.x;
		r.y = r.y + r.h;
		r.w = xe;
		r.h = lineH;
		ClipFill(clip, r, col);
	}


}

//_________________________________________________________________________

int WgGfxDevice::CalcCharOffset(WgPen *pPen, const WgTextPropPtr& pDefProp, const WgChar* pLine, Uint32 nChars, WgMode mode )
{
	if( !pLine )
		return 0;

	Uint16	hProp = 0xFFFF;

	pPen->FlushChar();

 	for( Uint32 i = 0; i < nChars; i++ )
 	{
		// Act on possible change of character attributes.

		if( pLine[i].GetPropHandle() != hProp )
		{
			hProp = pLine[i].GetPropHandle();

			int success = pPen->SetTextProp( pDefProp.GetHandle(), hProp, mode );
			if( !success )
				break;
		}

		// Calculate position and blit the glyph.
		Uint16 ch = pLine[i].GetGlyph();

		if( pPen->SetChar( ch ) )
			pPen->ApplyKerning();

		pPen->AdvancePos();

		if( pLine[i].IsEndOfLine() )
		{
			// If this was a WG_HYPHEN_BREAK_PERMITTED that was not rendered we need
			// to render a normal hyphen.

			if( pLine[i].GetGlyph() == WG_HYPHEN_BREAK_PERMITTED && !pPen->SetChar( ch ) && pPen->SetChar('-') )
				pPen->ApplyKerning();

			break;
		}
 	}
	int ofs = pPen->GetBlitPosX();
	pPen->FlushChar();
	return ofs;
}

//____ PrintLine() ________________________________________________________

void WgGfxDevice::PrintLine( WgPen * pPen, const WgTextPropPtr& pDefProp, WgMode mode, const WgChar * _pLine, int maxChars, bool bLineEnding )
{
	if( !_pLine )
		return;

	WgColor baseCol	= m_tintColor;
	WgColor	color	= baseCol;

	Uint16	hProp				= 0xFFFF;		// Setting to impossible value forces setting of properties in first loop.

	pPen->FlushChar();

	// Print loop

	int i;
	for( i = 0 ; i < maxChars && !_pLine[i].IsEndOfLine(); i++ )
 	{
		// Act on possible change of character attributes.

		if( _pLine[i].GetPropHandle() != hProp )
		{
			hProp = _pLine[i].GetPropHandle();

			int success = pPen->SetTextProp( pDefProp.GetHandle(), hProp, mode );
			if( !success )
				return;

			// Set tint colors (if changed)

			if( pPen->GetColor() != color )
			{
				color = pPen->GetColor();
				SetTintColor( baseCol * color );
			}

			// Check if this is start of underlined text with this font and in that case draw the underline.

			if( _pLine[i].IsUnderlined(pDefProp, mode) &&
				(i==0 || !(_pLine[i-1].IsUnderlined(pDefProp, mode)) || _pLine[i-1].GetFont(pDefProp) != _pLine[i].GetFont(pDefProp)) )
				DrawUnderline( WgRect(0,0,65535,65535), pDefProp, mode, pPen->GetPosX(), pPen->GetPosY(), _pLine+i, maxChars-i );

		}

		// Calculate position and blit the glyph.

		Uint16 ch = _pLine[i].GetGlyph();

		if( pPen->SetChar( ch ) )
		{
			pPen->ApplyKerning();

/*			if(selStartX == -1 && i >= iSelStart)
				selStartX = pPen->GetBlitPosX();

			if(selStartX >= 0 && i < iSelEnd)
				selEndX = pPen->GetBlitPosX();
*/
			pPen->BlitChar();
		}

		pPen->AdvancePos();

 	}

	// Render line-endings.

	if( bLineEnding )
	{
		// If character after line-end was a WG_HYPHEN_BREAK_PERMITTED we need
		// to render a normal hyphen.

		if( _pLine[i].GetGlyph() == WG_HYPHEN_BREAK_PERMITTED )
		{
			if( pPen->SetChar( '-' ) )
			{
				pPen->ApplyKerning();
				pPen->BlitChar();
			}
		}

		// TODO: print LF-character if there is one following and properties says it should be displayed.

	}

	// Restore tint color.

	if( m_tintColor != baseCol )
		SetTintColor(baseCol);
}


//____ DrawUnderline() ________________________________________________________

void WgGfxDevice::DrawUnderline( const WgRect& clip, const WgTextPropPtr& pDefProp, WgMode mode, int _x, int _y, const WgChar * pLine, int maxChars )
{
	Uint32 hProp = 0xFFFF;

	WgPen pen;

	for( int i = 0 ; i < maxChars && !pLine[i].IsEndOfLine() ; i++ )
	{
		if( pLine[i].GetPropHandle() != hProp )
		{
			if( pLine[i].IsUnderlined(pDefProp, mode) )
			{
				const WgFont * pFont = pen.GetFont();			// Save font for comparison.

				hProp = pLine[i].GetPropHandle();
				pen.SetTextProp( pDefProp.GetHandle(), hProp, mode );

				// We need to break if font has changed.

				if( pFont != 0 && pen.GetFont() != pFont )
					break;

			}
			else
				break;
		}

		pen.SetChar( pLine[i].GetGlyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
	}

	const WgUnderline * pUnderline = pen.GetFont()->GetUnderline( pen.GetSize() );

	ClipBlitHorrBar( clip, pUnderline->pSurf, pUnderline->rect, WgBorders( pUnderline->leftBorder, pUnderline->rightBorder, 0, 0 ), false,
					_x + pUnderline->bearingX, _y + pUnderline->bearingY, pen.GetPosX() );
}




