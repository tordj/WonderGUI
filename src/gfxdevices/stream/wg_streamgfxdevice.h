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

		static StreamGfxDevice_p	create( CGfxOutStream& stream );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		const TypeInfo&		surfaceType( void ) const override;

		//.____ Misc _______________________________________________________

		bool		defineCanvas( CanvasRef ref, StreamSurface * pSurface );
		bool		defineCanvas( CanvasRef ref, const SizeI& size );

		SurfaceFactory_p surfaceFactory() override;

		//.____ Geometry _________________________________________________
		
		SizeI		canvasSize(CanvasRef ref) const override;


		//.____ State _________________________________________________
        
        bool        setClipList(int nRectangles, const RectI * pRectangles) override;
        void        clearClipList() override;
        bool        pushClipList(int nRectangles, const RectI* pRectangles) override;
        bool        popClipList() override;

        void        setTintColor( HiColor color ) override;

        void        setTintGradient(const RectI& rect, const Gradient& gradient) override;
        void        clearTintGradient() override;

        bool        setBlendMode( BlendMode blendMode ) override;

        bool        setBlitSource(Surface * pSource) override;

        void        setMorphFactor(float factor) override;

        void        setRenderLayer(int layer) override;

        
        
		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;

        void    flush() override;
        
        void    endCanvasUpdate() override;

        
        // Draw methods.

        void    fill(HiColor col) override;
        void    fill( const RectI& rect, HiColor col ) override;
        void    fill(const RectF& rect, HiColor col) override;

        void    plotPixels( int nCoords, const CoordI * pCoords, const HiColor * pColors) override;

        void    drawLine( CoordI begin, CoordI end, HiColor color, float thickness = 1.f ) override;
        void    drawLine( CoordI begin, Direction dir, int length, HiColor col, float thickness = 1.f) override;

        // Blit methods

        void    blit(CoordI dest) override;
        void    blit(CoordI dest, const RectI& src) override;

        void    flipBlit(CoordI dest, GfxFlip flip ) override;
        void    flipBlit(CoordI dest, const RectI& src, GfxFlip flip ) override;

        void    stretchBlit(const RectI& dest) override;
        void    stretchBlit(const RectI& dest, const RectI& src) override;
        void    stretchBlit(const RectI& dest, const RectF& src) override;

        void    stretchFlipBlit(const RectI& dest, GfxFlip flip) override;
        void    stretchFlipBlit(const RectI& dest, const RectI& src, GfxFlip flip) override;
        void    stretchFlipBlit(const RectI& dest, const RectF& src, GfxFlip flip) override;

        void    rotScaleBlit(const RectI& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f, 0.5f }, CoordF destCenter = { 0.5f,0.5f }) override;

        void    tile(const RectI& dest, CoordI shift = { 0,0 }) override;
        void    flipTile(const RectI& dest, GfxFlip flip, CoordI shift = { 0,0 }) override;

        void    scaleTile(const RectI& dest, float scale, CoordI shift = { 0,0 }) override;
        void    scaleFlipTile(const RectI& dest, float scale, GfxFlip flip, CoordI shift = { 0,0 }) override;


        // Draw segments methods

        void    drawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill) override;
        void    flipDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip) override;

        void    drawElipse(const RectF& canvas, float thickness, HiColor color, float outlineThickness = 0.f, HiColor outlineColor = Color::Black) override;

        void    drawPieChart(const RectI& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize = 0.f, HiColor hubColor = Color::Transparent, HiColor backColor = Color::Transparent, bool bRectangular = false) override;

        void    drawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode = TintMode::Flat ) override;
        void    flipDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode = TintMode::Flat) override;


        // Special draw/blit methods

        void    blitNinePatch(const RectI& dstRect, const BorderI& dstFrame, const NinePatch& patch) override;


        //.____ Deprecated ________________________________________________

        void    blitHorrBar( const RectI& _src, const BorderI& _borders, bool _bTile, CoordI dest, int _len ) override;

        void    blitVertBar( const RectI& _src, const BorderI& _borders, bool _bTile, CoordI dest, int _len ) override;

	protected:
		StreamGfxDevice( CGfxOutStream& stream );
		~StreamGfxDevice();

        void    _streamEdgeSamples( int nSamples, int samplesPerLine, int linePitch, const int * pSamples );

        bool    _beginCanvasUpdate( CanvasRef canvasRef, Surface * pCanvas, int nUpdateRects, const RectI* pUpdateRects, CanvasLayers * pLayers, int startLayer) override;

        
        void    _canvasWasChanged() override;
        void    _renderLayerWasChanged() override;

        void    _transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2]) override;
        void    _transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2]) override;

		void	_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdges, const int * pEdges, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2]) override;

		std::vector<CanvasInfo>	m_definedCanvases;

		SurfaceFactory_p	m_pSurfaceFactory;
		CGfxOutStream_p		m_pStream;
		bool	m_bRendering;
	};
} // namespace wg
#endif //WG_STREAMGFXDEVICE_DOT_H

