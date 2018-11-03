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
#include <wg_gfxoutstream.h>

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

		static StreamGfxDevice_p	create( Size canvas, GfxOutStream * pStream );

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static StreamGfxDevice_p	cast( Object * pObject );
		const char *			surfaceClassName( void ) const;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory();

		//.____ Geometry _________________________________________________

		bool	setCanvas( Surface * pCanvas );

		//.____ State _________________________________________________

		void	setClip(const Rect& clip) override;
		void	setTintColor( Color color ) override;
		bool	setBlendMode( BlendMode blendMode ) override;
		bool	setBlitSource(Surface * pSource) override;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;

		void	fill(const Color& col) override;
		void	fill( const Rect& rect, const Color& col ) override;
		void	fill(const RectF& rect, const Color& col) override;

        void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors) override;

		void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f ) override;
		void	drawLine( Coord begin, Direction dir, int length, Color col, float thickness = 1.f) override;

//		void	fillPatches(const Color& col, int nPatches, const Rect * pPatches) override;
		void	fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches) override;
		void	fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches) override;

		void    plotPixelPatches(int nCoords, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches) override;

		void	drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches) override;
		void	drawLinePatches(Coord begin, Direction dir, int length, Color col, float thickness, int nPatches, const Rect * pPatches) override;


//		void	blit(Coord dest) override;
		void	blit(Coord dest, const Rect& srcrect ) override;

//		void	flipBlit(Coord dest, GfxFlip flip) override;
//		void	flipBlit(Coord dest, const Rect& src, GfxFlip flip) override;

//		void	stretchBlit(const Rect& dest) override;
//		void	stretchBlit(const Rect& dest, const Rect& src) override;
		void	stretchBlit(const Rect& dest, const RectF& src) override;

//		void	stretchFlipBlit(const Rect& dest, GfxFlip flip) override;
//		void	stretchFlipBlit(const Rect& dest, const Rect& src, GfxFlip flip) override;
//		void	stretchFlipBlit(const Rect& dest, const RectF& src, GfxFlip flip) override;

//		void	rotScaleBlit(const Rect& dest, CoordF srcCenter, float rotationDegrees, float scale) override;

		// Patch blit methods

//		void	blitPatches(Coord dest, int nPatches, const Rect * pPatches) override;
//		void	blitPatches(Coord dest, const Rect& src, int nPatches, const Rect * pPatches) override;

//		void	flipBlitPatches(Coord dest, GfxFlip flip, int nPatches, const Rect * pPatches) override;
//		void	flipBlitPatches(Coord dest, const Rect& src, GfxFlip flip, int nPatches, const Rect * pPatches) override;

//		void	stretchBlitPatches(const Rect& dest, int nPatches, const Rect * pPatches) override;
//		void	stretchBlitPatches(const Rect& dest, const Rect& src, int nPatches, const Rect * pPatches) override;
//		void	stretchBlitPatches(const Rect& dest, const RectF& src, int nPatches, const Rect * pPatches) override;

//		void	stretchFlipBlitPatches(const Rect& dest, GfxFlip flip, int nPatches, const Rect * pPatches) override;
//		void	stretchFlipBlitPatches(const Rect& dest, const Rect& src, GfxFlip flip, int nPatches, const Rect * pPatches) override;
//		void	stretchFlipBlitPatches(const Rect& dest, const RectF& src, GfxFlip flip, int nPatches, const Rect * pPatches) override;

//		void	rotScaleBlitPatches(const Rect& dest, CoordF srcCenter, float rotationDegrees, float scale, int nPatches, const Rect * pPatches) override;

		// Draw segments methods

//		void	drawWave(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill) override;
//		void	flipDrawWave(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip) override;

//		void	drawElipse(const RectF& canvas, float thickness, Color color, float outlineThickness = 0.f, Color outlineColor = Color::Black) override;

//		void	drawSegments(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch) override;
//		void	flipDrawSegments(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, GfxFlip flip) override;


		// Draw segment patches methods

//		void	drawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, int nPatches, const Rect * pPatches) override;
//		void	flipDrawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip, int nPatches, const Rect * pPatches) override;

//		void	drawElipsePatches(const RectF& canvas, float thickness, Color color, float outlineThickness, Color outlineColor, int nPatches, const Rect * pPatches) override;

//		void	drawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, int nPatches, const Rect * pPatches) override;
//		void	flipDrawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, GfxFlip flip, int nPatches, const Rect * pPatches) override;


		// Low level methods

//		void	transformBlit(const Rect& dest, Coord src, const int simpleTransform[2][2]) override;
//		void	transformBlit(const Rect& dest, CoordF src, const float complexTransform[2][2]) override;

		void	transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;
		void	transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches) override;

//		void	transformDrawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;
		void	transformDrawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;



	protected:
		StreamGfxDevice( Size canvas, GfxOutStream * pStream );
		~StreamGfxDevice();

		void _addPatches(int nPatches, const Rect * pPatches);


        SurfaceFactory_p	m_pSurfaceFactory;
		GfxOutStream_p		m_pStream;
	    bool	m_bRendering;
	};
} // namespace wg
#endif //WG_STREAMGFXDEVICE_DOT_H

