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

	static const int flipTransforms[GfxFlip_size][2][2] = { { 1,0,0,1 },			// Normal
															{ -1,0,0,1 },			// FlipX
															{ 1,0,0,-1 },			// FlipY
															{ 0,-1,1,0 },			// Rot90
															{ 0,1,1,0 },			// Rot90FlipX
															{ 0,-1,-1,0 },			// Rot90FlipY
															{ -1,0,0,-1 },			// Rot180
															{ 1,0,0,-1 },			// Rot180FlipX
															{ -1,0,0,1 },			// Rot180FlipY
															{ 0,1,-1,0 },			// Rot270
															{ 0,-1,-1,0 },			// Rot270FlipX
															{ 0,1,1,0 } };			// Rot270FlipY

	static const int flipOffsets[GfxFlip_size][2] = {	{ 0,0 },				// Normal
														{ 1,0 },				// FlipX
														{ 0,1 },				// FlipY
														{ 0,1 },				// Rot90
														{ 0,0 },				// Rot90FlipX
														{ 1,1 },				// Rot90FlipY
														{ 1,1 },				// Rot180
														{ 0,1 },				// Rot180FlipX
														{ 1,0 },				// Rot180FlipY
														{ 1,0 },				// Rot270
														{ 1,1 },				// Rot270FlipX
														{ 0,0 } };				// Rot270FlipY


	//____ Constructor _____________________________________________________________
	
	GfxDevice::GfxDevice( Size canvasSize )
	{
		m_tintColor 		= Color(255,255,255);
		m_blendMode 		= BlendMode::Blend;
		m_canvasSize		= canvasSize;
		m_clip				= canvasSize;

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

	//____ setClip() __________________________________________________________

	void GfxDevice::setClip(const Rect& clip)
	{
		m_clip = clip;

		limit(m_clip.x, 0, m_canvasSize.w);
		limit(m_clip.y, 0, m_canvasSize.h);
		limit(m_clip.w, 0, m_canvasSize.w-m_clip.x);
		limit(m_clip.h, 0, m_canvasSize.h - m_clip.y);
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

	//____ fill() _____________________________________________________________

	void GfxDevice::fill(const Rect& rect, const Color& col)
	{
		fill(RectF((float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h), col);
	}

	//____ drawLine() _________________________________________________________

	void GfxDevice::drawLine(Coord begin, Direction dir, int length, Color color, float thickness)
	{
		Coord end;

		switch (dir)
		{
			case Direction::Up:
				end.x = begin.x;
				end.y = begin.y - length;
				break;
			case Direction::Down:
				end.x = begin.x;
				end.y = begin.y + length;
				break;
			case Direction::Left:
				end.x = begin.x - length;
				end.y = begin.y;
				break;
			case Direction::Right:
				end.x = begin.x + length;
				end.y = begin.y;
				break;
		}

		drawLine(begin, end, color, thickness);
	}


	//____ blit() __________________________________________________________________
	
	void GfxDevice::blit(Coord dest, Surface * pSrc)
	{

		transformBlit({ dest, pSrc->size() }, pSrc, { 0,0 }, flipTransforms[0]);
	}

	void GfxDevice::blit(Coord dest, Surface * pSrc, const Rect& src)
	{
		transformBlit({ dest, src.size() }, pSrc, src.pos(), flipTransforms[0]);
	}

	//____ flipBlit() _________________________________________________________

	void GfxDevice::flipBlit(Coord dest, Surface * pSrc, GfxFlip flip)
	{
		Size srcSize  = pSrc->size();

		int ofsX = (srcSize.w - 1) * flipOffsets[(int)flip][0];
		int ofsY = (srcSize.h - 1) * flipOffsets[(int)flip][1];

		Size dstSize = srcSize;
		if (flipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);

		transformBlit({ dest, dstSize }, pSrc, { ofsX, ofsY }, flipTransforms[(int)flip]);
	}

	void GfxDevice::flipBlit(Coord dest, Surface * pSrc, const Rect& src, GfxFlip flip)
	{
		int ofsX = src.x + (src.w-1) * flipOffsets[(int)flip][0];
		int ofsY = src.y + (src.h-1) * flipOffsets[(int)flip][1];

		Size dstSize = pSrc->size();
		if (flipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);


		transformBlit({ dest, dstSize }, pSrc, src.pos() + Coord(ofsX, ofsY), flipTransforms[(int)flip]);
	}


	//____ stretchBlit() ___________________________________________________________
	
	void GfxDevice::stretchBlit(const Rect& dest, Surface * pSrc )
	{
		stretchBlit(dest, pSrc, Rect(0, 0, pSrc->size()) );
	}
	
	void GfxDevice::stretchBlit(const Rect& dest, Surface * pSrc, const Rect& _src )
	{
		RectF src{ (float)_src.x, (float)_src.y, (float)_src.w, (float)_src.h };

		float	mtx[2][2];

		if (pSrc->scaleMode() == ScaleMode::Interpolate)
		{
			src.x += 0.5f;
			src.y += 0.5f;

			mtx[0][0] = (src.w-1) / (dest.w-1);
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = (src.h-1) / (dest.h-1);
		}
		else
		{
			mtx[0][0] = src.w / dest.w;
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = src.h / dest.h;
		}


		transformBlit(dest, pSrc, { src.x, src.y }, mtx);
	}
	
	void GfxDevice::stretchBlit(const Rect& dest, Surface * pSrc, const RectF& src)
	{
		float	mtx[2][2];

		if (pSrc->scaleMode() == ScaleMode::Interpolate)
		{
			mtx[0][0] = src.w / (dest.w - 1);
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = src.h / (dest.h - 1);
		}
		else
		{
			mtx[0][0] = src.w / dest.w;
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = src.h / dest.h;
		}

		transformBlit(dest, pSrc, { src.x,src.y }, mtx );
	}

	//____ stretchFlipBlit() _____________________________________________________
	
	void GfxDevice::stretchFlipBlit(const Rect& dest, Surface * pSrc, GfxFlip flip)
	{
		stretchFlipBlit(dest, pSrc, Rect(0, 0, pSrc->size()), flip);
	}

	void GfxDevice::stretchFlipBlit(const Rect& dest, Surface * pSrc, const Rect& src, GfxFlip flip)
	{
		float scaleX, scaleY;
		float ofsX, ofsY;

		if (pSrc->scaleMode() == ScaleMode::Interpolate)
		{
			float srcW = (float)(src.w - 1);
			float srcH = (float)(src.h - 1);

			scaleX = srcW / (dest.w-1);
			scaleY = srcH / (dest.h-1);

			ofsX = src.x + 0.5f + srcW * flipOffsets[(int)flip][0];
			ofsY = src.y + 0.5f + srcH * flipOffsets[(int)flip][1];
		}
		else
		{
			float srcW = (float)src.w;
			float srcH = (float)src.h;

			scaleX = srcW / dest.w;
			scaleY = srcH / dest.h;

			ofsX = src.x + (srcW - scaleX) * flipOffsets[(int)flip][0];
			ofsY = src.y + (srcH - scaleY) * flipOffsets[(int)flip][1];
		}

		float	mtx[2][2];

		mtx[0][0] = scaleX * flipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * flipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * flipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * flipTransforms[(int)flip][1][1];

		transformBlit(dest, pSrc, { ofsX, ofsY }, mtx);


/*
		float srcW = (float)src.w;
		float srcH = (float)src.h;
		float flipSub = 1.f;

		if (pSrc->scaleMode() == ScaleMode::Interpolate)
		{
			flipSub = 1.0078125f;							// Need to subtract a tiny fraction extra (1/128 seems good).

			if (srcW < (float)dest.w)
				srcW--;

			if (srcH < (float)dest.h)
				srcH--;
		}

		float ofsX = src.x + (src.w - flipSub) * flipOffsets[(int)flip][0];
		float ofsY = src.y + (src.h - flipSub) * flipOffsets[(int)flip][1];


		float	scaleX = srcW / dest.w;
		float	scaleY = srcH / dest.h;

		float	mtx[2][2];

		mtx[0][0] = scaleX * flipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * flipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * flipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * flipTransforms[(int)flip][1][1];

		transformBlit(dest, pSrc, { ofsX, ofsY }, mtx);
*/
	}

	void GfxDevice::stretchFlipBlit(const Rect& dest, Surface * pSrc, const RectF& src, GfxFlip flip)
	{
		float	scaleX = src.w / dest.w;
		float	scaleY = src.h / dest.h;

		float ofsX = src.x + (src.w-scaleX) * flipOffsets[(int)flip][0];
		float ofsY = src.y + (src.h-scaleY) * flipOffsets[(int)flip][1];

		float	mtx[2][2];

		mtx[0][0] = scaleX * flipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * flipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * flipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * flipTransforms[(int)flip][1][1];

		transformBlit(dest, pSrc, { src.x,src.y }, mtx);
	}

	//____ rotScaleBlit() _____________________________________________________

	void GfxDevice::rotScaleBlit(const Rect& dest, Surface * pSrc, CoordF srcCenter, float rotationDegrees, float scale)
	{
		if (scale <= 0.f)
			return;

		CoordF	src;
		float	mtx[2][2];

		float	sz = (float)sin(-rotationDegrees*3.14159265/180);
		float	cz = (float)cos(-rotationDegrees*3.14159265 / 180);

		scale = 1.f / scale;

		mtx[0][0] = cz * scale;
		mtx[0][1] = sz * scale;

		mtx[1][0] = -sz * scale;
		mtx[1][1] = cz * scale;

		src = srcCenter;

		src.x -= dest.w / 2.f * mtx[0][0] + dest.h / 2.f * mtx[1][0];
		src.y -= dest.w / 2.f * mtx[0][1] + dest.h / 2.f * mtx[1][1];


		transformBlit(dest, pSrc, { src.x,src.y }, mtx);
	}

	//_____ blitFromCanvas() ______________________________________________

	void GfxDevice::blitFromCanvas(Coord dest, Surface* pSrc, const Rect& src)
	{
		blit(dest, pSrc, src);				// Default is a normal blit, only OpenGL needs to flip (until that has been fixed)
	}
	
	//____ blitNinePatch() ________________________________________________

	void GfxDevice::blitNinePatch(const Rect& dstRect, const Border& dstFrame, Surface* pSrc, const Rect& srcRect, const Border& srcFrame)
	{
		if (srcRect.w == dstRect.w && srcRect.h == dstRect.h && srcFrame == dstFrame )
		{
			blit(dstRect.pos(), pSrc, srcRect);
			return;
		}

		if (srcFrame.isEmpty() || dstFrame.isEmpty())
		{
			stretchBlit(dstRect.pos(), pSrc, srcRect);
			return;
		}

		//TODO: Optimize! Use blit instead of stretchBlit on opportunity,fill if center is only 1 pixel and just blit corners if not stretched.

		Size srcMidSize(srcRect.w - srcFrame.left - srcFrame.right, srcRect.h - srcFrame.top - srcFrame.bottom);
		Size dstMidSize(dstRect.w - dstFrame.left - dstFrame.right, dstRect.h - dstFrame.top - dstFrame.bottom);

		if (srcFrame.top + dstFrame.top > 0)
		{
			Rect	srcNW(srcRect.x, srcRect.y, srcFrame.left, srcFrame.top);
			Rect	srcN(srcRect.x + srcFrame.left, srcRect.y, srcMidSize.w, srcFrame.top);
			Rect	srcNE(srcRect.x + srcRect.w-srcFrame.right, srcRect.y, srcFrame.right, srcFrame.top);

			Rect	dstNW(dstRect.x, dstRect.y, dstFrame.left, dstFrame.top);
			Rect	dstN(dstRect.x + dstFrame.left, dstRect.y, dstMidSize.w, dstFrame.top);
			Rect	dstNE(dstRect.x + dstRect.w - dstFrame.right, dstRect.y, dstFrame.right, dstFrame.top);

			if (srcNW.w + dstNW.w > 0)
				stretchBlit(dstNW, pSrc, srcNW);

			if (srcN.w + dstN.w > 0)
				stretchBlit(dstN, pSrc, srcN);

			if (srcNE.w + dstNE.w > 0)
				stretchBlit(dstNE, pSrc, srcNE);
		}


		if (srcMidSize.h > 0 && dstMidSize.h > 0)
		{
			Rect	srcW(srcRect.x, srcRect.y + srcFrame.top, srcFrame.left, srcMidSize.h);
			Rect	srcC(srcRect.x + srcFrame.left, srcRect.y + srcFrame.top, srcMidSize.w, srcMidSize.h);
			Rect	srcE(srcRect.x + srcRect.w - srcFrame.right, srcRect.y + srcFrame.top, srcFrame.right, srcMidSize.h);

			Rect	dstW(dstRect.x, dstRect.y + dstFrame.top, dstFrame.left, dstMidSize.h);
			Rect	dstC(dstRect.x + dstFrame.left, dstRect.y + dstFrame.top, dstMidSize.w, dstMidSize.h);
			Rect	dstE(dstRect.x + dstRect.w - dstFrame.right, dstRect.y + dstFrame.top, dstFrame.right, dstMidSize.h);

			if (srcW.w + dstW.w > 0)
				stretchBlit(dstW, pSrc, srcW);

			if (srcC.w + dstC.w > 0)
				stretchBlit(dstC, pSrc, srcC);

			if (srcE.w + dstE.w > 0)
				stretchBlit(dstE, pSrc, srcE);
		}

		if (srcFrame.bottom + dstFrame.bottom > 0)
		{
			Rect	srcSW(srcRect.x, srcRect.y + srcRect.h - srcFrame.bottom, srcFrame.left, srcFrame.bottom);
			Rect	srcS(srcRect.x + srcFrame.left, srcRect.y + srcRect.h - srcFrame.bottom, srcMidSize.w, srcFrame.bottom);
			Rect	srcSE(srcRect.x + srcRect.w - srcFrame.right, srcRect.y + srcRect.h - srcFrame.bottom, srcFrame.right, srcFrame.bottom);

			Rect	dstSW(dstRect.x, dstRect.y + dstRect.h - dstFrame.bottom, dstFrame.left, dstFrame.bottom);
			Rect	dstS(dstRect.x + dstFrame.left, dstRect.y + dstRect.h - dstFrame.bottom, dstMidSize.w, dstFrame.bottom);
			Rect	dstSE(dstRect.x + dstRect.w - dstFrame.right, dstRect.y + dstRect.h - dstFrame.bottom, dstFrame.right, dstFrame.bottom);

			if (srcSW.w + dstSW.w > 0)
				stretchBlit(dstSW, pSrc, srcSW);

			if (srcS.w + dstS.w > 0)
				stretchBlit(dstS, pSrc, srcS);

			if (srcSE.w + dstSE.w > 0)
				stretchBlit(dstSE, pSrc, srcSE);
		}

	}

	//____ blitHorrBar() ______________________________________________________
	
	void GfxDevice::blitHorrBar( Surface * _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, Coord dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		blit( dest, _pSurf, r );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		dest.x += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				blit( dest, _pSurf, r );
				_len -= r.w;
				dest.x += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				blit( dest, _pSurf, r );
				dest.x += _len;
			}
		}
		else
		{
			stretchBlit( Rect(dest, _len, r.h), _pSurf, r );
			dest.x += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		blit( dest, _pSurf, r );
	}
	
	//____ blitVertBar() ______________________________________________________
	
	void GfxDevice::blitVertBar(	Surface * _pSurf,
									  	const Rect& _src, const Border& _borders,
									  	bool _bTile, Coord dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		blit( dest, _pSurf, r );
	
		_len -= _borders.height();			// Remove top and bottom edges from len.
		dest.y += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				blit( dest, _pSurf, r );
				_len -= r.h;
				dest.y += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				blit( dest, _pSurf, r );
				dest.y += _len;
			}
		}
		else
		{
			stretchBlit( Rect(dest, r.w, _len), _pSurf, r );
			dest.y += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		blit( dest, _pSurf, r );
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
