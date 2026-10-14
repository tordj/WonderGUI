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

#include <wg_referencebackend.h>
#include <wg_softsurfacefactory.h>
#include <wg_softedgemapfactory.h>
#include <wg_gfxutil.h>
#include <wg_gfxbase.h>

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <limits>

namespace wg
{
	const TypeInfo ReferenceBackend::TYPEINFO = { "ReferenceBackend", &GfxBackend::TYPEINFO };

	// Small helpers for the math.

	static double _clamp01(double value) { return std::clamp(value, 0.0, 1.0); }

	static double _mix(double from, double to, double fraction) { return from + (to - from) * fraction; }

	// Length of the overlap between the ranges begin1 -> end1 and begin2 -> end2.

	static double _overlap(double begin1, double end1, double begin2, double end2)
	{
		return std::max(0.0, std::min(end1, end2) - std::max(begin1, begin2));
	}

	// Moves a position onto the closest pixel boundary if it is within tolerance from it.

	static double _snapToPixelBoundary(double position, double tolerance)
	{
		double boundary = std::round(position);
		return std::abs(position - boundary) <= tolerance ? boundary : position;
	}

	// Modulo that is positive also for negative values, for wrapping around tiles.

	static int _wrap(int value, int size)
	{
		int result = value % size;
		return result < 0 ? result + size : result;
	}

	// Reads a 32-bit value from the command stream, where it is stored as two 16-bit words.

	static int32_t _read32(const uint16_t*& p)
	{
		int32_t value;
		std::memcpy(&value, p, sizeof(value));
		p += 2;
		return value;
	}


	//____ create() _____________________________________________

	ReferenceBackend_p ReferenceBackend::create()
	{
		return ReferenceBackend_p(new ReferenceBackend());
	}

	//____ constructor _____________________________________________

	ReferenceBackend::ReferenceBackend()
	{
		_resetStates();
	}

	//____ destructor _____________________________________________

	ReferenceBackend::~ReferenceBackend()
	{
		if (m_pBlitSource)
			m_pBlitSource->freePixelBuffer(m_blitSourceBuffer);

		if (m_pCanvas)
			m_pCanvas->freePixelBuffer(m_canvasBuffer);
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& ReferenceBackend::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ beginRender() _____________________________________________

	void ReferenceBackend::beginRender()
	{
	}

	//____ endRender() _____________________________________________

	void ReferenceBackend::endRender()
	{
		setCanvas((Surface*) nullptr);
	}

	//____ beginSession() _______________________________________________________

	void ReferenceBackend::beginSession( CanvasRef canvasRef, Surface * pCanvas, int nUpdateRects, const RectSPX * pUpdateRects, const SessionInfo * pInfo )
	{
		// We don't need the update rects. Everything we draw is within them anyway.

		if( canvasRef != CanvasRef::None )
			setCanvas(canvasRef);
		else
			setCanvas(pCanvas);
	}

	//____ endSession() _______________________________________________________

	void ReferenceBackend::endSession()
	{
	}

	//____ setCanvas() _____________________________________________

	void ReferenceBackend::setCanvas(CanvasRef ref)
	{
		auto pInfo = canvasInfo(ref);

		if (!pInfo)
		{
			GfxBase::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "Canvas reference has not been defined.", this, &TYPEINFO, __func__, __FILE__, __LINE__);
			return;
		}

		setCanvas(pInfo->pSurface);
	}

	void ReferenceBackend::setCanvas(Surface* pSurface)
	{
		// Give the pixels of the previous canvas back.

		if (m_pCanvas)
		{
			m_pCanvas->pullPixels(m_canvasBuffer, false);
			m_pCanvas->freePixelBuffer(m_canvasBuffer);
			m_pCanvas = nullptr;
		}

		// Start using the new canvas, if we can.

		if (pSurface)
		{
			PixelFormat format = pSurface->pixelFormat();

			if (format != PixelFormat::ARGB_8 && format != PixelFormat::XRGB_8 && format != PixelFormat::RGB_565 &&
				format != PixelFormat::BGR_565 && format != PixelFormat::Alpha_8)
			{
				GfxBase::throwError(ErrorLevel::Error, ErrorCode::InvalidParam, "ReferenceBackend can't render to surfaces of this pixel format.", this, &TYPEINFO, __func__, __FILE__, __LINE__);
			}
			else
			{
				m_pCanvas = pSurface;
				m_canvasBuffer = pSurface->allocPixelBuffer();
				m_bAlphaOnlyCanvas = (format == PixelFormat::Alpha_8);
				m_bCanvasHasAlpha = (format == PixelFormat::ARGB_8 || format == PixelFormat::Alpha_8);
			}
		}

		// All states are reset when a canvas is set.

		_resetStates();
	}

	//____ _resetStates() ________________________________________________________

	void ReferenceBackend::_resetStates()
	{
		if (m_pBlitSource)
		{
			m_pBlitSource->freePixelBuffer(m_blitSourceBuffer);
			m_pBlitSource = nullptr;
		}

		m_blendMode = BlendMode::Blend;
		m_tintColor = { 1, 1, 1, 1 };
		m_tint.nLayers = 0;
		m_morphFactor = 0.5;
		m_fixedBlendColor = { 1, 1, 1, 1 };

		m_blurRadius = s_defaultBlurRadius / 64.0;
		for (int color = 0; color < 3; color++)
			for (int i = 0; i < 9; i++)
				m_blurWeights[color][i] = s_defaultBlur[i] / 65536.0;
	}

	//____ setObjects() _____________________________________________

	void ReferenceBackend::setObjects(Object* const * pBeg, Object* const * pEnd)
	{
		m_pObjectsPtr = pBeg;
	}

	//____ setRects() _____________________________________________

	void ReferenceBackend::setRects(const RectSPX* pBeg, const RectSPX* pEnd)
	{
		m_pRectsPtr = pBeg;
	}

	//____ setColors() _____________________________________________

	void ReferenceBackend::setColors(const HiColor* pBeg, const HiColor* pEnd)
	{
		m_pColorsPtr = pBeg;
	}

	//____ setTransforms() _____________________________________________

	void ReferenceBackend::setTransforms(const Transform* pBeg, const Transform* pEnd)
	{
		m_pTransformsBeg = pBeg;
	}

	//____ processCommands() _____________________________________________
	/*
		Reads the commands and their parameters. Rects, colors and objects are read from their
		buffers as the commands need them, in the order the commands come.
	*/

	void ReferenceBackend::processCommands(const uint16_t* pBeg, const uint16_t* pEnd, int version)
	{
		// Transforms after the standard ones are custom transforms from the transform buffer.

		int customTransformStart = (version == 1) ? GfxFlip_size : NbStandardTransforms;

		auto p = pBeg;
		while (p < pEnd)
		{
			auto cmd = Command(*p++);
			switch (cmd)
			{
				case Command::None:
					break;

				case Command::StateChange:
					p = _changeStates(p);
					break;

				case Command::Fill:
				{
					int nRects = *p++;
					Color color = _toLinear(*m_pColorsPtr++);

					for (int i = 0; i < nRects; i++)
						_fill(*m_pRectsPtr++, color);
					break;
				}

				case Command::Line:
				{
					int nClipRects = *p++;
					int nLines = *p++;
					p++;										// padding

					const RectSPX* pClipRects = m_pRectsPtr;
					m_pRectsPtr += nClipRects;

					for (int i = 0; i < nLines; i++)
					{
						Color color = _toLinear(*m_pColorsPtr++);

						CoordSPX begin, end;
						begin.x = _read32(p);
						begin.y = _read32(p);
						end.x = _read32(p);
						end.y = _read32(p);

						spx thickness = *p++;
						p++;									// padding

						_drawLine(begin, end, thickness, color, nClipRects, pClipRects);
					}
					break;
				}

				case Command::Blit:
				case Command::ClipBlit:
				case Command::Tile:
				case Command::Blur:
				{
					int nRects = *p++;

					for (int i = 0; i < nRects; i++)
					{
						CoordI source;							// In 1/1024 pixels.
						source.x = _read32(p);
						source.y = _read32(p);

						CoordSPX dest;
						dest.x = _read32(p);
						dest.y = _read32(p);

						int transform = *p++;
						p++;									// padding

						bool bCustomTransform = (transform >= customTransformStart);
						const Transform& mtx = bCustomTransform ? m_pTransformsBeg[transform - customTransformStart] : s_standardTransforms[transform];

						_blit(cmd, source, dest, mtx, bCustomTransform, *m_pRectsPtr++);
					}
					break;
				}

				case Command::DrawEdgemap:
				case Command::TileEdgemap:
				{
					auto pEdgemap = static_cast<SoftEdgemap*>(*m_pObjectsPtr++);

					int nRects = *p++;
					GfxFlip flip = GfxFlip(*p++);
					p++;										// padding

					CoordSPX origo;
					origo.x = _read32(p);
					origo.y = _read32(p);

					for (int i = 0; i < nRects; i++)
						_drawEdgemap(pEdgemap, cmd == Command::TileEdgemap, flip, origo, *m_pRectsPtr++);
					break;
				}

				default:
					break;
			}
		}
	}

	//____ _changeStates() ________________________________________________________
	/*
		Reads a StateChange command. Each state that has changed has its parameters in the stream
		(or its color/object in their buffers), in the order of the StateChange bits.
	*/

	const uint16_t* ReferenceBackend::_changeStates(const uint16_t* p)
	{
		int statesChanged = *p++;

		if (statesChanged & int(StateChange::BlitSource))
		{
			if (m_pBlitSource)
				m_pBlitSource->freePixelBuffer(m_blitSourceBuffer);

			m_pBlitSource = static_cast<Surface*>(*m_pObjectsPtr++);

			if (m_pBlitSource)
				m_blitSourceBuffer = m_pBlitSource->allocPixelBuffer();
		}

		if (statesChanged & int(StateChange::TintColor))
			m_tintColor = _toLinear(*m_pColorsPtr++);

		if (statesChanged & int(StateChange::Tint))
			p = TintTools::decodeTint(p, m_pColorsPtr, m_tint);

		if (statesChanged & int(StateChange::BlendMode))
			m_blendMode = BlendMode(*p++);

		if (statesChanged & int(StateChange::MorphFactor))
			m_morphFactor = *p++ / 4096.0;

		if (statesChanged & int(StateChange::FixedBlendColor))
			m_fixedBlendColor = _toLinear(*m_pColorsPtr++);

		if (statesChanged & int(StateChange::Blur))
		{
			// Radius in spx, followed by nine weights each for red, green and blue, where 32768 is 1.0.

			m_blurRadius = *p++ / 64.0;

			for (int color = 0; color < 3; color++)
				for (int i = 0; i < 9; i++)
					m_blurWeights[color][i] = *p++ / 32768.0;
		}

		// The next command starts on a 32-bit boundary.

		if ((uintptr_t(p) & 0x2) == 2)
			p++;

		return p;
	}

	//____ _fill() ________________________________________________________________
	/*
		Fills a rectangle, which can have edges anywhere within pixels. Each pixel is covered
		by the overlap of the pixel and the rectangle.
	*/

	void ReferenceBackend::_fill(const RectSPX& rect, const Color& color)
	{
		double left = rect.x / 64.0;
		double top = rect.y / 64.0;
		double right = (rect.x + rect.w) / 64.0;
		double bottom = (rect.y + rect.h) / 64.0;

		for (int y = int(std::floor(top)); y < int(std::ceil(bottom)); y++)
		{
			for (int x = int(std::floor(left)); x < int(std::ceil(right)); x++)
			{
				double coverage = _overlap(x, x + 1, left, right) * _overlap(y, y + 1, top, bottom);

				_plotPixel(x, y, color * _deviceTint(x, y), coverage);
			}
		}
	}

	//____ _drawLine() ____________________________________________________________
	/*
		Draws a straight line with flat ends, as defined by the GfxBackend contract:

		Begin and end are rounded to whole pixels. A line that mainly runs along the x-axis is
		drawn as a column of color for every pixel from begin.x up to (but not including) end.x.
		Each column is centered on the line through the centers of the begin and end pixels and is
		thick enough to make the line 'thickness' thick, measured at right angle to the line. Lines
		that mainly run along the y-axis are drawn the same way with x and y swapped. A line from a
		pixel to itself has no length and draws nothing.

		Each pixel is covered by the overlap of the pixel and the column of color.

		Lines are tinted by the tint color, but not by the Tint.
	*/

	void ReferenceBackend::_drawLine(CoordSPX _begin, CoordSPX _end, spx thickness, const Color& color, int nClipRects, const RectSPX* pClipRects)
	{
		CoordI begin = Util::roundToPixels(_begin);
		CoordI end = Util::roundToPixels(_end);

		// We draw along the 'major' axis, which is the axis the line mainly runs along,
		// placing a column of color across the 'minor' axis at each pixel.

		bool bAlongX = std::abs(end.x - begin.x) > std::abs(end.y - begin.y);

		int majorBegin = bAlongX ? begin.x : begin.y;
		int majorEnd = bAlongX ? end.x : end.y;
		int minorBegin = bAlongX ? begin.y : begin.x;
		int minorEnd = bAlongX ? end.y : end.x;

		if (majorBegin > majorEnd)
		{
			std::swap(majorBegin, majorEnd);
			std::swap(minorBegin, minorEnd);
		}

		int length = majorEnd - majorBegin;
		if (length == 0)
			return;

		double slope = double(minorEnd - minorBegin) / length;
		double columnThickness = (thickness / 64.0) * std::sqrt(1.0 + slope * slope);

		Color lineColor = color * m_tintColor;

		for (int major = majorBegin; major < majorEnd; major++)
		{
			double center = minorBegin + 0.5 + slope * (major - majorBegin);
			double columnBegin = center - columnThickness / 2;
			double columnEnd = center + columnThickness / 2;

			for (int minor = int(std::floor(columnBegin)); minor < int(std::ceil(columnEnd)); minor++)
			{
				int x = bAlongX ? major : minor;
				int y = bAlongX ? minor : major;

				// Only pixels within the clip rectangles are drawn.

				bool bInside = false;
				for (int i = 0; i < nClipRects && !bInside; i++)
					bInside = _pixelRect(pClipRects[i]).contains(CoordI(x, y));

				if (bInside)
					_plotPixel(x, y, lineColor, _overlap(minor, minor + 1, columnBegin, columnEnd));
			}
		}
	}

	//____ _blit() ________________________________________________________________
	/*
		Blits the part of a Blit, ClipBlit, Tile or Blur that is within rect. Rect and dest are
		always whole pixels.

		Each destination pixel reads the source at a position given by the transform, which tells
		how far we move over the source for each pixel we move along x and y in the destination.
		The GfxBackend contract defines the position differently for the standard and the custom
		transforms:

		Standard transforms (blits 1:1 with flips and rotations): source is the position of the
		corner of the source rectangle that ends up at dest. Every pixel reads the source at the
		center of the pixel, transformed to the source.

		Custom transforms (scaling and arbitrary transforms): source is where the first destination
		pixel samples the source. Nearest sampling reads the pixel the position is in, while
		Bilinear sampling has the centers of the source pixels on whole positions.

		Blit reads within the source, ClipBlit reads transparent black outside the source, Tile
		repeats the source and Blur blends nine samples around the position.
	*/

	void ReferenceBackend::_blit(Command cmd, CoordI source, CoordSPX dest, const Transform& mtx, bool bCustomTransform, const RectSPX& rect)
	{
		if (!m_pBlitSource)
			return;

		bool bBilinear = (m_pBlitSource->sampleMethod() == SampleMethod::Bilinear);

		EdgeMode edgeMode = (cmd == Command::Tile) ? EdgeMode::Tile : (cmd == Command::ClipBlit) ? EdgeMode::Clip : EdgeMode::Clamp;

		CoordI destPixel = Util::roundToPixels(dest);
		RectI pixels = _pixelRect(rect);

		for (int y = pixels.y; y < pixels.y + pixels.h; y++)
		{
			for (int x = pixels.x; x < pixels.x + pixels.w; x++)
			{
				// Our offset from dest, at the corner or the center of the pixel.

				double dx = x - destPixel.x;
				double dy = y - destPixel.y;

				if (!bCustomTransform)
				{
					dx += 0.5;
					dy += 0.5;
				}

				// Position in the source, where pixel (x,y) covers x -> x+1 and y -> y+1.

				double srcX = source.x / 1024.0 + dx * mtx.xx + dy * mtx.yx;
				double srcY = source.y / 1024.0 + dx * mtx.xy + dy * mtx.yy;

				if (bCustomTransform)
				{
					// Custom transforms are floats, which can't describe steps like 1/3 exactly, so a position
					// that should end up exactly on a pixel boundary might end up a tiny bit before it. Positions
					// that are within the rounding error of the transform from a pixel boundary are moved onto it.

					srcX = _snapToPixelBoundary(srcX, (std::abs(dx * mtx.xx) + std::abs(dy * mtx.yx)) * FLT_EPSILON);
					srcY = _snapToPixelBoundary(srcY, (std::abs(dx * mtx.xy) + std::abs(dy * mtx.yy)) * FLT_EPSILON);

					if (bBilinear)
					{
						srcX += 0.5;
						srcY += 0.5;
					}
				}

				Color texel = (cmd == Command::Blur) ? _sampleBlurred(srcX, srcY) : _sampleSource(srcX, srcY, edgeMode);

				_plotPixel(x, y, texel * _deviceTint(x, y), 1.0);
			}
		}
	}

	//____ _drawEdgemap() _________________________________________________________
	/*
		Draws the part of an edgemap that is within rect, flipped/rotated by flip, with the
		top-left corner of the flipped edgemap at origo. When tiling, the edgemap is repeated
		over all of rect, with one of the copies at origo.

		The GfxBackend contract places edgemaps on whole pixels, so origo is rounded to nearest
		pixel. Each pixel of the canvas therefore covers exactly one pixel of the edgemap.
	*/

	void ReferenceBackend::_drawEdgemap(SoftEdgemap* pEdgemap, bool bTile, GfxFlip flip, CoordSPX _origo, const RectSPX& rect)
	{
		if (m_blendMode == BlendMode::Ignore)
			return;

		SizeI mapSize = pEdgemap->pixelSize();
		CoordI origo = Util::roundToPixels(_origo);

		// The standard transforms tell how we move over the edgemap when we move over the canvas,
		// just like for blits. The corner of the edgemap we start from is the one that makes us
		// move into the edgemap.

		const Transform& mtx = s_standardTransforms[int(flip)];

		double cornerX = (mtx.xx < 0 || mtx.yx < 0) ? mapSize.w : 0;
		double cornerY = (mtx.xy < 0 || mtx.yy < 0) ? mapSize.h : 0;

		// Decode the tints of the segments, placed over the edgemap.

		int nSegments = pEdgemap->renderSegments();

		m_segmentTints.resize(nSegments);
		for (int seg = 0; seg < nSegments; seg++)
		{
			Tint* pTint = pEdgemap->tints()[seg].rawPtr();
			m_segmentTints[seg].nLayers = 0;

			if (pTint)
			{
				uint16_t	words[TintTools::c_maxEncodedTintWords];
				HiColor		colors[TintTools::c_maxEncodedTintColors];
				int			nColors;

				TintTools::encodeTint(pTint, RectSPX(0, 0, mapSize.w * 64, mapSize.h * 64), words, colors, nColors);

				const HiColor* pColors = colors;
				TintTools::decodeTint(words, pColors, m_segmentTints[seg]);
			}
		}

		// Draw the pixels

		RectI pixels = _pixelRect(rect);

		for (int y = pixels.y; y < pixels.y + pixels.h; y++)
		{
			for (int x = pixels.x; x < pixels.x + pixels.w; x++)
			{
				// Center of the canvas pixel, transformed to the edgemap.

				double dx = x - origo.x + 0.5;
				double dy = y - origo.y + 0.5;

				double mapX = cornerX + dx * mtx.xx + dy * mtx.yx;
				double mapY = cornerY + dx * mtx.xy + dy * mtx.yy;

				int column = int(std::floor(mapX));
				int row = int(std::floor(mapY));

				if (bTile)
				{
					column = _wrap(column, mapSize.w);
					row = _wrap(row, mapSize.h);
				}
				else if (column < 0 || row < 0 || column >= mapSize.w || row >= mapSize.h)
					continue;

				_drawEdgemapPixel(pEdgemap, column, row, { x, y });
			}
		}
	}

	//____ _drawEdgemapPixel() ____________________________________________________
	/*
		Draws the pixel at column, row of the edgemap onto the canvas pixel. The pixel gets the
		average of what each segment would give the whole pixel, weighted by how much of the pixel
		the segment covers.
	*/

	void ReferenceBackend::_drawEdgemapPixel(SoftEdgemap* pEdgemap, int column, int row, CoordI canvasPixel)
	{
		int nSegments = pEdgemap->renderSegments();
		int nEdges = nSegments - 1;

		// Samples are stored column by column, with the samples of all edges of a column together.
		// There is always one more column of samples than columns of pixels.

		int samplePitch = pEdgemap->m_nbSegments - 1;
		const spx* pLeftSamples = pEdgemap->m_pSamples + column * samplePitch;
		const spx* pRightSamples = pLeftSamples + samplePitch;

		EdgeLine edges[Edgemap::maxSegments];

		for (int edge = 0; edge < nEdges; edge++)
			edges[edge] = { pLeftSamples[edge] / 64.0, pRightSamples[edge] / 64.0 };

		double coverage[Edgemap::maxSegments];
		_segmentCoverage(edges, nEdges, row, coverage);

		//

		Color dst = _canvasPixel(canvasPixel.x, canvasPixel.y);
		Color results[Edgemap::maxSegments];

		for (int seg = 0; seg < nSegments; seg++)
		{
			Color color = _segmentColor(pEdgemap, seg, column, row) * _deviceTint(canvasPixel.x, canvasPixel.y);
			results[seg] = _blend(color, dst);
		}

		_setCanvasPixel(canvasPixel.x, canvasPixel.y, _average(nSegments, results, coverage));
	}

	//____ _segmentCoverage() _____________________________________________________
	/*
		Calculates how much of the pixel at row in a column of an edgemap that each segment covers.
		Segment 0 is above edge 0, segment n is between edge n-1 and edge n and the last segment is
		below the last edge.

		Edges are not supposed to cross, but if one goes above an edge before it, it is pushed down
		to that edge. So the top of a segment is the lowest of all the edges above it.
	*/

	void ReferenceBackend::_segmentCoverage(const EdgeLine* pEdges, int nEdges, int row, double* pCoverage)
	{
		double areaBelowTop = 1.0;					// No edges above the first segment, all of the pixel is below its top.

		for (int seg = 0; seg <= nEdges; seg++)
		{
			double areaBelowBottom = (seg < nEdges) ? _areaBelow(pEdges, seg + 1, row) : 0.0;

			pCoverage[seg] = std::max(0.0, areaBelowTop - areaBelowBottom);
			areaBelowTop = areaBelowBottom;
		}
	}

	//____ _areaBelow() ___________________________________________________________
	/*
		Area of the pixel at row (0.0 -> 1.0) that is below the lowest of the given edges.

		At position x across the column, the height of the pixel below the edges is:

			height(x) = clamp( row + 1 - lowest edge at x, 0.0, 1.0 )

		Height is linear except where two edges cross (the lowest one changes) and where the lowest
		edge enters or leaves the pixel (the clamp starts or stops). We find all those points and
		integrate exactly between them, where height is linear.
	*/

	double ReferenceBackend::_areaBelow(const EdgeLine* pEdges, int nEdges, int row)
	{
		auto height = [&](double x)
		{
			double lowest = -std::numeric_limits<double>::infinity();
			for (int i = 0; i < nEdges; i++)
				lowest = std::max(lowest, pEdges[i].at(x));

			return _clamp01(row + 1 - lowest);
		};

		// Points where height might stop being linear.

		std::vector<double> points = { 0.0, 1.0 };

		auto addPoint = [&points](double x)
		{
			if (x > 0.0 && x < 1.0)
				points.push_back(x);
		};

		for (int i = 0; i < nEdges; i++)
		{
			const EdgeLine& edge = pEdges[i];
			double rise = edge.y1 - edge.y0;

			// Where the edge passes the top and bottom of the pixel.

			if (rise != 0.0)
			{
				addPoint((row - edge.y0) / rise);
				addPoint((row + 1 - edge.y0) / rise);
			}

			// Where the edge crosses the edges after it.

			for (int j = i + 1; j < nEdges; j++)
			{
				const EdgeLine& other = pEdges[j];
				double riseDiff = rise - (other.y1 - other.y0);

				if (riseDiff != 0.0)
					addPoint((other.y0 - edge.y0) / riseDiff);
			}
		}

		std::sort(points.begin(), points.end());

		// Integrate, height is linear between the points.

		double area = 0.0;

		for (size_t i = 1; i < points.size(); i++)
		{
			double x0 = points[i - 1];
			double x1 = points[i];

			area += (height(x0) + height(x1)) / 2 * (x1 - x0);
		}

		return area;
	}

	//____ _isInsideCanvas() ______________________________________________________

	bool ReferenceBackend::_isInsideCanvas(int x, int y) const
	{
		if (!m_pCanvas)
			return false;

		const RectI& r = m_canvasBuffer.rect;
		return (x >= r.x && y >= r.y && x < r.x + r.w && y < r.y + r.h);
	}

	//____ _canvasPixel() _________________________________________________________

	ReferenceBackend::Color ReferenceBackend::_canvasPixel(int x, int y) const
	{
		if (!_isInsideCanvas(x, y))
			return Color();

		return _readPixel(m_canvasBuffer, x, y);
	}

	//____ _setCanvasPixel() ______________________________________________________

	void ReferenceBackend::_setCanvasPixel(int x, int y, const Color& color)
	{
		if (_isInsideCanvas(x, y))
			_writePixel(m_canvasBuffer, x, y, color);
	}

	//____ _plotPixel() ___________________________________________________________
	/*
		Blends color into a pixel of the canvas, where it covers coverage (0.0 -> 1.0) of the
		pixel. The pixel gets the area weighted average of the blended color for the covered
		part and the original color for the rest.
	*/

	void ReferenceBackend::_plotPixel(int x, int y, const Color& color, double coverage)
	{
		if (m_blendMode == BlendMode::Ignore || coverage <= 0.0 || !_isInsideCanvas(x, y))
			return;

		Color dst = _canvasPixel(x, y);
		Color result = _blend(color, dst);

		if (coverage < 1.0)
		{
			Color colors[2] = { dst, result };
			double areas[2] = { 1.0 - coverage, coverage };

			result = _average(2, colors, areas);
		}

		_setCanvasPixel(x, y, result);
	}

	//____ _average() _____________________________________________________________
	/*
		Average of colors covering parts of a pixel, weighted by the area (0.0 -> 1.0) they cover.

		Colors with alpha are averaged as premultiplied colors, so that the more transparent a
		part is, the less its color counts and a fully transparent part only makes the pixel more
		transparent. The canvas might not have alpha though, in which case everything we write
		is opaque and the colors are averaged as they are.
	*/

	ReferenceBackend::Color ReferenceBackend::_average(int nColors, const Color* pColors, const double* pAreas) const
	{
		Color sum;
		double weightSum = 0.0;

		for (int i = 0; i < nColors; i++)
		{
			double weight = m_bCanvasHasAlpha ? pAreas[i] * pColors[i].a : pAreas[i];

			sum.r += pColors[i].r * weight;
			sum.g += pColors[i].g * weight;
			sum.b += pColors[i].b * weight;
			sum.a += pColors[i].a * pAreas[i];

			weightSum += weight;
		}

		if (weightSum > 0.0)
		{
			sum.r /= weightSum;
			sum.g /= weightSum;
			sum.b /= weightSum;
		}

		return sum;
	}

	//____ _blend() _______________________________________________________________
	/*
		Blends a source color with the destination color of the canvas, according to the blend
		mode. These are the definitions of the blend modes.
	*/

	ReferenceBackend::Color ReferenceBackend::_blend(const Color& src, const Color& dst) const
	{
		if (m_bAlphaOnlyCanvas)
			return _blendAlphaOnly(src, dst);

		switch (m_blendMode)
		{
			case BlendMode::Ignore:
				return dst;

			case BlendMode::Replace:
				return src;

			case BlendMode::Undefined:					// Undefined is treated as Blend.
			case BlendMode::Blend:
				return { _mix(dst.r, src.r, src.a), _mix(dst.g, src.g, src.a), _mix(dst.b, src.b, src.a),
						 src.a + dst.a * (1.0 - src.a) };

			case BlendMode::BlendFixedColor:			// As Blend, but onto the fixed blend color instead of the canvas.
			{
				const Color& fixed = m_fixedBlendColor;

				return { _mix(fixed.r, src.r, src.a), _mix(fixed.g, src.g, src.a), _mix(fixed.b, src.b, src.a),
						 src.a + fixed.a * (1.0 - src.a) };
			}

			case BlendMode::Add:
				return { _clamp01(dst.r + src.r * src.a), _clamp01(dst.g + src.g * src.a), _clamp01(dst.b + src.b * src.a), dst.a };

			case BlendMode::Subtract:
				return { _clamp01(dst.r - src.r * src.a), _clamp01(dst.g - src.g * src.a), _clamp01(dst.b - src.b * src.a), dst.a };

			case BlendMode::Multiply:
				return { dst.r * src.r, dst.g * src.g, dst.b * src.b, dst.a };

			case BlendMode::Invert:
			{
				auto invert = [](double s, double d) { return s * (1.0 - d) + d * (1.0 - s); };
				return { invert(src.r, dst.r), invert(src.g, dst.g), invert(src.b, dst.b), dst.a };
			}

			case BlendMode::Min:
				return { std::min(dst.r, src.r), std::min(dst.g, src.g), std::min(dst.b, src.b), dst.a };

			case BlendMode::Max:
				return { std::max(dst.r, src.r), std::max(dst.g, src.g), std::max(dst.b, src.b), dst.a };

			case BlendMode::Morph:
				return dst * (1.0 - m_morphFactor) + src * m_morphFactor;

			default:
				return dst;
		}
	}

	//____ _blendAlphaOnly() ______________________________________________________
	/*
		Blending onto a canvas with alpha only (Alpha_8), where the blend modes are applied to
		alpha as if it was a color channel.
	*/

	ReferenceBackend::Color ReferenceBackend::_blendAlphaOnly(const Color& src, const Color& dst) const
	{
		double a;

		switch (m_blendMode)
		{
			case BlendMode::Ignore:
				a = dst.a;
				break;
			case BlendMode::Replace:
				a = src.a;
				break;
			case BlendMode::Undefined:
			case BlendMode::Blend:
				a = src.a + dst.a * (1.0 - src.a);
				break;
			case BlendMode::BlendFixedColor:
				a = src.a + m_fixedBlendColor.a * (1.0 - src.a);
				break;
			case BlendMode::Add:
				a = _clamp01(dst.a + src.a);
				break;
			case BlendMode::Subtract:
				a = _clamp01(dst.a - src.a);
				break;
			case BlendMode::Multiply:
				a = dst.a * src.a;
				break;
			case BlendMode::Invert:
				a = src.a * (1.0 - dst.a) + dst.a * (1.0 - src.a);
				break;
			case BlendMode::Min:
				a = std::min(dst.a, src.a);
				break;
			case BlendMode::Max:
				a = std::max(dst.a, src.a);
				break;
			case BlendMode::Morph:
				a = _mix(dst.a, src.a, m_morphFactor);
				break;
			default:
				a = dst.a;
				break;
		}

		return { 1.0, 1.0, 1.0, a };
	}

	//____ _sampleSource() ________________________________________________________
	/*
		Reads the blit source at position x, y, where pixel (x,y) covers x -> x+1 and y -> y+1.

		Nearest sampling gives the pixel the position is in. Bilinear sampling interpolates between
		the four pixels with their centers closest to the position.
	*/

	ReferenceBackend::Color ReferenceBackend::_sampleSource(double x, double y, EdgeMode edgeMode) const
	{
		if (m_pBlitSource->sampleMethod() != SampleMethod::Bilinear)
			return _sourcePixel(int(std::floor(x)), int(std::floor(y)), edgeMode);

		// Position relative to the pixel centers.

		double cx = x - 0.5;
		double cy = y - 0.5;

		int x0 = int(std::floor(cx));
		int y0 = int(std::floor(cy));

		double fracX = cx - x0;
		double fracY = cy - y0;

		Color top = _sourcePixel(x0, y0, edgeMode) * (1.0 - fracX) + _sourcePixel(x0 + 1, y0, edgeMode) * fracX;
		Color bottom = _sourcePixel(x0, y0 + 1, edgeMode) * (1.0 - fracX) + _sourcePixel(x0 + 1, y0 + 1, edgeMode) * fracX;

		return top * (1.0 - fracY) + bottom * fracY;
	}

	//____ _sampleBlurred() _______________________________________________________
	/*
		Blurs the blit source around position x, y by sampling it at the position and at eight
		positions around it, the radius of the blur away (straight up, down, left and right) or
		0.7 times the radius away in both directions (diagonally). The red, green and blue channels
		are weighted sums of the samples, with weights from the blur state. Alpha is the alpha of the
		center sample. Samples outside the source read the pixels on its edge.
	*/

	ReferenceBackend::Color ReferenceBackend::_sampleBlurred(double x, double y) const
	{
		double r = m_blurRadius;
		double d = m_blurRadius * 0.7;

		const double offsets[9][2] = {	{ -d, -d }, { 0, -r }, { d, -d },
										{ -r,  0 }, { 0,  0 }, { r,  0 },
										{ -d,  d }, { 0,  r }, { d,  d } };

		Color result;

		for (int i = 0; i < 9; i++)
		{
			Color sample = _sampleSource(x + offsets[i][0], y + offsets[i][1], EdgeMode::Clamp);

			result.r += sample.r * m_blurWeights[0][i];
			result.g += sample.g * m_blurWeights[1][i];
			result.b += sample.b * m_blurWeights[2][i];

			if (i == 4)
				result.a = sample.a;
		}

		result.r = _clamp01(result.r);
		result.g = _clamp01(result.g);
		result.b = _clamp01(result.b);

		return result;
	}

	//____ _sourcePixel() _________________________________________________________

	ReferenceBackend::Color ReferenceBackend::_sourcePixel(int x, int y, EdgeMode edgeMode) const
	{
		SizeI size = m_pBlitSource->pixelSize();

		bool bOutside = (x < 0 || y < 0 || x >= size.w || y >= size.h);

		if (bOutside)
		{
			switch (edgeMode)
			{
				case EdgeMode::Clip:
					return Color();

				case EdgeMode::Tile:
					x = _wrap(x, size.w);
					y = _wrap(y, size.h);
					break;

				case EdgeMode::Clamp:
					x = std::clamp(x, 0, size.w - 1);
					y = std::clamp(y, 0, size.h - 1);
					break;
			}
		}

		return _readPixel(m_blitSourceBuffer, x, y);
	}

	//____ _deviceTint() __________________________________________________________
	/*
		The color the device tints pixel x,y of the canvas with: the tint color multiplied with
		the Tint (if any) at the center of the pixel.
	*/

	ReferenceBackend::Color ReferenceBackend::_deviceTint(int x, int y) const
	{
		if (m_tint.nLayers == 0)
			return m_tintColor;

		return m_tintColor * _tintColorAt(m_tint, x + 0.5, y + 0.5);
	}

	//____ _segmentColor() ________________________________________________________
	/*
		Color of a segment of an edgemap at the given pixel of the edgemap, from its Tint at
		the center of the pixel or its flat color.
	*/

	ReferenceBackend::Color ReferenceBackend::_segmentColor(SoftEdgemap* pEdgemap, int segment, int column, int row) const
	{
		if (pEdgemap->tints()[segment])
			return _tintColorAt(m_segmentTints[segment], column + 0.5, row + 0.5);

		return _toLinear(pEdgemap->flatColors()[segment]);
	}

	//____ _tintColorAt() _________________________________________________________
	/*
		Evaluates a Tint at position x, y (in pixels of the space the Tint is placed in).

		Each layer of the Tint is a gradient. The position along the gradient is calculated from its
		geometry and adjusted by its spread. The color is interpolated in sRGB between the two color
		stops around the position (or taken from the first or last stop if outside them). The
		resulting color is converted to linear and the layers are mixed by their weights.
	*/

	ReferenceBackend::Color ReferenceBackend::_tintColorAt(const TintTools::DecodedTint& tint, double x, double y)
	{
		if (tint.nLayers == 0)
			return { 1, 1, 1, 1 };

		Color result;

		for (int l = 0; l < tint.nLayers; l++)
		{
			const TintTools::TintLayer& layer = tint.layers[l];

			if (layer.weight == 0 || layer.nStops == 0)
				continue;

			// Position along the gradient. See TintTools::CanvasGeometry.

			double position;

			if (layer.shape == TintShape::Linear)
				position = layer.geo[0] * x + layer.geo[1] * y + layer.geo[2];
			else
			{
				double u = (x - layer.geo[0]) * layer.geo[2];
				double v = (y - layer.geo[1]) * layer.geo[3];
				position = std::sqrt(u * u + v * v);
			}

			// Spread

			switch (layer.spread)
			{
				default:
				case TintSpread::Pad:
					position = _clamp01(position);
					break;

				case TintSpread::Repeat:
					position = position - std::floor(position);
					break;

				case TintSpread::Reflect:
					position = position - 2.0 * std::floor(position / 2.0);			// 0.0 -> 2.0
					if (position > 1.0)
						position = 2.0 - position;
					break;
			}

			// Find the last stop at or before position. At a hard edge (two stops at the same position)
			// that is the second stop, so we get the color after the edge.

			const float* pStopPos = layer.stopPos;
			const HiColor* pStopColors = layer.stopColors;

			HiColor from, to;
			double fraction = 0.0;

			int stop = layer.nStops - 1;
			while (stop >= 0 && pStopPos[stop] > position)
				stop--;

			if (stop < 0)
				from = to = pStopColors[0];						// Before the first stop.
			else if (stop == layer.nStops - 1)
				from = to = pStopColors[stop];					// At or after the last stop.
			else
			{
				from = pStopColors[stop];
				to = pStopColors[stop + 1];
				fraction = (position - pStopPos[stop]) / (pStopPos[stop + 1] - pStopPos[stop]);
			}

			// Interpolate in sRGB, then convert to linear.

			Color color;
			color.r = _sRGBToLinear(_mix(from.r, to.r, fraction) / 4096.0);
			color.g = _sRGBToLinear(_mix(from.g, to.g, fraction) / 4096.0);
			color.b = _sRGBToLinear(_mix(from.b, to.b, fraction) / 4096.0);
			color.a = _mix(from.a, to.a, fraction) / 4096.0;

			result = result + color * (layer.weight / 4096.0);
		}

		return result;
	}

	//____ _toLinear() ____________________________________________________________
	/*
		Converts a HiColor, which has sRGB color channels and linear alpha, all 0 -> 4096.
	*/

	ReferenceBackend::Color ReferenceBackend::_toLinear(HiColor color)
	{
		return { _sRGBToLinear(color.r / 4096.0), _sRGBToLinear(color.g / 4096.0), _sRGBToLinear(color.b / 4096.0), color.a / 4096.0 };
	}

	//____ _sRGBToLinear() ________________________________________________________
	/*
		The sRGB transfer functions, exactly as specified.
	*/

	double ReferenceBackend::_sRGBToLinear(double value)
	{
		value = _clamp01(value);
		return value <= 0.04045 ? value / 12.92 : std::pow((value + 0.055) / 1.055, 2.4);
	}

	//____ _linearToSRGB() ________________________________________________________

	double ReferenceBackend::_linearToSRGB(double value)
	{
		value = _clamp01(value);
		return value <= 0.0031308 ? value * 12.92 : 1.055 * std::pow(value, 1.0 / 2.4) - 0.055;
	}

	//____ _readPixel() ___________________________________________________________
	/*
		Reads pixel x, y of a pixel buffer and converts it to a linear color.

		Channels are stored as integers from 0 up to the largest value that fits their bits, which
		is 0.0 -> 1.0. Color channels are in the color space of the pixels, alpha is always linear.
		Formats without color channels are white, formats without alpha are opaque.
	*/

	ReferenceBackend::Color ReferenceBackend::_readPixel(const PixelBuffer& buffer, int x, int y)
	{
		int bytesPerPixel = Util::pixelFormatToDescription(buffer.format).bits / 8;
		const uint8_t* p = buffer.pixels + (y - buffer.rect.y) * buffer.pitch + (x - buffer.rect.x) * bytesPerPixel;

		bool bLinear = (buffer.colorSpace == ColorSpace::Linear);			// Undefined is treated as sRGB.

		auto decode = [bLinear](int value, int maxValue)
		{
			double encoded = double(value) / maxValue;
			return bLinear ? encoded : _sRGBToLinear(encoded);
		};

		switch (buffer.format)
		{
			case PixelFormat::ARGB_8:
			case PixelFormat::XRGB_8:
			{
				// Channels are A, R, G, B from most to least significant byte.

				uint32_t v = buffer.bigEndian ? (uint32_t(p[0]) << 24) | (p[1] << 16) | (p[2] << 8) | p[3]
											  : (uint32_t(p[3]) << 24) | (p[2] << 16) | (p[1] << 8) | p[0];

				double alpha = (buffer.format == PixelFormat::ARGB_8) ? (v >> 24) / 255.0 : 1.0;

				return { decode((v >> 16) & 0xFF, 255), decode((v >> 8) & 0xFF, 255), decode(v & 0xFF, 255), alpha };
			}

			case PixelFormat::RGB_565:
			case PixelFormat::BGR_565:
			{
				// Five bits of red (RGB_565) or blue (BGR_565) at the top, six bits of green in the middle
				// and five bits of the other one at the bottom.

				uint16_t v = buffer.bigEndian ? (p[0] << 8) | p[1] : (p[1] << 8) | p[0];

				double high = decode(v >> 11, 31);
				double green = decode((v >> 5) & 0x3F, 63);
				double low = decode(v & 0x1F, 31);

				if (buffer.format == PixelFormat::RGB_565)
					return { high, green, low, 1.0 };
				else
					return { low, green, high, 1.0 };
			}

			case PixelFormat::Alpha_8:
				return { 1.0, 1.0, 1.0, p[0] / 255.0 };

			case PixelFormat::Index_8:
			case PixelFormat::Index_16:
			{
				int index = (buffer.format == PixelFormat::Index_8) ? p[0] : buffer.bigEndian ? (p[0] << 8) | p[1] : (p[1] << 8) | p[0];

				if (!buffer.palette)
					return Color();

				Color8 c = buffer.palette[index];
				return { decode(c.r, 255), decode(c.g, 255), decode(c.b, 255), c.a / 255.0 };
			}

			default:
				return Color();
		}
	}

	//____ _writePixel() __________________________________________________________
	/*
		Writes a linear color to pixel x, y of a pixel buffer. This is where colors are rounded,
		to nearest value the pixel can hold. Padding bits are left as they are.
	*/

	void ReferenceBackend::_writePixel(const PixelBuffer& buffer, int x, int y, const Color& color)
	{
		int bytesPerPixel = Util::pixelFormatToDescription(buffer.format).bits / 8;
		uint8_t* p = buffer.pixels + (y - buffer.rect.y) * buffer.pitch + (x - buffer.rect.x) * bytesPerPixel;

		bool bLinear = (buffer.colorSpace == ColorSpace::Linear);			// Undefined is treated as sRGB.

		auto encode = [bLinear](double linear, int maxValue)
		{
			double encoded = bLinear ? _clamp01(linear) : _linearToSRGB(linear);
			return uint32_t(std::floor(encoded * maxValue + 0.5));
		};

		auto encodeAlpha = [](double alpha, int maxValue)
		{
			return uint32_t(std::floor(_clamp01(alpha) * maxValue + 0.5));
		};

		switch (buffer.format)
		{
			case PixelFormat::ARGB_8:
			case PixelFormat::XRGB_8:
			{
				// Channels are A, R, G, B from most to least significant byte. XRGB_8 keeps its padding byte.

				uint8_t& top = buffer.bigEndian ? p[0] : p[3];
				uint32_t a = (buffer.format == PixelFormat::ARGB_8) ? encodeAlpha(color.a, 255) : top;

				uint32_t v = (a << 24) | (encode(color.r, 255) << 16) | (encode(color.g, 255) << 8) | encode(color.b, 255);

				for (int i = 0; i < 4; i++)
				{
					int shift = buffer.bigEndian ? 24 - i * 8 : i * 8;
					p[i] = uint8_t(v >> shift);
				}
				break;
			}

			case PixelFormat::RGB_565:
			case PixelFormat::BGR_565:
			{
				uint32_t red = encode(color.r, 31);
				uint32_t green = encode(color.g, 63);
				uint32_t blue = encode(color.b, 31);

				uint32_t high = (buffer.format == PixelFormat::RGB_565) ? red : blue;
				uint32_t low = (buffer.format == PixelFormat::RGB_565) ? blue : red;

				uint16_t v = uint16_t((high << 11) | (green << 5) | low);

				p[buffer.bigEndian ? 0 : 1] = uint8_t(v >> 8);
				p[buffer.bigEndian ? 1 : 0] = uint8_t(v);
				break;
			}

			case PixelFormat::Alpha_8:
				p[0] = uint8_t(encodeAlpha(color.a, 255));
				break;

			default:
				break;						// We can't render to other formats, setCanvas() makes sure we don't try.
		}
	}

	//____ _pixelRect() ___________________________________________________________
	/*
		The pixels of a rectangle in spx, e.g. the pixels with their centers inside it.
	*/

	RectI ReferenceBackend::_pixelRect(const RectSPX& rect)
	{
		int left = Util::roundToPixels(rect.x);
		int top = Util::roundToPixels(rect.y);
		int right = Util::roundToPixels(rect.x + rect.w);
		int bottom = Util::roundToPixels(rect.y + rect.h);

		return RectI(left, top, right - left, bottom - top);
	}

	//____ defineCanvas() _____________________________________________

	bool ReferenceBackend::defineCanvas(CanvasRef ref, SoftSurface* pSurface)
	{
		auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](CanvasInfo& entry) { return (ref == entry.ref); });

		if (it != m_definedCanvases.end())
			m_definedCanvases.erase(it);

		if (pSurface)
			m_definedCanvases.push_back(CanvasInfo(ref, pSurface, pSurface->pixelSize() * 64, pSurface->pixelFormat(), pSurface->scale(),
												   pSurface->colorSpace(), pSurface->isBigEndian()));
		return true;
	}

	//____ canvasInfo() _____________________________________________

	const CanvasInfo* ReferenceBackend::canvasInfo(CanvasRef ref) const
	{
		auto it = std::find_if(m_definedCanvases.begin(), m_definedCanvases.end(), [ref](const CanvasInfo& entry) { return (ref == entry.ref); });

		if (it != m_definedCanvases.end())
			return &*it;

		return nullptr;
	}

	//____ surfaceFactory() ___________________________________________________

	SurfaceFactory_p ReferenceBackend::surfaceFactory()
	{
		if (!m_pSurfaceFactory)
			m_pSurfaceFactory = SoftSurfaceFactory::create();

		return m_pSurfaceFactory;
	}

	//____ edgemapFactory() ___________________________________________________

	EdgemapFactory_p ReferenceBackend::edgemapFactory()
	{
		if (!m_pEdgemapFactory)
			m_pEdgemapFactory = SoftEdgemapFactory::create();

		return m_pEdgemapFactory;
	}

	//____ maxEdges() _____________________________________________

	int ReferenceBackend::maxEdges() const
	{
		return Edgemap::maxSegments - 1;
	}

	//____ canBeBlitSource() ___________________________________________

	bool ReferenceBackend::canBeBlitSource(const TypeInfo& type) const
	{
		return (type == SoftSurface::TYPEINFO);
	}

	//____ canBeCanvas() ______________________________________________

	bool ReferenceBackend::canBeCanvas(const TypeInfo& type) const
	{
		// Any surface that can give us a pixel buffer of a format we know.

		return true;
	}

	//____ waitForCompletion() ___________________________________________________

	void ReferenceBackend::waitForCompletion()
	{
	}

} // namespace wg
