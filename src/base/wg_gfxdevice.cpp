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
	
	GfxDevice_p GfxDevice::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return GfxDevice_p( static_cast<GfxDevice*>(pObject) );
	
		return 0;
	}
	
	
	//____ setTintColor() __________________________________________________________
	
	void GfxDevice::setTintColor( Color color )
	{
		m_tintColor = color;
	}
	
	//____ setBlendMode() __________________________________________________________
	
	bool GfxDevice::setBlendMode( BlendMode blendMode )
	{
		if( blendMode == BlendMode::Undefined )
			m_blendMode = BlendMode::Blend;
		else
			m_blendMode = blendMode;
			
		return false;				// Not implemented.
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
	
	void GfxDevice::blit( Surface * pSrc )
	{
		blit( pSrc, Rect( 0, 0, pSrc->width(), pSrc->height() ), Coord(0,0) );
	}
	
	void GfxDevice::blit( Surface * pSrc, Coord dest )
	{
		blit( pSrc, Rect( 0, 0, pSrc->width(), pSrc->height() ), dest );
	}
	
	//____ stretchBlit() ___________________________________________________________
	
	void GfxDevice::stretchBlit( Surface * pSrc, const Rect& dest )
	{
		stretchBlit( pSrc, Rect(0, 0, pSrc->width(),pSrc->height()), dest );
	}
	
	void GfxDevice::stretchBlit( Surface * pSrc, const Rect& src, const Rect& dest )
	{
		float srcW = (float) src.w;
		float srcH = (float) src.h;
	
		float destW = (float) dest.w;
		float destH = (float) dest.h;
	
		if( pSrc->scaleMode() == ScaleMode::Interpolate )
		{
			if( srcW < destW )
				srcW--;
	
			if( srcH < destH )
				srcH--;
		}
	
		stretchBlitSubPixel( pSrc, (float) src.x, (float) src.y, srcW, srcH, (float) dest.x, (float) dest.y, destW, destH );
	}
	
	//____ tileBlit() ______________________________________________________________
	
	void GfxDevice::tileBlit( Surface * _pSrc, const Rect& _dest )
	{
		tileBlit( _pSrc, Rect( 0, 0, _pSrc->width(), _pSrc->height() ), _dest );
	}
	
	void GfxDevice::tileBlit( Surface * _pSrc, const Rect& _src, const Rect& _dest )
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
				blit( _pSrc, r, Coord(destX, destY) );
				destX += r.w;
			}
	
			// Blit any left over part at end of row.
	
			if( r2.w > 0 )
				blit( _pSrc, r2, Coord(destX, destY) );
	
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
	
	void GfxDevice::clipBlit( const Rect& clip, Surface * pSrc )
	{
		clipBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), Coord(0,0) );
	}
	
	void GfxDevice::clipBlit( const Rect& clip, Surface * pSrc, Coord dest )
	{
		clipBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), dest );
	}
	
	void GfxDevice::clipBlit( const Rect& clip, Surface * pSrc, const Rect& srcRect, Coord dest  )
	{
		if( (clip.x <= dest.x) && (clip.x + clip.w > dest.x + srcRect.w) &&
	      (clip.y <= dest.y) && (clip.y + clip.h > dest.y + srcRect.h) )
		{
			blit( pSrc, srcRect, dest );														// Totally inside clip-rect.
			return;
		}
	
		if( (clip.x > dest.x + srcRect.w) || (clip.x + clip.w < dest.x) ||
	      (clip.y > dest.y + srcRect.h) || (clip.y + clip.h < dest.y) )
			return;																						// Totally outside clip-rect.
	
		// Do Clipping
	
		Rect	newSrc = srcRect;
	
		if( dest.x < clip.x )
		{
			newSrc.w -= clip.x - dest.x;
			newSrc.x += clip.x - dest.x;
			dest.x = clip.x;
		}
	
		if( dest.y < clip.y )
		{
			newSrc.h -= clip.y - dest.y;
			newSrc.y += clip.y - dest.y;
			dest.y = clip.y;
		}
	
		if( dest.x + newSrc.w > clip.x + clip.w )
			newSrc.w = (clip.x + clip.w) - dest.x;
	
		if( dest.y + newSrc.h > clip.y + clip.h )
			newSrc.h = (clip.y + clip.h) - dest.y;
	
	
		blit( pSrc, newSrc, dest );
	}
	
	//____ clipStretchBlit() _______________________________________________________
	
	void GfxDevice::clipStretchBlit( const Rect& clip, Surface * pSrc, const Rect& dest )
	{
		clipStretchBlit( clip, pSrc, Rect(0,0,pSrc->width(), pSrc->height()), dest );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, Surface * pSrc, const Rect& src, const Rect& dest )
	{
		clipStretchBlit( clip, pSrc, RectF(src), Rect(dest) );
	}
	
	void GfxDevice::clipStretchBlit( const Rect& clip, Surface * pSrc, const RectF& _src, const Rect& _dest)
	{
		RectF src = _src;
		RectF  dest = _dest;
		
		// With interpolation we only 'touch' the edge pixels, we don't include their full size.

		if( pSrc->scaleMode() == ScaleMode::Interpolate )
		{
			if( src.w < dest.w )
				src.w--;
	
			if( src.h < dest.h )
				src.h--;
		}
	
		float cx = std::max(float(clip.x), dest.x);
		float cy = std::max(float(clip.y), dest.y);
		float cw = std::min(float(clip.x + clip.w), dest.x + dest.w) - cx;
		float ch = std::min(float(clip.y + clip.h), dest.y + dest.h) - cy;
	
		if(cw <= 0 || ch <= 0)
			return;
	
		if( dest.w > cw )
		{
			float	sdxr = src.w / dest.w;			// Source/Destination X Ratio.
	
			src.w = sdxr * cw;
	
			if( dest.x < cx )
				src.x += sdxr * (cx - dest.x);
		}
	
		if( dest.h > ch )
		{
			float	sdyr = src.h / dest.h;			// Source/Destination Y Ratio.
	
			src.h = sdyr * ch;
	
			if( dest.y < cy )
				src.y += sdyr * (cy - dest.y);
		}
	
		stretchBlitSubPixel( pSrc, src.x, src.y, src.w, src.h, cx, cy, cw, ch );
	}
	
	//____ clipTileBlit() __________________________________________________________
	
	void GfxDevice::clipTileBlit( const Rect& clip, Surface * pSrc,
									  const Rect& dest )
	{
		clipTileBlit( clip, pSrc, Rect(0,0,pSrc->width(),pSrc->height()), dest );
	}
	
	
	void GfxDevice::clipTileBlit( const Rect& _clip, Surface * _pSrc, const Rect& _src, const Rect& _dest )
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
	
		Coord dest = clip.pos();
	
		myRect.y = yStart;
		myRect.h =_src.y + _src.h - yStart;
	
		while( dest.y < clip.y + clip.h )
		{
			if( myRect.h > clip.y + clip.h - dest.y )
				myRect.h = clip.y + clip.h - dest.y;
	
			myRect.x = xStart;
			myRect.w = _src.x + _src.w - xStart;
			if( myRect.w > clip.w )
				myRect.w = clip.w;
	
	
			// Blit a row.
	
			blit( _pSrc, myRect, dest );
			dest.x += myRect.w;
			myRect.x = _src.x;
			myRect.w = _src.w;
	
			while( dest.x <= clip.x + clip.w - _src.w )
			{
				blit( _pSrc, myRect, dest );
				dest.x += myRect.w;
			}
			myRect.w = clip.x + clip.w - dest.x;
			if( myRect.w > 0 )
				blit( _pSrc, myRect, dest );
	
			dest.y += myRect.h;
			myRect.y = _src.y;
			myRect.h = _src.h;
		}
		return;
	}
	
	
	//____ clipBlitHorrBar() ______________________________________________________
	
	void GfxDevice::clipBlitHorrBar(	const Rect& _clip, Surface * _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, Coord dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		clipBlit( _clip, _pSurf, r, dest );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		dest.x += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				clipBlit( _clip, _pSurf, r, dest );
				_len -= r.w;
				dest.x += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				clipBlit( _clip, _pSurf, r, dest );
				dest.x += _len;
			}
		}
		else
		{
			clipStretchBlit( _clip, _pSurf, r, Rect( dest, _len, r.h ) );
			dest.x += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		clipBlit( _clip, _pSurf, r, dest );
	}
	
	//____ clipBlitVertBar() ______________________________________________________
	
	void GfxDevice::clipBlitVertBar(	const Rect& _clip, Surface * _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, Coord dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		clipBlit( _clip, _pSurf, r, dest );
	
		_len -= _borders.height();			// Remove top and bottom edges from len.
		dest.y += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				clipBlit( _clip, _pSurf, r, dest );
				_len -= r.h;
				dest.y += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				clipBlit( _clip, _pSurf, r, dest );
				dest.y += _len;
			}
		}
		else
		{
			clipStretchBlit( _clip, _pSurf, r, Rect( dest, r.w, _len ) );
			dest.y += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		clipBlit( _clip, _pSurf, r, dest );
	}
	
	
	//____ blitHorrBar() __________________________________________________________
	
	void GfxDevice::blitHorrBar(	Surface * _pSurf, const Rect& _src,
									const Border& _borders, bool _bTile,
									Coord dest, int _len )
	{
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		blit( _pSurf, r, dest );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		dest.x += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				blit( _pSurf, r, dest );
				_len -= r.w;
				dest.x += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				blit( _pSurf, r, dest );
				dest.x += _len;
			}
		}
		else
		{
			stretchBlit( _pSurf, r, Rect( dest, _len, r.h ) );
			dest.x += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		blit( _pSurf, r, dest );
	}
	
	//____ blitVertBar() __________________________________________________________
	
	void GfxDevice::blitVertBar(	Surface * _pSurf, const Rect& _src,
									const Border& _borders, bool _bTile,
									Coord dest, int _len )
	{
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		blit( _pSurf, r, dest );
	
		_len -= _borders.height();			// Remove top and bottom borders from len.
		dest.y += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				blit( _pSurf, r, dest );
				_len -= r.h;
				dest.y += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				blit( _pSurf, r, dest );
				dest.y += _len;
			}
		}
		else
		{
			stretchBlit( _pSurf, r, Rect( dest, r.w, _len ) );
			dest.y += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		blit( _pSurf, r, dest );
	}
	
} // namespace wg
