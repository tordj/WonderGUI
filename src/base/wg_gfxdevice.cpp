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
#include <wg_util.h>
#include <algorithm>

using namespace std;

namespace wg 
{
	
	const char GfxDevice::CLASSNAME[] = {"GfxDevice"};
	
	int GfxDevice::s_gfxDeviceCount = 0;
	int *	GfxDevice::s_pCurveTab = nullptr;


	//____ Constructor _____________________________________________________________
	
	GfxDevice::GfxDevice( Size canvasSize )
	{
		m_tintColor 		= Color(255,255,255);
		m_blendMode 		= BlendMode::Blend;
		m_canvasSize		= canvasSize;

		m_dummyClip = { numeric_limits<int>::min() >> 1,numeric_limits<int>::min() >> 1,numeric_limits<int>::max(),numeric_limits<int>::max() };

		if (s_gfxDeviceCount == 0)
		{
			_genCurveTab();
		}
		s_gfxDeviceCount++;
	}
	
	//____ Destructor _________________________________________________________

	GfxDevice::~GfxDevice()
	{
		s_gfxDeviceCount--;
		if (s_gfxDeviceCount == 0)
		{
			delete [] s_pCurveTab;
		}
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

	//____ plotPixels() _______________________________________________________

	void GfxDevice::plotPixels(int nCoords, const Coord * pCoords, const Color * pColors)
	{
		clipPlotPixels(m_dummyClip, nCoords, pCoords, pColors);
	}


	//____ drawLine() _________________________________________________________

	void GfxDevice::drawLine(Coord begin, Coord end, Color color, float thickness)
	{
		clipDrawLine(m_dummyClip, begin, end, color, thickness);
	}

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void GfxDevice::drawLine(Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		if (thickness <= 0.f)
			return;

		switch (dir)
		{
			case Direction::Left:
				begin.x -= length;
			case Direction::Right:
			{
				if (thickness <= 1.f)
				{
					Color col = _col;
					col.a = (uint8_t)(thickness * col.a);

					_drawStraightLine(begin, Orientation::Horizontal, length, col);
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					int beginY = begin.y - expanse;
					int endY = begin.y + expanse + 1;

					_drawStraightLine({ begin.x, beginY }, Orientation::Horizontal, length, edgeColor);
					_drawStraightLine({ begin.x, endY - 1 }, Orientation::Horizontal, length, edgeColor);
					fill({ begin.x, beginY + 1, length, endY - beginY - 2 }, _col);
				}
				break;
			}
			case Direction::Up:
				begin.y -= length;
			case Direction::Down:
			{
				if (thickness <= 1.f)
				{
					Color col = _col;
					col.a = (uint8_t)(thickness * col.a);

					_drawStraightLine(begin, Orientation::Vertical, length, col);
				}
				else
				{
					int expanse = (int)(1 + (thickness - 1) / 2);
					Color edgeColor(_col.r, _col.g, _col.b, (uint8_t)(_col.a * ((thickness - 1) / 2 - (expanse - 1))));

					int beginX = begin.x - expanse;
					int endX = begin.x + expanse + 1;

					_drawStraightLine({ beginX, begin.y }, Orientation::Vertical, length, edgeColor);
					_drawStraightLine({ endX - 1, begin.y }, Orientation::Vertical, length, edgeColor);
					fill({ beginX + 1, begin.y, endX - beginX - 2, length }, _col);
				}
				break;
			}
		}
	}


	//____ clipDrawLine() _________________________________________________________

	// Coordinates for start are considered to be + 0.5 in the width dimension, so they start in the middle of a line/column.
	// A one pixel thick line will only be drawn one pixel think, while a two pixels thick line will cover three pixels in thickness,
	// where the outer pixels are faded.

	void GfxDevice::clipDrawLine(const Rect& clip, Coord begin, Direction dir, int length, Color _col, float thickness)
	{
		if (thickness <= 0.f)
			return;

		switch (dir)
		{
		case Direction::Left:
			begin.x -= length;
		case Direction::Right:
		{
			if (begin.x > clip.x + clip.w)
				return;

			if (begin.x < clip.x)
			{
				length -= clip.x - begin.x;
				if (length <= 0)
					return;
				begin.x = clip.x;
			}

			if (begin.x + length > clip.x + clip.w)
			{
				length = clip.x + clip.w - begin.x;
				if (length <= 0)
					return;
			}

			if (thickness <= 1.f)
			{
				if (begin.y < clip.y || begin.y >= clip.y + clip.h)
					return;

				Color col = _col;
				col.a = (uint8_t)(thickness * col.a);

				_drawStraightLine(begin, Orientation::Horizontal, length, col);
			}
			else
			{
				int expanse = (int) (1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t) (_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				if (begin.y + expanse <= clip.y || begin.y - expanse >= clip.y + clip.h)
					return;

				int beginY = begin.y - expanse;
				int endY = begin.y + expanse + 1;

				if (beginY < clip.y)
					beginY = clip.y - 1;
				else
					_drawStraightLine({ begin.x, beginY }, Orientation::Horizontal, length, edgeColor);

				if (endY > clip.y + clip.h)
					endY = clip.y + clip.h + 1;
				else
					_drawStraightLine({ begin.x, endY - 1 }, Orientation::Horizontal, length, edgeColor);

				fill({ begin.x, beginY + 1, length, endY - beginY - 2 }, _col);
			}

			break;
		}
		case Direction::Up:
			begin.y -= length;
		case Direction::Down:
			if (begin.y > clip.y + clip.h)
				return;

			if (begin.y < clip.y)
			{
				length -= clip.y - begin.y;
				if (length <= 0)
					return;
				begin.y = clip.y;
			}

			if (begin.y + length > clip.y + clip.h)
			{
				length = clip.y + clip.h - begin.y;
				if (length <= 0)
					return;
			}

			if (thickness <= 1.f)
			{
				if (begin.x < clip.x || begin.x >= clip.x + clip.w)
					return;

				Color col = _col;
				col.a = (uint8_t)(thickness * col.a);

				_drawStraightLine(begin, Orientation::Vertical, length, col);
			}
			else
			{
				int expanse = (int) (1 + (thickness - 1) / 2);
				Color edgeColor(_col.r, _col.g, _col.b, (uint8_t) (_col.a * ((thickness - 1) / 2 - (expanse - 1))));

				if (begin.x + expanse <= clip.x || begin.x - expanse >= clip.x + clip.w)
					return;

				int beginX = begin.x - expanse;
				int endX = begin.x + expanse + 1;

				if (beginX < clip.x)
					beginX = clip.x - 1;
				else
					_drawStraightLine({ beginX, begin.y }, Orientation::Vertical, length, edgeColor);

				if (endX > clip.x + clip.w)
					endX = clip.x + clip.w + 1;
				else
					_drawStraightLine({ endX - 1, begin.y }, Orientation::Vertical, length, edgeColor);

				fill({ beginX + 1, begin.y, endX - beginX - 2, length }, _col);
			}

			break;
		}
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
	

	void GfxDevice::stretchBlit(Surface * pSrc)
	{
		stretchBlit(pSrc, Rect(0, 0, pSrc->width(), pSrc->height()), Rect(0,0,m_pCanvas->size()));
	}

	void GfxDevice::stretchBlit( Surface * pSrc, const Rect& dest )
	{
		stretchBlit( pSrc, Rect(0, 0, pSrc->size()), dest );
	}
	
	void GfxDevice::stretchBlit( Surface * pSrc, const Rect& src, const Rect& dest )
	{
		float srcW = (float) src.w;
		float srcH = (float) src.h;
		
		if( pSrc->scaleMode() == ScaleMode::Interpolate )
		{
			if( srcW < (float) dest.w )
				srcW--;
	
			if( srcH < (float) dest.h )
				srcH--;
		}
	
		stretchBlit( pSrc, RectF( (float) src.x, (float) src.y, srcW, srcH), dest );
	}
	
	//____ tileBlit() ______________________________________________________________

	void GfxDevice::tileBlit(Surface * _pSrc)
	{
		tileBlit(_pSrc, Rect(0, 0, _pSrc->size()), Rect(0, 0, m_pCanvas->size()));
	}
	
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
	
	void GfxDevice::clipStretchBlit(const Rect& clip, Surface * pSrc)
	{
		clipStretchBlit(clip, pSrc, Rect(0, 0, pSrc->size()), Rect(0,0,m_pCanvas->size()));
	}

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
		Rect  dest = _dest;
		
		// With interpolation we only 'touch' the edge pixels, we don't include their full size.

		if( pSrc->scaleMode() == ScaleMode::Interpolate )
		{
			if( src.w < dest.w )
				src.w--;
	
			if( src.h < dest.h )
				src.h--;
		}
	
		int cx = std::max(clip.x, dest.x);
		int cy = std::max(clip.y, dest.y);
		int cw = std::min(clip.x + clip.w, dest.x + dest.w) - cx;
		int ch = std::min(clip.y + clip.h, dest.y + dest.h) - cy;
	
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
	
		stretchBlit(pSrc, src, { cx, cy, cw, ch });
	}
	
	//____ clipTileBlit() __________________________________________________________
	
	void GfxDevice::clipTileBlit(const Rect& clip, Surface * pSrc )
	{
		clipTileBlit(clip, pSrc, Rect(0, 0, pSrc->size()), Rect(0,0, m_pCanvas->size() ));
	}


	void GfxDevice::clipTileBlit( const Rect& clip, Surface * pSrc, const Rect& dest )
	{
		clipTileBlit( clip, pSrc, Rect(0,0,pSrc->size()), dest );
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
			dest.x = xStart;
			myRect.y = _src.y;
			myRect.h = _src.h;
		}
		return;
	}

	//____ drawHorrWave() _____________________________________________________

	void GfxDevice::drawHorrWave(Coord begin, int length, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill)
	{
		clipDrawHorrWave(m_dummyClip, begin, length, pTopBorder, pBottomBorder, frontFill, backFill);
	}

	//_____ clipBlitFromCanvas() ______________________________________________

	void GfxDevice::clipBlitFromCanvas(const Rect& clip, Surface* pSrc, const Rect& src, Coord dest)
	{
		clipBlit(clip, pSrc, src, dest);		// Default is a normal blit, only OpenGL needs to flip (until that has been fixed)
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

	//____ _genCurveTab() ___________________________________________________________

	void GfxDevice::_genCurveTab()
	{
		s_pCurveTab = new int[c_nCurveTabEntries];

		//		double factor = 3.14159265 / (2.0 * c_nCurveTabEntries);

		for (int i = 0; i < c_nCurveTabEntries; i++)
		{
			double y = 1.f - i / (double)c_nCurveTabEntries;
			s_pCurveTab[i] = (int)(Util::squareRoot(1.f - y*y)*65536.f);
		}
	}

	//____ _traceLine() __________________________________________________________

	void GfxDevice::_traceLine(int * pDest, int nPoints, const WaveLine * pWave, int offset)
	{
		static const int c_supersamples = 4;

		static int brush[128 * c_supersamples];
		static float prevThickness = -1.f;

		float thickness = pWave->thickness;
		int brushSteps = (int)(thickness * c_supersamples / 2);

		// Generate brush

		if (thickness != prevThickness)
		{
			int scaledThickness = (int)(thickness / 2 * 256);

			brush[0] = scaledThickness;
			for (int i = 1; i <= brushSteps; i++)
				brush[i] = (scaledThickness * s_pCurveTab[c_nCurveTabEntries - (i*c_nCurveTabEntries) / brushSteps]) >> 16;
			prevThickness = thickness;
		}

		int nTracePoints = max(0, min(nPoints, pWave->length - offset));
		int nFillPoints = nPoints - nTracePoints;

		// Trace...

		int * pSrc = pWave->pWave + offset;
		for (int i = 0; i < nTracePoints; i++)
		{
			// Start with top and bottom for current point

			int top = pSrc[i] - brush[0];
			int bottom = pSrc[i] + brush[0];

			// Check brush's coverage from previous points

			int end = min(i + 1, brushSteps + 1);

			for (int j = 1; j < end; j++)
			{
				int from = pSrc[i - j / c_supersamples];
				int to = pSrc[i - j / c_supersamples - 1];

				int sample = (to - from) * (j%c_supersamples) / c_supersamples + from;

				int topCover = sample - brush[j];
				int bottomCover = sample + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Check brush's coverage from following points

			end = min(nPoints - i, brushSteps + 1);

			for (int j = 1; j < end; j++)
			{
				int from = pSrc[i + j / c_supersamples];
				int to = pSrc[i + j / c_supersamples + 1];

				int sample = (to - from) * (j%c_supersamples) / c_supersamples + from;

				int topCover = sample - brush[j];
				int bottomCover = sample + brush[j];

				if (topCover < top)
					top = topCover;
				else if (bottomCover > bottom)
					bottom = bottomCover;
			}

			// Save traced values

			*pDest++ = top;
			*pDest++ = bottom;
		}

		// Fill...

		if (nFillPoints)
		{
			int top = pWave->hold - brush[0];
			int bottom = pWave->hold + brush[0];
			for (int i = 0; i < nFillPoints; i++)
			{
				*pDest++ = top;
				*pDest++ = bottom;
			}
		}
	}

} // namespace wg
