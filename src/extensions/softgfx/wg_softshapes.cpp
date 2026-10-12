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
#include <climits>

namespace wg
{
	// Pixel index of spx value, rounded down or up, for values that can be far outside any canvas.

	static inline int floorPixel(int64_t value)
	{
		int64_t pixel = value >> 6;
		return int(pixel < -0x40000000 ? -0x40000000 : pixel > 0x40000000 ? 0x40000000 : pixel);
	}

	static inline int ceilPixel(int64_t value) { return floorPixel(value + 63); }

	static const int64_t c_one = int64_t(1) << 32;			// 1.0 in 32.32 fixed point.

	namespace
	{
		//____ Sine table _____________________________________________________
		// sin(i/1024 * pi/2) in 2.30 fixed point, generated at compile time.

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

		//____ Reciprocal table _______________________________________________
		// 2^37 / n for edges spanning up to 16 pixels along a row, to avoid divisions.

		struct RecipTable
		{
			int64_t v[1024];

			constexpr RecipTable() : v{}
			{
				for (int i = 1; i < 1024; i++)
					v[i] = (int64_t(1) << 37) / i;
			}
		};

		constexpr RecipTable c_recipTable;

		inline int64_t recip37(int64_t n)
		{
			return n < 1024 ? c_recipTable.v[n] : (int64_t(1) << 37) / n;
		}

		//____ isqrt() ________________________________________________________
		// Square root rounded to nearest, digit by digit.

		inline int64_t isqrt(uint64_t value)
		{
			if (value == 0)
				return 0;

#if defined(__GNUC__) || defined(__clang__)
			int topBit = 63 - __builtin_clzll(value);
#else
			int topBit = 0;
			for (int step = 32; step > 0; step >>= 1)
				if ((value >> (topBit + step)) != 0)
					topBit += step;
#endif

			uint64_t root = 0;
			uint64_t bit = uint64_t(1) << (topBit & ~1);

			while (bit != 0)
			{
				if (value >= root + bit)
				{
					value -= root + bit;
					root = (root >> 1) + bit;
				}
				else
					root >>= 1;

				bit >>= 2;
			}

			return int64_t(root + (value > root ? 1 : 0));
		}

		//____ mulFrac() ______________________________________________________
		// value * frac / 2^32, without overflow for values up to 2^47.

		inline int64_t mulFrac(int64_t value, uint32_t frac)
		{
			return ((value * int64_t(frac >> 16)) >> 16) + ((value * int64_t(frac & 0xFFFF)) >> 32);
		}

		const int16_t c_noColor[4] = { 0, 0, 0, 0 };
	}

	//____ EdgeShape __________________________________________________________
	/*
		A shape as edges crossing sub-rows, given by the shape to the rasterizer. Shapes
		derive from it and provide:

		int		edges(int64_t ya, int64_t yb, EdgeDef* pEdges) const

			Edges crossing the sub-row from ya to yb, in order along x. Returns their number.

		int64_t	x(const EdgeDef& edge, int64_t y)

			x of an edge at y. Edges must not cross, except clamped edges by rounding. Each
			edge has an id, the same wherever it is, so x can be reused from the bottom of
			one sub-row for the top of the next.

		bool	split(const EdgeDef& edge, int64_t ya, int64_t xa, int64_t yb, int64_t xb, int64_t& y)

			Tells if the edge from (ya, xa) to (yb, xb) needs to be split into a polyline,
			where it bends or kinks, and at which y strictly between ya and yb.

		int		uniformEnd(int row) const

			Row after the rows from row on that are the same as row.
	*/

	class SoftShapes::EdgeShape
	{
	public:
		const int64_t*	pSplits = nullptr;		// Where sub-rows must begin and end, sorted.
		int				nSplits = 0;
		int				maxEdges = 0;			// Most edges in a sub-row.
		int				nIds = 0;				// Ids of edges are 0 up to nIds.
	};

	//____ splitPoint() _______________________________________________________
	/*
		Point strictly between a and b, which are at least 2 apart, where the most low
		bits are zero. It is near the middle and, unlike the middle rounded down, where
		the mirror image of the interval would be split, so that symmetric shapes stay
		symmetric.
	*/

	static inline int64_t splitPoint(int64_t a, int64_t b)
	{
		int bits = 0;
		while ((int64_t(2) << bits) <= b - a)
			bits++;

		int64_t mid = ((a >> bits) + 1) << bits;
		while (mid >= b)
		{
			bits--;
			mid = ((a >> bits) + 1) << bits;
		}

		return mid;
	}

	//____ _area() ____________________________________________________________
	/*
		Area right of a piece in a sub-row as high as the piece, left of x, 64 for each
		spx along x right of the piece. A pixel gets _area(right side) - _area(left side),
		which is 4096 if fully right of the piece.

		The part of a vertical line at x right of the piece grows linearly from lo to hi,
		so the area is quadratic between them.
	*/

	inline int64_t SoftShapes::_area(const Piece& piece, int64_t x)
	{
		if (x <= piece.lo)
			return 0;

		if (x >= piece.hi)
			return (piece.hi - piece.lo) * 32 + (x - piece.hi) * 64;

		int64_t d = x - piece.lo;
		return (d * ((d * piece.inv) >> 5)) >> 27;			// d^2 * 32 / (hi - lo)
	}

	inline int SoftShapes::_rightOf(const Piece& piece, int pixel)
	{
		if (pixel < piece.bandBeg)
			return 0;

		if (pixel >= piece.bandEnd)
			return c_fullCoverage;

		int64_t x = int64_t(pixel) * 64;
		return int(_area(piece, x + 64) - _area(piece, x));
	}

	//____ _edgeArea() ________________________________________________________
	// Area of pixel right of edge, 4096 for each spx of height of the sub-row.

	inline int SoftShapes::_edgeArea(const Edge& edge, int pixel) const
	{
		const Piece* pPiece = m_pieces.data() + edge.first;

		int area = 0;
		for (int i = 0; i < edge.n; i++)
			area += pPiece[i].h * _rightOf(pPiece[i], pixel);

		return area;
	}

	//____ _edgeX() ___________________________________________________________

	template<class Shape>
	inline int64_t SoftShapes::_edgeX(Shape& shape, const EdgeDef& edge, int64_t y)
	{
		if (m_cacheY[edge.id] == y)
			return m_cacheX[edge.id];

		return shape.x(edge, y);
	}

	//____ _mixColor() ________________________________________________________
	// Colors of a drawn segment at x on current row, null if not drawn.

	inline const int16_t* SoftShapes::_mixColor(int segment, int x)
	{
		MixSource& mix = m_mix[segment];

		if (mix.stride == 0)
			return mix.p;

		if (mix.row != m_row)
		{
			bool bFlat;
			mix.p = m_pSink->segmentColors(segment, m_row, m_clipBeg, m_clipEnd, bFlat);
			mix.row = m_row;
		}

		return mix.p + (x - m_clipBeg) * 4;
	}

	//____ _fillRun() _________________________________________________________

	inline void SoftShapes::_fillRun(int beg, int end, int segment)
	{
		if (!m_mix[segment].p)
			return;

		if (m_nRuns > 0)
		{
			Run& last = m_runs[m_nRuns - 1];
			if (last.segment == segment && last.end == beg)
			{
				last.end = end;
				return;
			}
		}

		m_runs[m_nRuns++] = { beg, end, segment };
	}

	//____ _mixedPixel() ______________________________________________________

	inline SoftShapes::CoveragePixel& SoftShapes::_mixedPixel(int x)
	{
		if (m_nRuns > 0)
		{
			Run& last = m_runs[m_nRuns - 1];
			if (last.segment < 0 && last.end == x)
			{
				last.end = x + 1;
				return m_pixels[m_nPixels++];
			}
		}

		m_runs[m_nRuns++] = { x, x + 1, -1 };
		return m_pixels[m_nPixels++];
	}

	//____ _mixedRun() ________________________________________________________

	inline void SoftShapes::_mixedRun(int beg, int end, const CoveragePixel& pixel)
	{
		Run* pLast = m_nRuns > 0 ? &m_runs[m_nRuns - 1] : nullptr;

		if (pLast && pLast->segment < 0 && pLast->end == beg)
			pLast->end = end;
		else
			m_runs[m_nRuns++] = { beg, end, -1 };

		CoveragePixel* pPixels = m_pixels.data() + m_nPixels;
		for (int i = 0; i < end - beg; i++)
			pPixels[i] = pixel;

		m_nPixels += end - beg;
	}

	//____ _accumulate() ______________________________________________________

	inline void SoftShapes::_accumulate(int segment, int value)
	{
		if (segment < 0 || value <= 0)
			return;

		if (m_accumulated[segment] == 0)
			m_touched[m_nTouched++] = segment;

		m_accumulated[segment] += value;
	}

	//____ _rasterize() _______________________________________________________
	/*
		Rasterizes a shape into rows rowBeg up to rowEnd, columns clipBeg up to clipEnd.
	*/

	template<class Shape>
	void SoftShapes::_rasterize(Shape& shape, int nSegments, int rowBeg, int rowEnd, int clipBeg, int clipEnd, RowSink* pSink)
	{
		if (rowBeg >= rowEnd || clipBeg >= clipEnd)
			return;

		m_pSink = pSink;
		m_clipBeg = clipBeg;
		m_clipEnd = clipEnd;

		// Colors of segments. Rows can only be done together if all are flat.

		if ((int)m_mix.size() < nSegments)
			m_mix.resize(nSegments);

		bool bAnyDrawn = false;
		bool bAllFlat = true;

		for (int i = 0; i < nSegments; i++)
		{
			bool bFlat = true;
			const int16_t* p = pSink->segmentColors(i, rowBeg, clipBeg, clipEnd, bFlat);

			m_mix[i] = { p, (p && !bFlat) ? 4 : 0, rowBeg };

			if (p)
			{
				bAnyDrawn = true;
				if (!bFlat)
					bAllFlat = false;
			}
		}

		if (!bAnyDrawn)
			return;

		const int width = clipEnd - clipBeg;

		if ((int)m_runs.size() < width + 1)
		{
			m_runs.resize(width + 1);
			m_pixels.resize(width + 1);
		}

		if ((int)m_accumulated.size() < nSegments)
		{
			m_accumulated.resize(nSegments, 0);
			m_touched.resize(nSegments);
			m_layerSegments.resize(nSegments);
			m_layerCoverage.resize(nSegments);
		}

		if ((int)m_edgeDefs.size() < shape.maxEdges)
		{
			m_edgeDefs.resize(shape.maxEdges);
			m_edgeTop.resize(shape.maxEdges);
			m_edgeBottom.resize(shape.maxEdges);
		}

		m_cacheY.assign(shape.nIds, INT64_MIN);
		m_cacheX.resize(shape.nIds);

		if (m_pieces.size() < 64)
			m_pieces.resize(64);

		const int64_t* pSplits = shape.pSplits;
		const int nSplits = shape.nSplits;
		int splitIdx = 0;

		for (int row = rowBeg; row < rowEnd; )
		{
			const int64_t y0 = int64_t(row) * 64;
			const int64_t y1 = y0 + 64;

			int rowsEnd = row + 1;
			if (bAllFlat)
				rowsEnd = std::max(row + 1, std::min(shape.uniformEnd(row), rowEnd));

			m_row = row;

			// Sub-rows, split where edges begin and end.

			while (splitIdx < nSplits && pSplits[splitIdx] <= y0)
				splitIdx++;

			int splitEnd = splitIdx;
			while (splitEnd < nSplits && pSplits[splitEnd] < y1)
				splitEnd++;

			const int nSubRows = splitEnd - splitIdx + 1;

			if ((int)m_subRows.size() < nSubRows)
				m_subRows.resize(nSubRows);

			if ((int)m_edges.size() < nSubRows * shape.maxEdges)
				m_edges.resize(nSubRows * shape.maxEdges);

			m_nPieces = 0;
			int nEdges = 0;

			for (int k = 0; k < nSubRows; k++)
			{
				const int64_t ya = k == 0 ? y0 : pSplits[splitIdx + k - 1];
				const int64_t yb = k == nSubRows - 1 ? y1 : pSplits[splitIdx + k];

				SubRow& subRow = m_subRows[k];
				subRow.h = int(yb - ya);
				subRow.first = nEdges;
				subRow.n = _subRowEdges(shape, ya, yb, m_edges.data() + nEdges);
				nEdges += subRow.n;
			}

			splitIdx = splitEnd;

			// Runs

			m_nRuns = 0;
			m_nPixels = 0;

			int nFilled = 0;
			int filled = 0;

			for (int k = 0; k < nSubRows; k++)
				if (m_subRows[k].n > 0)
				{
					nFilled++;
					filled = k;
				}

			if (nFilled == 1)
				_simpleRow(m_edges.data() + m_subRows[filled].first, m_subRows[filled].n, m_subRows[filled].h);
			else if (nFilled > 0)
				_generalRow(nSubRows);

			if (m_nRuns > 0)
			{
				if (rowsEnd > row + 1)
					pSink->rows(row, rowsEnd, m_nRuns, m_runs.data(), m_pixels.data());
				else
					pSink->row(row, m_nRuns, m_runs.data(), m_pixels.data());
			}

			row = rowsEnd;
		}
	}

	//____ _split() ___________________________________________________________
	// Tells if an edge must be split between (ya, xa) and (yb, xb), and where.

	template<class Shape>
	inline bool SoftShapes::_split(Shape& shape, const EdgeDef& edge, int64_t ya, int64_t xa, int64_t yb, int64_t xb, int64_t& y)
	{
		if (edge.bShapeSplit)
			return shape.split(edge, ya, xa, yb, xb, y);

		for (int i = 0; i < 2; i++)
		{
			if (edge.junction[i] > ya && edge.junction[i] < yb)
			{
				y = edge.junction[i];
				return true;
			}
		}

		const int region = ya >= edge.junction[1] ? 2 : ya >= edge.junction[0] ? 1 : 0;
		const int64_t limit = edge.limit[region];
		const int64_t h = yb - ya;

		if (limit == 0 || h <= 4 || (xa - xb) * (xa - xb) + h * h <= limit)
			return false;

		y = splitPoint(ya, yb);
		return true;
	}

	//____ _subRowEdges() _____________________________________________________
	/*
		Edges of a sub-row as polylines, from the top to the bottom of the sub-row, split
		where the shape needs it. Pieces are added to m_pieces.
	*/

	template<class Shape>
	int SoftShapes::_subRowEdges(Shape& shape, int64_t ya, int64_t yb, Edge* pEdges)
	{
		EdgeDef* pDefs = m_edgeDefs.data();
		const int nEdges = shape.edges(ya, yb, pDefs);

		int64_t* pTop = m_edgeTop.data();
		int64_t* pBottom = m_edgeBottom.data();

		for (int i = 0; i < nEdges; i++)
		{
			const int id = pDefs[i].id;

			pTop[i] = _edgeX(shape, pDefs[i], ya);
			pBottom[i] = _edgeX(shape, pDefs[i], yb);

			m_cacheY[id] = yb;
			m_cacheX[id] = pBottom[i];
		}

		// Clamped edges between their neighbors.

		for (int i = 1; i < nEdges; i++)
			if (pDefs[i].bClamped)
			{
				pTop[i] = std::max(pTop[i], pTop[i - 1]);
				pBottom[i] = std::max(pBottom[i], pBottom[i - 1]);
			}

		for (int i = nEdges - 2; i >= 0; i--)
			if (pDefs[i].bClamped)
			{
				pTop[i] = std::min(pTop[i], pTop[i + 1]);
				pBottom[i] = std::min(pBottom[i], pBottom[i + 1]);
			}

		// Polylines

		const int64_t clipBeg64 = int64_t(m_clipBeg) * 64;
		const int64_t clipEnd64 = int64_t(m_clipEnd) * 64;

		if ((int)m_pieces.size() < m_nPieces + nEdges * c_maxVertices)
			m_pieces.resize(m_nPieces + nEdges * c_maxVertices + 64);

		Piece* pPieces = m_pieces.data();

		for (int i = 0; i < nEdges; i++)
		{
			Edge& edge = pEdges[i];
			edge.segment = pDefs[i].segment;
			edge.first = m_nPieces;
			edge.bandBeg = INT_MAX;
			edge.bandEnd = INT_MIN;

			// Vertices still to reach, last one first.

			int64_t stackY[c_maxVertices], stackX[c_maxVertices];
			int nStack = 1;
			stackY[0] = yb;
			stackX[0] = pBottom[i];

			int64_t y = ya;
			int64_t x = pTop[i];

			while (nStack > 0)
			{
				const int64_t y2 = stackY[nStack - 1];
				const int64_t x2 = stackX[nStack - 1];

				int64_t ySplit;
				if (!pDefs[i].bClamped && nStack < c_maxVertices && y2 - y > 1 && _split(shape, pDefs[i], y, x, y2, x2, ySplit))
				{
					stackY[nStack] = ySplit;
					stackX[nStack++] = shape.x(pDefs[i], ySplit);
					continue;
				}

				Piece& piece = pPieces[m_nPieces++];

				const int64_t lo = std::min(x, x2);
				const int64_t hi = std::max(x, x2);

				piece.lo = lo;
				piece.hi = hi;
				piece.h = int(y2 - y);
				piece.bandBeg = lo < clipBeg64 ? m_clipBeg - 1 : lo >= clipEnd64 ? m_clipEnd + 1 : int(lo >> 6);
				piece.bandEnd = hi < clipBeg64 ? m_clipBeg - 1 : hi >= clipEnd64 ? m_clipEnd + 1 : int((hi + 63) >> 6);
				piece.inv = (hi > lo && piece.bandEnd > m_clipBeg && piece.bandBeg < m_clipEnd) ? recip37(hi - lo) : 0;

				edge.bandBeg = std::min(edge.bandBeg, piece.bandBeg);
				edge.bandEnd = std::max(edge.bandEnd, piece.bandEnd);

				y = y2;
				x = x2;
				nStack--;

				if (m_nPieces == (int)m_pieces.size())
				{
					m_pieces.resize(m_pieces.size() * 2);
					pPieces = m_pieces.data();
				}
			}

			edge.n = m_nPieces - edge.first;
		}

		return nEdges;
	}

	//____ _rightOfEdge() _____________________________________________________
	// Coverage of pixel right of edge in a sub-row of height h, 64 * h if fully right.

	inline int SoftShapes::_rightOfEdge(const Edge& edge, int pixel, int h) const
	{
		if (edge.n == 1)
		{
			int right = _rightOf(m_pieces[edge.first], pixel);
			return h == 64 ? right : (right * h + 32) >> 6;
		}

		return (_edgeArea(edge, pixel) + 32) >> 6;
	}

	//____ _simpleRow() _______________________________________________________
	/*
		A row where only one sub-row has edges, the common case. Pixels between bands of
		partly covered pixels are covered by one segment. In a band of one edge, pixels are
		split between the segments on either side of it, where bands overlap between the
		segments of all edges reaching the pixel.
	*/

	void SoftShapes::_simpleRow(const Edge* pEdges, int nEdges, int h)
	{
		const int clipBeg = m_clipBeg;
		const int clipEnd = m_clipEnd;
		const int full = h * 64;					// Coverage of pixel by the sub-row.

		// Pixels between bands, fully covered if the sub-row is the whole row.

		auto between = [&](int beg, int end, int segment)
		{
			if (full == c_fullCoverage)
				_fillRun(beg, end, segment);
			else
			{
				const int16_t* pColor = _mixColor(segment, beg);
				if (!pColor)
					return;

				auto mixed = [&](const int16_t* pColor) -> CoveragePixel
				{
					return { int16_t((pColor[0] * full) >> 12), int16_t((pColor[1] * full) >> 12), int16_t((pColor[2] * full) >> 12),
							 int16_t((pColor[3] * full) >> 12), int16_t(full), 0 };
				};

				if (m_mix[segment].stride == 0)
					_mixedRun(beg, end, mixed(pColor));
				else
				{
					for (int p = beg; p < end; p++)
						_mixedPixel(p) = mixed(_mixColor(segment, p));
				}
			}
		};

		int x = clipBeg;
		int segLeft = -1;

		for (int k = 0; k < nEdges; )
		{
			// Edges with overlapping bands.

			int groupEnd = pEdges[k].bandEnd;
			int j = k + 1;

			while (j < nEdges && pEdges[j].bandBeg < groupEnd)
			{
				groupEnd = std::max(groupEnd, pEdges[j].bandEnd);
				j++;
			}

			const int bandBeg = std::max(pEdges[k].bandBeg, clipBeg);
			const int bandEnd = std::min(groupEnd, clipEnd);

			if (bandBeg > x && x < clipEnd && segLeft >= 0)
				between(x, std::min(bandBeg, clipEnd), segLeft);

			if (j == k + 1)
			{
				// One edge, pixels split between the segments on either side.

				const Edge& edge = pEdges[k];
				const int segRight = edge.segment;
				const Piece& piece = m_pieces[edge.first];
				const bool bStraight = edge.n == 1;

				int64_t prevArea = bStraight ? _area(piece, int64_t(bandBeg) * 64) : 0;

				for (int p = bandBeg; p < bandEnd; p++)
				{
					int right;

					if (bStraight)
					{
						int64_t area = _area(piece, int64_t(p + 1) * 64);
						right = int(area - prevArea);
						prevArea = area;

						if (h != 64)
							right = (right * h + 32) >> 6;
					}
					else
						right = (_edgeArea(edge, p) + 32) >> 6;

					if (right <= 0)
					{
						if (segLeft >= 0)
							between(p, p + 1, segLeft);
						continue;
					}

					if (right >= full)
					{
						if (segRight >= 0)
							between(p, p + 1, segRight);
						continue;
					}

					const int16_t* pLeft = segLeft >= 0 ? _mixColor(segLeft, p) : nullptr;
					const int16_t* pRight = segRight >= 0 ? _mixColor(segRight, p) : nullptr;

					int left = full - right;

					if (!pLeft)
					{
						if (!pRight)
							continue;

						pLeft = c_noColor;
						left = 0;
					}
					else if (!pRight)
					{
						pRight = c_noColor;
						right = 0;
					}

					CoveragePixel& px = _mixedPixel(p);
					px.b = int16_t((pLeft[0] * left + pRight[0] * right) >> 12);
					px.g = int16_t((pLeft[1] * left + pRight[1] * right) >> 12);
					px.r = int16_t((pLeft[2] * left + pRight[2] * right) >> 12);
					px.a = int16_t((pLeft[3] * left + pRight[3] * right) >> 12);
					px.coverage = int16_t(left + right);
				}
			}
			else
			{
				// Several edges. Each segment gets the coverage right of the edge before it
				// minus that right of the edge after it.

				for (int p = bandBeg; p < bandEnd; p++)
				{
					int b = 0, g = 0, r = 0, a = 0, c = 0;
					int whole = -2;							// Segment covering all of pixel, -1 if none.

					int prevRight = full;
					int segment = segLeft;

					for (int i = k; i <= j; i++)
					{
						int right = i < j ? std::min(std::max(_rightOfEdge(pEdges[i], p, h), 0), prevRight) : 0;
						int coverage = prevRight - right;

						if (coverage > 0 && segment >= 0)
						{
							if (coverage == full)
								whole = segment;
							else if (const int16_t* pColor = _mixColor(segment, p))
							{
								b += pColor[0] * coverage;
								g += pColor[1] * coverage;
								r += pColor[2] * coverage;
								a += pColor[3] * coverage;
								c += coverage;
							}
						}
						else if (coverage == full)
							whole = -1;

						prevRight = right;
						if (i < j)
							segment = pEdges[i].segment;
					}

					if (whole >= 0)
						between(p, p + 1, whole);
					else if (c > 0)
						_mixedPixel(p) = { int16_t(b >> 12), int16_t(g >> 12), int16_t(r >> 12), int16_t(a >> 12), int16_t(c), 0 };
				}
			}

			x = std::max(x, bandEnd);
			segLeft = pEdges[j - 1].segment;
			k = j;
		}

		if (x < clipEnd && segLeft >= 0)
			between(x, clipEnd, segLeft);
	}

	//____ _generalRow() ______________________________________________________
	/*
		A row of several sub-rows, or with overlapping bands. Coverage of each pixel is
		accumulated per segment over the sub-rows, weighted by their height. Stretches of
		pixels outside all bands have the same coverage throughout.
	*/

	void SoftShapes::_generalRow(int nSubRows)
	{
		SubRow* pSubRows = m_subRows.data();
		const Edge* pEdges = m_edges.data();

		for (int k = 0; k < nSubRows; k++)
		{
			pSubRows[k].next = pSubRows[k].first;
			pSubRows[k].segment = -1;
		}

		int x = m_clipBeg;

		while (x < m_clipEnd)
		{
			bool bPartial = false;
			int nextX = m_clipEnd;

			for (int k = 0; k < nSubRows; k++)
			{
				SubRow& subRow = pSubRows[k];
				const int end = subRow.first + subRow.n;

				while (subRow.next < end && pEdges[subRow.next].bandEnd <= x)
					subRow.segment = pEdges[subRow.next++].segment;

				if (subRow.next < end)
				{
					if (pEdges[subRow.next].bandBeg <= x)
						bPartial = true;
					else
						nextX = std::min(nextX, pEdges[subRow.next].bandBeg);
				}
			}

			if (!bPartial)
			{
				for (int k = 0; k < nSubRows; k++)
					_accumulate(pSubRows[k].segment, pSubRows[k].h * c_fullCoverage);

				_emitAccumulated(x, nextX);
				x = nextX;
				continue;
			}

			for (int k = 0; k < nSubRows; k++)
			{
				const SubRow& subRow = pSubRows[k];
				const int end = subRow.first + subRow.n;

				int prevArea = subRow.h * c_fullCoverage;
				int segment = subRow.segment;

				for (int i = subRow.next; i < end && pEdges[i].bandBeg <= x; i++)
				{
					int area = std::min(std::max(_edgeArea(pEdges[i], x), 0), prevArea);
					_accumulate(segment, prevArea - area);
					prevArea = area;
					segment = pEdges[i].segment;
				}

				_accumulate(segment, prevArea);
			}

			_emitAccumulated(x, x + 1);
			x++;
		}
	}

	//____ _emitAccumulated() _________________________________________________
	/*
		Turns coverage accumulated over sub-rows into pixels beg up to end, rounded
		cumulatively so that segments add up to the rounded total.
	*/

	void SoftShapes::_emitAccumulated(int beg, int end)
	{
		int* pSegments = m_layerSegments.data();
		int* pCoverage = m_layerCoverage.data();

		int nLayers = 0;
		int full = -1;
		int total = 0;
		int prev = 0;

		for (int i = 0; i < m_nTouched; i++)
		{
			int segment = m_touched[i];
			total += m_accumulated[segment];
			m_accumulated[segment] = 0;

			int rounded = (total + 32) >> 6;
			int coverage = rounded - prev;
			prev = rounded;

			if (coverage == c_fullCoverage)
				full = segment;

			if (coverage > 0)
			{
				pSegments[nLayers] = segment;
				pCoverage[nLayers++] = coverage;
			}
		}

		m_nTouched = 0;

		if (full >= 0)
		{
			_fillRun(beg, end, full);
			return;
		}

		bool bFlat = true;
		for (int i = 0; i < nLayers; i++)
			if (m_mix[pSegments[i]].stride != 0)
				bFlat = false;

		for (int x = beg; x < end; x++)
		{
			int b = 0, g = 0, r = 0, a = 0, c = 0;

			for (int i = 0; i < nLayers; i++)
			{
				const int16_t* pColor = _mixColor(pSegments[i], x);
				if (!pColor)
					continue;

				int coverage = pCoverage[i];
				b += pColor[0] * coverage;
				g += pColor[1] * coverage;
				r += pColor[2] * coverage;
				a += pColor[3] * coverage;
				c += coverage;
			}

			if (c == 0)
				return;

			CoveragePixel px = { int16_t(b >> 12), int16_t(g >> 12), int16_t(r >> 12), int16_t(a >> 12), int16_t(c), 0 };

			if (bFlat)
			{
				_mixedRun(x, end, px);
				return;
			}

			_mixedPixel(x) = px;
		}
	}

	//____ BoxShape ___________________________________________________________
	/*
		Box with rounded corners and an outline inside it, as two contours: the outer edge
		and the inner edge of the outline.

		Along a row, a contour reaches from its left edge, the rightmost of its left side and
		the left corners whose quarter the row is in, to its right edge, the leftmost of its
		right side and right corners. The fill reaches from the rightmost of the left edges
		of both contours to the leftmost of their right edges, so where an inner corner
		reaches past the outer edge, as with uneven outlines, the outline just gets thin.
		Where left and right edge of a contour pass each other it is closed.

		Edges are split where corners begin and end, and bisected where they bend too much
		or kink, where another side or corner takes over or the contour closes.

		Edge ids: 0 outer left, 1 outer right, 2 inner left, 3 inner right.
	*/

	class SoftShapes::BoxShape : public SoftShapes::EdgeShape
	{
	public:

		struct Contour
		{
			bool		bExists;
			int64_t		left, top, right, bottom;
			int64_t		r[4];					// Top-left, top-right, bottom-right, bottom-left. 0 for square corner.
			int64_t		cx[4], cy[4];			// Center of circle.
			uint64_t	r2[4];
			int64_t		limit[4];				// Most squared length of a chord.
		};

		// Edges of both contours at y, with the candidates they are the max (left) or min
		// (right) of: side, top corner and bottom corner.

		struct Eval
		{
			int64_t		y;
			int64_t		cand[2][2][3];			// Contour, left/right, candidate.
			bool		bValid[2][2][3];
			int64_t		x[4];					// Edges, see ids.
			bool		bClosed[2];				// Contour closed, left and right edge put between them.
		};

		Contour		outer;
		Contour		inner;
		bool		bOutline;
		bool		bRegular;					// Inner contour stays inside outer and doesn't close, as with even outlines.

		int			nEvents = 0;
		int64_t		eventBeg[12], eventEnd[12];	// Where rows aren't all the same, see uniformEnd().

		Eval		cache[4];
		int			nextCache = 0;

		struct Root
		{
			int64_t		y, cy, r;
			int64_t		dx;						// Half width of circle at y.
		};

		static constexpr int	c_roots = 32;
		Root		roots[c_roots];

		BoxShape()
		{
			for (auto& eval : cache)
				eval.y = INT64_MIN;

			for (auto& root : roots)
				root.y = INT64_MIN;
		}

		//____ setupContour() _________________________________________________

		static void setupContour(Contour& c, const RectSPX& rect, const spx radius[4])
		{
			c.bExists = rect.w > 0 && rect.h > 0;
			c.left = rect.x;
			c.top = rect.y;
			c.right = int64_t(rect.x) + rect.w;
			c.bottom = int64_t(rect.y) + rect.h;

			for (int i = 0; i < 4; i++)
			{
				int64_t r = radius[i] >= 4 ? radius[i] : 0;			// Smaller corners are square, within 1/256 pixel.
				c.r[i] = r;
				c.r2[i] = uint64_t(r) * uint64_t(r);
				c.limit[i] = 8 * r * c_maxSagitta;
				c.cx[i] = (i == 0 || i == 3) ? c.left + r : c.right - r;
				c.cy[i] = (i == 0 || i == 1) ? c.top + r : c.bottom - r;
			}
		}

		//____ addEvents() ____________________________________________________
		// Splits where contours begin and end, events for uniformEnd().

		void addEvent(int64_t beg, int64_t end)
		{
			eventBeg[nEvents] = beg;
			eventEnd[nEvents++] = end;
		}

		void addEvents(const Contour& c, std::vector<int64_t>& splits)
		{
			if (!c.bExists)
				return;

			splits.push_back(c.top);
			splits.push_back(c.bottom);
			addEvent(c.top, c.top);
			addEvent(c.bottom, c.bottom);

			for (int i = 0; i < 4; i++)
			{
				if (c.r[i] == 0)
					continue;

				if (i == 0 || i == 1)
					addEvent(c.top, std::min(c.cy[i], c.bottom));
				else
					addEvent(std::max(c.cy[i], c.top), c.bottom);
			}
		}

		//____ eval() _________________________________________________________

		const Eval& eval(int64_t y)
		{
			for (auto& e : cache)
				if (e.y == y)
					return e;

			Eval& e = cache[nextCache];
			nextCache = (nextCache + 1) & 3;

			e.y = y;
			evalContour(outer, 0, y, e);

			int64_t& left = e.x[0];
			int64_t& right = e.x[1];
			e.bClosed[0] = closeContour(left, right, INT64_MIN, INT64_MAX);

			if (bOutline && inner.bExists && y >= inner.top && y <= inner.bottom)
			{
				evalContour(inner, 1, y, e);

				int64_t& innerLeft = e.x[2];
				int64_t& innerRight = e.x[3];
				innerLeft = std::max(innerLeft, left);
				innerRight = std::min(innerRight, right);
				e.bClosed[1] = closeContour(innerLeft, innerRight, left, right);
			}
			else
				e.bClosed[1] = false;

			return e;
		}

		static void evalContour(const Contour& c, int k, int64_t y, Eval& e)
		{
			for (int side = 0; side < 2; side++)
			{
				const bool bLeft = side == 0;
				const int corners[2] = { bLeft ? 0 : 1, bLeft ? 3 : 2 };

				int64_t* pCand = e.cand[k][side];
				bool* pValid = e.bValid[k][side];

				pCand[0] = bLeft ? c.left : c.right;
				pValid[0] = true;

				int64_t x = pCand[0];

				for (int j = 0; j < 2; j++)
				{
					const int i = corners[j];
					const bool bInQuarter = c.r[i] > 0 && (j == 0 ? y <= c.cy[i] : y >= c.cy[i]) && y >= c.top && y <= c.bottom;

					pValid[1 + j] = bInQuarter;

					if (bInQuarter)
					{
						int64_t dy = y - c.cy[i];
						int64_t dx = isqrt(c.r2[i] - uint64_t(dy * dy));
						int64_t cand = bLeft ? c.cx[i] - dx : c.cx[i] + dx;

						pCand[1 + j] = cand;
						x = bLeft ? std::max(x, cand) : std::min(x, cand);
					}
				}

				e.x[k * 2 + side] = x;
			}
		}

		// Where left and right edge have passed each other the contour is closed, both are
		// put between them, within outer left and right.

		static bool closeContour(int64_t& left, int64_t& right, int64_t outerLeft, int64_t outerRight)
		{
			if (left <= right)
				return false;

			left = right = std::min(std::max((left + right) >> 1, outerLeft), outerRight);
			return true;
		}

		bool within(const Contour& c, int64_t ya, int64_t yb) const
		{
			return c.bExists && ya >= c.top && yb <= c.bottom;
		}

		//____ edges() ________________________________________________________

		int edges(int64_t ya, int64_t yb, EdgeDef* pEdges) const
		{
			if (!within(outer, ya, yb))
				return 0;

			int n = 0;
			edge(pEdges[n++], 0, bOutline ? 1 : 0);

			if (bOutline && within(inner, ya, yb))
			{
				edge(pEdges[n++], 2, 0);
				edge(pEdges[n++], 3, 1);
			}

			edge(pEdges[n++], 1, -1);
			return n;
		}

		// Edge with where it goes from top corner to side and from side to bottom corner,
		// unless the shape needs to split it.

		void edge(EdgeDef& def, int id, int segment) const
		{
			def.id = id;
			def.segment = segment;
			def.bClamped = false;
			def.bShapeSplit = !bRegular;

			if (bRegular)
			{
				const Contour& c = id < 2 ? outer : inner;
				const bool bLeft = (id & 1) == 0;
				const int top = bLeft ? 0 : 1;
				const int bottom = bLeft ? 3 : 2;

				def.junction[0] = c.r[top] > 0 ? c.cy[top] : INT64_MIN;
				def.junction[1] = c.r[bottom] > 0 ? c.cy[bottom] : INT64_MAX;
				def.limit[0] = c.limit[top];
				def.limit[1] = 0;
				def.limit[2] = c.limit[bottom];
			}
		}

		//____ x() ____________________________________________________________

		int64_t x(const EdgeDef& edge, int64_t y)
		{
			if (!bRegular)
				return eval(y).x[edge.id];

			// Side, or the corner whose quarter y is in.

			const Contour& c = edge.id < 2 ? outer : inner;
			const bool bLeft = (edge.id & 1) == 0;
			const int top = bLeft ? 0 : 1;
			const int bottom = bLeft ? 3 : 2;

			int i;
			if (c.r[top] > 0 && y < c.cy[top])
				i = top;
			else if (c.r[bottom] > 0 && y > c.cy[bottom])
				i = bottom;
			else
				return bLeft ? c.left : c.right;

			// Corners on either side are usually the same, so half width is kept for reuse.

			const int64_t cy = c.cy[i];
			const int64_t r = c.r[i];

			Root& root = roots[(y + cy * 5) & (c_roots - 1)];
			if (root.y != y || root.cy != cy || root.r != r)
			{
				int64_t dy = y - cy;
				root = { y, cy, r, isqrt(c.r2[i] - uint64_t(dy * dy)) };
			}

			return bLeft ? c.cx[i] - root.dx : c.cx[i] + root.dx;
		}

		//____ split() ________________________________________________________

		bool split(const EdgeDef& edge, int64_t ya, int64_t xa, int64_t yb, int64_t xb, int64_t& y)
		{
			if (bRegular)
			{
				// Where corners begin and end, and where they bend too much.

				const Contour& c = edge.id < 2 ? outer : inner;
				const bool bLeft = (edge.id & 1) == 0;
				const int top = bLeft ? 0 : 1;
				const int bottom = bLeft ? 3 : 2;

				int i = -1;

				if (c.r[top] > 0)
				{
					if (c.cy[top] > ya && c.cy[top] < yb)
					{
						y = c.cy[top];
						return true;
					}

					if (yb <= c.cy[top])
						i = top;
				}

				if (c.r[bottom] > 0)
				{
					if (c.cy[bottom] > ya && c.cy[bottom] < yb)
					{
						y = c.cy[bottom];
						return true;
					}

					if (ya >= c.cy[bottom])
						i = bottom;
				}

				if (i < 0 || yb - ya <= 4 || (xa - xb) * (xa - xb) + (yb - ya) * (yb - ya) <= c.limit[i])
					return false;

				y = splitPoint(ya, yb);
				return true;
			}

			const int k = edge.id >> 1;				// Contour
			const int side = edge.id & 1;
			const bool bLeft = side == 0;

			// Corners begin and end.

			const int corners[2] = { bLeft ? 0 : 1, bLeft ? 3 : 2 };

			for (int c = 0; c <= k; c++)
			{
				const Contour& contour = c == 0 ? outer : inner;

				for (int i : corners)
				{
					if (contour.r[i] > 0 && contour.cy[i] > ya && contour.cy[i] < yb)
					{
						y = contour.cy[i];
						return true;
					}
				}
			}

			const Eval a = eval(ya);
			const Eval& b = eval(yb);

			// Contour closes or opens.

			if (a.bClosed[0] != b.bClosed[0] || (k == 1 && a.bClosed[1] != b.bClosed[1]))
			{
				y = splitPoint(ya, yb);
				return true;
			}

			// The same candidate must be the edge at both ends, otherwise the edge kinks.
			// The max (or min) of the candidates before closing is the edge, unless closed.

			if ((k == 0 && a.bClosed[0]) || (k == 1 && (a.bClosed[0] || a.bClosed[1])))
				return false;

			int64_t limit = -1;

			for (int c = 0; c <= k && limit < 0; c++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (a.bValid[c][side][j] && b.bValid[c][side][j] && a.cand[c][side][j] == xa && b.cand[c][side][j] == xb)
					{
						limit = j == 0 ? 0 : (c == 0 ? outer : inner).limit[corners[j - 1]];
						if (limit == 0)
							break;
					}
				}
			}

			if (limit < 0 || (limit > 0 && yb - ya > 4 && (xa - xb) * (xa - xb) + (yb - ya) * (yb - ya) > limit))
			{
				y = splitPoint(ya, yb);
				return true;
			}

			return false;
		}

		//____ uniformEnd() ___________________________________________________
		// Rows not reached by corners and not cut by the top or bottom of a contour are the same.

		int uniformEnd(int row) const
		{
			const int64_t y0 = int64_t(row) * 64;
			const int64_t y1 = y0 + 64;

			int end = INT_MAX;

			for (int i = 0; i < nEvents; i++)
			{
				if (eventBeg[i] < y1 && eventEnd[i] > y0)
					return row + 1;

				if (eventBeg[i] >= y1)
					end = std::min(end, floorPixel(eventBeg[i]));
			}

			return end;
		}
	};

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

		BoxShape shape;
		shape.bOutline = (outline.top | outline.right | outline.bottom | outline.left) != 0;

		BoxShape::setupContour(shape.outer, rect, radius);

		if (shape.bOutline)
		{
			RectSPX innerRect(rect.x + outline.left, rect.y + outline.top,
							  rect.w - outline.left - outline.right, rect.h - outline.top - outline.bottom);

			spx innerRadius[4];
			Util::innerCornerRadii(radius, outline, innerRadius);

			BoxShape::setupContour(shape.inner, innerRect, innerRadius);

			// With an even outline, the inner contour stays inside the outer one. It doesn't
			// close either, unless its corners reach past each other.

			const int64_t* r = shape.inner.r;
			const int64_t w = innerRect.w, h = innerRect.h;

			shape.bRegular = outline.top == outline.right && outline.top == outline.bottom && outline.top == outline.left &&
							 r[0] + r[1] <= w && r[3] + r[2] <= w && r[0] + r[3] <= h && r[1] + r[2] <= h;
		}
		else
		{
			shape.inner.bExists = false;
			shape.bRegular = true;
		}

		m_splits.clear();
		shape.addEvents(shape.outer, m_splits);
		shape.addEvents(shape.inner, m_splits);

		std::sort(m_splits.begin(), m_splits.end());
		m_splits.erase(std::unique(m_splits.begin(), m_splits.end()), m_splits.end());

		shape.pSplits = m_splits.data();
		shape.nSplits = (int)m_splits.size();
		shape.maxEdges = 4;
		shape.nIds = 4;

		int clipBeg = std::max(clip.x, floorPixel(rect.x));
		int clipEnd = std::min(clip.x + clip.w, ceilPixel(int64_t(rect.x) + rect.w));
		int rowBeg = std::max(clip.y, floorPixel(rect.y));
		int rowEnd = std::min(clip.y + clip.h, ceilPixel(int64_t(rect.y) + rect.h));

		_rasterize(shape, 2, rowBeg, rowEnd, clipBeg, clipEnd, pSink);
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

	//____ _isBefore() ________________________________________________________
	/*
		Tells if direction a (y pointing down) comes before direction b clockwise from
		12 o'clock. Needs no trigonometry or division, just the half turn each is in and
		the sign of their cross product.
	*/

	inline bool SoftShapes::_isBefore(int64_t ax, int64_t ay, int64_t bx, int64_t by)
	{
		bool aFirstHalf = ax > 0 || (ax == 0 && ay < 0);			// From 12 o'clock up to 6 o'clock.
		bool bFirstHalf = bx > 0 || (bx == 0 && by < 0);

		if (aFirstHalf != bFirstHalf)
			return aFirstHalf;

		return ax * by - ay * bx > 0;
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
		m_crossings[0].clear();
		m_crossings[1].clear();

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

			// Rays too close to horizontal to cross rows within any canvas, except at the center,
			// are taken as horizontal.

			b.bFlat = std::abs(b.sy) < 1024;

			if (!b.bFlat)
			{
				int64_t slope = (int64_t(b.sx) * c_one) / b.sy;		// 32.32
				b.slopeInt = slope >> 32;
				b.slopeFrac = uint32_t(slope & 0xFFFFFFFF);
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
					m_crossings[0].push_back(i);
			}

		for (int i = nBoundaries - 1; i >= 0; i--)
		{
			const Boundary& b = m_boundaries[i];
			if (!b.bFlat && b.sy > 0)
				m_crossings[1].push_back(i);
		}
	}

	//____ _elementAt() _______________________________________________________

	// Element in direction x, y from the center. The center itself counts as 12 o'clock.

	int SoftShapes::_elementAt(int64_t x, int64_t y) const
	{
		if (m_boundaries.empty())
			return 0;

		if (x == 0 && y == 0)
			y = -1;

		// Last boundary at or before the direction, wrapping around to the last one.

		int lo = 0, hi = (int)m_boundaries.size();
		while (lo < hi)
		{
			int mid = (lo + hi) / 2;
			const Boundary& b = m_boundaries[mid];
			if (!_isBefore(x, y, b.sx, b.sy))
				lo = mid + 1;
			else
				hi = mid;
		}

		int idx = lo - 1;
		if (idx < 0)
			idx = (int)m_boundaries.size() - 1;

		return m_boundaries[idx].element;
	}

	//____ RingShape __________________________________________________________
	/*
		Ring as up to four circles, with slices between the middle two and rays between the
		slices. Circle 0 is the outer edge, 1 the inside of the outer outline, 2 the outside
		of the inner outline and 3 the inner edge. Without outline, 1 and 2 are left out and
		0 and 3 limit the slices. In a square ring 0 and 1 are rectangles.

		A ray crosses rows where it is between the circles limiting the slices. Along a row
		in the upper half they come clockwise, in the lower half counter-clockwise, and the
		slice right of each is known. Rows are split where rays end and at the center, so
		within a sub-row each ray is either left of the slices, between them, in the hole
		or right of them.

		Edge ids: 0-3 left side of circle, 4-7 right side, 8 and 9 left and right side of
		rect, then the rays above the center followed by those below.
	*/

	class SoftShapes::RingShape : public SoftShapes::EdgeShape
	{
	public:

		int64_t		cx, cy;
		int64_t		left, top, right, bottom;	// Rect.
		bool		bBack, bSquare, bHole, bOutline, bSlices;

		int64_t		radius[4];					// 0 for no circle.
		uint64_t	radius2[4];
		int64_t		limit[4];					// Most squared length of a chord.

		int64_t		sqLeft[2], sqTop[2], sqRight[2], sqBottom[2];	// Square ring.

		int			outerLimit, innerLimit;		// Circles limiting slices.

		const Ray*	pRays[2];
		int			nRays[2];
		int			startSegment[2];			// Segment at left end of rows above and below center.

		int64_t		cacheY[4] = { INT64_MIN, INT64_MIN, INT64_MIN, INT64_MIN };
		int64_t		cacheX[4];

		static constexpr int	c_firstRayId = 10;

		//____ halfWidth() ____________________________________________________

		int64_t halfWidth(int i, int64_t y)
		{
			if (cacheY[i] != y)
			{
				int64_t r = radius[i];
				int64_t dy = y - cy;
				cacheY[i] = y;
				cacheX[i] = (dy <= -r || dy >= r) ? 0 : isqrt(radius2[i] - uint64_t(dy * dy));
			}

			return cacheX[i];
		}

		//____ x() ____________________________________________________________

		int64_t x(const EdgeDef& edge, int64_t y)
		{
			const int id = edge.id;

			if (id < 8)
			{
				const int i = id & 3;
				const bool bLeft = id < 4;

				if (bSquare && i < 2)
					return bLeft ? sqLeft[i] : sqRight[i];

				int64_t w = halfWidth(i, y);
				return bLeft ? cx - w : cx + w;
			}

			if (id < c_firstRayId)
				return id == 8 ? left : right;

			int ray = id - c_firstRayId;
			const Ray& r = ray < nRays[0] ? pRays[0][ray] : pRays[1][ray - nRays[0]];

			int64_t dy = y - cy;
			return cx + dy * r.slopeInt + mulFrac(dy, r.slopeFrac);
		}

		int uniformEnd(int row) const { return row + 1; }

		//____ split() ________________________________________________________
		// Not used, circles are split by the limits given with their edges.

		bool split(const EdgeDef& edge, int64_t ya, int64_t xa, int64_t yb, int64_t xb, int64_t& y)
		{
			return false;
		}

		//____ edges() ________________________________________________________

		bool within(int i, int64_t ya, int64_t yb) const
		{
			if (bSquare && i < 2)
				return ya >= sqTop[i] && yb <= sqBottom[i];

			return radius[i] > 0 && ya >= cy - radius[i] && yb <= cy + radius[i];
		}

		int edges(int64_t ya, int64_t yb, EdgeDef* pEdges) const
		{
			const int half = ya < cy ? 0 : 1;
			const Ray* pRay = pRays[half];
			const int nRay = nRays[half];
			const int firstId = c_firstRayId + (half == 0 ? 0 : nRays[0]);

			int n = 0;

			auto put = [&](int id, int segment)
			{
				const int i = id & 3;
				const int64_t lim = (id < 8 && !(bSquare && i < 2)) ? limit[i] : 0;
				pEdges[n++] = { id, segment, false, false, { INT64_MAX, INT64_MAX }, { lim, lim, lim } };
			};

			auto putRay = [&](int i)
			{
				pEdges[n++] = { firstId + i, pRay[i].segment, true, false, { INT64_MAX, INT64_MAX }, { 0, 0, 0 } };
			};

			// 0: beyond the slices, 1: between, 2: in the hole.

			auto rayClass = [&](const Ray& ray) -> int
			{
				if (half == 0)
					return yb <= ray.yOuter ? 0 : (bHole && ya >= ray.yInner) ? 2 : 1;
				else
					return ya >= ray.yOuter ? 0 : (bHole && yb <= ray.yInner) ? 2 : 1;
			};

			const bool bInRect = bBack && !bSquare && ya >= top && yb <= bottom;
			const int segmentRight = (bBack && !bSquare) ? c_ringBackSegment : -1;

			if (bInRect)
				put(8, c_ringBackSegment);

			if (within(0, ya, yb))
			{
				const bool bInHub = bHole && within(3, ya, yb);

				if (!bSlices)
				{
					put(0, c_ringOutlineSegment);

					if (bInHub)
					{
						put(3, c_ringHubSegment);
						put(7, c_ringOutlineSegment);
					}
				}
				else
				{
					const bool bInSlices = within(outerLimit, ya, yb);
					const bool bInHole = bInSlices && bHole && within(innerLimit, ya, yb);

					int segment = startSegment[half];
					int i = 0;

					if (bOutline)
						put(0, c_ringOutlineSegment);

					if (bInSlices)
					{
						while (i < nRay && pRay[i].bLeft && rayClass(pRay[i]) == 0)
							segment = pRay[i++].segment;

						put(outerLimit, segment);

						if (bInHole)
						{
							while (i < nRay && pRay[i].bLeft && rayClass(pRay[i]) == 1)
							{
								putRay(i);
								segment = pRay[i++].segment;
							}

							while (i < nRay && rayClass(pRay[i]) == 2)
								segment = pRay[i++].segment;

							if (bOutline)
							{
								put(2, c_ringOutlineSegment);

								if (bInHub)
								{
									put(3, c_ringHubSegment);
									put(7, c_ringOutlineSegment);
								}

								put(6, segment);
							}
							else
							{
								put(3, c_ringHubSegment);
								put(7, segment);
							}
						}

						for (; i < nRay; i++)
							if (rayClass(pRay[i]) == 1)
								putRay(i);

						if (bOutline)
							put(5, c_ringOutlineSegment);
					}
				}

				put(4, segmentRight);
			}

			if (bInRect)
				put(9, -1);

			return n;
		}
	};

	//____ rasterizeRing() ____________________________________________________

	void SoftShapes::rasterizeRing(const Ring& ring, const RectI& clip, RowSink* pSink)
	{
		const RectSPX& rect = ring.rect;

		if (rect.w <= 0 || rect.h <= 0 || clip.w <= 0 || clip.h <= 0)
			return;

		RingShape shape;

		shape.cx = int64_t(rect.x) + rect.w / 2;
		shape.cy = int64_t(rect.y) + rect.h / 2;
		shape.left = rect.x;
		shape.top = rect.y;
		shape.right = int64_t(rect.x) + rect.w;
		shape.bottom = int64_t(rect.y) + rect.h;

		const int64_t cx = shape.cx, cy = shape.cy;

		// Radial geometry

		const int64_t outerRadius = std::min(rect.w, rect.h) / 2;

		const bool bHole = ring.thickness > 0 && ring.thickness < outerRadius;
		const int64_t innerRadius = bHole ? outerRadius - ring.thickness : 0;

		int64_t outline = std::max(ring.outline, 0);
		outline = std::min(outline, bHole ? int64_t(ring.thickness / 2) : outerRadius);

		shape.bBack = ring.bBack;
		shape.bSquare = ring.bSquare;
		shape.bHole = bHole;
		shape.bOutline = outline > 0;

		int64_t* radius = shape.radius;

		radius[0] = ring.bSquare ? 0 : outerRadius;
		radius[1] = (shape.bOutline && !ring.bSquare) ? std::max(outerRadius - outline, int64_t(0)) : 0;
		radius[2] = (shape.bOutline && bHole) ? innerRadius + outline : 0;
		radius[3] = innerRadius;

		for (int i = 0; i < 4; i++)
		{
			shape.radius2[i] = uint64_t(radius[i]) * uint64_t(radius[i]);
			shape.limit[i] = 8 * radius[i] * c_maxSagitta;
		}

		for (int i = 0; i < 2; i++)
		{
			int64_t inset = i == 0 ? 0 : outline;
			shape.sqLeft[i] = shape.left + inset;
			shape.sqTop[i] = shape.top + inset;
			shape.sqRight[i] = shape.right - inset;
			shape.sqBottom[i] = shape.bottom - inset;
		}

		shape.outerLimit = shape.bOutline ? 1 : 0;
		shape.innerLimit = shape.bOutline ? 2 : 3;

		// Slices between circles outerLimit and innerLimit.

		if (ring.bSquare)
			shape.bSlices = shape.sqRight[shape.outerLimit] > shape.sqLeft[shape.outerLimit] && shape.sqBottom[shape.outerLimit] > shape.sqTop[shape.outerLimit];
		else
			shape.bSlices = radius[shape.outerLimit] > 0;

		if (bHole)
		{
			int64_t holeRadius = radius[shape.innerLimit];
			int64_t sliceRadius = ring.bSquare ? std::min(cx - shape.sqLeft[shape.outerLimit], cy - shape.sqTop[shape.outerLimit]) : radius[shape.outerLimit];
			if (sliceRadius <= holeRadius)
				shape.bSlices = false;
		}

		// Rays

		_setupRingAngles(ring);

		for (int half = 0; half < 2; half++)
		{
			auto& rays = m_rays[half];
			rays.clear();

			for (int idx : m_crossings[half])
			{
				const Boundary& b = m_boundaries[idx];

				Ray ray;
				ray.slopeInt = b.slopeInt;
				ray.slopeFrac = b.slopeFrac;
				ray.segment = m_elementSegments[half == 0 ? b.element : b.prevElement];
				ray.bLeft = b.sx < 0;

				// Where the ray leaves the slices, through the circle or a side of the square.

				if (ring.bSquare)
				{
					int o = shape.outerLimit;
					int64_t halfW = b.sx < 0 ? cx - shape.sqLeft[o] : shape.sqRight[o] - cx;
					int64_t halfH = b.sy < 0 ? cy - shape.sqTop[o] : shape.sqBottom[o] - cy;

					if (int64_t(std::abs(b.sx)) * halfH > int64_t(std::abs(b.sy)) * halfW)
						ray.yOuter = cy + (int64_t(b.sy) * halfW) / std::abs(b.sx);
					else
						ray.yOuter = b.sy < 0 ? shape.sqTop[o] : shape.sqBottom[o];
				}
				else
					ray.yOuter = cy + ((radius[shape.outerLimit] * b.sy) >> 30);

				ray.yInner = bHole ? cy + ((radius[shape.innerLimit] * b.sy) >> 30) : cy;
				rays.push_back(ray);
			}

			shape.pRays[half] = rays.data();
			shape.nRays[half] = (int)rays.size();

			if (rays.empty())
				shape.startSegment[half] = m_elementSegments[_elementAt(0, half == 0 ? -1 : 1)];
			else
			{
				const Boundary& b = m_boundaries[m_crossings[half][0]];
				shape.startSegment[half] = m_elementSegments[half == 0 ? b.prevElement : b.element];
			}
		}

		// Sub-rows split where edges begin and end.

		m_splits.clear();

		if (ring.bBack || ring.bSquare)
		{
			m_splits.push_back(shape.top);
			m_splits.push_back(shape.bottom);
		}

		if (ring.bSquare && shape.bOutline)
		{
			m_splits.push_back(shape.sqTop[1]);
			m_splits.push_back(shape.sqBottom[1]);
		}

		for (int i = 0; i < 4; i++)
		{
			if (radius[i] > 0)
			{
				m_splits.push_back(cy - radius[i]);
				m_splits.push_back(cy + radius[i]);
			}
		}

		if (!m_boundaries.empty())
			m_splits.push_back(cy);

		if (shape.bSlices)
		{
			for (int half = 0; half < 2; half++)
				for (auto& ray : m_rays[half])
				{
					m_splits.push_back(ray.yOuter);
					if (bHole)
						m_splits.push_back(ray.yInner);
				}
		}

		std::sort(m_splits.begin(), m_splits.end());
		m_splits.erase(std::unique(m_splits.begin(), m_splits.end()), m_splits.end());

		shape.pSplits = m_splits.data();
		shape.nSplits = (int)m_splits.size();
		shape.maxEdges = 10 + (int)std::max(m_rays[0].size(), m_rays[1].size());
		shape.nIds = RingShape::c_firstRayId + (int)(m_rays[0].size() + m_rays[1].size());

		// Rows and columns. Without back, the outer edge bounds the ring instead of rect.

		int64_t boundsLeft = shape.left, boundsTop = shape.top, boundsRight = shape.right, boundsBottom = shape.bottom;

		if (!ring.bBack && !ring.bSquare)
		{
			boundsLeft = cx - outerRadius;
			boundsTop = cy - outerRadius;
			boundsRight = cx + outerRadius;
			boundsBottom = cy + outerRadius;
		}

		int clipBeg = std::max(clip.x, floorPixel(boundsLeft));
		int clipEnd = std::min(clip.x + clip.w, ceilPixel(boundsRight));
		int rowBeg = std::max(clip.y, floorPixel(boundsTop));
		int rowEnd = std::min(clip.y + clip.h, ceilPixel(boundsBottom));

		_rasterize(shape, c_ringFirstSliceSegment + ring.nSlices, rowBeg, rowEnd, clipBeg, clipEnd, pSink);
	}

}
