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
#ifndef WG_STREAMGFXDEVICE_DOT_H
#define WG_STREAMGFXDEVICE_DOT_H
#pragma once

#include <wg_geo.h>

#include <wg_gfxdevice.h>
#include <wg_cgfxoutstream.h>

namespace wg
{
	class StreamSurface;
	class StreamGfxDevice;
	typedef	StrongPtr<StreamGfxDevice> StreamGfxDevice_p;
	typedef	WeakPtr<StreamGfxDevice>	StreamGfxDevice_wp;

	class StreamGfxDevice : public GfxDevice
	{
	public:

		//.____ Creation __________________________________________

		static StreamGfxDevice_p	create( SizeI canvas, CGfxOutStream& stream );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		const TypeInfo&		surfaceType( void ) const override;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory() override;

		//.____ Geometry _________________________________________________

		bool	setCanvas( Surface * pCanvas, bool bResetClipRects = true ) override;

		//.____ State _________________________________________________

		bool	setClipList(int nRectangles, const RectI * pRectangles) override;
		void	clearClipList() override;
		void	setTintColor( Color color ) override;
		bool	setBlendMode( BlendMode blendMode ) override;
		bool	setBlitSource(Surface * pSource) override;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;

//		void	fill(const Color& col) override;
		void	fill( const RectI& rect, const Color& col ) override;
		void	fill(const RectF& rect, const Color& col) override;

		void    plotPixels( int nCoords, const CoordI * pCoords, const Color * pColors) override;

		void	drawLine( CoordI begin, CoordI end, Color color, float thickness = 1.f ) override;
		void	drawLine( CoordI begin, Direction dir, int length, Color col, float thickness = 1.f) override;


//		void	blit(CoordI dest) override;
		void	blit(CoordI dest, const RectI& srcrect ) override;

//		void	flipBlit(CoordI dest, GfxFlip flip) override;
//		void	flipBlit(CoordI dest, const RectI& src, GfxFlip flip) override;

//		void	stretchBlit(const RectI& dest) override;
//		void	stretchBlit(const RectI& dest, const RectI& src) override;
		void	stretchBlit(const RectI& dest, const RectF& src) override;

//		void	stretchFlipBlit(const RectI& dest, GfxFlip flip) override;
//		void	stretchFlipBlit(const RectI& dest, const RectI& src, GfxFlip flip) override;
//		void	stretchFlipBlit(const RectI& dest, const RectF& src, GfxFlip flip) override;

//		void	rotScaleBlit(const RectI& dest, CoordF srcCenter, float rotationDegrees, float scale) override;

		// Draw segments methods

//		void	drawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill) override;
//		void	flipDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip) override;

//		void	drawElipse(const RectF& canvas, float thickness, Color color, float outlineThickness = 0.f, Color outlineColor = Color::Black) override;

//		void	drawSegments(const RectI& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch) override;
//		void	flipDrawSegments(const RectI& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, GfxFlip flip) override;

		// Low level methods




	protected:
		StreamGfxDevice( SizeI canvas, CGfxOutStream& stream );
		~StreamGfxDevice();

		void	_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2]) override;
		void	_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2]) override;

		//		void	_transformDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, const int simpleTransform[2][2]) override;
		void	_transformDrawSegments(const RectI& dest, int nSegments, const Color * pSegmentColors, int nEdges, const int * pEdges, int edgeStripPitch, const int simpleTransform[2][2]) override;


		void _addPatches(int nPatches, const RectI * pPatches);


		SurfaceFactory_p	m_pSurfaceFactory;
		CGfxOutStream_p		m_pStream;
		bool	m_bRendering;
	};
} // namespace wg
#endif //WG_STREAMGFXDEVICE_DOT_H

