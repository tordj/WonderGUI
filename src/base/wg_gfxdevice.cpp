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

#include <algorithm>

#include <wg_gfxdevice.h>
#include <wg_geo.h>
#include <wg_util.h>
#include <wg_base.h>

using namespace std;

namespace wg 
{
	
	const char GfxDevice::CLASSNAME[] = {"GfxDevice"};
	
	int GfxDevice::s_gfxDeviceCount = 0;
	int *	GfxDevice::s_pCurveTab = nullptr;

	// Transforms for flipping movement over SOURCE when blitting

	static const int blitFlipTransforms[GfxFlip_size][2][2] = { { 1,0,0,1 },			// Normal
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

	static const int blitFlipOffsets[GfxFlip_size][2] = {	{ 0,0 },				// Normal
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

	// Transforms for flipping movement over CANVAS when drawing

	static const int drawFlipTransforms[GfxFlip_size][2][2] = { { 1,0,0,1 },		// Normal
															{ -1,0,0,1 },			// FlipX
															{ 1,0,0,-1 },			// FlipY
															{ 0,1,-1,0 },			// Rot90
															{ 0,1,1,0 },			// Rot90FlipX
															{ 0,-1,-1,0 },			// Rot90FlipY
															{ -1,0,0,-1 },			// Rot180
															{ 1,0,0,-1 },			// Rot180FlipX
															{ -1,0,0,1 },			// Rot180FlipY
															{ 0,-1,1,0 },			// Rot270
															{ 0,-1,-1,0 },			// Rot270FlipX
															{ 0,1,1,0 } };			// Rot270FlipY


	




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
		if (blendMode == BlendMode::Undefined)
			m_blendMode = BlendMode::Blend;
		else
			m_blendMode = blendMode;
			
		return true;
	}
	
	//____ setBlitSource() ____________________________________________________

	bool GfxDevice::setBlitSource(Surface * pSource)
	{
		m_pBlitSource = pSource;
		return true;
	}

	//____ beginRender() ___________________________________________________________
	
	bool GfxDevice::beginRender()
	{
		return true;	// Assumed to be ok if device doesn't have its own method.
	}
	
	//____ endRender() ________________________________________________________
	
	bool GfxDevice::endRender()
	{
		return true;	// Assumed to be ok if device doesn't have its own method.
	}

	//____ fill() _____________________________________________________________

	void GfxDevice::fill(const Color& col)
	{
		fillPatches(m_clip, col, 1, &m_clip);
	}

	void GfxDevice::fill(const Rect& rect, const Color& col)
	{
		fillPatches(rect, col, 1, &m_clip );
	}

	void GfxDevice::fill(const RectF& rect, const Color& col)
	{
		fillPatches(rect, col, 1, &m_clip);
	}

	//____ plotPixels() _______________________________________________________

	void GfxDevice::plotPixels(int nCoords, const Coord * pCoords, const Color * pColors)
	{
		plotPixelPatches(nCoords, pCoords, pColors, 1, &m_clip);
	}

	//____ drawLine() _________________________________________________________

	void GfxDevice::drawLine(Coord begin, Coord end, Color color, float thickness)
	{
		drawLinePatches(begin, end, color, thickness, 1, &m_clip);
	}

	void GfxDevice::drawLine(Coord begin, Direction dir, int length, Color color, float thickness)
	{
		drawLinePatches(begin, dir, length, color, thickness, 1, &m_clip);
	}

	//____ fillPatches() ______________________________________________________

	void GfxDevice::fillPatches(const Color& col, int nPatches, const Rect * pPatches)
	{
		fillPatches(m_clip, col, nPatches, pPatches);
	}

	//____ drawLinePatches() __________________________________________________

	void GfxDevice::drawLinePatches(Coord begin, Direction dir, int length, Color color, float thickness, int nPatches, const Rect * pPatches)
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

		drawLinePatches(begin, end, color, thickness, nPatches, pPatches);
	}


	//____ blit() __________________________________________________________________
	
	void GfxDevice::blit(Coord dest)
	{
		transformBlit({ dest, m_pBlitSource->size() }, { 0,0 }, blitFlipTransforms[0]);
	}

	void GfxDevice::blit(Coord dest, const Rect& src)
	{
		transformBlit({ dest, src.size() }, src.pos(), blitFlipTransforms[0]);
	}

	//____ flipBlit() _________________________________________________________

	void GfxDevice::flipBlit(Coord dest, GfxFlip flip)
	{
		Size srcSize  = m_pBlitSource->size();

		int ofsX = (srcSize.w - 1) * blitFlipOffsets[(int)flip][0];
		int ofsY = (srcSize.h - 1) * blitFlipOffsets[(int)flip][1];

		Size dstSize = srcSize;
		if (blitFlipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);

		transformBlit({ dest, dstSize }, { ofsX, ofsY }, blitFlipTransforms[(int)flip]);
	}

	void GfxDevice::flipBlit(Coord dest, const Rect& src, GfxFlip flip)
	{
		int ofsX = src.x + (src.w-1) * blitFlipOffsets[(int)flip][0];
		int ofsY = src.y + (src.h-1) * blitFlipOffsets[(int)flip][1];

		Size dstSize = m_pBlitSource->size();
		if (blitFlipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);


		transformBlit({ dest, dstSize }, src.pos() + Coord(ofsX, ofsY), blitFlipTransforms[(int)flip]);
	}



	//____ stretchBlit() ___________________________________________________________
	
	void GfxDevice::stretchBlit(const Rect& dest )
	{
		stretchBlit(dest, Rect(0, 0, m_pBlitSource->size()) );
	}
	
	void GfxDevice::stretchBlit(const Rect& dest, const Rect& _src )
	{
		RectF src{ (float)_src.x, (float)_src.y, (float)_src.w, (float)_src.h };

		float	mtx[2][2];

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
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

		transformBlit(dest, { src.x, src.y }, mtx);
	}
	
	void GfxDevice::stretchBlit(const Rect& dest, const RectF& src)
	{
		float	mtx[2][2];

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
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

		transformBlit(dest, { src.x,src.y }, mtx );
	}

	//____ stretchFlipBlit() _____________________________________________________
	
	void GfxDevice::stretchFlipBlit(const Rect& dest, GfxFlip flip)
	{
		stretchFlipBlit(dest, Rect(0, 0, m_pBlitSource->size()), flip);
	}

	void GfxDevice::stretchFlipBlit(const Rect& dest, const Rect& src, GfxFlip flip)
	{
		float scaleX, scaleY;
		float ofsX, ofsY;

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
		{
			float srcW = (float)(src.w - 1);
			float srcH = (float)(src.h - 1);

			scaleX = srcW / (dest.w-1);
			scaleY = srcH / (dest.h-1);

			ofsX = src.x + 0.5f + srcW * blitFlipOffsets[(int)flip][0];
			ofsY = src.y + 0.5f + srcH * blitFlipOffsets[(int)flip][1];
		}
		else
		{
			float srcW = (float)src.w;
			float srcH = (float)src.h;

			scaleX = srcW / dest.w;
			scaleY = srcH / dest.h;

			ofsX = src.x + (srcW - scaleX) * blitFlipOffsets[(int)flip][0];
			ofsY = src.y + (srcH - scaleY) * blitFlipOffsets[(int)flip][1];
		}

		float	mtx[2][2];

		mtx[0][0] = scaleX * blitFlipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * blitFlipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * blitFlipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * blitFlipTransforms[(int)flip][1][1];

		transformBlit(dest, { ofsX, ofsY }, mtx);
	}

	void GfxDevice::stretchFlipBlit(const Rect& dest, const RectF& src, GfxFlip flip)
	{
		float	scaleX = src.w / dest.w;
		float	scaleY = src.h / dest.h;

		float ofsX = src.x + (src.w-scaleX) * blitFlipOffsets[(int)flip][0];
		float ofsY = src.y + (src.h-scaleY) * blitFlipOffsets[(int)flip][1];

		float	mtx[2][2];

		mtx[0][0] = scaleX * blitFlipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * blitFlipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * blitFlipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * blitFlipTransforms[(int)flip][1][1];

		transformBlit(dest, { src.x,src.y }, mtx);
	}

	//____ rotScaleBlit() _____________________________________________________

	void GfxDevice::rotScaleBlit(const Rect& dest, CoordF srcCenter, float rotationDegrees, float scale)
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

		transformBlit(dest, { src.x,src.y }, mtx);
	}

	//____ blitPatches() _________________________________________________________

	void GfxDevice::blitPatches(Coord dest, int nPatches, const Rect * pPatches)
	{
		transformBlitPatches({ dest, m_pBlitSource->size() }, { 0,0 }, blitFlipTransforms[0], nPatches, pPatches);
	}

	void GfxDevice::blitPatches(Coord dest, const Rect& src, int nPatches, const Rect * pPatches)
	{
		transformBlitPatches({ dest, src.size() }, src.pos(), blitFlipTransforms[0], nPatches, pPatches);
	}

	//____ flipBlitPatches() _________________________________________________________

	void GfxDevice::flipBlitPatches(Coord dest, GfxFlip flip, int nPatches, const Rect * pPatches)
	{
		Size srcSize = m_pBlitSource->size();

		int ofsX = (srcSize.w - 1) * blitFlipOffsets[(int)flip][0];
		int ofsY = (srcSize.h - 1) * blitFlipOffsets[(int)flip][1];

		Size dstSize = srcSize;
		if (blitFlipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);

		transformBlitPatches({ dest, dstSize }, { ofsX, ofsY }, blitFlipTransforms[(int)flip], nPatches, pPatches );
	}

	void GfxDevice::flipBlitPatches(Coord dest, const Rect& src, GfxFlip flip, int nPatches, const Rect * pPatches)
	{
		int ofsX = src.x + (src.w - 1) * blitFlipOffsets[(int)flip][0];
		int ofsY = src.y + (src.h - 1) * blitFlipOffsets[(int)flip][1];

		Size dstSize = m_pBlitSource->size();
		if (blitFlipTransforms[(int)flip][0][0] == 0)
			swap(dstSize.w, dstSize.h);

		transformBlitPatches({ dest, dstSize }, src.pos() + Coord(ofsX, ofsY), blitFlipTransforms[(int)flip], nPatches, pPatches);
	}

	//____ stretchBlitPatches() _______________________________________________

	void GfxDevice::stretchBlitPatches(const Rect& dest, int nPatches, const Rect * pPatches)
	{
		stretchBlitPatches(dest, Rect(0, 0, m_pBlitSource->size()), nPatches, pPatches);
	}

	void GfxDevice::stretchBlitPatches(const Rect& dest, const Rect& _src, int nPatches, const Rect * pPatches)
	{
		RectF src{ (float)_src.x, (float)_src.y, (float)_src.w, (float)_src.h };

		float	mtx[2][2];

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
		{
			src.x += 0.5f;
			src.y += 0.5f;

			mtx[0][0] = (src.w - 1) / (dest.w - 1);
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = (src.h - 1) / (dest.h - 1);
		}
		else
		{
			mtx[0][0] = src.w / dest.w;
			mtx[0][1] = 0;
			mtx[1][0] = 0;
			mtx[1][1] = src.h / dest.h;
		}

		transformBlitPatches(dest, { src.x, src.y }, mtx, nPatches, pPatches);
	}

	void GfxDevice::stretchBlitPatches(const Rect& dest, const RectF& src, int nPatches, const Rect * pPatches)
	{
		float	mtx[2][2];

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
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

		transformBlitPatches(dest, { src.x,src.y }, mtx, nPatches, pPatches);
	}

	//____ stretchFlipBlitPatches() _____________________________________________________

	void GfxDevice::stretchFlipBlitPatches(const Rect& dest, GfxFlip flip, int nPatches, const Rect * pPatches)
	{
		stretchFlipBlitPatches(dest, Rect(0, 0, m_pBlitSource->size()), flip, nPatches, pPatches);
	}

	void GfxDevice::stretchFlipBlitPatches(const Rect& dest, const Rect& src, GfxFlip flip, int nPatches, const Rect * pPatches)
	{
		float scaleX, scaleY;
		float ofsX, ofsY;

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
		{
			float srcW = (float)(src.w - 1);
			float srcH = (float)(src.h - 1);

			scaleX = srcW / (dest.w - 1);
			scaleY = srcH / (dest.h - 1);

			ofsX = src.x + 0.5f + srcW * blitFlipOffsets[(int)flip][0];
			ofsY = src.y + 0.5f + srcH * blitFlipOffsets[(int)flip][1];
		}
		else
		{
			float srcW = (float)src.w;
			float srcH = (float)src.h;

			scaleX = srcW / dest.w;
			scaleY = srcH / dest.h;

			ofsX = src.x + (srcW - scaleX) * blitFlipOffsets[(int)flip][0];
			ofsY = src.y + (srcH - scaleY) * blitFlipOffsets[(int)flip][1];
		}

		float	mtx[2][2];

		mtx[0][0] = scaleX * blitFlipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * blitFlipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * blitFlipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * blitFlipTransforms[(int)flip][1][1];

		transformBlitPatches(dest, { ofsX, ofsY }, mtx, nPatches, pPatches);
	}

	void GfxDevice::stretchFlipBlitPatches(const Rect& dest, const RectF& src, GfxFlip flip, int nPatches, const Rect * pPatches)
	{
		float	scaleX = src.w / dest.w;
		float	scaleY = src.h / dest.h;

		float ofsX = src.x + (src.w - scaleX) * blitFlipOffsets[(int)flip][0];
		float ofsY = src.y + (src.h - scaleY) * blitFlipOffsets[(int)flip][1];

		float	mtx[2][2];

		mtx[0][0] = scaleX * blitFlipTransforms[(int)flip][0][0];
		mtx[0][1] = scaleY * blitFlipTransforms[(int)flip][0][1];
		mtx[1][0] = scaleX * blitFlipTransforms[(int)flip][1][0];
		mtx[1][1] = scaleY * blitFlipTransforms[(int)flip][1][1];

		transformBlitPatches(dest, { src.x,src.y }, mtx, nPatches, pPatches);
	}

	//____ rotScaleBlitPatches() _____________________________________________________

	void GfxDevice::rotScaleBlitPatches(const Rect& dest, CoordF srcCenter, float rotationDegrees, float scale, int nPatches, const Rect * pPatches)
	{
		if (scale <= 0.f)
			return;

		CoordF	src;
		float	mtx[2][2];

		float	sz = (float)sin(-rotationDegrees * 3.14159265 / 180);
		float	cz = (float)cos(-rotationDegrees * 3.14159265 / 180);

		scale = 1.f / scale;

		mtx[0][0] = cz * scale;
		mtx[0][1] = sz * scale;

		mtx[1][0] = -sz * scale;
		mtx[1][1] = cz * scale;

		src = srcCenter;

		src.x -= dest.w / 2.f * mtx[0][0] + dest.h / 2.f * mtx[1][0];
		src.y -= dest.w / 2.f * mtx[0][1] + dest.h / 2.f * mtx[1][1];

		transformBlitPatches(dest, { src.x,src.y }, mtx, nPatches, pPatches);
	}


	//_____ blitFromCanvas() ______________________________________________

	void GfxDevice::blitFromCanvas(Coord dest, const Rect& src)
	{
		blit(dest, src);				// Default is a normal blit, only OpenGL needs to flip (until that has been fixed)
	}
	
	//____ blitNinePatch() ________________________________________________

	void GfxDevice::blitNinePatch(const Rect& dstRect, const Border& dstFrame, const Rect& srcRect, const Border& srcFrame)
	{
		if (srcRect.w == dstRect.w && srcRect.h == dstRect.h && srcFrame == dstFrame )
		{
			blit(dstRect.pos(), srcRect);
			return;
		}

		if (srcFrame.isEmpty() || dstFrame.isEmpty())
		{
			stretchBlit(dstRect.pos(), srcRect);
			return;
		}

		//TODO: Optimize! Call transformBlit directly instead of going through stretchBlit(), reuse transforms where possible.
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
				stretchBlit(dstNW, srcNW);

			if (srcN.w + dstN.w > 0)
				stretchBlit(dstN, srcN);

			if (srcNE.w + dstNE.w > 0)
				stretchBlit(dstNE, srcNE);
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
				stretchBlit(dstW, srcW);

			if (srcC.w + dstC.w > 0)
				stretchBlit(dstC, srcC);

			if (srcE.w + dstE.w > 0)
				stretchBlit(dstE, srcE);
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
				stretchBlit(dstSW, srcSW);

			if (srcS.w + dstS.w > 0)
				stretchBlit(dstS, srcS);

			if (srcSE.w + dstSE.w > 0)
				stretchBlit(dstSE, srcSE);
		}

	}

	//____ blitNinePatchPatches() ________________________________________________

	void GfxDevice::blitNinePatchPatches(const Rect& dstRect, const Border& dstFrame, const Rect& srcRect, const Border& srcFrame, int nPatches, const Rect * pPatches )
	{
		if (srcRect.w == dstRect.w && srcRect.h == dstRect.h && srcFrame == dstFrame)
		{
			blitPatches(dstRect.pos(), srcRect, nPatches, pPatches);
			return;
		}

		if (srcFrame.isEmpty() || dstFrame.isEmpty())
		{
			stretchBlitPatches(dstRect.pos(), srcRect, nPatches, pPatches);
			return;
		}

		//TODO: Optimize! Clip patches against our geometry first.
		//TODO: Optimize! Call transformBlit directly instead of going through stretchBlit(), reuse transforms where possible.
		//TODO: Optimize! Use blit instead of stretchBlit on opportunity,fill if center is only 1 pixel and just blit corners if not stretched.

		Size srcMidSize(srcRect.w - srcFrame.left - srcFrame.right, srcRect.h - srcFrame.top - srcFrame.bottom);
		Size dstMidSize(dstRect.w - dstFrame.left - dstFrame.right, dstRect.h - dstFrame.top - dstFrame.bottom);

		if (srcFrame.top + dstFrame.top > 0)
		{
			Rect	srcNW(srcRect.x, srcRect.y, srcFrame.left, srcFrame.top);
			Rect	srcN(srcRect.x + srcFrame.left, srcRect.y, srcMidSize.w, srcFrame.top);
			Rect	srcNE(srcRect.x + srcRect.w - srcFrame.right, srcRect.y, srcFrame.right, srcFrame.top);

			Rect	dstNW(dstRect.x, dstRect.y, dstFrame.left, dstFrame.top);
			Rect	dstN(dstRect.x + dstFrame.left, dstRect.y, dstMidSize.w, dstFrame.top);
			Rect	dstNE(dstRect.x + dstRect.w - dstFrame.right, dstRect.y, dstFrame.right, dstFrame.top);

			if (srcNW.w + dstNW.w > 0)
				stretchBlitPatches(dstNW, srcNW, nPatches, pPatches);

			if (srcN.w + dstN.w > 0)
				stretchBlitPatches(dstN, srcN, nPatches, pPatches);

			if (srcNE.w + dstNE.w > 0)
				stretchBlitPatches(dstNE, srcNE, nPatches, pPatches);
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
				stretchBlitPatches(dstW, srcW, nPatches, pPatches);

			if (srcC.w + dstC.w > 0)
				stretchBlitPatches(dstC, srcC, nPatches, pPatches);

			if (srcE.w + dstE.w > 0)
				stretchBlitPatches(dstE, srcE, nPatches, pPatches);
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
				stretchBlitPatches(dstSW, srcSW, nPatches, pPatches);

			if (srcS.w + dstS.w > 0)
				stretchBlitPatches(dstS, srcS, nPatches, pPatches);

			if (srcSE.w + dstSE.w > 0)
				stretchBlitPatches(dstSE, srcSE, nPatches, pPatches);
		}

	}


	//____ drawWave() ___________________________________________________

	void GfxDevice::drawWave(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill)
	{
		transformDrawWavePatches(dest, pTopBorder, pBottomBorder, frontFill, backFill, drawFlipTransforms[(int)GfxFlip::Normal], 1, &m_clip);
	}

	//____ flipDrawWave() ___________________________________________________

	void GfxDevice::flipDrawWave(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip)
	{
		flipDrawWavePatches(dest, pTopBorder, pBottomBorder, frontFill, backFill, flip, 1, &m_clip);
	}

	//____ drawElipse() ___________________________________________________

	void GfxDevice::drawElipse(const RectF& canvas, float thickness, Color fillColor, float outlineThickness, Color outlineColor)
	{
		Rect patch((int)canvas.x, (int)canvas.y, (int)canvas.w + 1, (int)canvas.h + 1);

		drawElipsePatches(canvas, thickness, fillColor, outlineThickness, outlineColor, 1, &patch);
	}

	//____ drawSegments() ______________________________________________________

	void GfxDevice::drawSegments(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch)
	{
		transformDrawSegmentPatches(dest, nSegments, pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, drawFlipTransforms[(int)GfxFlip::Normal], 1, &dest );
	}

	//____ flipDrawSegments() ______________________________________________________

	void GfxDevice::flipDrawSegments(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip)
	{
		transformDrawSegmentPatches(dest, nSegments, pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, drawFlipTransforms[(int)flip], 1, &dest );
	}

	//____ drawWavePatches() ______________________________________________________

	void GfxDevice::drawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, int nPatches, const Rect * pPatches)
	{
		transformDrawWavePatches(dest, pTopBorder, pBottomBorder, frontFill, backFill, drawFlipTransforms[(int)GfxFlip::Normal], nPatches, pPatches);
	}

	//____ flipDrawWavePatches() ______________________________________________________

	void GfxDevice::flipDrawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip, int nPatches, const Rect * pPatches)
	{
		transformDrawWavePatches(dest, pTopBorder, pBottomBorder, frontFill, backFill, drawFlipTransforms[(int)flip], nPatches, pPatches);
	}

	//____ transformDrawWavePatches() ______________________________________________________

	void GfxDevice::transformDrawWavePatches(const Rect& _dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{
		//TODO: If borders have different colors and cross, colors are not swapped.

		// Make a bounding box around all patches (after clipping the patches)

		Rect bounds;
		int patch = 0;
		while( (bounds.w == 0 ||bounds.h == 0) && patch < nPatches )
			bounds = Rect(pPatches[patch++], m_clip);

		while (patch < nPatches)
		{
			Rect clipped(m_clip, pPatches[patch++]);
			if (clipped.w != 0 && clipped.h != 0)
				bounds.growToContain(clipped);
		}

		if (bounds.w == 0 || bounds.h == 0)
			return;

		// Generate new destination rect and get waveLength, traceLength and clipping info.

		Rect	dest;
		int		clipBeg, clipEnd;
		int		length;
		int		traceLength;

		if (simpleTransform[0][0] != 0)								// Wave is horizontal
		{
			traceLength = _dest.w;
			dest = { bounds.x, _dest.y, bounds.w, _dest.h };
			clipBeg = bounds.x - _dest.x;
			clipEnd = (_dest.x + _dest.w) - (bounds.x + bounds.w);

			if (simpleTransform[0][0] < 0)							// Wave goes from right to left
				swap(clipBeg, clipEnd);

			length = bounds.w;
		}
		else														// Wave is vertical
		{
			traceLength = _dest.h;
			dest = { _dest.x, bounds.y, _dest.w, bounds.h };
			clipBeg = bounds.y - _dest.y;
			clipEnd = (_dest.y + _dest.h) - (bounds.y + bounds.h);

			if (simpleTransform[0][1] < 0)							// Wave goes from bottom to top
				swap(clipBeg, clipEnd);

			length = bounds.h;
		}

		// Do clipping of our trace

		int traceOfs = 0;
		int startColumn = clipBeg;										// Column of traced lines we start to generate edges from.

		int margin = (int)(max(pTopBorder->thickness, pBottomBorder->thickness) / 2 + 0.99);

		if (clipBeg > margin)
		{
			traceOfs = clipBeg - margin;
			traceLength -= traceOfs;
			startColumn -= traceOfs;
		}

		if (clipEnd > margin)
			traceLength -= clipEnd - margin;

		// Generate line traces

		int	lineBufferSize = (traceLength + 1) * 2 * sizeof(int) * 2;	// length+1 * values per point * sizeof(int) * 2 separate traces.
		char * pBuffer = Base::memStackAlloc(lineBufferSize);
		int * pTopBorderTrace = (int*)pBuffer;
		int * pBottomBorderTrace = (int*)(pBuffer + lineBufferSize / 2);

		_traceLine(pTopBorderTrace, traceLength + 1, pTopBorder, traceOfs);
		_traceLine(pBottomBorderTrace, traceLength + 1, pBottomBorder, traceOfs);




		// Generate edges

		int edgeBufferSize;

		if (frontFill == backFill)
		{
			// Generate edges

			edgeBufferSize = (length + 1) * 4 * sizeof(int);
			int *	pEdgeBuffer = (int*)Base::memStackAlloc(edgeBufferSize);
			int *	pEdges = pEdgeBuffer;

			for (int i = startColumn; i <= length + startColumn; i++)
			{
				pEdges[0] = pTopBorderTrace[i * 2];
				pEdges[1] = pTopBorderTrace[i * 2 + 1];

				pEdges[2] = pBottomBorderTrace[i * 2];
				pEdges[3] = pBottomBorderTrace[i * 2 + 1];

				if (pTopBorderTrace[i * 2] > pBottomBorderTrace[i * 2])
					swap(pTopBorderTrace, pBottomBorderTrace);

				if (pEdges[2] < pEdges[1])
				{
					pEdges[2] = pEdges[1];
					if (pEdges[3] < pEdges[2])
						pEdges[3] = pEdges[2];
				}

				pEdges += 4;
			}

			// Render the segments 

			Color	col[5];

			col[0] = Color::Transparent;
			col[1] = pTopBorder->color;
			col[2] = frontFill;
			col[3] = pBottomBorder->color;
			col[4] = Color::Transparent;

			transformDrawSegmentPatches(dest, 5, col, length + 1, pEdgeBuffer, 4, simpleTransform, nPatches, pPatches);

		}
		else
		{
			edgeBufferSize = (length + 1) * 5 * sizeof(int);
			int *	pEdgeBuffer = (int*)Base::memStackAlloc(edgeBufferSize);
			int *	pEdges = pEdgeBuffer;
			int		midEdgeFollows = 3;

			for (int i = startColumn; i <= length + startColumn; i++)
			{
				pEdges[0] = pTopBorderTrace[i * 2];
				pEdges[1] = pTopBorderTrace[i * 2 + 1];

				pEdges[3] = pBottomBorderTrace[i * 2];
				pEdges[4] = pBottomBorderTrace[i * 2 + 1];

				if (pTopBorderTrace[i * 2] > pBottomBorderTrace[i * 2])
				{
					swap(pTopBorderTrace, pBottomBorderTrace);
					midEdgeFollows ^= 0x2;												// Swap between 1 and 3.
				}

				if (pEdges[3] < pEdges[1])
				{
					pEdges[3] = pEdges[1];
					if (pEdges[4] < pEdges[3])
						pEdges[4] = pEdges[3];
				}

				pEdges[2] = pEdges[midEdgeFollows];
				pEdges += 5;
			}

			// Render the segments 

			Color	col[6];

			col[0] = Color::Transparent;
			col[1] = pTopBorder->color;
			col[2] = frontFill;
			col[3] = backFill;
			col[4] = pBottomBorder->color;
			col[5] = Color::Transparent;

			transformDrawSegmentPatches(dest, 6, col, length + 1, pEdgeBuffer, 5, simpleTransform, nPatches, pPatches);
		}


		// Free temporary work memory

		Base::memStackRelease(edgeBufferSize);
		Base::memStackRelease(lineBufferSize);
	}

	//____ drawElipsePatches() ______________________________________________________

	void GfxDevice::drawElipsePatches(const RectF& canvas, float thickness, Color fillColor, float outlineThickness, Color outlineColor, int nPatches, const Rect * pPatches)
	{
		// Center and corners in 24.8 format.

		int x1 = (int)(canvas.x * 256);
		int y1 = (int)(canvas.y * 256);
		int x2 = (int)((canvas.x + canvas.w) * 256);
		int y2 = (int)((canvas.y + canvas.h) * 256);

		Coord center = { (x1 + x2) / 2, (y1 + y2) / 2 };

		// Outer rect of elipse rounded to full pixels.

		Rect outerRect;
		outerRect.x = x1 >> 8;
		outerRect.y = y1 >> 8;
		outerRect.w = ((x2 + 255) >> 8) - outerRect.x;
		outerRect.h = ((y2 + 255) >> 8) - outerRect.y;

		// Adjusted clip

		Rect clip(m_clip, outerRect);
		if (clip.w == 0 || clip.h == 0)
			return;

		int clipLeft = clip.x - outerRect.x;

		// Calculate maximum width and height from center for the 4 edges of the elipse.

		int radiusY[4];
		radiusY[0] = (y2 - y1) / 2;
		radiusY[1] = (int)(radiusY[0] - (outlineThickness * 256));
		radiusY[2] = (int)(radiusY[1] - (thickness * 256));
		radiusY[3] = (int)(radiusY[2] - (outlineThickness * 256));

		int radiusX[4];
		radiusX[0] = (x2 - x1) / 2;
		radiusX[1] = (int)(radiusX[0] - (outlineThickness * 256));
		radiusX[2] = (int)(radiusX[1] - (thickness * 256));
		radiusX[3] = (int)(radiusX[2] - (outlineThickness * 256));

		// Reserve buffer for our line traces

		int samplePoints = clip.w + 1;

		int bufferSize = samplePoints * sizeof(int) * 4 * 2;		// length+1 * sizeof(int) * 4 separate traces * 2 halves.
		int * pBuffer = (int*)Base::memStackAlloc(bufferSize);

		// Do line traces.

		int yMid = (center.y & 0xFFFFFF00) - outerRect.y * 256;
		int yAdjust = center.y & 0xFF;						// Compensate for center not being on pixel boundary.
		int centerOfs = center.x - (outerRect.x << 8);
		int samplePitch = 4;

		for (int edge = 0; edge < 4; edge++)
		{
			int * pOutUpper = pBuffer + edge;
			int * pOutLower = pBuffer + 3 - edge + samplePoints * 4;

			if (radiusX[edge] <= 0 || radiusY[edge] <= 0)
			{
				for (int sample = 0; sample < samplePoints; sample++)
				{
					pOutUpper[sample*samplePitch] = yMid;
					pOutLower[sample*samplePitch] = yMid;
				}
			}
			else
			{
				int xStart = (centerOfs - radiusX[edge] + 255) >> 8;		// First pixel-edge inside curve.
				int xMid = centerOfs >> 8;								// Pixel edge on or right before center.
				int xEnd = (centerOfs + radiusX[edge]) >> 8;				// Last pixel-edge inside curve.


				int curveInc = (int)(((int64_t)65536) * 256 * (c_nCurveTabEntries - 1) / radiusX[edge]); // Keep as many decimals as possible, this is important!
				int curvePos = (((radiusX[edge] - centerOfs) & 0xFF) * ((int64_t)curveInc)) >> 8;

				if (clipLeft > 0)
				{
					xStart -= clipLeft;
					xMid -= clipLeft;
					xEnd -= clipLeft;

					if (xStart < 0)
						curvePos += (-xStart) * curveInc;
				}

				// Clip xStart, xMid and xEnd

				if (xEnd >= samplePoints)
				{
					xEnd = samplePoints - 1;

					xStart = min(xStart, xEnd);
					xMid = min(xMid, xEnd);
				}

				//

				int sample = 0;
				while (sample < xStart)
				{
					pOutUpper[sample*samplePitch] = yMid;
					pOutLower[sample*samplePitch] = yMid;
					sample++;
				}

				while (sample <= xMid)
				{
					int i = curvePos >> 16;
					uint32_t f = curvePos & 0xFFFF;

					uint32_t heightFactor = (s_pCurveTab[i] * (65535 - f) + s_pCurveTab[i + 1] * f) >> 16;
					int height = ((radiusY[edge] >> 16) * heightFactor) + ((radiusY[edge] & 0xFFFF) * heightFactor >> 16);  // = (radiusY[edge] * heightFactor) / 65536, but without overflow.

					pOutUpper[sample*samplePitch] = yMid + yAdjust - height;
					pOutLower[sample*samplePitch] = yMid + yAdjust + height;
					sample++;
					curvePos += curveInc;
				}

				curvePos = (c_nCurveTabEntries - 1) * 65536 * 2 - curvePos;

				while (sample <= xEnd)
				{
					int i = curvePos >> 16;
					uint32_t f = curvePos & 0xFFFF;

					uint32_t heightFactor = (s_pCurveTab[i] * (65535 - f) + s_pCurveTab[i + 1] * f) >> 16;
					int height = ((radiusY[edge] >> 16) * heightFactor) + ((radiusY[edge] & 0xFFFF) * heightFactor >> 16); // = (radiusY[edge] * heightFactor) / 65536, but without overflow.

					pOutUpper[sample*samplePitch] = yMid + yAdjust - height;
					pOutLower[sample*samplePitch] = yMid + yAdjust + height;
					sample++;
					curvePos -= curveInc;
				}

				while (sample < samplePoints)
				{
					pOutUpper[sample*samplePitch] = yMid;
					pOutLower[sample*samplePitch] = yMid;
					sample++;
				}

				// Take care of left and right edges that needs more calculations to get the angle right.

				int pixFracLeft = (xStart << 8) - (centerOfs - radiusX[edge]);
				int pixFracRight = (centerOfs + radiusX[edge]) & 0xFF;

				if (pixFracLeft > 0 && xStart > 0)
				{
					pOutUpper[(xStart - 1)*samplePitch] = pOutUpper[xStart*samplePitch] + (yMid + yAdjust - pOutUpper[xStart*samplePitch]) * 256 / pixFracLeft;
					pOutLower[(xStart - 1)*samplePitch] = pOutLower[xStart*samplePitch] + (yMid + yAdjust - pOutLower[xStart*samplePitch]) * 256 / pixFracLeft;

				}
				if (pixFracRight > 0 && xEnd < samplePoints - 1)
				{
					pOutUpper[(xEnd + 1)*samplePitch] = pOutUpper[xEnd*samplePitch] + (yMid + yAdjust - pOutUpper[xEnd*samplePitch]) * 256 / pixFracRight;
					pOutLower[(xEnd + 1)*samplePitch] = pOutLower[xEnd*samplePitch] + (yMid + yAdjust - pOutLower[xEnd*samplePitch]) * 256 / pixFracRight;
				}

			}
		}

		// Split patches into upper and lower half, so we clip at the middle.

		int split = min(clip.y + clip.h, outerRect.y + (yMid >> 8));

		int patchBufferSize = sizeof(Rect)*nPatches * 2;
		Rect * pTopPatches = (Rect*)Base::memStackAlloc(patchBufferSize);
		Rect * pBottomPatches = pTopPatches + nPatches;
		int nTopPatches = 0;
		int nBottomPatches = 0;

		for (int i = 0; i < nPatches; i++)
		{
			const Rect& patch = pPatches[i];

			if (patch.y < split)
			{
				pTopPatches[nTopPatches] = patch;
				if (patch.bottom() > split)
					pTopPatches[nTopPatches].h = split - patch.y;
				nTopPatches++;
			}
			if (patch.bottom() > split)
			{
				pBottomPatches[nBottomPatches] = patch;
				if (patch.y < split)
				{
					pBottomPatches[nBottomPatches].h -= split - patch.y;
					pBottomPatches[nBottomPatches].y = split;
				}
				nBottomPatches++;
			}
		}

		// Render columns

		Color	col[5];
		col[0] = Color::Transparent;
		col[1] = outlineColor;
		col[2] = fillColor;
		col[3] = outlineColor;
		col[4] = Color::Transparent;

		drawSegmentPatches(outerRect, 5, col, samplePoints, pBuffer, 4, nTopPatches, pTopPatches);
		drawSegmentPatches(outerRect, 5, col, samplePoints, pBuffer + samplePoints * 4, 4, nBottomPatches, pBottomPatches);

		// Free temporary work memory

		Base::memStackRelease(patchBufferSize);
		Base::memStackRelease(bufferSize);

	}

	//____ drawSegmentPatches() ______________________________________________________

	void GfxDevice::drawSegmentPatches(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, int nPatches, const Rect * pPatches)
	{
		transformDrawSegmentPatches( dest, nSegments, pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, drawFlipTransforms[(int)GfxFlip::Normal], nPatches, pPatches);
	}

	//____ flipDrawSegmentPatches() ______________________________________________________

	void GfxDevice::flipDrawSegmentPatches(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, int nPatches, const Rect * pPatches)
	{
		transformDrawSegmentPatches(dest, nSegments, pSegmentColors, nEdgeStrips, pEdgeStrips, edgeStripPitch, drawFlipTransforms[(int)flip], nPatches, pPatches);
	}

	//____ transformBlit() ______________________________________________________

	void GfxDevice::transformBlit(const Rect& dest, Coord src, const int simpleTransform[2][2])
	{
		transformBlitPatches(dest, src, simpleTransform, 1, &dest);
	}

	void GfxDevice::transformBlit(const Rect& dest, CoordF src, const float complexTransform[2][2])
	{
		transformBlitPatches(dest, src, complexTransform, 1, &dest);
	}



	//____ blitHorrBar() ______________________________________________________
	
	void GfxDevice::blitHorrBar( 	  	const Rect& _src, const Border& _borders,
									  	bool _bTile, Coord dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit left edge
	
		Rect	r( _src.x, _src.y, _borders.left, _src.h );
		blit( dest, r );
	
		_len -= _borders.width();			// Remove left and right edges from len.
		dest.x += _borders.left;
	
		// Blit tiling part
	
		r.x += _borders.left;
		r.w = _src.w - _borders.width();
	
		if( _bTile )
		{
			while( _len > r.w )
			{
				blit( dest, r );
				_len -= r.w;
				dest.x += r.w;
			}
			if( _len != 0 )
			{
				r.w = _len;
				blit( dest, r );
				dest.x += _len;
			}
		}
		else
		{
			stretchBlit( Rect(dest, _len, r.h), r );
			dest.x += _len;
		}
	
		// Blit right edge
	
		r.x = _src.x + _src.w - _borders.right;
		r.w = _borders.right;
		blit( dest, r );
	}
	
	//____ blitVertBar() ______________________________________________________
	
	void GfxDevice::blitVertBar(	  	const Rect& _src, const Border& _borders,
									  	bool _bTile, Coord dest, int _len )
	{
		/*
			This can be optimized by handling clipping directly instead of calling clipBlit().
		*/
	
		// Blit top edge
	
		Rect	r( _src.x, _src.y, _src.w, _borders.top );
		blit( dest, r );
	
		_len -= _borders.height();			// Remove top and bottom edges from len.
		dest.y += _borders.top;
	
		// Blit tiling part
	
		r.y += _borders.top;
		r.h = _src.h - _borders.height();
	
		if( _bTile )
		{
			while( _len > r.h )
			{
				blit( dest, r );
				_len -= r.h;
				dest.y += r.h;
			}
			if( _len != 0 )
			{
				r.h = _len;
				blit( dest, r );
				dest.y += _len;
			}
		}
		else
		{
			stretchBlit( Rect(dest, r.w, _len), r );
			dest.y += _len;
		}
	
		// Blit bottom edge
	
		r.y = _src.y + _src.h - _borders.bottom;
		r.h = _borders.bottom;
		blit( dest, r );
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
