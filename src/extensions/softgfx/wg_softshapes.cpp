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

	//____ Sine table _________________________________________________________
	// sin(i/1024 * pi/2) in 2.30 fixed point, generated at compile time.

	namespace
	{
		constexpr double constexprSin(double x)
		{
			double term = x, sum = x;
			for (int n = 1; n < 20; n++)
			{
				term *= -x * x / ((2 * n) * (2 * n + 1));
				sum += term;
			}
			return sum;
		}

		struct SinTable
		{
			int32_t v[1026];

			constexpr SinTable() : v{}
			{
				for (int i = 0; i <= 1025; i++)
				{
					double angle = (i > 1024 ? 1024 : i) * (3.14159265358979323846 / 2) / 1024;
					v[i] = int32_t(constexprSin(angle) * 1073741824.0 + 0.5);
				}
			}
		};

		constexpr SinTable c_sinTable;
	}

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

			// Split into spans, fill = inner coverage and outline = outer - inner. Steps through
			// stretches of pixels where no piece begins or ends.

			Span*		pSpans = m_spans.data();
			int			nSpans = 0;
			uint16_t*	pOut = m_outputBuffer.data();

			int io = 0, ii = 0;
			int next = outerPieces[0].begin;

			while (true)
			{
				const int beg = next;

				while (io < nOuter && outerPieces[io].end <= beg)
					io++;

				if (io == nOuter)
					break;

				const Piece* po = &outerPieces[io];

				if (po->begin > beg)
				{
					next = po->begin;
					continue;
				}

				next = po->end;

				while (ii < nInner && pInnerPieces[ii].end <= beg)
					ii++;

				const Piece* pi = nullptr;

				if (ii < nInner)
				{
					if (pInnerPieces[ii].begin <= beg)
					{
						pi = &pInnerPieces[ii];
						next = std::min(next, pi->end);
					}
					else
						next = std::min(next, pInnerPieces[ii].begin);
				}

				const int end = next;

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
							pSpans[nSpans++] = { beg, end, segment, false, nullptr, 0, nullptr };
					}
					else
					{
						pSpans[nSpans++] = { beg, end, -1, true, pOut, 2, layerSegments };
						*pOut++ = uint16_t(innerCov);
						*pOut++ = uint16_t(outerCov - innerCov);
					}
				}
				else
				{
					if (pPrev && pPrev->segment == -1 && !pPrev->bUniform && pPrev->end == beg)
						pPrev->end = end;
					else
						pSpans[nSpans++] = { beg, end, -1, false, pOut, 2, layerSegments };

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
				pSink->row(y, nSpans, pSpans);
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


	//____ _sin30() ___________________________________________________________
	// Sine in 2.30 fixed point, of angle where 2^32 is a full turn.

	int32_t SoftShapes::_sin30(uint32_t angle)
	{
		uint32_t quadrant = angle >> 30;
		uint32_t a = angle & 0x3FFFFFFF;

		if (quadrant & 1)
			a = 0x40000000 - a;

		uint32_t idx = a >> 20;
		uint32_t frac = a & 0xFFFFF;

		int32_t v0 = c_sinTable.v[idx];
		int32_t v1 = c_sinTable.v[idx + 1];
		int32_t v = v0 + int32_t((int64_t(v1 - v0) * frac) >> 20);

		return quadrant >= 2 ? -v : v;
	}

	//____ _pseudoAngle() _____________________________________________________
	/*
		A value that grows with the angle of direction (x,y) (y pointing down), clockwise
		from 12 o'clock, 2^32 being a full turn. Not linear in the angle, but monotonic, which
		is all that is needed to tell which slice a direction is in, and needs no trigonometry.
	*/

	uint32_t SoftShapes::_pseudoAngle(int64_t x, int64_t y)
	{
		int64_t u = x;
		int64_t v = -y;

		if (u == 0 && v == 0)
			return 0;

		if (u >= 0 && v > 0)
			return uint32_t((u << 30) / (u + v));
		if (u > 0 && v <= 0)
			return uint32_t((uint64_t(1) << 30) + uint64_t(((-v) << 30) / (u - v)));
		if (u <= 0 && v < 0)
			return uint32_t((uint64_t(2) << 30) + uint64_t(((-u) << 30) / (-u - v)));

		return uint32_t((uint64_t(3) << 30) + uint64_t((v << 30) / (v - u)));
	}

	//____ _setupRingAngles() _________________________________________________
	/*
		Splits the turn into elements: the slices, followed by a gap (back segment) if they
		don't make a full turn. Each element starts at a boundary.
	*/

	void SoftShapes::_setupRingAngles(const Ring& ring)
	{
		m_boundaries.clear();
		m_elementSegments.clear();
		m_upperOrder.clear();
		m_lowerOrder.clear();

		const uint64_t fullTurn = uint64_t(1) << 32;

		std::vector<uint64_t>& starts = m_elementStarts;
		starts.clear();

		uint64_t pos = 0;
		for (int i = 0; i < ring.nSlices && pos < fullTurn; i++)
		{
			uint64_t size = uint64_t(std::min(ring.pSliceSizes[i], c_fullTurn)) << 1;
			if (size == 0)
				continue;

			starts.push_back(pos);
			m_elementSegments.push_back(c_ringFirstSliceSegment + i);
			pos += size;
		}

		if (pos < fullTurn)
		{
			starts.push_back(pos);
			m_elementSegments.push_back(c_ringBackSegment);
		}

		int nElements = (int)starts.size();
		if (nElements <= 1)
			return;

		uint32_t start = (ring.start % c_fullTurn) << 1;

		m_boundaries.resize(nElements);

		for (int e = 0; e < nElements; e++)
		{
			Boundary& b = m_boundaries[e];
			b.angle = uint32_t(start + starts[e]);
			b.element = e;
			b.prevElement = (e == 0) ? nElements - 1 : e - 1;
		}

		std::sort(m_boundaries.begin(), m_boundaries.end(), [](const Boundary& a, const Boundary& b) { return a.angle < b.angle; });

		for (auto& b : m_boundaries)
		{
			b.sx = _sin30(b.angle);
			b.sy = -_sin30(b.angle + 0x40000000);
			b.pseudoAngle = _pseudoAngle(b.sx, b.sy);

			// Rays too close to horizontal cross rows only far outside any canvas, except
			// for the rows around the center, which are sampled.

			b.bFlat = std::abs(b.sy) < 1024;

			if (!b.bFlat)
			{
				int64_t slope = (int64_t(b.sx) * c_one) / b.sy;		// 32.32
				b.slopeInt = slope >> 32;
				b.slopeFrac = uint32_t(slope & 0xFFFFFFFF);
				b.halfBand = (int64_t(32) << 30) / std::abs(b.sy);
			}
		}

		// Boundaries crossing rows above and below the center, in order of their crossings
		// from left to right. Rays from the center don't cross, so that is clockwise from
		// 9 o'clock above and counter-clockwise from 9 o'clock below.

		int nBoundaries = (int)m_boundaries.size();

		for (int pass = 0; pass < 2; pass++)
			for (int i = 0; i < nBoundaries; i++)
			{
				const Boundary& b = m_boundaries[i];
				if (!b.bFlat && b.sy < 0 && (b.angle >= 0xC0000000) == (pass == 0))
					m_upperOrder.push_back(i);
			}

		for (int i = nBoundaries - 1; i >= 0; i--)
		{
			const Boundary& b = m_boundaries[i];
			if (!b.bFlat && b.sy > 0)
				m_lowerOrder.push_back(i);
		}
	}

	//____ _elementAt() _______________________________________________________

	int SoftShapes::_elementAt(uint32_t pseudoAngle) const
	{
		if (m_boundaries.empty())
			return 0;

		// Last boundary at or before pseudoAngle, wrapping around to the last one.

		int lo = 0, hi = (int)m_boundaries.size();
		while (lo < hi)
		{
			int mid = (lo + hi) / 2;
			if (m_boundaries[mid].pseudoAngle <= pseudoAngle)
				lo = mid + 1;
			else
				hi = mid;
		}

		int idx = lo - 1;
		if (idx < 0)
			idx = (int)m_boundaries.size() - 1;

		return m_boundaries[idx].element;
	}

	//____ _angularRow() ______________________________________________________
	/*
		Splits a row into pieces by slice. Between slice boundaries a piece belongs to one
		element. Where boundaries cross the row, pixels are split between elements, worked
		out per pixel. Rows around the center are sampled per pixel.
	*/

	int SoftShapes::_angularRow(int64_t rowY, int clipBeg, int clipEnd)
	{
		m_angularPieces.clear();
		m_active.clear();

		auto add = [&](int beg, int end, int element, int firstActive, int nActive, bool bSampled)
		{
			beg = std::max(beg, clipBeg);
			end = std::min(end, clipEnd);
			if (beg < end)
				m_angularPieces.push_back({ beg, end, element, firstActive, nActive, bSampled });
		};

		if (m_boundaries.empty())
		{
			add(clipBeg, clipEnd, 0, 0, 0, false);
			return (int)m_angularPieces.size();
		}

		int64_t dy = rowY - m_cy;

		if (dy > -96 && dy < 96)
		{
			add(clipBeg, clipEnd, -1, 0, 0, true);
			return (int)m_angularPieces.size();
		}

		bool bUpper = dy < 0;

		// Crossings, in order along x.

		const std::vector<int>& order = bUpper ? m_upperOrder : m_lowerOrder;
		int nActive = (int)order.size();

		if (nActive == 0)
		{
			add(clipBeg, clipEnd, _elementAt(bUpper ? 0 : 0x80000000), 0, 0, false);
			return (int)m_angularPieces.size();
		}

		m_active.resize(nActive);

		for (int i = 0; i < nActive; i++)
		{
			const Boundary& b = m_boundaries[order[i]];

			int64_t x = m_cx + dy * b.slopeInt + ((dy * int64_t(b.slopeFrac)) >> 32);

			ActiveBoundary& a = m_active[i];
			a.boundary = order[i];
			a.x = x;
			a.bandBeg = floorPixel(x - b.halfBand - 33) + 1;
			a.bandEnd = floorPixel(x + b.halfBand - 31) + 1;
		}

		// Merge overlapping bands.

		struct Group { int beg, end, first, last; };
		Group groups[64];
		int nGroups = 0;

		std::vector<Group> moreGroups;			// In the unlikely case of many groups.
		bool bManyGroups = false;

		auto groupAt = [&](int i) -> Group& { return bManyGroups ? moreGroups[i] : groups[i]; };

		for (int i = 0; i < (int)m_active.size(); i++)
		{
			Group g = { m_active[i].bandBeg, m_active[i].bandEnd, i, i };

			while (nGroups > 0 && g.beg < groupAt(nGroups - 1).end)
			{
				Group& prev = groupAt(nGroups - 1);
				g.beg = std::min(g.beg, prev.beg);
				g.end = std::max(g.end, prev.end);
				g.first = prev.first;
				nGroups--;
			}

			if (!bManyGroups && nGroups == 64)
			{
				moreGroups.assign(groups, groups + 64);
				bManyGroups = true;
			}

			if (bManyGroups)
			{
				if ((int)moreGroups.size() <= nGroups)
					moreGroups.resize(nGroups + 1);
				moreGroups[nGroups++] = g;
			}
			else
				groups[nGroups++] = g;
		}

		// Elements left and right of boundaries along x.

		auto elementLeftOf = [&](int active) { const Boundary& b = m_boundaries[m_active[active].boundary]; return bUpper ? b.prevElement : b.element; };
		auto elementRightOf = [&](int active) { const Boundary& b = m_boundaries[m_active[active].boundary]; return bUpper ? b.element : b.prevElement; };

		int x = clipBeg;
		int element = elementLeftOf(0);

		for (int i = 0; i < nGroups; i++)
		{
			const Group& g = groupAt(i);

			add(std::min(x, g.beg), g.beg, element, 0, 0, false);
			add(g.beg, g.end, -1, g.first, g.last - g.first + 1, false);

			element = elementRightOf(g.last);
			x = std::max(x, g.end);
		}

		add(x, clipEnd, element, 0, 0, false);

		return (int)m_angularPieces.size();
	}

	//____ _pastShare() _______________________________________________________
	// Share (0 -> 4096) of pixel, centered at dx, dy from the center, past boundary clockwise.

	inline int SoftShapes::_pastShare(const Boundary& b, int32_t dx, int32_t dy)
	{
		int64_t d = -int64_t(b.sy) * dx + int64_t(b.sx) * dy;		// Distance from boundary, spx with 30 binals.
		int64_t h = 2048 + (d >> 24);
		return int(h < 0 ? 0 : h > c_fullCoverage ? c_fullCoverage : h);
	}

	//____ _pixelBoundaries() _________________________________________________
	/*
		Boundaries running through a pixel not containing the center, centered at dx, dy
		from it. Returns their number and the index of the first in clockwise order, or of
		the next boundary if there are none. Found from the angles of the pixel corners.
	*/

	int SoftShapes::_pixelBoundaries(int32_t dx, int32_t dy, int& first) const
	{
		uint32_t corners[4] = { _pseudoAngle(dx - 32, dy - 32), _pseudoAngle(dx + 32, dy - 32),
								_pseudoAngle(dx + 32, dy + 32), _pseudoAngle(dx - 32, dy + 32) };

		// The pixel spans less than half a turn, so differences to one corner tell the extent.

		int32_t lo = 0, hi = 0;
		for (int i = 1; i < 4; i++)
		{
			int32_t diff = int32_t(corners[i] - corners[0]);
			lo = std::min(lo, diff);
			hi = std::max(hi, diff);
		}

		uint32_t beg = corners[0] + uint32_t(lo);
		uint32_t extent = uint32_t(hi - lo);

		int nBoundaries = (int)m_boundaries.size();

		int l = 0, h = nBoundaries;
		while (l < h)
		{
			int mid = (l + h) / 2;
			if (m_boundaries[mid].pseudoAngle < beg)
				l = mid + 1;
			else
				h = mid;
		}

		first = l == nBoundaries ? 0 : l;

		int n = 0;
		while (n < nBoundaries && m_boundaries[(first + n) % nBoundaries].pseudoAngle - beg <= extent)
			n++;

		return n;
	}

	//____ _samplePixel() _____________________________________________________
	// Elements covering the pixel containing the center and their share (0 -> 4096) of it.

	void SoftShapes::_samplePixel(int32_t dx, int32_t dy, int* pElements, int* pFractions, int& nElements) const
	{
		nElements = 0;

		static const int offsets[4] = { -24, -8, 8, 24 };

		for (int sy = 0; sy < 4; sy++)
		{
			for (int sx = 0; sx < 4; sx++)
			{
				int e = _elementAt(_pseudoAngle(dx + offsets[sx], dy + offsets[sy]));

				int i = 0;
				while (i < nElements && pElements[i] != e)
					i++;

				if (i == nElements)
				{
					pElements[nElements] = e;
					pFractions[nElements++] = 0;
				}

				pFractions[i] += c_fullCoverage / 16;
			}
		}
	}

	//____ rasterizeRing() ____________________________________________________

	void SoftShapes::rasterizeRing(const Ring& ring, const RectI& clip, RowSink* pSink)
	{
		const RectSPX& rect = ring.rect;

		if (rect.w <= 0 || rect.h <= 0 || clip.w <= 0 || clip.h <= 0)
			return;

		// Radial geometry

		m_cx = int64_t(rect.x) + rect.w / 2;
		m_cy = int64_t(rect.y) + rect.h / 2;

		spx outerRadius = std::min(rect.w, rect.h) / 2;

		bool bHole = ring.thickness > 0 && ring.thickness < outerRadius;
		spx innerRadius = bHole ? outerRadius - ring.thickness : 0;

		spx outline = std::max(ring.outline, 0);
		outline = std::min(outline, bHole ? ring.thickness / 2 : outerRadius);

		// Nested shapes: rect, outer edge, inside of outer outline, outside of inner outline, inner edge.

		Shape shapes[5];

		auto circle = [&](Shape& shape, spx r)
		{
			if (r <= 0)
			{
				shape.bEmpty = true;
				return;
			}

			spx radius[4] = { r, r, r, r };
			_setupShape(shape, RectSPX(spx(m_cx - r), spx(m_cy - r), r * 2, r * 2), radius);
		};

		spx square[4] = { 0, 0, 0, 0 };

		_setupShape(shapes[0], rect, square);

		if (ring.bSquare)
		{
			_setupShape(shapes[1], rect, square);
			_setupShape(shapes[2], RectSPX(rect.x + outline, rect.y + outline, rect.w - outline * 2, rect.h - outline * 2), square);
		}
		else
		{
			circle(shapes[1], outerRadius);
			circle(shapes[2], outerRadius - outline);
		}

		if (bHole)
		{
			circle(shapes[3], innerRadius + outline);
			circle(shapes[4], innerRadius);
		}
		else
		{
			shapes[3].bEmpty = true;
			shapes[4].bEmpty = true;
		}

		_setupRingAngles(ring);

		// Rows and columns

		int clipBeg = std::max(clip.x, floorPixel(rect.x));
		int clipEnd = std::min(clip.x + clip.w, ceilPixel(int64_t(rect.x) + rect.w));

		int rowBeg = std::max(clip.y, floorPixel(rect.y));
		int rowEnd = std::min(clip.y + clip.h, ceilPixel(int64_t(rect.y) + rect.h));

		if (clipBeg >= clipEnd || rowBeg >= rowEnd)
			return;

		int width = clipEnd - clipBeg;

		if ((int)m_coverageBuffer.size() < width * 10 + 16)
			m_coverageBuffer.resize(width * 10 + 16);

		// Rows

		Piece	pieces[5][c_maxPieces];
		int		nPieces[5] = { 0, 0, 0, 0, 0 };

		// Shapes that need rows of their own. Without outline its edges coincide with the
		// outer and inner edge, without hole there are no inner edges.

		int		ownShapes[5];
		int		nOwnShapes = 0;

		ownShapes[nOwnShapes++] = 0;
		ownShapes[nOwnShapes++] = 1;
		if (outline > 0)
			ownShapes[nOwnShapes++] = 2;
		if (bHole)
		{
			if (outline > 0)
				ownShapes[nOwnShapes++] = 3;
			ownShapes[nOwnShapes++] = 4;
		}

		std::vector<int>& spanLayerOfs = m_spanLayerOfs;
		std::vector<int>& spanCoverageOfs = m_spanCoverageOfs;

		int		elements[16];
		int		fractions[16];

		for (int y = rowBeg; y < rowEnd; y++)
		{
			int64_t rowY = int64_t(y) * 64 + 32;

			uint16_t* pBuffer = m_coverageBuffer.data();

			for (int i = 0; i < nOwnShapes; i++)
				nPieces[ownShapes[i]] = _shapeRow(shapes[ownShapes[i]], y, clipBeg, clipEnd, pieces[ownShapes[i]], pBuffer);

			if (nPieces[0] == 0)
				continue;

			_angularRow(rowY, clipBeg, clipEnd);

			// Produce spans. Layer lists and coverage are written to growing buffers, pointers
			// set when the row is done.

			m_spans.clear();
			spanLayerOfs.clear();
			spanCoverageOfs.clear();
			m_layerBuffer.clear();
			m_ringOutput.clear();

			// Coverage of each shape in the current stretch: per pixel from pValues (indexed
			// from base), or value.

			struct Source
			{
				const uint16_t*	pValues;
				int				base;
				int				value;
			};

			Source src[5] = {};

			auto radialAt = [&src](int x)
			{
				int v[5];
				for (int i = 0; i < 5; i++)
					v[i] = src[i].pValues ? src[i].pValues[x - src[i].base] : src[i].value;

				int r = v[0];
				int c0 = std::min(v[1], r);
				int c1 = std::min(v[2], c0);
				int c2 = std::min(v[3], c1);
				int c3 = std::min(v[4], c2);

				return RadialValues{ r - c0, c0 - c1 + c2 - c3, c3, c1 - c2 };
			};

			auto beginSpan = [&](int beg, int end, int segment, bool bUniform)
			{
				m_spans.push_back({ beg, end, segment, bUniform, nullptr, 0, nullptr });
				spanLayerOfs.push_back((int)m_layerBuffer.size());
				spanCoverageOfs.push_back((int)m_ringOutput.size());
			};

			auto addLayer = [&](int segment)
			{
				m_layerBuffer.push_back(segment);
				m_spans.back().nLayers++;
			};

			// Layers of the radial segments. Outline and hub are left out where they can't
			// have any coverage.

			auto addRadialLayers = [&]()
			{
				addLayer(c_ringBackSegment);
				if (outline > 0)
					addLayer(c_ringOutlineSegment);
				if (bHole)
					addLayer(c_ringHubSegment);
			};

			auto pushRadial = [&](const RadialValues& v)
			{
				m_ringOutput.push_back(uint16_t(v.back));
				if (outline > 0)
					m_ringOutput.push_back(uint16_t(v.outline));
				if (bHole)
					m_ringOutput.push_back(uint16_t(v.hub));
			};

			// Step through stretches of pixels where no shape or angular piece begins or ends.

			int pieceIdx[5] = { 0, 0, 0, 0, 0 };
			int angularIdx = 0;
			const int nAngular = (int)m_angularPieces.size();

			int next = clipBeg;

			while (next < clipEnd)
			{
				const int beg = next;
				next = clipEnd;

				bool bInRect = false;
				bool bConstRadial = true;

				for (int k = 0; k < nOwnShapes; k++)
				{
					int i = ownShapes[k];
					const Piece* pPieces = pieces[i];
					int& idx = pieceIdx[i];

					while (idx < nPieces[i] && pPieces[idx].end <= beg)
						idx++;

					Source& source = src[i];
					source.pValues = nullptr;
					source.value = 0;

					if (idx < nPieces[i])
					{
						const Piece& piece = pPieces[idx];

						if (piece.begin <= beg)
						{
							next = std::min(next, piece.end);

							if (piece.pValues)
							{
								source.pValues = piece.pValues;
								source.base = piece.begin;
								bConstRadial = false;
							}
							else
								source.value = piece.value;

							if (i == 0)
								bInRect = true;
						}
						else
							next = std::min(next, piece.begin);
					}
				}

				if (outline == 0)
				{
					src[2] = src[1];
					src[3] = src[4];
				}

				if (!bInRect)
					continue;

				while (angularIdx < nAngular && m_angularPieces[angularIdx].end <= beg)
					angularIdx++;

				if (angularIdx == nAngular)
					break;

				const AngularPiece& ap = m_angularPieces[angularIdx];
				next = std::min(next, ap.end);

				const int end = next;

				if (bConstRadial && ap.element >= 0)
				{
					RadialValues v = radialAt(beg);
					int ringSegment = m_elementSegments[ap.element];

					int segment = -1;
					if (v.ring == c_fullCoverage)
						segment = ringSegment;
					else if (v.back == c_fullCoverage)
						segment = c_ringBackSegment;
					else if (v.outline == c_fullCoverage)
						segment = c_ringOutlineSegment;
					else if (v.hub == c_fullCoverage)
						segment = c_ringHubSegment;

					if (segment >= 0)
					{
						if (!m_spans.empty() && m_spans.back().segment == segment && m_spans.back().end == beg)
							m_spans.back().end = end;
						else
							beginSpan(beg, end, segment, false);
					}
					else if (v.back + v.outline + v.hub + v.ring > 0)
					{
						beginSpan(beg, end, -1, true);
						addRadialLayers();
						addLayer(ringSegment);

						pushRadial(v);
						m_ringOutput.push_back(uint16_t(v.ring));
					}
				}
				else if (ap.element >= 0)
				{
					// Radial edges within one slice.

					beginSpan(beg, end, -1, false);
					addRadialLayers();
					addLayer(m_elementSegments[ap.element]);

					for (int x = beg; x < end; x++)
					{
						RadialValues v = radialAt(x);
						pushRadial(v);
						m_ringOutput.push_back(uint16_t(v.ring));
					}
				}
				else if (!ap.bSampled)
				{
					// Slice boundaries crossing the row. A pixel is split between the elements
					// around the boundaries running through it, in clockwise order. Boundaries
					// left and right of it don't matter. Where these limits are moves right with
					// x, so they are followed from pixel to pixel. Pixels split between the same
					// elements share a span.

					bool bUpper = rowY < m_cy;
					const ActiveBoundary* pActive = m_active.data() + ap.firstActive;
					int nActive = ap.nActive;

					int32_t dy = int32_t(rowY - m_cy);
					int32_t dx = 0;

					auto boundary = [&](int j) -> const Boundary& { return m_boundaries[pActive[j].boundary]; };
					auto rightShare = [&](int j) { int h = _pastShare(boundary(j), dx, dy); return bUpper ? h : c_fullCoverage - h; };

					int lo = 0, hi = -1;					// Boundaries running through pixel, along x.
					int spanLo = -1, spanHi = -1;

					for (int x = beg; x < end; x++)
					{
						dx = int32_t(int64_t(x) * 64 + 32 - m_cx);

						while (lo < nActive && rightShare(lo) == c_fullCoverage)
							lo++;

						hi = std::max(hi, lo - 1);

						while (hi + 1 < nActive && rightShare(hi + 1) > 0)
							hi++;

						int first = bUpper ? lo : hi;			// Clockwise.
						int step = bUpper ? 1 : -1;
						int n = hi - lo + 1;

						if (lo != spanLo || hi != spanHi)
						{
							// Element before first boundary, or the one pixel is in.

							int element;
							if (first >= 0 && first < nActive)
								element = boundary(first).prevElement;
							else
								element = boundary(bUpper ? nActive - 1 : 0).element;

							beginSpan(x, x + 1, -1, false);
							addRadialLayers();
							addLayer(m_elementSegments[element]);

							for (int i = 0, j = first; i < n; i++, j += step)
								addLayer(m_elementSegments[boundary(j).element]);

							spanLo = lo;
							spanHi = hi;
						}
						else
							m_spans.back().end = x + 1;

						RadialValues v = radialAt(x);

						pushRadial(v);

						// Share past each boundary, kept decreasing clockwise. Rounded cumulatively,
						// so that slices add up to exactly the ring.

						int prevH = c_fullCoverage, prevCoverage = 0;

						for (int i = 0, j = first; i < n; i++, j += step)
						{
							int h = std::min(_pastShare(boundary(j), dx, dy), prevH);
							prevH = h;

							int coverage = (v.ring * (c_fullCoverage - h) + 2048) >> 12;
							m_ringOutput.push_back(uint16_t(coverage - prevCoverage));
							prevCoverage = coverage;
						}

						m_ringOutput.push_back(uint16_t(v.ring - prevCoverage));
					}
				}
				else
				{
					// Rows around the center, where boundaries can be too close to horizontal to
					// follow along the row. Boundaries running through each pixel are found from
					// the angles of its corners instead. The pixel containing the center is sampled.

					int32_t dy = int32_t(rowY - m_cy);
					int nBoundaries = (int)m_boundaries.size();

					for (int x = beg; x < end; x++)
					{
						RadialValues v = radialAt(x);

						beginSpan(x, x + 1, -1, false);
						addRadialLayers();

						pushRadial(v);

						if (v.ring == 0)
							continue;

						int32_t dx = int32_t(int64_t(x) * 64 + 32 - m_cx);

						int prevCoverage = 0;

						if (dx >= -32 && dx <= 32 && dy >= -32 && dy <= 32)
						{
							int nElements;
							_samplePixel(dx, dy, elements, fractions, nElements);

							int cumulative = 0;
							for (int i = 0; i < nElements - 1; i++)
							{
								addLayer(m_elementSegments[elements[i]]);

								cumulative += fractions[i];
								int coverage = (v.ring * cumulative + 2048) >> 12;
								m_ringOutput.push_back(uint16_t(coverage - prevCoverage));
								prevCoverage = coverage;
							}

							addLayer(m_elementSegments[elements[nElements - 1]]);
						}
						else
						{
							int first;
							int n = _pixelBoundaries(dx, dy, first);

							addLayer(m_elementSegments[m_boundaries[first].prevElement]);

							int prevH = c_fullCoverage;

							for (int i = 0; i < n; i++)
							{
								const Boundary& b = m_boundaries[(first + i) % nBoundaries];
								addLayer(m_elementSegments[b.element]);

								int h = std::min(_pastShare(b, dx, dy), prevH);
								prevH = h;

								int coverage = (v.ring * (c_fullCoverage - h) + 2048) >> 12;
								m_ringOutput.push_back(uint16_t(coverage - prevCoverage));
								prevCoverage = coverage;
							}
						}

						m_ringOutput.push_back(uint16_t(v.ring - prevCoverage));
					}
				}
			}

			// Set pointers into buffers, now that they won't move.

			for (int i = 0; i < (int)m_spans.size(); i++)
			{
				Span& span = m_spans[i];
				if (span.segment < 0)
				{
					span.pLayerSegments = m_layerBuffer.data() + spanLayerOfs[i];
					span.pCoverage = m_ringOutput.data() + spanCoverageOfs[i];
				}
			}

			if (!m_spans.empty())
				pSink->row(y, (int)m_spans.size(), m_spans.data());
		}
	}

}
