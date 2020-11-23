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

#ifndef	WG_GFXDEVICE_DOT_H
#define WG_GFXDEVICE_DOT_H
#pragma once

#include <climits>

#include <wg_pointers.h>
#include <wg_types.h>
#include <wg_color.h>
#include <wg_gradient.h>


#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>
#include <wg_canvaslayers.h>

#include <vector>

namespace wg
{
	class	Surface;
	class	LegacyCTextDisplay;
	class	CaretInstance;
	class 	Pen;

	class GfxDevice;
	typedef	StrongPtr<GfxDevice>	GfxDevice_p;
	typedef	WeakPtr<GfxDevice>	GfxDevice_wp;


	//____ WaveLine ___________________________________________________________

	struct WaveLine
	{
		int		length;
		float	thickness;
		Color	color;
		int *	pWave;      // Pixel offset in 24.8 format.
		int		hold;      // Value for extending the line if it is too short (or completely missing).
	};

	//____ GfxDevice __________________________________________________________

	class GfxDevice : public Object
	{
	public:
		friend class GfxStreamPlayer;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		virtual const TypeInfo&	surfaceType( void ) const = 0;

		//.____ Misc _______________________________________________________

		virtual SurfaceFactory_p	surfaceFactory() = 0;

		//.____ Geometry _________________________________________________

		inline bool			beginCanvasUpdate( const RectI& canvas, int nUpdateRects = 0, RectI* pUpdateRects = nullptr, int startLayer = -1);
		inline bool			beginCanvasUpdate(Surface * pCanvas, int nUpdateRects = 0, RectI* pUpdateRects = nullptr, int startLayer = -1);
		void				endCanvasUpdate();
		inline Surface_p	canvas() const { return m_pCanvas; }

		inline SizeI		canvasSize() const { return m_canvasSize; }

		//.____ State _________________________________________________

		bool				setClipList(int nRectangles, const RectI * pRectangles);
		void				clearClipList();
		bool				pushClipList(int nRectangles, const RectI* pRectangles);
		bool				popClipList();

		inline const RectI*	clipList() const { return m_pClipRects; }
		inline int			clipListSize() const { return m_nClipRects; }
		inline RectI		clipBounds() const { return m_clipBounds; }

		virtual void		setTintColor( HiColor color );
		inline const HiColor&	tintColor() const { return m_tintColor; }

		virtual void		setTintGradient(const RectI& rect, const Gradient& gradient);
		virtual void		clearTintGradient();

		virtual bool		setBlendMode( BlendMode blendMode );
		inline BlendMode 	blendMode() const { return m_blendMode; }

		virtual bool		setBlitSource(Surface * pSource);
		inline Surface_p 	blitSource() const { return m_pBlitSource; }

		virtual void		setMorphFactor(float factor);
		float				morphFactor() const { return m_morphFactor; }

		void				setRenderLayer(int layer);
		int					renderLayer() const { return m_renderLayer; }

		//.____ Rendering ________________________________________________

		virtual bool	beginRender();
		virtual bool	endRender();
		virtual bool    isRendering();
		virtual bool	isIdle();
		virtual void	flush();

		// Draw methods.

		virtual void	fill(HiColor col);
		virtual void	fill( const RectI& rect, HiColor col ) = 0;
		virtual void	fill(const RectF& rect, HiColor col) = 0;

		virtual void    plotPixels( int nCoords, const CoordI * pCoords, const HiColor * pColors) = 0;

	 	virtual void	drawLine( CoordI begin, CoordI end, HiColor color, float thickness = 1.f ) = 0;
		virtual void	drawLine( CoordI begin, Direction dir, int length, HiColor col, float thickness = 1.f);

		// Blit methods

		virtual void	blit(CoordI dest);
		virtual void	blit(CoordI dest, const RectI& src);

		virtual void	flipBlit(CoordI dest, GfxFlip flip );
		virtual void	flipBlit(CoordI dest, const RectI& src, GfxFlip flip );

		virtual void	stretchBlit(const RectI& dest);
		virtual void	stretchBlit(const RectI& dest, const RectI& src);
		virtual void	stretchBlit(const RectI& dest, const RectF& src);

		virtual void	stretchFlipBlit(const RectI& dest, GfxFlip flip);
		virtual void	stretchFlipBlit(const RectI& dest, const RectI& src, GfxFlip flip);
		virtual void	stretchFlipBlit(const RectI& dest, const RectF& src, GfxFlip flip);

		virtual void	rotScaleBlit(const RectI& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f, 0.5f }, CoordF destCenter = { 0.5f,0.5f });

		virtual void	tile(const RectI& dest, CoordI shift = { 0,0 });
		virtual void	flipTile(const RectI& dest, GfxFlip flip, CoordI shift = { 0,0 });

		virtual void	scaleTile(const RectI& dest, float scale, CoordI shift = { 0,0 });
		virtual void	scaleFlipTile(const RectI& dest, float scale, GfxFlip flip, CoordI shift = { 0,0 });


		// Draw segments methods

		virtual void	drawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill);
		virtual void	flipDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip);

		virtual void	drawElipse(const RectF& canvas, float thickness, HiColor color, float outlineThickness = 0.f, HiColor outlineColor = HiColor::Black);

		virtual void	drawPieChart(const RectI& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize = 0.f, HiColor hubColor = HiColor::Transparent, HiColor backColor = HiColor::Transparent, bool bRectangular = false);

		virtual void	drawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode = TintMode::Flat );
		virtual void	flipDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode = TintMode::Flat);


		// Special draw/blit methods

		virtual void	blitNinePatch(const RectI& dstRect, const BorderI& dstFrame, const RectI& srcRect, const BorderI& srcFrame);




		//.____ Deprecated ________________________________________________

		virtual void	blitHorrBar(		const RectI& _src,
											const BorderI& _borders, bool _bTile,
											CoordI dest, int _len );

		virtual void	blitVertBar(		const RectI& _src,
											const BorderI& _borders, bool _bTile,
											CoordI dest, int _len );

		//


	protected:
		GfxDevice();
		virtual ~GfxDevice();

		const static int	c_maxSegments = 16;

		//

		virtual void	_canvasWasChanged() = 0;
		virtual void	_renderLayerWasChanged() = 0;	// Checked for errors before we get here.
		virtual void	_clipListWasChanged();			// Called when cliplist has been changed.

		virtual void	_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2]) = 0;
		virtual void	_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2]) = 0;

		virtual void	_transformDrawWave(const RectI& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, const int simpleTransform[2][2]);
		virtual void	_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2]) = 0;


		// Static, shared data

		static	int		s_gfxDeviceCount;				// Number of existing gfxDevices. Ref count for shared data.

		void	_genCurveTab();
		void	_traceLine(int * pDest, int nPoints, const WaveLine * pWave, int offset);

		bool	_beginCanvasUpdate(const RectI& canvas, Surface * pCanvas, int nUpdateRects, RectI* pUpdateRects, int startLayer);
		void	_clearRenderLayer();						// Initializes and possibly clear render layer. 


		const static int c_nCurveTabEntries = 1024;
		static int *	s_pCurveTab;

		//

		struct StashedClipList
		{
			int				nClipRects;
			const RectI* pClipRects;
			RectI			clipBounds;
		};

		struct StashedCanvas
		{
			CanvasLayers_p	pLayerDef;
			Surface_p		pCanvas;
			StashedClipList	updateRects;
			StashedClipList	clipRects;
			int				renderLayer;
			Bitmask<int>	layersInitialized;
			HiColor			tintColor;
			Gradient		tintGradient;
			RectI			tintGradientRect;
			bool			bTintGradient;
			BlendMode		blendMode;
			float			morphFactor;
			SizeI			canvasSize;

			Surface_p		layerSurfaces[CanvasLayers::c_maxLayers];		// Should maybe be a separate stack...
		};

		std::vector<StashedCanvas>		m_canvasStack;			// Offset of layer 0 in m_canvasLayers for each recursion of canvas.
		std::vector<StashedClipList>	m_clipListStack;

		//

		CanvasLayers_p	m_pLayerDef;

		Surface_p	m_pCanvas;
		Surface_p	m_pBlitSource;

		int			m_renderLayer = 0;

		Surface_p	m_layerSurfaces[CanvasLayers::c_maxLayers];

		const RectI* m_pCanvasUpdateRects;
		int			m_nCanvasUpdateRects;
		RectI		m_canvasUpdateBounds;

		const RectI * m_pClipRects = nullptr;
		int			m_nClipRects = 0;
		RectI		m_clipBounds = { 0,0,0,0 };

		HiColor		m_tintColor = Color::White;			// Current Tint color.
		BlendMode	m_blendMode = BlendMode::Blend;		// Current BlendMode.
		float		m_morphFactor = 0.5f;				// Factor used for morphing in BlendMode::Morph.

		Gradient	m_tintGradient;
		RectI		m_tintGradientRect = { 0,0,0,0 };
		bool		m_bTintGradient = false;

		SizeI		m_canvasSize = { 0,0 };
		bool        m_bRendering = false;
	};


	bool GfxDevice::beginCanvasUpdate(const RectI& canvas, int nUpdateRects, RectI* pUpdateRects, int startLayer)
	{
		return _beginCanvasUpdate(canvas, nullptr, nUpdateRects, pUpdateRects, startLayer);
	}

	bool GfxDevice::beginCanvasUpdate(Surface* pCanvas, int nUpdateRects, RectI* pUpdateRects, int startLayer)
	{
		return _beginCanvasUpdate(pCanvas->size(), pCanvas, nUpdateRects, pUpdateRects, startLayer);
	}


} // namespace wg
#endif	// WG_GFXDEVICE_DOT_H

