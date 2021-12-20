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
		HiColor	color;
		int *	pWave;      // Pixel offset in 24.8 format.
		int		hold;      // Value for extending the line if it is too short (or completely missing).
	};

	struct CanvasInfo
	{
		CanvasInfo() {};
		CanvasInfo( CanvasRef _ref, Surface_p _pSurface, SizeI _size, int _scale ) :
			ref(_ref),
			pSurface(_pSurface),
			size(_size),
			scale(_scale) {}
		
		CanvasRef	ref = CanvasRef::None;
		Surface_p	pSurface = nullptr;
		SizeSPX		size;
		int			scale = 64;
	};


	//____ GfxDevice __________________________________________________________

	class GfxDevice : public Object
	{
	public:

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		virtual const TypeInfo&	surfaceType( void ) const = 0;

		//.____ Misc _______________________________________________________

		inline const CanvasInfo& canvas() const { return m_canvas; }
		virtual const CanvasInfo& canvas(CanvasRef ref) const = 0;

		inline CanvasLayers_p 		canvasLayers() const { return m_pCanvasLayers; }

		virtual SurfaceFactory_p	surfaceFactory() = 0;
		
		//.____ Geometry _________________________________________________

		inline 	SizeSPX	canvasSize() const { return m_canvas.size; }


		//.____ State _________________________________________________

		virtual bool		setClipList(int nRectangles, const RectSPX * pRectangles);
		virtual void		clearClipList();
		virtual bool		pushClipList(int nRectangles, const RectSPX* pRectangles);
		virtual bool		popClipList();

		inline const RectSPX*	clipList() const { return m_pClipRects; }
		inline int			clipListSize() const { return m_nClipRects; }
		inline RectSPX		clipBounds() const { return m_clipBounds; }

		virtual void		setTintColor( HiColor color );
		inline const HiColor&	tintColor() const { return m_tintColor; }

		virtual void		setTintGradient(const RectSPX& rect, const Gradient& gradient);
		virtual void		clearTintGradient();

		virtual bool		setBlendMode( BlendMode blendMode );
		inline BlendMode 	blendMode() const { return m_blendMode; }

		virtual bool		setBlitSource(Surface * pSource);
		inline Surface_p 	blitSource() const { return m_pBlitSource; }

		virtual void		setMorphFactor(float factor);
		float				morphFactor() const { return m_morphFactor; }

		virtual void		setRenderLayer(int layer);
		int					renderLayer() const { return m_renderLayer; }

		//.____ Rendering ________________________________________________

		virtual bool	beginRender();
		virtual bool	endRender();
		virtual bool    isRendering();
		virtual bool	isIdle();
		virtual void	flush();

        inline bool     beginCanvasUpdate( CanvasRef canvas, int nUpdateRects = 0, const RectI* pUpdateRects = nullptr, CanvasLayers * pLayers = nullptr, int startLayer = -1 );
        inline bool     beginCanvasUpdate( Surface * pCanvas, int nUpdateRects = 0, const RectI* pUpdateRects = nullptr, CanvasLayers * pLayers = nullptr, int startLayer = -1 );
        virtual void    endCanvasUpdate();

        
		// Draw methods.

		virtual void	fill(HiColor col);
		virtual void	fill( const RectSPX& rect, HiColor col ) = 0;
		virtual void	fill(const RectF& rect, HiColor col) = 0;

		virtual void    plotPixels( int nCoords, const CoordSPX * pCoords, const HiColor * pColors) = 0;

	 	virtual void	drawLine( CoordSPX begin, CoordSPX end, HiColor color, float thickness = 1.f ) = 0;
		virtual void	drawLine( CoordSPX begin, Direction dir, spx length, HiColor col, float thickness = 1.f);

		// Blit methods

		virtual void	blit(CoordSPX dest);
		virtual void	blit(CoordSPX dest, const RectSPX& src);

		virtual void	flipBlit(CoordSPX dest, GfxFlip flip );
		virtual void	flipBlit(CoordSPX dest, const RectSPX& src, GfxFlip flip );

		virtual void	stretchBlit(const RectSPX& dest);
		virtual void	stretchBlit(const RectSPX& dest, const RectSPX& src);
		virtual void	stretchBlit(const RectSPX& dest, const RectF& src);

		virtual void	stretchFlipBlit(const RectSPX& dest, GfxFlip flip);
		virtual void	stretchFlipBlit(const RectSPX& dest, const RectSPX& src, GfxFlip flip);
		virtual void	stretchFlipBlit(const RectSPX& dest, const RectF& src, GfxFlip flip);

		virtual void	rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f, 0.5f }, CoordF destCenter = { 0.5f,0.5f });

		virtual void	tile(const RectSPX& dest, CoordSPX shift = { 0,0 });
		virtual void	flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift = { 0,0 });

		virtual void	scaleTile(const RectSPX& dest, float scale, CoordSPX shift = { 0,0 });
		virtual void	scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift = { 0,0 });


		// Draw segments methods

		virtual void	drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill);
		virtual void	flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, GfxFlip flip);

		virtual void	drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness = 0, HiColor outlineColor = Color::Black);

		virtual void	drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize = 0.f, HiColor hubColor = Color::Transparent, HiColor backColor = Color::Transparent, bool bRectangular = false);

		virtual void	drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode = TintMode::Flat );
		virtual void	flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode = TintMode::Flat);


		// Special draw/blit methods

		virtual void	blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale);



	protected:
		GfxDevice();
		virtual ~GfxDevice();

		const static int	c_maxSegments = 16;

		//

		virtual void	_canvasWasChanged() = 0;
		virtual void	_renderLayerWasChanged() = 0;	// Checked for errors before we get here.
		virtual void	_clipListWasChanged();			// Called when cliplist has been changed.

		virtual void	_transformBlit(const RectSPX& dest, CoordSPX src, const int simpleTransform[2][2]) = 0;
		virtual void	_transformBlit(const RectSPX& dest, CoordF src, const float complexTransform[2][2]) = 0;

		virtual void	_transformDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor frontFill, HiColor backFill, const int simpleTransform[2][2]);
		virtual void	_transformDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2]) = 0;


		// Static, shared data

		static	int		s_gfxDeviceCount;				// Number of existing gfxDevices. Ref count for shared data.

		void	_genCurveTab();
		void	_traceLine(int * pDest, int nPoints, const WaveLine * pWave, int offset);

		virtual bool _beginCanvasUpdate(CanvasRef ref, Surface * pCanvas, int nUpdateRects, const RectI* pUpdateRects, CanvasLayers * pLayers, int startLayer);
		void	_clearRenderLayer();						// Initializes and possibly clear render layer. 


		void	_stretchBlitWithRigidPartX(const RectSPX& src, const RectSPX& dst, spx rigidPartOfs, spx rigidPartLength, spx rigidPartLengthDst);
		void	_stretchBlitWithRigidPartY(const RectSPX& src, const RectSPX& dst, spx rigidPartOfs, spx rigidPartLength, spx rigidPartLengthDst);


		const static int c_nCurveTabEntries = 1024;
		static int *	s_pCurveTab;

		//

		struct StashedClipList
		{
			int				nClipRects;
			const RectSPX * pClipRects;
			RectSPX			clipBounds;
		};

		struct StashedCanvas
		{
			CanvasInfo		canvas;
			CanvasLayers_p	pCanvasLayers;
			StashedClipList	updateRects;
			StashedClipList	clipRects;
			int				renderLayer;
			Bitmask<int>	layersInitialized;
			HiColor			tintColor;
			Gradient		tintGradient;
			RectSPX			tintGradientRect;
			bool			bTintGradient;
			BlendMode		blendMode;
			float			morphFactor;

			Surface_p		layerSurfaces[CanvasLayers::c_maxLayers];		// Should maybe be a separate stack...
		};

		std::vector<StashedCanvas>		m_canvasStack;			// Offset of layer 0 in m_canvasLayers for each recursion of canvas.
		std::vector<StashedClipList>	m_clipListStack;

		//

		CanvasInfo		m_canvas;
		CanvasInfo		m_dummyCanvas;			// Returned when calling canvas() with an undefined reference.
		CanvasLayers_p	m_pCanvasLayers;

		Surface_p	m_pBlitSource;

		int			m_renderLayer = 0;

		Surface_p	m_layerSurfaces[CanvasLayers::c_maxLayers+1];

		const RectSPX*	m_pCanvasUpdateRects;
		int				m_nCanvasUpdateRects;
		RectI			m_canvasUpdateBounds;

		const RectSPX * m_pClipRects = nullptr;
		int			m_nClipRects = 0;
		RectSPX		m_clipBounds = { 0,0,0,0 };

		HiColor		m_tintColor = Color::White;		// Current Tint color.
		BlendMode	m_blendMode = BlendMode::Blend;		// Current BlendMode.
		float		m_morphFactor = 0.5f;				// Factor used for morphing in BlendMode::Morph.

		Gradient	m_tintGradient;
		RectSPX		m_tintGradientRect = { 0,0,0,0 };
		bool		m_bTintGradient = false;

		bool        m_bRendering = false;
	};


	bool GfxDevice::beginCanvasUpdate(CanvasRef ref, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
	{
		return _beginCanvasUpdate(ref, nullptr, nUpdateRects, pUpdateRects, pLayers, startLayer);
	}

	bool GfxDevice::beginCanvasUpdate(Surface * pCanvas, int nUpdateRects, const RectSPX* pUpdateRects, CanvasLayers * pLayers, int startLayer)
	{
		return _beginCanvasUpdate( CanvasRef::None, pCanvas, nUpdateRects, pUpdateRects, pLayers, startLayer);
	}


} // namespace wg
#endif	// WG_GFXDEVICE_DOT_H

