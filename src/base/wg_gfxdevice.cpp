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

#include <wg_surface.h>
#include <wg_geo.h>
#include <algorithm>

namespace wg 
{
	
	const char GfxDevice::CLASSNAME[] = {"GfxDevice"};
	
	//____ Constructor _____________________________________________________________
	
	GfxDevice::GfxDevice( Size canvasSize )
	{
		m_tintColor 		= Color(255,255,255);
		m_blendMode 		= BlendMode::Blend;
		m_bSaveDirtyRects 	= false;
		m_renderFlags		= 0;
		m_canvasSize		= canvasSize;
	}
	
	//____ isInstanceOf() _________________________________________________________
	
	bool GfxDevice::isInstanceOf( const char * pClassName ) const
	{ 
		if( pClassName==CLASSNAME )
			return true;
	
		return Object::isInstanceOf(pClassName);
	}
	
	//____ className() ____________________________________________________________
	
	const char * GfxDevice::className( void ) const
	{ 
		return CLASSNAME; 
	}
	
	//____ cast() _________________________________________________________________
	
	GfxDevice_p GfxDevice::cast( const Object_p& pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return GfxDevice_p( static_cast<GfxDevice*>(pObject.rawPtr()) );
	
		return 0;
	}
	
	//____ setSaveDirtyRects() _____________________________________________________
	
	bool GfxDevice::setSaveDirtyRects( bool bSave )
	{
		m_bSaveDirtyRects = bSave;
		return true;
	}
	
	//____ setTintColor() __________________________________________________________
	
	void GfxDevice::setTintColor( Color color )
	{
		m_tintColor = color;
	}
	
	//____ setBlendMode() __________________________________________________________
	
	bool GfxDevice::setBlendMode( BlendMode blendMode )
	{
		m_blendMode = blendMode;
		return false;				// Not implemented.
	}
	
	//______________________________________________________________________________
	uint32_t GfxDevice::setRenderFlags( uint32_t flags )
	{
		uint32_t oldFlags = m_renderFlags;
		m_renderFlags = flags;
		return oldFlags;
	}
	
	//____ beginRender() ___________________________________________________________
	
	bool GfxDevice::beginRender()
	{
		return true;	// Assumed to be ok if device doesn't have its own method.
	}
	
	//____ endRender() _____________________________________________________________
	
	bool GfxDevice::endRender()
	{
		return true;	// Assumed to be ok if device doesn't have its own method.
	}
	
	//____ blit() __________________________________________________________________
	
	void GfxDevice::blit( const Surface_p& pSrc )
	{
		blit( pSrc, Rect( 0, 0, pSrc->width(), pSrc->height() ), 0, 0 );
	}
	
	void GfxDevice::blit( const Surface_p& pSrc, int dx, int dy )
	{
		blit( pSrc, Rect( 0, 0, pSrc->width(), pSrc->height() ), dx, dy );
	}
	
	//____ stretchBlit() ___________________________________________________________
	
	void GfxDevice::stretchBlit( const Surface_p& pSrc, bool bTriLinear, float mipmapBias )
	{
		stretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->width(), (float) pSrc->height(), 0, 0, (float) m_canvasSize.w, (float) m_canvasSize.h, bTriLinear, mipmapBias );
	}
	
	void GfxDevice::stretchBlit( const Surface_p& pSrc, const Rect& dest, bool bTriLinear, float mipmapBias )
	{
		stretchBlitSubPixel( pSrc, 0, 0, (float) pSrc->width(), (float) pSrc->height(), (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
	}
	
	void GfxDevice::stretchBlit( const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear, float mipmapBias )
	{
		stretchBlitSubPixel( pSrc, (float) src.x, (float) src.y, (float) src.w, (float) src.h, (float) dest.x, (float) dest.y, (float) dest.w, (float) dest.h, bTriLinear, mipmapBias );
	}
	
	//____ tileBlit() ______________________________________________________________
	
	void GfxDevice::tileBlit( const Surface_p& _pSrc )
	{
		tileBlit( _pSrc, Rect( 0, 0, _pSrc->width(), _pSrc->height() ), Rect(0,0,m_canvasSize) );
	}
	
	void GfxDevice::tileBlit( const Surface_p& _pSrc, const Rect& _dest )
	{
		tileBlit( _pSrc, Rect( 0, 0, _pSrc->width(), _pSrc->height() ), _dest );
	}
	
	void GfxDevice::tileBlit( const Surface_p& _pSrc, const Rect& _src, const Rect& _dest )
	{
		if( !_pSrc || _dest.h == 0 || _dest.w == 0 )
			return;
	
		Rect	r = _src;
		Rect	r2 = _src;
	
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
				blit( _pSrc, r, destX, destY );
				destX += r.w;
			}
	
			// Blit any left over part at end of row.
	
			if( r2.w > 0 )
				blit( _pSrc, r2, destX, destY );
	
			destX = _dest.x;
			destY += _src.h;
		}
		return;
	}
	
	
	//____ clipFill() ______________________________________________________________
	
	void GfxDevice::clipFill( const Rect& _clip, const Rect& _rect, const Color& _col )
	{
		fill( Rect( _clip, _rect ), _col );
	}
	
	//____ clipBlit() ______________________________________________________________
	
	void GfxDevice::clipBlit( const Rect& clip, const Surface_p& pSrc )
	{
		clipBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), 0, 0 );
	}
	
	void GfxDevice::clipBlit( const Rect& clip, const Surface_p& pSrc, int dx, int dy  )
	{
		clipBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), dx, dy );
	}
	
	void GfxDevice::clipBlit( const Rect& clip, const Surface_p& pSrc, const Rect& srcRect, int dx, int dy  )
	{
		if( (clip.x <= dx) && (clip.x + clip.w > dx + srcRect.w) &&
	      (clip.y <= dy) && (clip.y + clip.h > dy + srcRect.h) )
		{
			blit( pSrc, srcRect, dx, dy );														// Totally inside clip-rect.
			return;
		}
	
		if( (clip.x > dx + srcRect.w) || (clip.x + clip.w < dx) ||
	      (clip.y > dy + srcRect.h) || (clip.y + clip.h < dy) )
			return;																						// Totally outside clip-rect.
	
		// Do Clipping
	
		Rect	newSrc = srcRect;
	
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
	
	
		blit( pSrc, newSrc, dx, dy );
	}
	
	//____ clipStretchBlit() _______________________________________________________
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, bool bTriLinear, float mipBias )
	{
		clipStretchBlit( clip, pSrc, Rect(0,0,pSrc->width(), pSrc->height()), Rect( 0,0,m_canvasSize), bTriLinear, mipBias );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& dest, bool bTriLinear, float mipBias )
	{
		clipStretchBlit( clip, pSrc, Rect(0,0,pSrc->width(), pSrc->height()), dest, bTriLinear, mipBias );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, const Rect& src, const Rect& dest, bool bTriLinear, float mipBias )
	{
		clipStretchBlit( clip, pSrc, (float)src.x, (float)src.y, (float)src.w, (float)src.h, (float)dest.x, (float)dest.y, (float)dest.w, (float)dest.h, false );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, const Surface_p& pSrc, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias)
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
	
		stretchBlitSubPixel( pSrc, sx, sy, sw, sh, cx, cy, cw, ch, bTriLinear, mipBias );
	}
	
	//____ clipTileBlit() __________________________________________________________
	
	void GfxDevice::clipTileBlit( const Rect& clip, const Surface_p& pSrc )
	{
		clipTileBlit( clip, pSrc, Rect( 0,0,pSrc->width(),pSrc->height() ),
					  Rect(0,0,m_canvasSize) );
	}
	
	void GfxDevice::clipTileBlit( const Rect& clip, const Surface_p& pSrc,
									  const Rect& dest )
	{
		clipTileBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), dest );
	}
	
	
	void GfxDevice::clipTileBlit( const Rect& _clip, const Surface_p& _pSrc, const Rect& _src, const Rect& _dest )
	{
		if( !_pSrc )
			return;
	
		Rect	myRect;
	
		Rect	clip;
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
			blit( _pSrc, myRect, destX, destY );
			destX += myRect.w;
			myRect.x = _src.x;
			myRect.w = _src.w;
	
			while( destX <= clip.x + clip.w - _src.w )
			{
				blit( _pSrc, myRect, destX, destY );
				destX += myRect.w;
			}
			myRect.w = clip.x + clip.w - destX;
			if( myRect.w > 0 )
				blit( _pSrc, myRect, destX, destY );
	
			destY += myRect.h;
			myRect.y = _src.y;
			myRect.h = _src.h;
		}
		return;
	}
	
	
	//____ clipBlitHorrBar() ______________________________________________________
	
	void GfxDevice::clipBlitHorrBar(	const Rect& _clip, const Surface_p& _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, int _dx, int _dy, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		_dx += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_len -= r.w;
				_dx += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_dx += _len;
			}
		}
		else
		{
			clipStretchBlit( _clip, _pSurf, r, Rect( _dx, _dy, _len, r.h ) );
			_dx += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	}
	
	//____ clipBlitVertBar() ______________________________________________________
	
	void GfxDevice::clipBlitVertBar(	const Rect& _clip, const Surface_p& _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, int _dx, int _dy, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	
		_len -= _borders.height();			// Remove top and bottom edges from len.
		_dy += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_len -= r.h;
				_dy += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				clipBlit( _clip, _pSurf, r, _dx, _dy );
				_dy += _len;
			}
		}
		else
		{
			clipStretchBlit( _clip, _pSurf, r, Rect( _dx, _dy, r.w, _len ) );
			_dy += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		clipBlit( _clip, _pSurf, r, _dx, _dy );
	}
	
	
	//____ blitHorrBar() __________________________________________________________
	
	void GfxDevice::blitHorrBar(	const Surface_p& _pSurf, const Rect& _src,
									const Border& _borders, bool _bTile,
									int _dx, int _dy, int _len )
	{
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		blit( _pSurf, r, _dx, _dy );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		_dx += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				blit( _pSurf, r, _dx, _dy );
				_len -= r.w;
				_dx += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				blit( _pSurf, r, _dx, _dy );
				_dx += _len;
			}
		}
		else
		{
			stretchBlit( _pSurf, r, Rect( _dx, _dy, _len, r.h ) );
			_dx += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		blit( _pSurf, r, _dx, _dy );
	}
	
	//____ blitVertBar() __________________________________________________________
	
	void GfxDevice::blitVertBar(	const Surface_p& _pSurf, const Rect& _src,
									const Border& _borders, bool _bTile,
									int _dx, int _dy, int _len )
	{
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		blit( _pSurf, r, _dx, _dy );
	
		_len -= _borders.height();			// Remove top and bottom borders from len.
		_dy += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				blit( _pSurf, r, _dx, _dy );
				_len -= r.h;
				_dy += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				blit( _pSurf, r, _dx, _dy );
				_dy += _len;
			}
		}
		else
		{
			stretchBlit( _pSurf, r, Rect( _dx, _dy, r.w, _len ) );
			_dy += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		blit( _pSurf, r, _dx, _dy );
	}
	
} // namespace wg
