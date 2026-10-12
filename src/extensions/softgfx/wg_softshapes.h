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
#ifndef	WG_SOFTSHAPES_DOT_H
#define	WG_SOFTSHAPES_DOT_H
#pragma once

#include <wg_geo.h>
#include <wg_gfxtypes.h>

#include <cstdint>
#include <vector>

namespace wg
{
	//____ SoftShapes _________________________________________________________
	/**
	 * @brief Rasterizes analytic shapes row by row for the software backends.
	 *
	 * Shapes are made of segments (fill, outline, slices...) that are drawn in one
	 * go, so that segments sharing an edge never let the background shine through.
	 *
	 * Each row is delivered to a RowSink as spans of pixels:
	 *
	 *	- Runs of pixels fully covered by one segment. Most of a shape, drawn with
	 *	  the fill kernels.
	 *	- Spans of partly covered pixels, with coverage per layer for each pixel. Only
	 *	  along edges, drawn with the coverage kernels. Layers map to segments.
	 *
	 * Coverage is 0 -> 4096. Shapes are placed with subpixel (spx) precision.
	 *
	 * Coverage model:
	 *
	 *	- Straight, axis-aligned edges give the exact area of the pixel covered.
	 *	- Circular edges use the signed distance d from the pixel center to the edge,
	 *	  coverage = clamp(0.5 + d, 0, 1). The distance is approximated without square
	 *	  roots, from d' = (r^2 - |p - c|^2) / 2r, which is stepped from pixel to pixel
	 *	  with additions only, and refined as d = d' + d'^2 / 2r for radii of 2 pixels
	 *	  and more, which is within 1/64 pixel of the true distance at a radius of
	 *	  2 pixels and better for larger radii.
	 *	- Nested shapes split their coverage between segments, e.g. fill = inner and
	 *	  outline = outer - inner, so coverage of neighboring segments always adds up
	 *	  to exactly the coverage of the shape.
	 *
	 * Everything is integer math, without divisions per pixel or row, so that it runs
	 * well on CPUs without FPU.
	 */

	class SoftShapes
	{
	public:

		static constexpr int c_fullCoverage = 4096;

		//____ Span ___________________________________________________________

		struct Span
		{
			int				begin;			// First pixel column.
			int				end;			// Column after the last pixel.
			int				segment;		// Pixels fully covered by this segment, or -1 for partly covered pixels.
			bool			bUniform;		// Partly covered pixels that all have the coverage of the first pixel.
			const uint16_t*	pCoverage;		// Partly covered pixels: coverage of each layer, for each pixel (just one if bUniform).
			int				nLayers;		// Partly covered pixels: number of coverage values per pixel.
			const int*		pLayerSegments;	// Partly covered pixels: segment of each layer.
		};

		//____ RowSink ________________________________________________________

		class RowSink
		{
		public:
			/**
			 * Receives the spans of one row, in increasing order, not overlapping and
			 * within the clip rectangle. Pixels in no span are not covered at all.
			 * Partly covered pixels have nLayers coverage values each, layer i belonging
			 * to segment pLayerSegments[i]. Several layers can belong to the same segment.
			 */
			virtual void	row(int y, int nSpans, const Span* pSpans) = 0;
		};

		//____ RoundedBox _____________________________________________________

		struct RoundedBox
		{
			RectSPX		rect;							// Outer edge, the outline lies inside.
			spx			radius[4] = { 0, 0, 0, 0 };		// Top-left, top-right, bottom-right, bottom-left. Clamped as by Util::clampCornerRadii().
			BorderSPX	outline;						// Thickness of outline along each side.
		};

		//.____ Rasterization _____________________________________________________

		/**
		 * Rasterizes a box with rounded corners into rows within clip (pixels).
		 *
		 * Two segments: 0 is the fill (inside the outline) and 1 the outline. The inner
		 * edge of the outline has corners with radius r - max(width of the two sides),
		 * circular and touching both sides, which equals CSS when the widths are equal.
		 */

		void	rasterizeRoundedBox(const RoundedBox& box, const RectI& clip, RowSink* pSink);

		//____ Ring ___________________________________________________________

		static constexpr uint32_t c_fullTurn = 0x80000000;	// Angles and slice sizes of Ring.

		struct Ring
		{
			RectSPX			rect;
			spx				thickness = 0;			// Width of ring from outer edge, outlines included. 0 or at least the radius for a full disc.
			spx				outline = 0;			// Outline along outer and inner edge.
			bool			bSquare = false;		// Outer edge is rect, not the largest circle centered in it.
			uint32_t		start = 0;				// Start of first slice, clockwise from 12 o'clock. c_fullTurn is a full turn.
			int				nSlices = 0;
			const uint32_t*	pSliceSizes = nullptr;	// Size of each slice, c_fullTurn is a full turn. Slices beyond a full turn are cut.
		};

		static constexpr int	c_ringBackSegment = 0;			// Rest of rect, and the part of the ring the slices don't reach.
		static constexpr int	c_ringOutlineSegment = 1;
		static constexpr int	c_ringHubSegment = 2;			// Inside the ring.
		static constexpr int	c_ringFirstSliceSegment = 3;	// Followed by the other slices.

		/**
		 * Rasterizes a ring, pie or disc into rows within clip (pixels).
		 *
		 * The circle is the largest centered in rect, or rect itself if bSquare. Slices are
		 * split along straight lines from the center, so neighboring slices always add up
		 * to the coverage of the ring. Pixels around the center, where many slices may meet,
		 * are supersampled.
		 */

		void	rasterizeRing(const Ring& ring, const RectI& clip, RowSink* pSink);

	protected:

		// Coverage of a pixel row by one shape, as pieces in increasing order. Gaps have no coverage.

		struct Piece
		{
			int				begin;
			int				end;
			int				value;			// Coverage of all pixels, if pValues is null.
			const uint16_t*	pValues;		// Coverage of each pixel, starting with pixel 'begin'.
		};

		static const int	c_maxPieces = 8;

		// Steps along a quarter circle, keeping the approximated distance d' of the pixel center
		// from the edge. 32.32 fixed point pixels.

		struct Arc
		{
			bool		bActive = false;
			bool		bLeft;				// On the left side of the shape, coverage increases with x.
			bool		bInitialized;
			bool		bRefine;			// Refine d' to d.

			spx			cx, cy, r;			// Center and radius of the circle.

			int			colBeg, colEnd;		// Columns and rows of pixels whose coverage this corner gives.
			int			rowBeg, rowEnd;

			int			col, row;			// Pixel at which d is kept.
			int64_t		d;					// d' of pixel.
			int64_t		dx;					// d' of pixel to the right minus d'.
			int64_t		dy;					// d' of pixel below minus d'.
			int64_t		step;				// Change of dx (and dy) from one pixel to the next.
			int64_t		lo;					// Pixels with d' at or below lo are not covered.
			int64_t		hi;					// Pixels with d' at or above hi are fully covered.
			int			invRadius;			// 1/r, 12 binals.
		};

		struct Shape
		{
			bool		bEmpty;
			spx			left, top, right, bottom;
			Arc			arcs[4];			// Top-left, top-right, bottom-right, bottom-left.
		};

		void	_setupShape(Shape& shape, const RectSPX& rect, const spx radius[4]);
		void	_initArc(Arc& arc, int row);
		int		_arcRow(Arc& arc, int row, int clipBeg, int clipEnd, int& bandBeg, int& bandEnd, uint16_t* pOutput);
		int		_shapeRow(Shape& shape, int row, int clipBeg, int clipEnd, Piece* pPieces, uint16_t*& pBuffer);

		static inline int	_arcCoverage(const Arc& arc, int64_t d);

		// Ring

		struct Boundary
		{
			uint32_t	angle;				// Clockwise from 12 o'clock, 2^32 is a full turn.
			uint32_t	pseudoAngle;		// Monotonic with angle, see _pseudoAngle().
			int32_t		sx, sy;				// Direction from center, 2.30 fixed point, y pointing down.
			bool		bFlat;				// Too close to horizontal to cross any row but those around the center.
			int64_t		slopeInt;			// sx / sy, integer part (rounded down)...
			uint32_t	slopeFrac;			// ...and 32 binals.
			int64_t		halfBand;			// Half width (spx) of partly covered pixels along a row.
			int			element;			// Element (slice or gap) starting here.
			int			prevElement;		// Element ending here.
		};

		struct ActiveBoundary
		{
			int			boundary;
			int64_t		x;					// Crossing of row, spx.
			int			bandBeg;			// Columns of partly covered pixels.
			int			bandEnd;
		};

		struct AngularPiece
		{
			int			begin;
			int			end;
			int			element;			// Element of all pixels, or -1 if per pixel.
			int			firstActive;		// Per pixel: first of the active boundaries (in order along x)...
			int			nActive;			// ...and their number.
			bool		bSampled;			// Per pixel: sample each pixel (around center).
		};

		struct RadialValues
		{
			int			back, outline, hub, ring;
		};

		void	_setupRingAngles(const Ring& ring);
		int		_elementAt(uint32_t pseudoAngle) const;
		int		_angularRow(int64_t rowCenterY, int clipBeg, int clipEnd);
		int		_pixelBoundaries(int32_t dx, int32_t dy, int& first) const;
		void	_samplePixel(int32_t dx, int32_t dy, int* pElements, int* pFractions, int& nElements) const;

		static inline int	_pastShare(const Boundary& b, int32_t dx, int32_t dy);

		static uint32_t	_pseudoAngle(int64_t x, int64_t y);
		static int32_t	_sin30(uint32_t angle);

		int64_t						m_cx, m_cy;			// Center of ring, spx.
		std::vector<Boundary>		m_boundaries;		// Sorted by angle.
		std::vector<int>			m_elementSegments;	// Segment of each element.
		std::vector<int>			m_upperOrder;		// Boundaries crossing rows above center, in order along x.
		std::vector<int>			m_lowerOrder;		// Boundaries crossing rows below center, in order along x.
		std::vector<ActiveBoundary>	m_active;
		std::vector<AngularPiece>	m_angularPieces;
		std::vector<int>			m_layerBuffer;
		std::vector<uint16_t>		m_ringOutput;
		std::vector<int>			m_spanLayerOfs;
		std::vector<int>			m_spanCoverageOfs;
		std::vector<uint64_t>		m_elementStarts;

		std::vector<uint16_t>	m_coverageBuffer;
		std::vector<uint16_t>	m_outputBuffer;
		std::vector<Span>		m_spans;
	};

}

#endif //WG_SOFTSHAPES_DOT_H
