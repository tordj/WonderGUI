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

#include <wg_softshapes.h>
#include <wg_gfxutil.h>

#include <algorithm>

namespace wg
{
	// Pixel index of spx value, rounded down or up.

	static inline int floorPixel(int64_t value) { return int(value >> 6); }
	static inline int ceilPixel(int64_t value) { return int((value + 63) >> 6); }

	static const int64_t c_one = int64_t(1) << 32;			// 1.0 in 32.32 fixed point.

	//____ rasterizeRoundedBox() ______________________________________________

	void SoftShapes::rasterizeRoundedBox(const RoundedBox& box, const RectI& clip, RowSink* pSink)
	{
		const RectSPX& rect = box.rect;

		if (rect.w <= 0 || rect.h <= 0 || clip.w <= 0 || clip.h <= 0)
			return;

		spx radius[4] = { box.radius[0], box.radius[1], box.radius[2], box.radius[3] };
		Util::clampCornerRadii(rect.size(), radius);

		BorderSPX outline = { std::max(box.outline.top, 0), std::max(box.outline.right, 0),
							  std::max(box.outline.bottom, 0), std::max(box.outline.left, 0) };

		bool bOutline = (outline.top | outline.right | outline.bottom | outline.left) != 0;

		// Setup outer and inner shape.

		Shape outer, inner;

		_setupShape(outer, rect, radius);

		if (bOutline)
		{
			RectSPX innerRect(rect.x + outline.left, rect.y + outline.top,
							  rect.w - outline.left - outline.right, rect.h - outline.top - outline.bottom);

			spx innerRadius[4] = {	std::max(radius[0] - std::max(outline.left, outline.top), 0),
									std::max(radius[1] - std::max(outline.top, outline.right), 0),
									std::max(radius[2] - std::max(outline.right, outline.bottom), 0),
									std::max(radius[3] - std::max(outline.bottom, outline.left), 0) };

			_setupShape(inner, innerRect, innerRadius);
		}

		// Rows and columns to rasterize

		int clipBeg = std::max(clip.x, floorPixel(rect.x));
		int clipEnd = std::min(clip.x + clip.w, ceilPixel(int64_t(rect.x) + rect.w));

		int rowBeg = std::max(clip.y, floorPixel(rect.y));
		int rowEnd = std::min(clip.y + clip.h, ceilPixel(int64_t(rect.y) + rect.h));

		if (clipBeg >= clipEnd || rowBeg >= rowEnd)
			return;

		int width = clipEnd - clipBeg;

		if ((int)m_coverageBuffer.size() < width * 4 + 16)
			m_coverageBuffer.resize(width * 4 + 16);

		if ((int)m_outputBuffer.size() < width * 2 + c_maxPieces * 8)
			m_outputBuffer.resize(width * 2 + c_maxPieces * 8);

		if ((int)m_spans.size() < c_maxPieces * 4)
			m_spans.resize(c_maxPieces * 4);

		static const int layerSegments[2] = { 0, 1 };

		Piece	outerPieces[c_maxPieces];
		Piece	innerPieces[c_maxPieces];

		for (int y = rowBeg; y < rowEnd; y++)
		{
			uint16_t* pBuffer = m_coverageBuffer.data();

			int nOuter = _shapeRow(outer, y, clipBeg, clipEnd, outerPieces, pBuffer);
			if (nOuter == 0)
				continue;

			const Piece* pInnerPieces = outerPieces;
			int nInner = nOuter;

			if (bOutline)
			{
				nInner = _shapeRow(inner, y, clipBeg, clipEnd, innerPieces, pBuffer);
				pInnerPieces = innerPieces;
			}

			// Collect the edges of all pieces.

			int breaks[c_maxPieces * 4];
			int nBreaks = 0;

			for (int i = 0; i < nOuter; i++)
			{
				breaks[nBreaks++] = outerPieces[i].begin;
				breaks[nBreaks++] = outerPieces[i].end;
			}

			if (bOutline)
			{
				for (int i = 0; i < nInner; i++)
				{
					breaks[nBreaks++] = pInnerPieces[i].begin;
					breaks[nBreaks++] = pInnerPieces[i].end;
				}
			}

			std::sort(breaks, breaks + nBreaks);
			nBreaks = int(std::unique(breaks, breaks + nBreaks) - breaks);

			// Split into spans, fill = inner coverage and outline = outer - inner.

			Span*		pSpans = m_spans.data();
			int			nSpans = 0;
			uint16_t*	pOut = m_outputBuffer.data();

			int io = 0, ii = 0;

			for (int k = 0; k + 1 < nBreaks; k++)
			{
				int beg = breaks[k];
				int end = breaks[k + 1];

				while (io < nOuter && outerPieces[io].end <= beg)
					io++;
				while (ii < nInner && pInnerPieces[ii].end <= beg)
					ii++;

				const Piece* po = (io < nOuter && outerPieces[io].begin <= beg) ? &outerPieces[io] : nullptr;
				const Piece* pi = (ii < nInner && pInnerPieces[ii].begin <= beg) ? &pInnerPieces[ii] : nullptr;

				if (!po)
					continue;

				Span* pPrev = nSpans > 0 ? &pSpans[nSpans - 1] : nullptr;

				if (!po->pValues && (!pi || !pi->pValues))
				{
					int outerCov = po->value;
					int innerCov = pi ? std::min(pi->value, outerCov) : 0;

					int segment = -1;
					if (innerCov == c_fullCoverage)
						segment = 0;
					else if (outerCov == c_fullCoverage && innerCov == 0)
						segment = 1;

					if (segment >= 0)
					{
						if (pPrev && pPrev->segment == segment && pPrev->end == beg)
							pPrev->end = end;
						else
							pSpans[nSpans++] = { beg, end, segment, false, nullptr };
					}
					else
					{
						pSpans[nSpans++] = { beg, end, -1, true, pOut };
						*pOut++ = uint16_t(innerCov);
						*pOut++ = uint16_t(outerCov - innerCov);
					}
				}
				else
				{
					if (pPrev && pPrev->segment == -1 && !pPrev->bUniform && pPrev->end == beg)
						pPrev->end = end;
					else
						pSpans[nSpans++] = { beg, end, -1, false, pOut };

					for (int x = beg; x < end; x++)
					{
						int outerCov = po->pValues ? po->pValues[x - po->begin] : po->value;
						int innerCov = pi ? (pi->pValues ? pi->pValues[x - pi->begin] : pi->value) : 0;

						if (innerCov > outerCov)
							innerCov = outerCov;

						*pOut++ = uint16_t(innerCov);
						*pOut++ = uint16_t(outerCov - innerCov);
					}
				}
			}

			if (nSpans > 0)
				pSink->row(y, nSpans, pSpans, 2, layerSegments);
		}
	}

	//____ _setupShape() ______________________________________________________

	void SoftShapes::_setupShape(Shape& shape, const RectSPX& rect, const spx radius[4])
	{
		shape.bEmpty = (rect.w <= 0 || rect.h <= 0);
		if (shape.bEmpty)
			return;

		shape.left = rect.x;
		shape.top = rect.y;
		shape.right = rect.x + rect.w;
		shape.bottom = rect.y + rect.h;

		// Columns and rows touched by the shape, and those that aren't cut by a straight edge.
		// Corners never reach into pixels cut by the opposite edge. Their arcs meet that edge
		// at a tangent there, so the straight edge alone gives the coverage.

		int leftCol = floorPixel(shape.left);
		int rightCol = ceilPixel(shape.right) - 1;
		int topRow = floorPixel(shape.top);
		int bottomRow = ceilPixel(shape.bottom) - 1;

		int firstUncutCol = (shape.left & 63) ? leftCol + 1 : leftCol;
		int lastUncutCol = (shape.right & 63) ? rightCol - 1 : rightCol;
		int firstUncutRow = (shape.top & 63) ? topRow + 1 : topRow;
		int lastUncutRow = (shape.bottom & 63) ? bottomRow - 1 : bottomRow;

		for (int i = 0; i < 4; i++)
		{
			Arc& arc = shape.arcs[i];

			spx r = radius[i];

			// Radii below half a pixel are drawn as square corners.

			arc.bActive = (r >= 32);
			if (!arc.bActive)
				continue;

			bool bTop = (i == 0 || i == 1);

			arc.bLeft = (i == 0 || i == 3);
			arc.bInitialized = false;
			arc.r = r;
			arc.cx = arc.bLeft ? shape.left + r : shape.right - r;
			arc.cy = bTop ? shape.top + r : shape.bottom - r;

			// Pixels with their center inside the square spanned by the corner.

			if (arc.bLeft)
			{
				arc.colBeg = leftCol;
				arc.colEnd = std::min(ceilPixel(int64_t(shape.left) + r - 32), lastUncutCol + 1);
			}
			else
			{
				arc.colBeg = std::max(ceilPixel(int64_t(shape.right) - r - 32), firstUncutCol);
				arc.colEnd = rightCol + 1;
			}

			if (bTop)
			{
				arc.rowBeg = topRow;
				arc.rowEnd = std::min(ceilPixel(int64_t(shape.top) + r - 32), lastUncutRow + 1);
			}
			else
			{
				arc.rowBeg = std::max(ceilPixel(int64_t(shape.bottom) - r - 32), firstUncutRow);
				arc.rowEnd = bottomRow + 1;
			}

			// d' changes by -1/r for each step, as does the change itself.

			arc.step = -(int64_t(64) * c_one / r);

			// Refining d' to the distance pays off from 2 pixels radius. Then the edge of the band
			// of partly covered pixels is where the refined distance is -0.5 and 0.5, which is
			// d' = d - d^2/2r = +/-0.5 - 0.125/r.

			arc.bRefine = (r >= 128);
			arc.invRadius = int((64 << 12) / r);

			int64_t half = c_one / 2;
			int64_t adjust = arc.bRefine ? (int64_t(1) << 35) / r : 0;

			arc.lo = -half - adjust;
			arc.hi = half - adjust;
		}
	}

	//____ _initArc() _________________________________________________________

	void SoftShapes::_initArc(Arc& arc, int row)
	{
		// d' = (r^2 - dx^2 - dy^2) / 2r for center of first pixel of row, in pixels.

		int64_t r = arc.r;
		int64_t dX = int64_t(arc.colBeg) * 64 + 32 - arc.cx;
		int64_t dY = int64_t(row) * 64 + 32 - arc.cy;

		int64_t q = r * r - dX * dX - dY * dY;			// spx^2

		arc.d = (q / r) * (int64_t(1) << 25) + (q % r) * (int64_t(1) << 25) / r;		// q / 128r in 32.32 pixels.
		arc.dx = -((dX + 32) * c_one / r);
		arc.dy = -((dY + 32) * c_one / r);

		arc.col = arc.colBeg;
		arc.row = row;
		arc.bInitialized = true;
	}

	//____ _arcCoverage() _____________________________________________________

	inline int SoftShapes::_arcCoverage(const Arc& arc, int64_t d)
	{
		int dist = int(d >> 20);					// 12 binals.

		if (arc.bRefine)
			dist += (((dist * dist) >> 12) * arc.invRadius) >> 13;	// d = d' + d'^2 / 2r

		int coverage = 2048 + dist;
		return coverage < 0 ? 0 : coverage > c_fullCoverage ? c_fullCoverage : coverage;
	}

	//____ _arcRow() __________________________________________________________
	/*
		Finds the band of partly covered pixels of the corner on given row and writes
		coverage for the part of it within clipBeg -> clipEnd to pOutput. Pixels of the
		corner before the band are uncovered (left corners) or fully covered (right corners),
		pixels after it the opposite.

		Returns number of coverage values written, which are for the pixels starting at
		max(bandBeg, clipBeg).
	*/

	int SoftShapes::_arcRow(Arc& arc, int row, int clipBeg, int clipEnd, int& bandBeg, int& bandEnd, uint16_t* pOutput)
	{
		if (!arc.bInitialized || row < arc.row)
			_initArc(arc, row);

		while (arc.row < row)
		{
			arc.d += arc.dy;
			arc.dy += arc.step;
			arc.row++;
		}

		// Move to the first pixel of the band.

		if (arc.bLeft)
		{
			while (arc.col > arc.colBeg && arc.d - (arc.dx - arc.step) > arc.lo)
			{
				arc.dx -= arc.step;
				arc.d -= arc.dx;
				arc.col--;
			}

			while (arc.col < arc.colEnd && arc.d <= arc.lo)
			{
				arc.d += arc.dx;
				arc.dx += arc.step;
				arc.col++;
			}
		}
		else
		{
			while (arc.col > arc.colBeg && arc.d - (arc.dx - arc.step) < arc.hi)
			{
				arc.dx -= arc.step;
				arc.d -= arc.dx;
				arc.col--;
			}

			while (arc.col < arc.colEnd && arc.d >= arc.hi)
			{
				arc.d += arc.dx;
				arc.dx += arc.step;
				arc.col++;
			}
		}

		bandBeg = arc.col;

		// Step through the band.

		int64_t d = arc.d;
		int64_t dx = arc.dx;
		int col = arc.col;
		int colEnd = std::min(arc.colEnd, clipEnd);
		int n = 0;

		if (arc.bLeft)
		{
			while (col < colEnd && d < arc.hi)
			{
				if (col >= clipBeg)
					pOutput[n++] = uint16_t(_arcCoverage(arc, d));

				d += dx;
				dx += arc.step;
				col++;
			}
		}
		else
		{
			while (col < colEnd && d > arc.lo)
			{
				if (col >= clipBeg)
					pOutput[n++] = uint16_t(_arcCoverage(arc, d));

				d += dx;
				dx += arc.step;
				col++;
			}
		}

		bandEnd = col;
		return n;
	}

	//____ _shapeRow() ________________________________________________________
	/*
		Coverage of one pixel row by a shape, as pieces clipped to clipBeg -> clipEnd.
		Coverage per pixel is written to pBuffer, which is advanced.
	*/

	int SoftShapes::_shapeRow(Shape& shape, int row, int clipBeg, int clipEnd, Piece* pPieces, uint16_t*& pBuffer)
	{
		if (shape.bEmpty)
			return 0;

		int64_t rowTop = int64_t(row) * 64;
		int oy = int(std::min(int64_t(shape.bottom), rowTop + 64) - std::max(int64_t(shape.top), rowTop));
		if (oy <= 0)
			return 0;

		int nPieces = 0;

		auto add = [&](int beg, int end, int value)
		{
			beg = std::max(beg, clipBeg);
			end = std::min(end, clipEnd);

			if (beg < end && value > 0)
				pPieces[nPieces++] = { beg, end, value, nullptr };
		};

		auto addBand = [&](int bandBeg, int nValues)
		{
			if (nValues > 0)
			{
				pPieces[nPieces++] = { std::max(bandBeg, clipBeg), std::max(bandBeg, clipBeg) + nValues, 0, pBuffer };
				pBuffer += nValues;
			}
		};

		int leftCol = floorPixel(shape.left);
		int rightCol = ceilPixel(shape.right) - 1;

		if (leftCol == rightCol)
		{
			add(leftCol, leftCol + 1, (shape.right - shape.left) * oy);
			return nPieces;
		}

		int rowCoverage = oy * 64;

		const Arc& tl = shape.arcs[0];
		const Arc& tr = shape.arcs[1];
		const Arc& br = shape.arcs[2];
		const Arc& bl = shape.arcs[3];

		int leftArc = -1, rightArc = -1;

		if (tl.bActive && row >= tl.rowBeg && row < tl.rowEnd)
			leftArc = 0;
		else if (bl.bActive && row >= bl.rowBeg && row < bl.rowEnd)
			leftArc = 3;

		if (tr.bActive && row >= tr.rowBeg && row < tr.rowEnd)
			rightArc = 1;
		else if (br.bActive && row >= br.rowBeg && row < br.rowEnd)
			rightArc = 2;

		int midBeg, midEnd;

		// Left side

		if (leftArc >= 0)
		{
			Arc& arc = shape.arcs[leftArc];

			int bandBeg, bandEnd;
			int nValues = _arcRow(arc, row, clipBeg, clipEnd, bandBeg, bandEnd, pBuffer);

			addBand(bandBeg, nValues);
			add(bandEnd, arc.colEnd, c_fullCoverage);
			midBeg = arc.colEnd;
		}
		else
		{
			int ox = (leftCol + 1) * 64 - shape.left;
			if (ox < 64)
			{
				add(leftCol, leftCol + 1, ox * oy);
				midBeg = leftCol + 1;
			}
			else
				midBeg = leftCol;
		}

		// Middle

		int oxRight = shape.right - rightCol * 64;

		if (rightArc >= 0)
			midEnd = shape.arcs[rightArc].colBeg;
		else
			midEnd = oxRight < 64 ? rightCol : rightCol + 1;

		add(midBeg, midEnd, rowCoverage);

		// Right side

		if (rightArc >= 0)
		{
			Arc& arc = shape.arcs[rightArc];

			int bandBeg, bandEnd;
			int nValues = _arcRow(arc, row, clipBeg, clipEnd, bandBeg, bandEnd, pBuffer);

			add(arc.colBeg, bandBeg, c_fullCoverage);
			addBand(bandBeg, nValues);
		}
		else if (oxRight < 64)
			add(rightCol, rightCol + 1, oxRight * oy);

		return nPieces;
	}

}
