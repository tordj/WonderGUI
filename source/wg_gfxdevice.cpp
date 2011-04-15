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
#include <wg_textlink.h>

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
	Blit( pSrc, WgRect( 0, 0, pSrc->Width(), pSrc->Height() ), 0, 0 );
}

void WgGfxDevice::Blit( const WgSurface* pSrc, int dx, int dy )
{
	Blit( pSrc, WgRect( 0, 0, pSrc->Width(), pSrc->Height() ), dx, dy );
}

//____ StretchBlit() ___________________________________________________________

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, bool bTriLinear, float mipmapBias )
{
	StretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->Width(), (float) pSrc->Height(), 0, 0, (float) m_canvasSize.w, (float) m_canvasSize.h, bTriLinear, mipmapBias );
}

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, const WgRect& dest, bool bTriLinear, float mipmapBias )
{
	StretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->Width(), (float) pSrc->Height(), (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
}

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest, bool bTriLinear, float mipmapBias )
{
	StretchBlitSubPixel( pSrc, (float) src.x, (float) src.y, (float) src.w, (float) src.h, (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
}

//____ TileBlit() ______________________________________________________________

void WgGfxDevice::TileBlit( const WgSurface* _pSrc )
{
	TileBlit( _pSrc, WgRect( 0, 0, _pSrc->Width(), _pSrc->Height() ), WgRect(0,0,m_canvasSize) );
}

void WgGfxDevice::TileBlit( const WgSurface* _pSrc, const WgRect& _dest )
{
	TileBlit( _pSrc, WgRect( 0, 0, _pSrc->Width(), _pSrc->Height() ), _dest );
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

	int		destX = _dest.x;
	int		destY = _dest.y;

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
	ClipBlit( clip, pSrc, WgRect(0,0,pSrc->Width(),pSrc->Height()), 0, 0 );
}

void WgGfxDevice::ClipBlit( const WgRect& clip, const WgSurface* pSrc, int dx, int dy  )
{
	ClipBlit( clip, pSrc, WgRect(0,0,pSrc->Width(),pSrc->Height()), dx, dy );
}

void WgGfxDevice::ClipBlit( const WgRect& clip, const WgSurface* pSrc, const WgRect& srcRect, int dx, int dy  )
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
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->Width(), pSrc->Height()), WgRect( 0,0,m_canvasSize), bTriLinear, mipBias );
}

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& dest, bool bTriLinear, float mipBias )
{
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->Width(), pSrc->Height()), dest, bTriLinear, mipBias );
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
	ClipTileBlit( clip, pSrc, WgRect( 0,0,pSrc->Width(),pSrc->Height() ),
				  WgRect(0,0,m_canvasSize) );
}

void WgGfxDevice::ClipTileBlit( const WgRect& clip, const WgSurface* pSrc,
								  const WgRect& dest )
{
	ClipTileBlit( clip, pSrc, WgRect(0,0,pSrc->Width(),pSrc->Height()), dest );
}


void WgGfxDevice::ClipTileBlit( const WgRect& _clip, const WgSurface* _pSrc, const WgRect& _src, const WgRect& _dest )
{
	if( !_pSrc )
		return;

	WgRect	myRect;

	WgRect	clip;
	if( !clip.intersection( _dest, _clip ) )
		return;

	// Take care of start-offset change caused by clipping.

	int		xStart = (clip.x - _dest.x) % _src.w;
	if( xStart < 0 )
		xStart += _src.w;
	xStart += _src.x;

	int		yStart = (clip.y - _dest.y) % _src.h;
	if( yStart < 0 )
		yStart += _src.h;
	yStart += _src.y;


	int		destY = clip.y, destX;

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

	if( _dest.h > (int) borders.height() )
	{
		if( borders.left > 0 )
		{
			WgRect sr( src.x, src.y + borders.top, borders.left, src.h - borders.height() );
			WgRect dr( _dest.x, _dest.y + borders.top, borders.left, _dest.h - borders.height() );

			if( _block.HasTiledLeftBorder() )
				TileBlit( pSurf, sr, dr );
			else
				StretchBlit( pSurf, sr, dr );
		}

		if( borders.right > 0 )
		{
			WgRect sr(	src.x + src.w - borders.right, src.y + borders.top,
						borders.right, src.h - borders.height() );
			WgRect dr(	_dest.x + _dest.w - borders.right, _dest.y + borders.top,
						borders.right, _dest.h - borders.height() );

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
					src.w - borders.width(), src.h - borders.height() );

		WgRect dr(	_dest.x + borders.left, _dest.y + borders.top,
					_dest.w - borders.width(), _dest.h - borders.height() );

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

	if( _clip.contains( _dest2 ) )
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

	if( _dest.h > (int) borders.height() )
	{
		if( borders.left > 0 )
		{
			WgRect sr( src.x, src.y + borders.top, borders.left, src.h - borders.height() );
			WgRect dr( _dest.x, _dest.y + borders.top, borders.left, _dest.h - borders.height() );

			if( _block.HasTiledLeftBorder() )
				ClipTileBlit( _clip, pSurf, sr, dr );
			else
				ClipStretchBlit( _clip, pSurf, sr, dr );
		}

		if( borders.right > 0 )
		{
			WgRect sr(	src.x + src.w - borders.right, src.y + borders.top,
						borders.right, src.h - borders.height() );
			WgRect dr(	_dest.x + _dest.w - borders.right, _dest.y + borders.top,
						borders.right, _dest.h - borders.height() );

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
					src.w - borders.width(), src.h - borders.height() );

		WgRect dr(	_dest.x + borders.left, _dest.y + borders.top,
					_dest.w - borders.width(), _dest.h - borders.height() );

		if( _block.HasTiledCenter() )
			ClipTileBlit( _clip, pSurf, sr, dr );
		else
			ClipStretchBlit( _clip, pSurf, sr, dr, bTriLinear, mipmapbias );
	}


}


//____ ClipBlitHorrBar() ______________________________________________________

void WgGfxDevice::ClipBlitHorrBar(	const WgRect& _clip, const WgSurface * _pSurf,
								  	const WgRect& _src, const WgBorders& _borders,
								  	bool _bTile, int _dx, int _dy, int _len )
{
	/*
		This can be optimized by handling clipping directly instead of calling clipBlit().
	*/

	// Blit left edge

	WgRect	r( _src.x, _src.y, _borders.left, _src.h );
	ClipBlit( _clip, _pSurf, r, _dx, _dy );

	_len -= _borders.width();			// Remove left and right edges from len.
	_dx += _borders.left;

	// Blit tiling part

	r.x += _borders.left;
	r.w = _src.w - _borders.width();

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
								  	bool _bTile, int _dx, int _dy, int _len )
{
	/*
		This can be optimized by handling clipping directly instead of calling clipBlit().
	*/

	// Blit top edge

	WgRect	r( _src.x, _src.y, _src.w, _borders.top );
	ClipBlit( _clip, _pSurf, r, _dx, _dy );

	_len -= _borders.height();			// Remove top and bottom edges from len.
	_dy += _borders.top;

	// Blit tiling part

	r.y += _borders.top;
	r.h = _src.h - _borders.height();

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
								int _dx, int _dy, int _len )
{
	// Blit left edge

	WgRect	r( _src.x, _src.y, _borders.left, _src.h );
	Blit( _pSurf, r, _dx, _dy );

	_len -= _borders.width();			// Remove left and right edges from len.
	_dx += _borders.left;

	// Blit tiling part

	r.x += _borders.left;
	r.w = _src.w - _borders.width();

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
								int _dx, int _dy, int _len )
{
	// Blit top edge

	WgRect	r( _src.x, _src.y, _src.w, _borders.top );
	Blit( _pSurf, r, _dx, _dy );

	_len -= _borders.height();			// Remove top and bottom borders from len.
	_dy += _borders.top;

	// Blit tiling part

	r.y += _borders.top;
	r.h = _src.h - _borders.height();

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

//____ PrintText() ____________________________________________________________

void WgGfxDevice::PrintText( const WgRect& clip, const WgText * pText, const WgRect& dest )
{
	DrawTextBg(clip, pText, dest);

	if( !pText || !pText->getFont()  )
		return;

	WgPen pen;
	pen.SetDevice( this );
	pen.SetTextNode( pText->getNode() );

	if( dest.h < (int) pText->height() || dest.w < (int) pText->width() || !clip.contains( dest ) )
		pen.SetClipRect( clip );

	pen.SetTextAttr( pText->GetAttr() );

	const WgCursorInstance* pCursor = 0;
	if( pText->isCursorShowing() )
		pCursor = pText->GetCursor();

	Uint32 cursLine = -1, cursCol = -1;
	if(pCursor)
		pCursor->getSoftPos( cursLine, cursCol );

	WgCord	pos;
	pos.y = pText->ScreenY( dest ) + pen.GetBaseline();

	Uint32				nLines = pText->nbSoftLines();
	const WgTextLine *	pLines = pText->getSoftLines();
	const WgChar *		pChars = pText->getText();

	for( unsigned int i = 0 ; i < nLines ; i++ )
	{
		pos.x = pText->ScreenX( i, dest );
		pen.SetOrigo( pos );		// So tab positions will start counting from start of line.
		pen.SetPos( pos );

		if( cursLine == i )
		{
			// Draw line parts, make space for cursor.

			PrintLine( &pen, pText->GetAttr(), pChars + pLines[i].ofs, cursCol, false, pText->GetMarkedLink(), pText->GetMarkedLinkMode() );
			WgCord cursorPos = pen.GetPos();
			pen.AdvancePosCursor( *pCursor );
			PrintLine( &pen, pText->GetAttr(), pChars + pLines[i].ofs + cursCol, pLines[i].nChars - cursCol, true, pText->GetMarkedLink(), pText->GetMarkedLinkMode() );

			// Blit the cursor

			WgCord restorePos = pen.GetPos();
			pen.SetPos( cursorPos );
			pen.BlitCursor( *pCursor );
			pen.SetPos( restorePos );
		}
		else
		{
			PrintLine( &pen, pText->GetAttr(), pChars + pLines[i].ofs, pLines[i].nChars, true, pText->GetMarkedLink(), pText->GetMarkedLinkMode() );
		}

		pos.y += pLines[i].lineSpacing;
	}
}

//____ DrawTextBg() ___________________________________________________________

void WgGfxDevice::DrawTextBg( const WgRect& clip, const WgText * pText, const WgRect& dest )
{
	WgRectChain	bgRects;
	WgColor		bgColor;
	bool		bBgColored;

	WgMode mode = pText->mode();

	// First take care of general background color

	WgTextPropPtr pDefProp = pText->getProperties();

	if( pDefProp->IsBgColored( mode ) )
	{
		bBgColored = true;
		bgColor = pDefProp->GetBgColor( mode );
		bgRects.Add( clip );
	}

	// Draw blocks of general background (should be done last once we cut out the other rectangles)

	WgRectLink * pBgRect = bgRects.pRectList;
	while( pBgRect )
	{
		Fill( *pBgRect, bgColor );
		pBgRect = pBgRect->pNext;
	}

	// Take care of selection background color (if we have any)

	int selStart, selEnd;

	Uint32 startLine, startCol, endLine, endCol;
	pText->getSelection( startLine, startCol, endLine, endCol );
	selStart = pText->LineColToOffset(startLine, startCol);
	selEnd = pText->LineColToOffset(endLine,endCol);

	WgTextPropPtr pSelProp = WgTextTool::GetSelectionProperties(pText);

	if( selStart != selEnd && pSelProp->IsBgColored() )
	{
		DrawTextSectionBg( clip, pText, dest, selStart, selEnd, pSelProp->GetBgColor(mode) );
	}
	else
	{
		selStart = -1;
		selEnd = -1;
	}

	// Scan through the text, drawing character specific backgrounds 
	// (and in the future punching holes in the general background?)

	Uint16	hProp = 0;
	WgColor	color = bgColor;
	int		startOfs = 0;

	const WgChar * pChars = pText->getBuffer()->Chars();
	int nChars = pText->getBuffer()->Length();

	for( int ofs = 0 ; ofs < nChars ; ofs++ )
	{
		if( ofs == selStart )
		{
			if( color != bgColor )
				DrawTextSectionBg( clip, pText, dest, startOfs, ofs, color );

			startOfs = selEnd;
			ofs = startOfs-1;
		}

		if( pChars[ofs].PropHandle() != hProp )
		{
			hProp = pChars[ofs].PropHandle();

			WgMode		linkMode = WG_MODE_NORMAL;
			WgTextLinkPtr pLink = WgTextTool::GetCharLink( hProp, pText->GetAttr() );
			if( pLink && pLink == pText->GetMarkedLink() )
				linkMode = pText->GetMarkedLinkMode();

			WgColor newColor = WgTextTool::GetCharBgColor( hProp, pText->GetAttr(), linkMode );

			if( ofs != startOfs && newColor != color && color != bgColor )
			{
				DrawTextSectionBg( clip, pText, dest, startOfs, ofs, color );
				hProp = pChars[ofs].PropHandle();
				color = newColor;
				startOfs = ofs;
			}
		}
	}

	// Draw last background section if it is colored

	if( startOfs != nChars && color != bgColor )
		DrawTextSectionBg( clip, pText, dest, startOfs, nChars, color );
}

/*
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

	WgColor col = pText->getSelectionBgColor();

	int lineH = pPen->GetLineSpacing() + pText->lineSpaceAdjustment();
	if(iSelStartLine == iSelEndLine)
	{
		r.x = dstPos.x + xs + LineAlignmentToOffset( pText, iSelStartLine, dstRect );
		r.y = dstPos.y + iSelStartLine * lineH;
		r.w = xe - xs;
		r.h = lineH;
		ClipFill(clip, r, col);
	}
	else
	{
		r.x = dstPos.x + xs  + LineAlignmentToOffset( pText, iSelStartLine, dstRect );
		r.y = dstPos.y + iSelStartLine * lineH;
		r.w = pText->getSoftLineSelectionWidth(iSelStartLine) - xs;
		r.h = lineH;
		ClipFill(clip, r, col);

		++iSelStartLine;
		for(; iSelStartLine < iSelEndLine; ++iSelStartLine)
		{
			r.x = dstPos.x  + LineAlignmentToOffset( pText, iSelStartLine, dstRect );
			r.y += lineH;
			r.w = pText->getSoftLineSelectionWidth(iSelStartLine);
			r.h = lineH;
			ClipFill(clip, r, col);
		}

		r.x = dstPos.x  + LineAlignmentToOffset( pText, iSelStartLine, dstRect );
		r.y = r.y + r.h;
		r.w = xe;
		r.h = lineH;
		ClipFill(clip, r, col);
	}


}
*/

//___________________________________________________________________________________________________
void WgGfxDevice::DrawTextSectionBg( const WgRect& clip, const WgText * pText, const WgRect& dstRect, 
									  int iStartOfs, int iEndOfs, WgColor color )
{
	const WgTextLine *	pLines = pText->getSoftLines();
	const WgChar *		pChars = pText->getText();

	int iStartLine, iStartCol;
	int iEndLine, iEndCol;

	pText->OffsetToSoftLineCol( iStartOfs, &iStartLine, &iStartCol );
	pText->OffsetToSoftLineCol( iEndOfs, &iEndLine, &iEndCol );
	
	WgPen pen;
	pen.SetTextNode( pText->getNode() );
	pen.SetTextAttr( pText->GetAttr() );

	int xs = CalcCharOffset(&pen, pChars + pLines[iStartLine].ofs, iStartCol );
	int xe = CalcCharOffset(&pen, pChars + pLines[iEndLine].ofs, iEndCol );

	int dstPosY = pText->ScreenY( dstRect );

	WgRect r;

	if(iStartLine == iEndLine)
	{
		r.x = pText->ScreenX( iStartLine, dstRect ) + xs;
		r.y = dstPosY + pText->getLineOfsY(iStartLine);
		r.w = xe - xs;
		r.h = pLines[iStartLine].height;
		ClipFill(clip, r, color);
	}
	else
	{
		r.x = pText->ScreenX( iStartLine, dstRect ) + xs;
		r.y = dstPosY + pText->getLineOfsY(iStartLine);
		r.w = pText->getSoftLineSelectionWidth(iStartLine) - xs;
		r.h = pLines[iStartLine].height;
		ClipFill(clip, r, color);
		r.y += pLines[iStartLine].lineSpacing;

		++iStartLine;
		for(; iStartLine < iEndLine; ++iStartLine)
		{
			r.x = pText->ScreenX( iStartLine, dstRect );
			r.w = pText->getSoftLineSelectionWidth(iStartLine);
			r.h = pLines[iStartLine].height;
			ClipFill(clip, r, color);
			r.y += pLines[iStartLine].lineSpacing;
		}

		r.x = pText->ScreenX( iStartLine, dstRect );
		r.w = xe;
		r.h = pLines[iStartLine].height;
		ClipFill(clip, r, color);
	}
}

//_________________________________________________________________________

int WgGfxDevice::CalcCharOffset(WgPen *pPen, const WgChar* pLine, Uint32 nChars )
{
	if( !pLine )
		return 0;

	Uint16	hProp = 0xFFFF;

	pPen->FlushChar();
	pPen->SetPos(WgCord(0, 0));

 	for( Uint32 i = 0; i < nChars; i++ )
 	{
		// Act on possible change of character attributes.

		if( pLine[i].PropHandle() != hProp )
		{
			hProp = pLine[i].PropHandle();

			int success = pPen->SetCharProp( hProp );
			if( !success )
				break;
		}

		// Calculate position.
		Uint16 ch = pLine[i].Glyph();

		if( pPen->SetChar( ch ) )
			pPen->ApplyKerning();

		pPen->AdvancePos();

		if( pLine[i].IsEndOfLine() )
		{
			// If this was a WG_HYPHEN_BREAK_PERMITTED that was not rendered we need
			// to render a normal hyphen.

			if( pLine[i].Glyph() == WG_HYPHEN_BREAK_PERMITTED && !pPen->SetChar( ch ) && pPen->SetChar('-') )
				pPen->ApplyKerning();

			break;
		}
 	}
	int ofs = pPen->GetPosX();
	pPen->FlushChar();
	return ofs;
}

//____ PrintLine() ________________________________________________________

void WgGfxDevice::PrintLine( WgPen * pPen, const WgTextAttr * pAttr, const WgChar * _pLine, int maxChars, bool bLineEnding, 
							 const WgTextLinkPtr pMarkedLink, WgMode markedLinkMode )
{
	if( !_pLine )
		return;

	WgColor baseCol	= m_tintColor;
	WgColor	color	= baseCol;

	Uint16	hProp				= 0xFFFF;		// Setting to impossible value forces setting of properties in first loop.

	bool	bSelected	= false;
	WgMode	linkMode	= WG_MODE_NORMAL;

	pPen->FlushChar();
	pPen->SetAllProps(0, pAttr, linkMode, bSelected );

	// Print loop

	int i;
	for( i = 0 ; i < maxChars && !_pLine[i].IsEndOfLine(); i++ )
 	{
		// Act on possible change of character attributes.

		if( _pLine[i].PropHandle() != hProp )
		{
			hProp = _pLine[i].PropHandle();

			int success = pPen->SetCharProp( hProp );
			if( !success )
				return;

			// Update link-mode if this is beginning of a link.

			WgTextLinkPtr pLink = WgTextTool::GetCharLink( hProp, pAttr );

			if( pLink )
			{
				if( pLink == pMarkedLink )
					linkMode = markedLinkMode;
				else
					linkMode = WG_MODE_NORMAL;

				pPen->SetLinkMode(linkMode);
			}


			// Set tint colors (if changed)

			if( pPen->GetColor() != color )
			{
				color = pPen->GetColor();
				SetTintColor( baseCol * color );
			}

			// Check if this is start of underlined text and in that case draw the underline.

			if( WgTextTool::IsCharUnderlined( _pLine[i].PropHandle(), pAttr, linkMode, bSelected ) &&
				(i==0 || !WgTextTool::IsCharUnderlined( _pLine[i-1].PropHandle(), pAttr, linkMode, bSelected )) )
			{
				WgRect clip = pPen->HasClipRect()?pPen->GetClipRect():WgRect(0,0,65535,65535);
				DrawUnderline( clip, pAttr, linkMode, bSelected, pPen->GetPosX(), pPen->GetPosY(), _pLine+i, maxChars-i );
			}

		}

		// Calculate position and blit the glyph.

		Uint16 ch = _pLine[i].Glyph();

		bool bBlit = pPen->SetChar( ch );
		pPen->ApplyKerning();
		if( bBlit )
		{

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

		if( _pLine[i].Glyph() == WG_HYPHEN_BREAK_PERMITTED )
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

void WgGfxDevice::DrawUnderline( const WgRect& clip, const WgTextAttr * pAttr, WgMode linkMode, bool bSelected, int _x, int _y, const WgChar * pLine, int maxChars )
{
	Uint32 hProp = 0xFFFF;

	WgPen pen;

	for( int i = 0 ; i < maxChars && !pLine[i].IsEndOfLine() ; i++ )
	{
		if( pLine[i].PropHandle() != hProp )
		{
			if( WgTextTool::IsCharUnderlined( pLine[i].PropHandle(), pAttr, linkMode, bSelected ) )
			{
				const WgFont * pFont = pen.GetFont();			// Save font for comparison.

				hProp = pLine[i].PropHandle();
				pen.SetAllProps( hProp, pAttr, linkMode, bSelected );
			}
			else
				break;
		}

		pen.SetChar( pLine[i].Glyph() );
		pen.ApplyKerning();
		pen.AdvancePos();
	}

	const WgUnderline * pUnderline = pen.GetFont()->GetUnderline( pen.GetSize() );

	ClipBlitHorrBar( clip, pUnderline->pSurf, pUnderline->rect, WgBorders( pUnderline->leftBorder, pUnderline->rightBorder, 0, 0 ), false,
					_x + pUnderline->bearingX, _y + pUnderline->bearingY, pen.GetPosX() );
}




