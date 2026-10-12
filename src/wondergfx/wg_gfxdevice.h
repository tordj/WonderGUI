/*=========================================================================

                             >>> WonderGUI <<<

  This file is part of Tord Bärnfors' WonderGUI UI Toolkit and copyright
  Tord Bärnfors, Sweden [mail: first name AT barnfors DOT c_o_m].

                                -----------

  The WonderGUI UI Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                                -----------

  The WonderGUI UI Toolkit is also available for use in commercial
  closed source projects under a separate license. Interested parties
  should contact Bärnfors Technology AB [www.barnfors.com] for details.

=========================================================================*/
#ifndef	WG_GFXDEVICE_DOT_H
#define WG_GFXDEVICE_DOT_H
#pragma once

#include <climits>

#include <wg_pointers.h>
#include <wg_gfxtypes.h>
#include <wg_color.h>
#include <wg_edgemap.h>
#include <wg_tint.h>
#include <wg_blurbrush.h>


#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>
#include <wg_edgemapfactory.h>
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
	typedef	WeakPtr<GfxDevice>		GfxDevice_wp;


	//____ WaveLine ___________________________________________________________

	struct WaveLine
	{
		int		length;
		float	thickness;
		HiColor	color;
		int *	pWave;      // Pixel offset in 24.8 format.
		int		hold;      // Value for extending the line if it is too short (or completely missing).
	};

	//____ RoundedBoxSpec _____________________________________________________
	/**
	 * A box with rounded corners and an outline, drawn by GfxDevice::drawRoundedBox().
	 *
	 * Fill and outline are drawn together, so no background shows through between
	 * them, and they are placed with subpixel precision.
	 *
	 * Corner radii that don't fit the box are scaled down, all four by the same factor,
	 * as in CSS. A radius of at least half the shorter side gives a pill.
	 *
	 * The outline lies inside rect, with its own thickness along each side. Its inner
	 * corners are circular, with the radius of the corner minus the thicker of the two
	 * sides meeting there, so a thick outline gets square inner corners.
	 *
	 * Tints are placed in rect and multiplied with the colors. The tint and tint color
	 * of the device apply on top, as for any other draw.
	 *
	 * Like fill(), a fully transparent color draws nothing in BlendMode::Blend.
	 */

	struct RoundedBoxSpec
	{
		RectSPX		rect;										// Outer edge of the box.
		CornersSPX	radius;										// Corner radii.
		BorderSPX	outline;									// Thickness of the outline along each side.
		HiColor		fillColor = HiColor::Transparent;
		HiColor		outlineColor = HiColor::Black;
		Tint*		pFillTint = nullptr;
		Tint*		pOutlineTint = nullptr;
	};

	//____ RingSlice __________________________________________________________

	struct RingSlice
	{
		float		size = 0.f;						// Fraction of a full turn.
		HiColor		color = HiColor::White;
		Tint*		pTint = nullptr;				// Placed in rect of ring.
	};

	//____ RingSpec ___________________________________________________________
	/**
	 * A circle, ring, pie or donut chart, gauge or progress ring, drawn by
	 * GfxDevice::drawRing().
	 *
	 * The circle is the largest one centered in rect. The ring reaches thickness in
	 * from its outer edge and is split into slices clockwise from start (12 o'clock is
	 * 0.0, 3 o'clock 0.25). Slices beyond a full turn are cut off.
	 *
	 * Optionally the ring has an outline along its outer and inner edge (inside the
	 * thickness), the hub (inside the ring) is filled and the rest of rect, together
	 * with the part of the ring not reached by slices, is filled with a back color.
	 * With bSquare, the outer edge of the ring follows rect instead.
	 *
	 * Everything is drawn together, so no background shows through between slices,
	 * outline, hub and back, and it is placed with subpixel precision.
	 *
	 * Tints are placed in rect and multiplied with the colors. The tint and tint color
	 * of the device apply on top. Like fill(), fully transparent colors draw nothing in
	 * BlendMode::Blend.
	 */

	struct RingSpec
	{
		RectSPX				rect;
		spx					thickness = 0;						// Width of ring, outline included. 0 or at least the radius for a full pie or disc.
		spx					outline = 0;						// Thickness of outline along outer and inner edge.
		float				start = 0.f;						// Start of first slice, fraction of a turn clockwise from 12 o'clock.
		int					nSlices = 0;						// At most GfxDevice::maxRingSlices().
		const RingSlice*	pSlices = nullptr;
		bool				bSquare = false;					// Outer edge follows rect.

		HiColor				outlineColor = HiColor::Black;
		Tint*				pOutlineTint = nullptr;
		HiColor				hubColor = HiColor::Transparent;	// Inside the ring.
		Tint*				pHubTint = nullptr;
		HiColor				backColor = HiColor::Transparent;	// Rest of rect and the part of the ring not reached by slices.
		Tint*				pBackTint = nullptr;
	};

	//____ CanvasInfo _________________________________________________________

	struct CanvasInfo
	{
		CanvasInfo() {};
		CanvasInfo( CanvasRef _ref, Surface_p _pSurface, SizeI _size, PixelFormat _format, int _scale,
					ColorSpace _colorSpace = ColorSpace::sRGB, bool _bigEndian = (WG_IS_BIG_ENDIAN == 1) ) :
			ref(_ref),
			pSurface(_pSurface),
			size(_size),
			scale(_scale),
			format(_format),
			colorSpace(_colorSpace),
			bigEndian(_bigEndian) {}
		
		CanvasRef	ref = CanvasRef::None;
		Surface_p	pSurface = nullptr;
		SizeSPX		size;
		int			scale = 64;

		PixelFormat	format = PixelFormat::Undefined;
		ColorSpace	colorSpace = ColorSpace::sRGB;
		bool		bigEndian = (WG_IS_BIG_ENDIAN == 1);
	};


	//____ GfxDevice __________________________________________________________

	class GfxDevice : public Object
	{
	public:

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Misc _______________________________________________________

		virtual const CanvasInfo&	canvas() const = 0;
		virtual const CanvasInfo	canvas(CanvasRef ref) const = 0;

		virtual CanvasLayers_p 		canvasLayers() const = 0;

		virtual SurfaceFactory_p	surfaceFactory() = 0;
		virtual EdgemapFactory_p	edgemapFactory() = 0;

		static constexpr int		maxSegments() { return c_maxSegments; }
		static constexpr int		maxRingSlices() { return c_maxRingSlices; }
		
		//.____ Geometry _________________________________________________

		virtual SizeSPX		canvasSize() const = 0;


		//.____ State _________________________________________________

		virtual bool		setClipList(int nRectangles, const RectSPX * pRectangles) = 0;
		virtual void		resetClipList() = 0;
		virtual bool		pushClipList(int nRectangles, const RectSPX* pRectangles) = 0;
		virtual bool		popClipList() = 0;

		virtual const RectSPX* clipList() const = 0;
		virtual int			clipListSize() const = 0;
		virtual const RectSPX& clipBounds() const = 0;

		virtual void		setTintColor(HiColor color) = 0;
		virtual void		clearTintColor() = 0;
		virtual bool		hasTintColor() const = 0;
		virtual HiColor		tintColor() const = 0;

		virtual void		setTint(const RectSPX& rect, Tint* pTint) = 0;
		virtual void		clearTint() = 0;
		virtual bool		hasTint() const = 0;
		virtual Tint_p		tint() const = 0;
		virtual RectSPX		tintRect() const = 0;

		virtual bool		setBlendMode( BlendMode blendMode ) = 0;
		virtual BlendMode 	blendMode() const = 0;

		virtual bool		setBlitSource(Surface * pSource) = 0;
		virtual Surface_p 	blitSource() const = 0;

		virtual void		setMorphFactor(float factor) = 0;
		virtual float		morphFactor() const = 0;

		virtual void		setBlurbrush(Blurbrush* pBrush) = 0;
		virtual Blurbrush_p	blurbrush() const = 0;

		
		virtual void		setFixedBlendColor( HiColor color ) = 0;
		virtual HiColor		fixedBlendColor() const = 0;

		virtual void		setRenderLayer(int layer) = 0;
		virtual int			renderLayer() const = 0;

		
		
		
		//.____ Rendering ________________________________________________

		virtual bool	beginRender() = 0;
		virtual bool	endRender() = 0;
		virtual bool    isRendering() = 0;
		virtual bool	isIdle() = 0;
		virtual void	flush() = 0;

        virtual bool     beginCanvasUpdate( CanvasRef canvas, int nUpdateRects = 0, const RectSPX* pUpdateRects = nullptr, CanvasLayers * pLayers = nullptr, int startLayer = -1 ) = 0;
        virtual bool     beginCanvasUpdate( Surface * pCanvas, int nUpdateRects = 0, const RectSPX* pUpdateRects = nullptr, CanvasLayers * pLayers = nullptr, int startLayer = -1 ) = 0;
        virtual void    endCanvasUpdate() = 0;

        
		// Draw methods.

		virtual void	fill(HiColor color) = 0;
		virtual void	fill( const RectSPX& rect, HiColor color ) = 0;

	 	virtual void	drawLine( CoordSPX begin, CoordSPX end, HiColor color, spx thickness = 64 ) = 0;
		virtual void	drawLine( CoordSPX begin, Direction dir, spx length, HiColor color, spx thickness = 64 ) = 0;

		// Shapes

		virtual void	drawRoundedBox( const RoundedBoxSpec& spec ) = 0;
		virtual void	drawRing( const RingSpec& spec ) = 0;

		// Blit methods

		virtual void	blit(CoordSPX dest) = 0;
		virtual void	blit(CoordSPX dest, const RectSPX& src) = 0;

		virtual void	flipBlit(CoordSPX dest, GfxFlip flip ) = 0;
		virtual void	flipBlit(CoordSPX dest, const RectSPX& src, GfxFlip flip ) = 0;

		virtual void	stretchBlit(const RectSPX& dest) = 0;
		virtual void	stretchBlit(const RectSPX& dest, const RectSPX& src) = 0;

		virtual void	stretchFlipBlit(const RectSPX& dest, GfxFlip flip) = 0;
		virtual void	stretchFlipBlit(const RectSPX& dest, const RectSPX& src, GfxFlip flip) = 0;

		virtual void	precisionBlit(const RectSPX& dest, const RectF& srcSPX) = 0;
		virtual void	transformBlit(const RectSPX& dest, CoordF srcSPX, const Transform& transform) = 0;
		virtual void	rotScaleBlit(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f, 0.5f }, CoordF destCenter = { 0.5f,0.5f }) = 0;

		virtual void	tile(const RectSPX& dest, CoordSPX shift = { 0,0 }) = 0;
		virtual void	flipTile(const RectSPX& dest, GfxFlip flip, CoordSPX shift = { 0,0 }) = 0;

		virtual void	scaleTile(const RectSPX& dest, float scale, CoordSPX shift = { 0,0 }) = 0;
		virtual void	scaleFlipTile(const RectSPX& dest, float scale, GfxFlip flip, CoordSPX shift = { 0,0 }) = 0;

		virtual void	blur(CoordSPX dest) = 0;
		virtual void	blur(CoordSPX dest, const RectSPX& src) = 0;
		virtual void	stretchBlur(const RectSPX& dest) = 0;
		virtual void	stretchBlur(const RectSPX& dest, const RectSPX& src) = 0;
		virtual void	transformBlur(const RectSPX& dest, CoordF srcSPX, const Transform& transform) = 0;
		virtual void	rotScaleBlur(const RectSPX& dest, float rotationDegrees, float scale, CoordF srcCenter = { 0.5f, 0.5f }, CoordF destCenter = { 0.5f,0.5f }) = 0;


		// Draw segments methods

		virtual void	drawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor fill) = 0;
		virtual void	flipDrawWave(const RectSPX& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, HiColor fill, GfxFlip flip) = 0;

		virtual void	drawElipse(const RectSPX& canvas, spx thickness, HiColor color, spx outlineThickness = 0, HiColor outlineColor = HiColor::Black) = 0;

		virtual void	drawPieChart(const RectSPX& canvas, float start, int nSlices, const float * pSliceSizes, const HiColor * pSliceColors, float hubSize = 0.f, HiColor hubColor = HiColor::Transparent, HiColor backColor = HiColor::Transparent, bool bRectangular = false) = 0;

		virtual void	drawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode = TintMode::Flat ) = 0;
		virtual void	flipDrawSegments(const RectSPX& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, GfxFlip flip, TintMode tintMode = TintMode::Flat) = 0;

		virtual void	drawEdgemap(CoordSPX dest, Edgemap * pEdgemap ) = 0;
		virtual void	flipDrawEdgemap(CoordSPX dest, Edgemap * pEdgemap, GfxFlip flip) = 0;

				

		// Special draw/blit methods

		virtual void	blitNinePatch(const RectSPX& dstRect, const BorderSPX& dstFrame, const NinePatch& patch, int scale) = 0;

		// Deprecated



	protected:
		GfxDevice();
		virtual ~GfxDevice();

		const static int	c_maxSegments = 16;
		const static int	c_maxRingSlices = 256;
	};



} // namespace wg
#endif	// WG_GFXDEVICE_DOT_H

