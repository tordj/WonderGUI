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

void WgGfxDevice::StretchBlit( const WgSurface * pSrc )
{
	StretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->GetWidth(), (float) pSrc->GetHeight(), 0, 0, (float) m_canvasWidth, (float) m_canvasHeight );
}

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, const WgRect& dest )
{
	StretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->GetWidth(), (float) pSrc->GetHeight(), (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h );
}

void WgGfxDevice::StretchBlit( const WgSurface * pSrc, const WgRect& src, const WgRect& dest )
{
	StretchBlitSubPixel( pSrc, (float) src.x, (float) src.y, (float) src.w, (float) src.h, (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h );
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

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc )
{
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->GetWidth(), pSrc->GetHeight()),
					 WgRect( 0,0,m_canvasWidth,m_canvasHeight) );
}

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc,
								   const WgRect& dest )
{
	ClipStretchBlit( clip, pSrc, WgRect(0,0,pSrc->GetWidth(), pSrc->GetHeight()), dest );
}

void WgGfxDevice::ClipStretchBlit( const WgRect& clip, const WgSurface * pSrc, const WgRect& src, const WgRect& dest )
{
	WgRect	mydest;

	if( !mydest.Intersection(clip,dest) )
		return;

	float	sx = (float) src.x;
	float	sy = (float) src.y;
	float	sw = (float) src.w;
	float	sh = (float) src.h;

	if( dest.w > mydest.w )
	{
		float	sdxr = src.w / (float) dest.w;			// Source/Destination X Ratio.

		sw = sdxr * mydest.w;

		if( dest.x < mydest.x )
			sx += sdxr * (mydest.x - dest.x);
	}

	if( dest.h > mydest.h )
	{
		float	sdyr = src.h / (float) dest.h;			// Source/Destination Y Ratio.

		sh = sdyr * mydest.h;

		if( dest.y < mydest.y )
			sy += sdyr * (mydest.y - dest.y);
	}

	StretchBlitSubPixel( pSrc, sx, sy, sw, sh, (float) mydest.x, (float) mydest.y, (float) mydest.w, (float) mydest.h );
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

void WgGfxDevice::BlitBlock( const WgBlock& _block, const WgRect& _dest2 )
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
			StretchBlit( pSurf, src, _dest );
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
			StretchBlit( pSurf, sr, dr );
	}


}


//____ ClipBlitBlock() ________________________________________________________

void WgGfxDevice::ClipBlitBlock( const WgRect& _clip, const WgBlock& _block,
								 const WgRect& _dest2 )
{
	if( !_block.GetSurface() )
		return;

	if( _block.IsSkipable() )
		return;

	// Shortcuts & optimizations for common special cases.

	if( _clip.Contains( _dest2 ) )
	{
		BlitBlock( _block, _dest2 );
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
			ClipStretchBlit( _clip, pSurf, src, _dest );
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
			ClipStretchBlit( _clip, pSurf, sr, dr );
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

//____ PrintTextWithCursor() __________________________________________________

void WgGfxDevice::PrintTextWithCursor( const WgText * pText, const WgCursorInstance& ci, const WgRect& dest )
{
	if( !pText || !pText->getFontSet() || !pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL) )
		return;

	const WgTextPropPtr	pDefProp = pText->getDefaultProperties();

	const WgOrigo& origo = pText->alignment();

	int glyphheight = pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL)->height();		//TODO: We need something WAY BETTER here!!!

	int	linespacing = glyphheight + pText->lineSpaceAdjustment();
	if( linespacing < 0 )
		linespacing = 0;
	int textheight = glyphheight + ( linespacing*(pText->nbSoftLines()-1) );

	int yPos = (int) dest.y + origo.calcOfsY( dest.h, textheight );

	const WgTextLine *	p1	= pText->getSoftLines();
	Uint32				n1	= pText->nbSoftLines();

	Uint32		cursLine, cursCol;
	ci.getSoftPos( cursLine, cursCol );

	for( Uint32 i = 0 ; i < (int) n1 ; i++ )
	{
		int linewidth = WgTextTool::lineWidthSoft( pDefProp, pText->mode(), p1[i].pText );
		if( cursLine == i )
		{
			// Determine what cursor to use and retrieve some of its properties

			int cursStyleOfs = cursCol;
			if( cursCol > 0 )
				cursStyleOfs--;				// Get cursor style from previous character if possible.
			WgFont * pFont = WgTextTool::GetCombFont( pDefProp.GetHandle(), p1[i].pText[cursStyleOfs].GetPropHandle() );
			WgCursor *		cursor		= pFont->GetCursor();

			WgCursor::Mode	cursMode 	= ci.mode();
			int				cursSpacing = cursor->spacing(cursMode);

			// Calculate cordinates for of text and cursor.

			linewidth += cursSpacing;
			int xPos = (int) (dest.x + dest.w * origo.anchorX()
								- linewidth * origo.hotspotX());
			int 			cursX 		= xPos + WgTextTool::lineWidthPartSoft( pDefProp, pText->mode(), p1[i].pText, cursCol );

			// Make sure cursor doesn't end up too much left of text so it is clipped.

			if( cursCol == 0 && cursor->ofsX(cursMode) < 0 )
				cursX -= cursor->ofsX(cursMode);

			//

			PrintLine( pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText, cursCol );
			PrintLine( pDefProp, pText->mode(), xPos, cursX + cursSpacing, yPos, p1[i].pText + cursCol  );

			WgGfxAnim * pAnim	= cursor->anim(cursMode);
			WgGfxFrame * pFrame = pAnim->getFrame( ci.time(), 0 );

			Blit( pFrame->pSurf, WgRect( pFrame->ofs.x, pFrame->ofs.y, pAnim->width(), pAnim->height() ), cursX + cursor->ofsX(cursMode), yPos + cursor->ofsY(cursMode) );

		}
		else
		{
			int xPos = (int) (dest.x + (dest.w - linewidth) * origo.anchorX()
								- linewidth * origo.hotspotX());

			PrintLine( pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText );
		}

		yPos += linespacing;
	}
}


//____ PrintText() ____________________________________________________________

void WgGfxDevice::PrintText( const WgText * pText, const WgRect& dest )
{
	if( !pText || !pText->getFontSet() || !pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL) )
		return;

	const WgTextPropPtr	pDefProp = pText->getDefaultProperties();

	const WgOrigo& origo	= pText->alignment();

	int glyphheight = pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL)->height();		//TODO: We need something WAY BETTER here!!!
	int	linespacing = glyphheight + pText->lineSpaceAdjustment();
	if( linespacing < 0 )
		linespacing = 0;
	int textheight = glyphheight + ( linespacing*(pText->nbLines()-1) );

	int yPos = (int) (dest.y + dest.h * origo.anchorY()
							- textheight * origo.hotspotY());

	Uint32				n1 = pText->nbSoftLines();
	const WgTextLine *	p1 = pText->getSoftLines();

	for( int i = 0 ; i < (int) n1 ; i++ )
	{
		int linewidth = WgTextTool::lineWidthSoft( pDefProp, pText->mode(), p1[i].pText );
		int xPos = (int) (dest.x + dest.w * origo.anchorX()
								- linewidth * origo.hotspotX());

		PrintLine( pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText );
		yPos += linespacing;
	}
}


//____ ClipPrintTextWithCursor() ______________________________________________

void WgGfxDevice::ClipPrintTextWithCursor( const WgRect& clip, const WgText * pText,
									const WgCursorInstance& ci, const WgRect& dest )
{
	if( !pText || !pText->getFontSet() || !pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL) )
		return;

	const WgTextPropPtr	pDefProp = pText->getDefaultProperties();

	const WgOrigo& origo = pText->alignment();

	int glyphheight = pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL)->height();		//TODO: We need something WAY BETTER here!!!
	int	linespacing = glyphheight + pText->lineSpaceAdjustment();
	if( linespacing < 0 )
		linespacing = 0;
	int textheight = glyphheight + ( linespacing*(pText->nbSoftLines()-1) );

	int yPos = (int) (dest.y + dest.h * origo.anchorY()
							- textheight * origo.hotspotY());

	Uint32 				n1 = pText->nbSoftLines();
	const WgTextLine *	p1 = pText->getSoftLines();

	Uint32		cursLine, cursCol;
	ci.getSoftPos( cursLine, cursCol );


	for( Uint32 i = 0 ; i < n1 ; i++ )
	{
		int linewidth = WgTextTool::lineWidthSoft( pDefProp, pText->mode(), p1[i].pText );
		if( cursLine == i )
		{
			// Determine what cursor to use and retrieve some of its properties

			int cursStyleOfs = cursCol;
			if( cursCol > 0 )
				cursStyleOfs--;				// Get cursor style from previous character if possible.
			WgFont * pFont = WgTextTool::GetCombFont( pDefProp.GetHandle(), p1[i].pText[cursStyleOfs].GetPropHandle() );
			WgCursor *		cursor		= pFont->GetCursor();

			WgCursor::Mode	cursMode 	= ci.mode();
			int				cursSpacing = cursor->spacing(cursMode);

			// Calculate cordinates for text and cursor.

			linewidth += cursSpacing;
			int xPos = (int) (dest.x + dest.w * origo.anchorX()
								- linewidth * origo.hotspotX());
			int 			cursX 		= xPos + WgTextTool::lineWidthPartSoft( pDefProp, pText->mode(), p1[i].pText, cursCol );

			// Make sure cursor doesn't end up too much left of text so it is clipped.

			if( cursCol == 0 && cursor->ofsX(cursMode) < 0 )
				cursX -= cursor->ofsX(cursMode);

			//

			ClipPrintLine( clip, pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText, cursCol );
			ClipPrintLine( clip, pDefProp, pText->mode(), xPos, cursX + cursSpacing, yPos, p1[i].pText + cursCol  );

			WgGfxAnim * pAnim	= cursor->anim(cursMode);
			WgGfxFrame * pFrame = pAnim->getFrame( ci.time(), 0 );

			ClipBlit( clip, pFrame->pSurf, WgRect( pFrame->ofs.x, pFrame->ofs.y, pAnim->width(), pAnim->height() ), cursX + cursor->ofsX(cursMode), yPos + cursor->ofsY(cursMode) );

		}
		else
		{
			int xPos = (int) (dest.x + (dest.w - linewidth) * origo.anchorX()
								- linewidth * origo.hotspotX());

			// printLine without clipping if we can, otherwise use clipPrintLine()

			if( clip.x <= xPos && clip.y <= yPos
				&& clip.x + clip.w >= xPos+linewidth
				&& clip.y + clip.h >= yPos+glyphheight )
				PrintLine( pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText );
			else
				ClipPrintLine( clip, pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText );
		}

		yPos += linespacing;
	}
}


//____ ClipPrintText() ________________________________________________________

void WgGfxDevice::ClipPrintText( const WgRect& clip, const WgText * pText, const WgRect& dest )
{
	if( !pText || !pText->getFontSet() || !pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL) )
		return;

	const WgTextPropPtr	pDefProp = pText->getDefaultProperties();

	const WgOrigo& origo	= pText->alignment();

	int glyphheight = pText->getFontSet()->GetGlyphSet(WG_STYLE_NORMAL)->height();		//TODO: We need something WAY BETTER here!!!
	int	linespacing = glyphheight + pText->lineSpaceAdjustment();
	if( linespacing < 0 )
		linespacing = 0;
	int textheight = glyphheight + ( linespacing*(pText->nbSoftLines()-1) );

	int yPos = (int) (dest.y + dest.h * origo.anchorY()
							- textheight * origo.hotspotY());

	Uint32				n1 = pText->nbSoftLines();
	const WgTextLine *	p1 = pText->getSoftLines();

	for( int i = 0 ; i < (int) n1 ; i++ )
	{
		int linewidth = WgTextTool::lineWidthSoft( pDefProp, pText->mode(), p1[i].pText );
		int xPos = (int) (dest.x + dest.w * origo.anchorX()
								- linewidth * origo.hotspotX());

		// printLine without clipping if we can, otherwise use clipPrintLine()

		if( clip.x <= xPos && clip.y <= yPos
			&& clip.x + clip.w >= xPos+linewidth
			&& clip.y + clip.h >= yPos+glyphheight )
			PrintLine( pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText );
		else
			ClipPrintLine( clip, pDefProp, pText->mode(), xPos, xPos, yPos, p1[i].pText );

		yPos += linespacing;
	}
}


//____ PrintLine() ____________________________________________________________

void WgGfxDevice::PrintLine( const WgTextPropPtr& pDefProp,
							 WgMode mode, int _tabOrigo, int _x, int _y,
							 const WgChar * _pLine, Uint32 nChars )
{
	if( !_pLine )
		return;

	WgColor baseCol	= m_tintColor;
	WgColor	oldCol	= baseCol;

	const WgGlyphSet *	pFont	= 0;
 	const WgGlyph*	pLastGlyph	= 0;

	Uint16	hProp				= 0xFFFF;		// Setting to impossible value forces setting of properties in first loop.


	// Print loop

 	for( Uint32 i = 0 ; i < nChars ; i++ )
 	{

		// Handle various kinds of linebreaks

		if( _pLine[i].IsEndOfLine() )
		{
			if( _pLine[i].isSoftBreakWithHyphen() )
			{
				// We need to render a hyphen, using whatever color and
				// attributes previous character had, then end the loop.

				if( pFont )
				{
					const WgGlyph * pGlyph = pFont->glyph( '-' );

					_x += pFont->kerning( pLastGlyph, pGlyph );
					Blit( pGlyph->pSurf, pGlyph->rect, _x + pFont->bearingX( '-' ), _y + pFont->bearingY( '-' ) );
				}
				break;
			}
			else if( _pLine[i].isSoftBreak() && !_pLine[i].IsBreakPermitted() )
			{
				// We have a forced soft-break, we need to render the character of the cell.
				// Easiest way is just to go through loop once more and then stop.

				nChars = i + 1;
			}
			else
			{
				// We have a line break where nothing extra needs to be rendered, so let's
				// just break the loop.

				break;
			}
		}


		// Act on possible change of character attributes.

		if( _pLine[i].GetPropHandle() != hProp )
		{
			hProp = _pLine[i].GetPropHandle();

			// Set GlyphSet pointer

			pFont = _pLine[i].GetGlyphSet(pDefProp, mode);
			if( !pFont )
				return;										// Failsafe...

			// Set tint colors (if changed)

			WgColor newCol = WgTextTool::GetCombColor( pDefProp.GetHandle(), _pLine[i].GetPropHandle(), mode );

			if( newCol.argb != oldCol.argb )
			{
				SetTintColor( baseCol * newCol );
				oldCol = newCol;
			}

			// Check if this is start of underlined text and in that case draw the underline.

			if( _pLine[i].IsUnderlined(pDefProp, mode) && (i==0 || !_pLine[i-1].IsUnderlined(pDefProp, mode)) )
				DrawUnderline( WgRect(0,0,65535,65535), pDefProp, mode, _x, _y, _pLine+i );

		}


		// Get glyph and check for special cases.

		Uint16 ch = _pLine[i].GetGlyph();

		if( ch == WG_BREAK_PERMITTED )
		{
			if( !WgTextTool::GetCombCharVisibility( WG_BREAK_PERMITTED, pDefProp.GetHandle(), hProp ) )
				continue;

			//TODO: Should change background color to show this is a special character.
		}

		if( ch == WG_HYPHEN_BREAK_PERMITTED )
		{
			if( !WgTextTool::GetCombCharVisibility( WG_HYPHEN_BREAK_PERMITTED, pDefProp.GetHandle(), hProp ) )
				continue;

			ch = '-';			// Character to be displayed is a normal hyphen.

			//TODO: Should change background color to show this is a special character.
		}

		if( ch == '\t' )
		{
			int tabWidth = 80;
			_x += tabWidth;
			_x -= (_x - _tabOrigo) % tabWidth;

			pLastGlyph = pFont->glyph( ch );
			continue;

			//TODO: Should display a tab character in the middle of the space if set to visible.
		}

		// Calculate position and blit the glyph.

		const WgGlyph * pGlyph = pFont->glyph( ch );

		_x += pFont->kerning( pLastGlyph, pGlyph );

		int renderX = _x + pFont->bearingX( pGlyph );
		int renderY = _y + pFont->bearingY( pGlyph );

		if( !_pLine[i].IsWhitespace() )
			Blit( pGlyph->pSurf, pGlyph->rect, renderX, renderY );

		_x += pFont->advance( pGlyph );

		pLastGlyph = pGlyph;
 	}

	// Restore tint color.

	if( m_tintColor != baseCol )
		SetTintColor(baseCol);
}


//____ ClipPrintLine() ________________________________________________________

void WgGfxDevice::ClipPrintLine( const WgRect& clip, const WgTextPropPtr& pDefProp,
								 WgMode mode, int _tabOrigo, int _x, int _y,
								 const WgChar * _pLine, Uint32 nChars )
{
	if( !_pLine )
		return;

	WgColor baseCol	= m_tintColor;
	WgColor	oldCol	= baseCol;

	const WgGlyphSet *	pFont	= 0;
 	const WgGlyph*	pLastGlyph	= 0;

	Uint16	hProp				= 0xFFFF;		// Setting to impossible value forces setting of properties in first loop.


	// Print loop

 	for( Uint32 i = 0 ; i < nChars ; i++ )
 	{

		// Handle various kinds of linebreaks

		if( _pLine[i].IsEndOfLine() )
		{
			if( _pLine[i].isSoftBreakWithHyphen() )
			{
				// We need to render a hyphen, using whatever color and
				// attributes previous character had, then end the loop.

				if( pFont )
				{
					const WgGlyph * pGlyph = pFont->glyph( '-' );

					_x += pFont->kerning( pLastGlyph, pGlyph );
					ClipBlit( clip, pGlyph->pSurf, pGlyph->rect, _x + pFont->bearingX( '-' ), _y + pFont->bearingY( '-' ) );
				}
				break;
			}
			else if( _pLine[i].isSoftBreak() && !_pLine[i].IsBreakPermitted() )
			{
				// We have a forced soft-break, we need to render the character of the cell.
				// Easiest way is just to go through loop once more and then stop.

				nChars = i + 1;
			}
			else
			{
				// We have a line break where nothing extra needs to be rendered, so let's
				// just break the loop.

				break;
			}
		}


		// Act on possible change of character attributes.

		if( _pLine[i].GetPropHandle() != hProp )
		{
			hProp = _pLine[i].GetPropHandle();

			// Set GlyphSet pointer

			pFont = _pLine[i].GetGlyphSet(pDefProp, mode);
			if( !pFont )
				return;										// Failsafe...

			// Set tint colors (if changed)

			WgColor newCol = WgTextTool::GetCombColor( pDefProp.GetHandle(), _pLine[i].GetPropHandle(), mode );

			if( newCol.argb != oldCol.argb )
			{
				SetTintColor( baseCol * newCol );
				oldCol = newCol;
			}

			// Check if this is start of underlined text and in that case draw the underline.

			if( _pLine[i].IsUnderlined(pDefProp, mode) && (i==0 || !_pLine[i-1].IsUnderlined(pDefProp, mode)) )
				DrawUnderline( WgRect(0,0,65535,65535), pDefProp, mode, _x, _y, _pLine+i );

		}


		// Get glyph and check for special cases.

		Uint16 ch = _pLine[i].GetGlyph();

		if( ch == WG_BREAK_PERMITTED )
		{
			if( !WgTextTool::GetCombCharVisibility( WG_BREAK_PERMITTED, pDefProp.GetHandle(), hProp ) )
				continue;

			//TODO: Should change background color to show this is a special character.
		}

		if( ch == WG_HYPHEN_BREAK_PERMITTED )
		{
			if( !WgTextTool::GetCombCharVisibility( WG_HYPHEN_BREAK_PERMITTED, pDefProp.GetHandle(), hProp ) )
				continue;

			ch = '-';			// Character to be displayed is a normal hyphen.

			//TODO: Should change background color to show this is a special character.
		}

		if( ch == '\t' )
		{
			int tabWidth = 80;
			_x += tabWidth;
			_x -= (_x - _tabOrigo) % tabWidth;

			pLastGlyph = pFont->glyph( ch );
			continue;

			//TODO: Should display a tab character in the middle of the space if set to visible.
		}

		// Calculate position and blit the glyph.

		const WgGlyph * pGlyph = pFont->glyph( ch );

		_x += pFont->kerning( pLastGlyph, pGlyph );

		int renderX = _x + pFont->bearingX( pGlyph );
		int renderY = _y + pFont->bearingY( pGlyph );

		if( !_pLine[i].IsWhitespace() )
			ClipBlit( clip, pGlyph->pSurf, pGlyph->rect, renderX, renderY );

		_x += pFont->advance( pGlyph );

		pLastGlyph = pGlyph;
 	}

	// Restore tint color.

	if( m_tintColor != baseCol )
		SetTintColor(baseCol);
}


//____ DrawUnderline() ________________________________________________________

void WgGfxDevice::DrawUnderline( const WgRect& clip, const WgTextPropPtr& pDefProp, WgMode mode, int _x, int _y, const WgChar * pLine )
{
	Uint32 len = 0;
	Uint16 lastGlyph = 0;
	Uint32 propNb = pLine->GetPropHandle();

	const WgGlyphSet * pFont = pLine->GetGlyphSet(pDefProp,mode);

	for( const WgChar * p = pLine ; !p->IsEndOfLine() ; p++ )
	{
		if( p->GetPropHandle() != propNb )
		{
			if( p->IsUnderlined(pDefProp, mode) )
			{
				propNb = p->GetPropHandle();
				pFont	= p->GetGlyphSet(pDefProp,mode);
			}
			else
				break;
		}

		Uint16 glyph			= p->GetGlyph();

		len += pFont->kerning( lastGlyph, glyph ) + pFont->advance( glyph );
		lastGlyph = glyph;
	}

	pFont = pLine->GetGlyphSet(pDefProp,mode);

	WgBorders borders(pFont->underlineStretchOfs(), pFont->underlineRect().w
						- pFont->underlineStretchOfs() -pFont->underlineStretchLen(), 0,0 );

	ClipBlitHorrBar( clip, pFont->glyph(32)->pSurf, pFont->underlineRect(), borders, false,
					_x + pFont->underlineBearingX(), _y + pFont->underlineBearingY(), len );
}




