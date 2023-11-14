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

#include	<wg_graphdisplay.h>
#include	<wg_gfxdevice.h>
#include	<wg_base.h>

namespace wg
{

	const TypeInfo GraphDisplay::TYPEINFO = { "GraphDisplay", &Widget::TYPEINFO };


	//____ constructor ____________________________________________________________

	GraphDisplay::GraphDisplay() : graphs(this), xLines(this), yLines(this),
		m_displaySkin(this), m_labelSkin(this)
	{
		m_displayCeiling = 0.f;
		m_displayFloor = 1.f;

		m_gridColor		= Color::DarkGray;
		m_gridThickness = 1;

		m_sideLabelPlacement = Placement::West;
		m_bottomLabelPlacement = Placement::South;

		m_sideLabelSpacing = 4;
		m_bottomLabelSpacing = 1;
	}

	//____ destructor _____________________________________________________________

	GraphDisplay::~GraphDisplay()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GraphDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setDisplayRange() __________________________________________________

	void GraphDisplay::setDisplayRange(float ceiling, float floor)
	{
		if (ceiling == m_displayCeiling && floor == m_displayFloor)
			return;

		m_displayCeiling = ceiling;
		m_displayFloor = floor;

		_fullRefreshOfAllWaveforms();
	}

	//____ _startedOrEndedTransition() __________________________________________

	void GraphDisplay::_startedOrEndedTransition()
	{
		bool bTransitioning = false;

		for(auto& graph : graphs)
		{
			if (graph.m_pSampleTransition || graph.m_pColorTransition)
			{
				bTransitioning = true;
				break;
			}
		}

		if( bTransitioning != m_bReceivingUpdates)
		{
			if (bTransitioning)
				_startReceiveUpdates();
			else
				_stopReceiveUpdates();
		}
	}

	//____ _graphVisibilityChanged() _________________________________________

	void GraphDisplay::_graphVisibilityChanged(Graph* pGraph)
	{
		// We need to render the area no matter if graph appeared or disappeared.

		_requestRenderGraph(pGraph, pGraph->m_begin, pGraph->m_end);

		// If we turned visible we might have a waveform that needs refresh.
		// (we don't perform refresh while hidden)

		if (pGraph->m_bVisible && pGraph->m_pWaveform && (pGraph->m_bSamplesChanged || pGraph->m_bColorsChanged))
			_waveformNeedsRefresh(pGraph, false, pGraph->m_bSamplesChanged, pGraph->m_bColorsChanged);
	}


	//____ _setGraphRange() __________________________________________________

	void GraphDisplay::_setGraphRange(Graph* pGraph, float begin, float end)
	{
		float leftmost = std::min(begin, pGraph->m_begin);
		float rightmost = std::max(end, pGraph->m_end);

		pGraph->m_begin = begin;
		pGraph->m_end = end;

		_requestRenderGraph( pGraph, leftmost, rightmost );
		_waveformNeedsRefresh(pGraph, true, false, false);
	}

	//____ _didAddEntries() ___________________________________________________

	void GraphDisplay::_didAddEntries(Graph* pEntry, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			pEntry->m_pDisplay = this;
			_waveformNeedsRefresh(pEntry + i, true, true, true);
		}
	}

	//____ _didMoveEntries() ___________________________________________________

	void GraphDisplay::_didMoveEntries(Graph* pFrom, Graph* pTo, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			Graph* p = pTo + i;
			_requestRenderGraph(p, p->m_begin, p->m_end);
		}

	}

	//____ _willEraseEntries() ___________________________________________________

	void GraphDisplay::_willEraseEntries(Graph* pEntry, int nb)
	{
		for (int i = 0; i < nb; i++)
		{
			Graph* p = pEntry + i;
			_requestRenderGraph(p, p->m_begin, p->m_end);
		}
	}

	//____ _didAddEntries() ___________________________________________________

	void GraphDisplay::_didAddEntries(GridLine* pEntry, int nb)
	{
		// Add new TextItems to TextLayout

		TextLayout* pDefaultLayout = m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout();

		for (int i = 0; i < nb; i++)
		{
			pEntry[i].m_pDisplay = this;
			
			TextLayout* pLayout = pEntry[i].m_pTextLayout ? pEntry[i].m_pTextLayout.rawPtr() : pDefaultLayout;
			pLayout->addText(&pEntry[i]);
		}

		//

		_recalcGraphCanvas();
		_repositionAllLabels();
		_requestRender();
	}

	//____ _didMoveEntries() __________________________________________________

	void GraphDisplay::_didMoveEntries(GridLine* pFrom, GridLine* pTo, int nb)
	{
		// Notify TextLayout of moved TextItems

		TextLayout* pDefaultLayout = m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout();

		for (int i = 0; i < nb; i++)
		{
			TextLayout* pLayout = pTo[i].m_pTextLayout ? pTo[i].m_pTextLayout.rawPtr() : pDefaultLayout;
			pLayout->didMoveText(&pFrom[i], &pTo[i]);
		}

		// Re-render in case 

		_requestRender();
	}

	//____ _willEraseEntries() ________________________________________________

	void GraphDisplay::_willEraseEntries(GridLine* pEntry, int nb)
	{
		// Remove TextItems from TextLayout

		TextLayout* pDefaultLayout = m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout();

		for (int i = 0; i < nb; i++)
		{
			TextLayout* pLayout = pEntry[i].m_pTextLayout ? pEntry[i].m_pTextLayout.rawPtr() : pDefaultLayout;
			pLayout->removeText(&pEntry[i]);
		}

		//

		if (_recalcGraphCanvas())
			_repositionAllLabels();

		_requestRender();
	}

	//____ _recalcGraphCanvas() _______________________________________________

	bool GraphDisplay::_recalcGraphCanvas()
	{
		RectSPX contentGeo = m_skin.isEmpty() ? RectSPX(0,0,m_size) : m_skin.contentRect(RectSPX(0,0,m_size),m_scale, m_state);

		BorderSPX margin;

		if (!m_displaySkin.isEmpty())
			margin = m_displaySkin.contentBorder(m_scale, m_state);



		// Space needed left of graph for left labels, we include all labels, including
		// the invisible and out of range ones se we don't need to adjust if they pop in.

		for (auto& line : xLines)
		{
			if (!line.m_label.isEmpty())
			{
				CoordSPX ofs = _sideLabelOffset(&line);
				ofs.x *= -1;

				if (ofs.x > margin.left)
					margin.left = ofs.x;
			}
		}

		// Space needed below graph for bottom labels, we include all labels, including
		// the invisible ones so we don't need to adjust when they are made visible.

		for (auto& line : yLines)
		{
			if (!line.m_label.isEmpty())
			{
				CoordSPX ofs = _bottomLabelOffset(&line);
				ofs.y += line.m_labelGeo.size().h;

				if (ofs.y > margin.bottom)
					margin.bottom = ofs.y;
			}
		}


		
		// Adjust top and bottom margins so no side labels sticks out above/below canvas.

		float	rangeMin = std::min(m_displayCeiling, m_displayFloor);
		float	rangeMax = std::max(m_displayCeiling, m_displayFloor);

		float valueFactor = (m_displayFloor - m_displayCeiling) * contentGeo.h;

		if (!xLines.isEmpty())
		{
			// Find top & bottom line with label.

			GridLine* pTop = nullptr;
			GridLine* pBottom = nullptr;

			for (auto& line : xLines)
			{
				if (line.m_bVisible && !line.m_label.isEmpty() && line.m_value >= rangeMin && line.m_value <= rangeMax )
				{
					if (pTop == nullptr || line.m_value > pTop->m_value)
						pTop = &line;

					if (pBottom == nullptr || line.m_value < pBottom->m_value)
						pBottom = &line;

				}
			}

			if (m_displayFloor > m_displayCeiling)
				std::swap(pTop, pBottom);

			if (pTop)		// If we have top we also have bottom.
			{
				spx topLabelOfs = _sideLabelOffset(pTop).y;
				spx bottomLabelOfs = _sideLabelOffset(pBottom).y + pBottom->m_labelGeo.h;


				bool bModified;
				do {
					bModified = false;

					spx canvasHeight = contentGeo.h - margin.top - margin.bottom;

					spx topLineOfs = (pTop->m_value - m_displayCeiling) * (m_displayFloor - m_displayCeiling) * canvasHeight;
					if (margin.top < -(topLineOfs + topLabelOfs) )
					{
						margin.top = -(topLineOfs + topLabelOfs);
						bModified = true;
					}

					spx bottomLineOfs = (pBottom->m_value - m_displayCeiling) * (m_displayFloor - m_displayCeiling) * canvasHeight;
					if (margin.bottom < (bottomLineOfs + bottomLabelOfs ) - canvasHeight )
					{
						margin.bottom = (bottomLineOfs + bottomLabelOfs) - canvasHeight;
						bModified = true;
					}

				} while (bModified == true);

			}
		}

		// Adjust side margins so no bottom labels sticks out left/right of canvas.

		if (!yLines.isEmpty())
		{
			// Find leftmost and rightmost line with label.

			GridLine* pLeftmost = nullptr;
			GridLine* pRightmost = nullptr;

			for (auto& line : yLines)
			{
				if (line.m_bVisible && !line.m_label.isEmpty() && line.m_value >= 0.f && line.m_value <= 1.f)
				{
					if (pLeftmost == nullptr || line.m_value < pLeftmost->m_value)
						pLeftmost = &line;

					if (pRightmost == nullptr || line.m_value > pRightmost->m_value)
						pRightmost = &line;

				}
			}

			if (pLeftmost)		// If we have leftmost we also have rightmost.
			{
				spx leftLabelOfs = _bottomLabelOffset(pLeftmost).x;
				spx rightLabelOfs = _bottomLabelOffset(pRightmost).x + pRightmost->m_labelGeo.w;

				bool bModified;
				do {
					bModified = false;

					spx canvasWidth = contentGeo.w - margin.left - margin.right;

					spx leftLineOfs = pLeftmost->m_value * canvasWidth;
					if (margin.left < -(leftLineOfs + leftLabelOfs))
					{
						margin.left = -(leftLineOfs + leftLabelOfs);
						bModified = true;
					}

					spx rightLineOfs = pRightmost->m_value * canvasWidth;
					if (margin.right < (rightLineOfs + rightLabelOfs) - canvasWidth)
					{
						margin.right = (rightLineOfs + rightLabelOfs) - canvasWidth;
						bModified = true;
					}

				} while (bModified == true);

			}
		}


		// Finish up

		RectSPX graphCanvas = contentGeo - margin;

		if (graphCanvas == m_graphCanvas)
			return false;

		m_graphCanvas = graphCanvas;
		return true;
	}

	//____ _repositionAllLabels() _____________________________________________

	void GraphDisplay::_repositionAllLabels()
	{
		float	rangeMin = std::min(m_displayCeiling, m_displayFloor);
		float	rangeMax = std::max(m_displayCeiling, m_displayFloor);

		float valueFactor = (m_displayFloor - m_displayCeiling) * m_graphCanvas.h;

		for (auto& line : xLines)
		{
			if (line.m_bVisible && line.m_value >= rangeMin && line.m_value <= rangeMax)
			{
				CoordSPX pos = m_graphCanvas.pos();
				pos.y += (line.m_value - m_displayCeiling) * valueFactor;

				pos += _sideLabelOffset(&line);
				line.m_labelGeo.setPos(Util::align(pos));
			}
		}

		for (auto& line : yLines)
		{
			if (line.m_bVisible)
			{
				CoordSPX pos = m_graphCanvas.bottomLeft();
				pos.x += line.m_value * m_graphCanvas.w;

				pos += _bottomLabelOffset(&line); 
				line.m_labelGeo.setPos(Util::align(pos));
			}
		}
	}


	//____ _sideLabelOffset() ___________________________________________________

	CoordSPX GraphDisplay::_sideLabelOffset(GridLine* pLine)
	{
		spx spacing = Util::ptsToSpx(m_sideLabelSpacing, m_scale);

		Placement placement = pLine->m_labelPlacement != Placement::Undefined ? pLine->m_labelPlacement : m_sideLabelPlacement;
		CoordSPX pos = Util::placeRectAroundCoord(placement, { 0,0 }, pLine->m_labelGeo.size() + SizeSPX(2 * spacing, 0));
		pos += Util::ptsToSpx(pLine->m_labelAdjustment, m_scale);
		pos.x += spacing;
		return pos;
	}

	//____ _bottomLabelOffset() ___________________________________________________

	CoordSPX GraphDisplay::_bottomLabelOffset(GridLine* pLine)
	{
		spx spacing = Util::ptsToSpx(m_bottomLabelSpacing, m_scale);

		Placement placement = pLine->m_labelPlacement != Placement::Undefined ? pLine->m_labelPlacement : m_bottomLabelPlacement;
		CoordSPX pos = Util::placeRectAroundCoord(placement, { 0,0 }, pLine->m_labelGeo.size() + SizeSPX(0, 2 * spacing));
		pos += Util::ptsToSpx(pLine->m_labelAdjustment, m_scale);
		pos.y += spacing;
		return pos;
	}


	//____ _requestRenderGraph() ______________________________________________

	void GraphDisplay::_requestRenderGraph(Graph* pGraph, float leftmost, float rightmost)
	{
		//TODO: Take max/min sample values into account after they are maintained.

		RectSPX dirt;

		dirt.x = m_graphCanvas.x + m_graphCanvas.w * leftmost;
		dirt.y = m_graphCanvas.y;
		dirt.w = m_graphCanvas.w * (rightmost - leftmost);
		dirt.h = m_graphCanvas.h;

		pts outlineThickness = std::max(pGraph->m_topOutlineThickness, pGraph->m_bottomOutlineThickness);

		spx outlineMargin = (Util::ptsToSpx(outlineThickness, m_scale) / 2);

		dirt += BorderSPX(outlineMargin+64);

		_requestRender(RectSPX::overlap(Util::alignUp(dirt), m_graphCanvas) );
	}

	//____ _waveformNeedsRefresh() ____________________________________________

	void GraphDisplay::_waveformNeedsRefresh(Graph * pGraph, bool bGeo, bool bSamples, bool bColors)
	{
		if (bGeo)
		{
			pGraph->m_pWaveform = nullptr;
		}
		else
		{
			if (bColors)
				pGraph->m_bColorsChanged = true;

			if (bSamples)
				pGraph->m_bSamplesChanged = true;
		}

		if (pGraph->m_bVisible && !m_bPreRenderRequested)
			m_bPreRenderRequested = _requestPreRenderCall();
	}

	//____ _fullRefreshOfAllWaveforms() _______________________________________

	void GraphDisplay::_fullRefreshOfAllWaveforms()
	{
		for (auto& graph : graphs)
			graph.m_pWaveform = nullptr;

		if (!m_bPreRenderRequested)
			m_bPreRenderRequested = _requestPreRenderCall();
	}



	//____ _update() ____________________________________________________________

	void GraphDisplay::_update(int microPassed, int64_t microsecTimestamp)
	{
		bool	transitionsActive = false;

		// Update all transitions

		for (auto& graph : graphs)
		{
			if (graph.m_pSampleTransition)
			{
				int timestamp = graph.m_sampleTransitionProgress + microPassed;

				if( timestamp >= graph.m_pSampleTransition->duration() )
				{
					graph.m_sampleTransitionProgress = 0;
					graph.m_pSampleTransition = nullptr;
					graph.m_topSamples.swap( graph.m_endTopSamples );
					graph.m_bottomSamples.swap( graph.m_endBottomSamples );
				}
				else
				{
					graph.m_sampleTransitionProgress = timestamp;

					graph.m_pSampleTransition->snapshot(timestamp, (int) graph.m_topSamples.size(),
														graph.m_startTopSamples.data(),
														graph.m_endTopSamples.data(), 
														graph.m_topSamples.data());	

					graph.m_pSampleTransition->snapshot(timestamp, (int) graph.m_bottomSamples.size(),
														graph.m_startBottomSamples.data(),
														graph.m_endBottomSamples.data(),
														graph.m_bottomSamples.data());

					transitionsActive = true;
				}

				_waveformNeedsRefresh(&graph, false, true, false);
			}

			if (graph.m_pColorTransition)
			{
				int timestamp = graph.m_colorTransitionProgress + microPassed;

				if (timestamp >= graph.m_pColorTransition->duration())
				{
					graph.m_colorTransitionProgress = 0;
					graph.m_pColorTransition = nullptr;

					graph.m_fillColor = graph.m_endFillColor;
					graph.m_outlineColor = graph.m_endOutlineColor;

					graph.m_fillGradient = graph.m_endFillGradient;
					graph.m_outlineGradient = graph.m_endOutlineGradient;	
				}
				else
				{
					graph.m_colorTransitionProgress = timestamp;

					graph.m_fillColor = graph.m_pColorTransition->snapshot(timestamp, graph.m_startFillColor, graph.m_endFillColor);
					graph.m_outlineColor = graph.m_pColorTransition->snapshot(timestamp, graph.m_startOutlineColor, graph.m_endOutlineColor);

					if (!graph.m_fillGradient.isUndefined())
					{
						graph.m_fillGradient.topLeft = graph.m_pColorTransition->snapshot(timestamp, graph.m_startFillGradient.topLeft, graph.m_endFillGradient.topLeft);
						graph.m_fillGradient.topRight = graph.m_pColorTransition->snapshot(timestamp, graph.m_startFillGradient.topRight, graph.m_endFillGradient.topRight);
						graph.m_fillGradient.bottomLeft = graph.m_pColorTransition->snapshot(timestamp, graph.m_startFillGradient.bottomLeft, graph.m_endFillGradient.bottomLeft);
						graph.m_fillGradient.bottomRight = graph.m_pColorTransition->snapshot(timestamp, graph.m_startFillGradient.bottomRight, graph.m_endFillGradient.bottomRight);

						graph.m_outlineGradient.topLeft = graph.m_pColorTransition->snapshot(timestamp, graph.m_startOutlineGradient.topLeft, graph.m_endOutlineGradient.topLeft);
						graph.m_outlineGradient.topRight = graph.m_pColorTransition->snapshot(timestamp, graph.m_startOutlineGradient.topRight, graph.m_endOutlineGradient.topRight);
						graph.m_outlineGradient.bottomLeft = graph.m_pColorTransition->snapshot(timestamp, graph.m_startOutlineGradient.bottomLeft, graph.m_endOutlineGradient.bottomLeft);
						graph.m_outlineGradient.bottomRight = graph.m_pColorTransition->snapshot(timestamp, graph.m_startOutlineGradient.bottomRight, graph.m_endOutlineGradient.bottomRight);
					}

					transitionsActive = true;
				}

				_waveformNeedsRefresh(&graph, false, false, true);
			}
		}

		if (!transitionsActive)
			_stopReceiveUpdates();
	}

	//____ _updateGraphs() ______________________________________________________

	void GraphDisplay::_updateGraphs()
	{
		for (auto& graph : graphs)
		{
			if (graph.m_bVisible)
			{
				bool bNeedsRendering = false;

				if (!graph.m_pWaveform)
				{
					// Generate waveform

					RectSPX rect = _graphRangeToRect(graph.m_begin, graph.m_end);

					graph.m_waveformPos = rect.pos();
					
					graph.m_pWaveform = Waveform::create(WGBP(Waveform,
						_.size = rect.size()/64,
						_.bottomOutlineThickness = graph.m_bottomOutlineThickness*m_scale,
						_.color = graph.m_fillColor,
						_.gradient = graph.m_fillGradient,
						_.origo = SampleOrigo::Top,
						_.outlineColor = graph.m_outlineColor,
						_.outlineGradient = graph.m_outlineGradient,
						_.topOutlineThickness = graph.m_topOutlineThickness*m_scale
					) );

					// Interpolate and set samples

					_updateWaveformEdge(graph.m_pWaveform, true, (int) graph.m_topSamples.size(), graph.m_topSamples.data() );
					_updateWaveformEdge(graph.m_pWaveform, false, (int) graph.m_bottomSamples.size(), graph.m_bottomSamples.data());

					bNeedsRendering = true;
				}
				else
				{
					if (graph.m_bColorsChanged)
					{
						auto pWF = graph.m_pWaveform;

						pWF->setColor(graph.m_fillColor);
						pWF->setOutlineColor(graph.m_outlineColor);

						if (graph.m_fillGradient.isUndefined())
							pWF->clearGradient();
						else
							pWF->setGradient(graph.m_fillGradient);

						if (graph.m_outlineGradient.isUndefined())
							pWF->clearOutlineGradient();
						else
							pWF->setOutlineGradient(graph.m_outlineGradient);

						graph.m_bColorsChanged = false;

						bNeedsRendering = true;
					}

					if (graph.m_bSamplesChanged)
					{
						_updateWaveformEdge(graph.m_pWaveform, true, (int) graph.m_topSamples.size(), graph.m_topSamples.data());
						_updateWaveformEdge(graph.m_pWaveform, false, (int) graph.m_bottomSamples.size(), graph.m_bottomSamples.data());
						graph.m_bSamplesChanged = false;

						bNeedsRendering = true;
					}
				}
				if (bNeedsRendering )
					_requestRenderGraph(&graph, graph.m_begin, graph.m_end);
			}

		}
	}

	//____ _updateWaveformEdge() _________________________________________________

	void GraphDisplay::_updateWaveformEdge(Waveform* pWaveform, bool bTopEdge, int nSamples, float* pSamples)
	{
		// TODO: Better interpolation, especially when shrinking.

		int wfSamples = pWaveform->nbSamples();

		if (nSamples <= 1)
		{
			float sample = nSamples == 0 ? 0.f : pSamples[0];
			
			spx spxSample = (sample - m_displayCeiling) / (m_displayFloor - m_displayCeiling) * m_graphCanvas.h;

			if (bTopEdge)
				pWaveform->setFlatTopLine(0, wfSamples, spxSample);
			else
				pWaveform->setFlatBottomLine(0, wfSamples, spxSample);
		}
		else
		{
			spx * pConverted = (spx*) Base::memStackAlloc(wfSamples * sizeof(spx));


			SizeI	canvas = m_graphCanvas.size() / 64;

			float stepFactor = (nSamples - 1) / (float) wfSamples;

			float valueFactor = (m_displayFloor - m_displayCeiling) * m_graphCanvas.h;

			for (int i = 0; i < wfSamples; i++)
			{
				float sample = stepFactor * i;
				int ofs = (int)sample;
				float frac2 = sample - ofs;
				float frac1 = 1.f - frac2;

				float val1 = (pSamples[ofs] - m_displayCeiling) * valueFactor;
				float val2 = (pSamples[ofs + 1] - m_displayCeiling) * valueFactor;

				pConverted[i] = int(val1 * frac1 + val2 * frac2);
			}

			if (bTopEdge)
				pWaveform->setSamples(0, wfSamples, pConverted, nullptr);
			else
				pWaveform->setSamples(0, wfSamples, nullptr, pConverted);

			Base::memStackFree(wfSamples * sizeof(spx));
		}

	}



	//____ _graphRangeToRect() __________________________________

	RectSPX GraphDisplay::_graphRangeToRect(float begin, float end) const
	{
		RectSPX rect( m_graphCanvas.w * begin, 0, m_graphCanvas.w * (end - begin), m_graphCanvas.h );
		
		return Util::align(rect);
	}

	//____ _preRender() _______________________________________________________

	void GraphDisplay::_preRender()
	{
		_updateGraphs();
		m_bPreRenderRequested = false;
	}


	//____ _render() ____________________________________________________________

	void GraphDisplay::_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window)
	{
		Widget::_render(pDevice, _canvas, _window);

		RectSPX graphCanvas = m_graphCanvas + _canvas.pos();

		if (!m_displaySkin.isEmpty())
			m_displaySkin.render(pDevice, graphCanvas + m_displaySkin.contentBorder(m_scale, m_state), m_scale, m_state);

		float	rangeMin = std::min(m_displayCeiling, m_displayFloor);
		float	rangeMax = std::max(m_displayCeiling, m_displayFloor);

		// Render grid xLines

		{
			for (auto& line : xLines)
			{
				if (line.m_bVisible && line.m_value >= rangeMin && line.m_value <= rangeMax )
				{
					float valueFactor = (m_displayFloor - m_displayCeiling) * m_graphCanvas.h;

					CoordSPX pos = graphCanvas.pos();
					pos.y += (line.m_value - m_displayCeiling) * valueFactor;

					HiColor color = line.m_color == HiColor::Undefined ? m_gridColor : line.m_color;
					pts thickness = line.m_thickness > 0 ? line.m_thickness : m_gridThickness;

					pDevice->drawLine(pos, Direction::Right, graphCanvas.w, color, thickness * m_scale );
				}
			}
		}

		// Render grid yLines

		{
			for (auto& line : yLines)
			{
				if (line.m_bVisible)
				{	
					CoordSPX pos = graphCanvas.pos();
					pos.x += line.m_value * graphCanvas.w;

					HiColor color = line.m_color == HiColor::Undefined ? m_gridColor : line.m_color;
					pts thickness = line.m_thickness > 0 ? line.m_thickness : m_gridThickness;

					pDevice->drawLine(pos, Direction::Down, graphCanvas.h, color, thickness * m_scale);
				}
			}
		}


		// Render graphs

		auto popData = Util::limitClipList(pDevice, graphCanvas);

		for (auto& graph : graphs)
		{
			if (graph.m_bVisible)
			{
				auto pEdgemap = graph.m_pWaveform->refresh();
				pDevice->drawEdgemap(graphCanvas + graph.m_waveformPos, pEdgemap);
			}
		}

		Util::popClipList(pDevice, popData);

		// Render labels

		for (auto& line : xLines)
		{
			if (line.m_bVisible && line.m_value >= rangeMin && line.m_value <= rangeMax && !line.m_label.isEmpty() )
			{
				RectSPX geo = line.m_labelGeo + _canvas.pos();

				SkinSlot * pSkin = line.m_labelSkin.isEmpty() ? &m_labelSkin : &line.m_labelSkin;

				if (!pSkin->isEmpty())
				{
					pSkin->render(pDevice, geo, m_scale, m_state, line.m_value);
					geo = pSkin->contentRect(geo, m_scale, m_state);
				}

				TextLayout* pLayout = line.m_pTextLayout ? line.m_pTextLayout : (m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout());

				pLayout->render(&line, pDevice, geo);
			}
		}

		for (auto& line : yLines)
		{
			if (line.m_bVisible && !line.m_label.isEmpty())
			{
				RectSPX geo = line.m_labelGeo + _canvas.pos();

				SkinSlot* pSkin = line.m_labelSkin.isEmpty() ? &m_labelSkin : &line.m_labelSkin;

				if (!pSkin->isEmpty())
				{
					pSkin->render(pDevice, geo, m_scale, m_state, line.m_value);
					geo = pSkin->contentRect(geo, m_scale, m_state);
				}

				TextLayout* pLayout = line.m_pTextLayout ? line.m_pTextLayout : (m_pTextLayout ? m_pTextLayout : Base::defaultTextLayout());

				pLayout->render(&line, pDevice, geo);
			}
		}



	}

	//____ _resize() __________________________________________________________

	void GraphDisplay::_resize(const SizeSPX& size, int scale)
	{
		int oldScale = m_scale;
		Widget::_resize(size, scale);

		// Resize labels if scale has changed

		if (scale != oldScale)
		{
			for (auto& gridLine : xLines)
				gridLine._resizeLabel(scale, oldScale);

			for (auto& gridLine : yLines)
				gridLine._resizeLabel(scale, oldScale);
		}


		_recalcGraphCanvas();
		_repositionAllLabels();

		_fullRefreshOfAllWaveforms();
	}




	//____ Graph::Graph() _______________________________________________________

	Graph::Graph(const Blueprint& bp)
	{
		m_bottomOutlineThickness = bp.bottomOutlineThickness;
		m_fillColor = bp.color;
		m_fillGradient = bp.gradient;
		m_outlineColor = bp.outlineColor;
		m_outlineGradient = bp.outlineGradient;
		m_begin = bp.rangeBegin;
		m_end = bp.rangeEnd;
		m_topOutlineThickness = bp.topOutlineThickness;
		m_bVisible = bp.visible;

		m_topSamples.push_back(0.f);
		m_bottomSamples.push_back(0.f);

	}

	//____ setColors() ________________________________________________________

	bool Graph::setColors(HiColor fill, HiColor outline, ColorTransition* pTransition)
	{
		if( !fill.isValid() || !outline.isValid() )
			return false;

		if (!m_fillGradient.isUndefined() || !m_outlineGradient.isUndefined())
		{
			m_fillGradient = Gradient::Undefined;
			m_outlineGradient = Gradient::Undefined;

			m_pDisplay->_waveformNeedsRefresh(this, false, false, true);
		}

		if( fill == m_fillColor && outline == m_outlineColor )
			return true;

		if (pTransition)
		{
			m_pColorTransition = pTransition;
			m_endFillColor = fill;
			m_endOutlineColor = outline;

			m_startFillColor = m_fillColor;
			m_startOutlineColor = m_outlineColor;

			m_pDisplay->_startedOrEndedTransition();
		}
		else
		{
			m_fillColor = fill;
			m_outlineColor = outline;

			m_pDisplay->_waveformNeedsRefresh(this, false, false, true);
		}

		return true;
	}

	//____ setGradients() _____________________________________________________

	bool Graph::setGradients(Gradient fill, Gradient outline, ColorTransition* pTransition)
	{
		if( !fill.isValid() || !outline.isValid() )
			return false;

		if (fill == m_fillGradient && outline == m_outlineGradient)
			return true;

		if (pTransition)
		{
			m_pColorTransition = pTransition;
			m_endFillGradient = fill;
			m_endOutlineGradient = outline;

			m_startFillGradient = m_fillGradient;
			m_startOutlineGradient = m_outlineGradient;

			m_pDisplay->_startedOrEndedTransition();
		}
		else
		{
			m_fillGradient = fill;
			m_outlineGradient = outline;

			m_pDisplay->_waveformNeedsRefresh(this, false, false, true);
		}

		return true;
	}

	//____ setRange() _________________________________________________________

	void Graph::setRange(float begin, float end)
	{
		limit(begin, 0.f, 1.f);
		limit(end, begin, 1.f);

		m_pDisplay->_setGraphRange(this, begin, end);
	}

	//____ setTopSamples() ____________________________________________________

	void Graph::setTopSamples(int nSamples, float* pSamples)
	{
		if (nSamples == 0)
		{
			m_topSamples.resize(1);
			m_topSamples[0] = 0.f;
		}
		else
		{
			m_topSamples.resize(nSamples);
			memcpy(m_topSamples.data(), pSamples, nSamples * sizeof(float));
		}

		_endSampleTransition();

		m_pDisplay->_waveformNeedsRefresh(this, false, true, false);
	}

	//____ setBottomSamples() _________________________________________________

	void Graph::setBottomSamples(int nSamples, float* pSamples)
	{
		if (nSamples == 0)
		{
			m_bottomSamples.resize(1);
			m_bottomSamples[0] = 0.f;
		}
		else
		{
			m_bottomSamples.resize(nSamples);
			memcpy(m_bottomSamples.data(), pSamples, nSamples * sizeof(float));
		}

		_endSampleTransition();
		m_pDisplay->_waveformNeedsRefresh(this, false, true, false);
	}

	//____ transitionSamples() ________________________________________________

	bool Graph::transitionSamples(ArrayTransition* pTransition, int nTopSamples, float* pNewTopSamples, int nBottomSamples, float* pNewBottomSamples)
	{
		if( nTopSamples != m_topSamples.size() || nBottomSamples != m_bottomSamples.size() )
			return false;

		m_pSampleTransition = pTransition;

		m_startBottomSamples.resize(nBottomSamples);
		memcpy(m_startBottomSamples.data(), m_bottomSamples.data(), nBottomSamples * sizeof(float));

		m_endBottomSamples.resize(nBottomSamples);
		memcpy(m_endBottomSamples.data(), pNewBottomSamples, nBottomSamples * sizeof(float));

		m_startTopSamples.resize(nTopSamples);
		memcpy(m_startTopSamples.data(), m_topSamples.data(), nTopSamples * sizeof(float));

		m_endTopSamples.resize(nTopSamples);
		memcpy(m_endTopSamples.data(), pNewTopSamples, nTopSamples * sizeof(float));

		m_pDisplay->_startedOrEndedTransition();

		return true;
	}

	//____ setVisible() _______________________________________________________

	void Graph::setVisible(bool bVisible)
	{
		if( bVisible != m_bVisible )
		{
			m_bVisible = bVisible;
			m_pDisplay->_graphVisibilityChanged(this);
		}
	}

	//____ topSamples() _______________________________________________________

	std::tuple<int, const float*>  Graph::topSamples() const 
	{ 
		return std::make_tuple((int)m_topSamples.size(), m_topSamples.data()); 
	}

	//____ bottomSamples() ____________________________________________________

	std::tuple<int, const float*>  Graph::bottomSamples() const 
	{ 
		return std::make_tuple((int)m_bottomSamples.size(), m_bottomSamples.data()); 
	}

	//____ _endSampleTransition() _____________________________________________

	void Graph::_endSampleTransition()
	{
		m_pSampleTransition = nullptr;

		m_startBottomSamples.resize(0);
		m_endBottomSamples.resize(0);

		m_startTopSamples.resize(0);
		m_endTopSamples.resize(0);

		m_pDisplay->_startedOrEndedTransition();
	}

	//____ _endColorTransition() ______________________________________________

	void Graph::_endColorTransition()
	{
		m_pColorTransition = nullptr;

		m_startFillColor = m_fillColor;
		m_endFillColor = m_fillColor;

		m_startOutlineColor = m_outlineColor;
		m_endOutlineColor = m_outlineColor;

		m_pDisplay->_startedOrEndedTransition();
	}

	//____ _textSize() ________________________________________________________

	SizeSPX GridLine::_textSize() const
	{
		if (!m_labelSkin.isEmpty())
			return m_labelGeo.size() - m_labelSkin.contentBorderSize(m_pDisplay->_scale());
		else if( !m_pDisplay->m_labelSkin.isEmpty() )
			return m_labelGeo.size() - m_pDisplay->m_labelSkin.contentBorderSize(m_pDisplay->_scale());
		else
			return m_labelGeo.size();
	}

	//____ _textState() _______________________________________________________

	State GridLine::_textState() const
	{
		return m_pDisplay->m_state;
	}

	//____ _textStyle() _______________________________________________________

	TextStyle* GridLine::_textStyle() const
	{
		if (m_pTextStyle)
			return m_pTextStyle;
		else if (m_pDisplay->m_pTextStyle)
			return m_pDisplay->m_pTextStyle;

		return Base::defaultStyle();
	}

	//____ _textBegin() _______________________________________________________

	const Char* GridLine::_textBegin() const
	{
		return m_label.chars();
	}

	//____ _textLength() ______________________________________________________

	int GridLine::_textLength() const
	{
		return m_label.length();
	}

	//____ _caretVisible() ____________________________________________________

	bool GridLine::_caretVisible() const
	{
		return false;
	}

	//____ _caretOffset() _____________________________________________________

	int GridLine::_caretOffset() const
	{
		return 0;
	}

	//____ _selectedText() ____________________________________________________

	std::tuple<int, int> GridLine::_selectedText() const
	{
		return std::make_tuple(0, 0);
	}

	//____ _mapperRequestRender() _____________________________________________

	void GridLine::_mapperRequestRender()
	{
		RectSPX textGeo = m_labelGeo;

		if (!m_labelSkin.isEmpty() )
			textGeo -= m_labelSkin.contentBorder(m_pDisplay->m_scale, m_pDisplay->m_state);
		else if (!m_pDisplay->m_labelSkin.isEmpty())
			textGeo -= m_pDisplay->m_labelSkin.contentBorder(m_pDisplay->m_scale, m_pDisplay->m_state);

		m_pDisplay->_requestRender(textGeo);
	}

	void GridLine::_mapperRequestRender(const RectSPX& rect)
	{
		CoordSPX textPos = m_labelGeo.pos();

		if (!m_labelSkin.isEmpty())
			textPos += m_labelSkin.contentOfs(m_pDisplay->m_scale, m_pDisplay->m_state);
		else if (!m_pDisplay->m_labelSkin.isEmpty())
			textPos += m_pDisplay->m_labelSkin.contentOfs(m_pDisplay->m_scale, m_pDisplay->m_state);


		m_pDisplay->_requestRender(rect + textPos);
	}

	//____ _mapperRequestResize() _____________________________________________

	void GridLine::_mapperRequestResize()
	{
		_resizeLabel(m_pDisplay->m_scale, m_pDisplay->m_scale);
	}

	//____ _scale() ___________________________________________________________

	int GridLine::_scale() const
	{
		return m_pDisplay->m_scale;
	}

	//____ _resizeLabel() _____________________________________________________

	void GridLine::_resizeLabel(int scale, int oldScale)
	{
		TextLayout* pLayout			= m_pTextLayout ? m_pTextLayout : (m_pDisplay->m_pTextLayout ? m_pDisplay->m_pTextLayout : Base::defaultTextLayout() );
		SkinSlot *	pLabelSkin		= m_labelSkin.isEmpty() ? &m_pDisplay->m_labelSkin : &m_labelSkin;
		SizeSPX		oldLabelPadding;
		SizeSPX		newLabelPadding;

		if( !pLabelSkin->isEmpty() )
		{
			oldLabelPadding = pLabelSkin->contentBorderSize(oldScale);
			newLabelPadding = pLabelSkin->contentBorderSize(scale);
		}

		SizeSPX newSize = pLayout->defaultSize(this, scale);
		SizeSPX oldSize;
		
		if( !m_labelGeo.size().isEmpty() )
			oldSize = m_labelGeo.size() - oldLabelPadding;

		pLayout->onResized(this, newSize, oldSize, scale, oldScale);
		m_labelGeo.setSize(newSize + newLabelPadding);
	}

	//____ _skinValue() _______________________________________________________

	float GridLine::_skinValue(const SkinSlot* pSlot) const
	{
		return 1.f;
	}

	//____ _skinValue2() ______________________________________________________

	float GridLine::_skinValue2(const SkinSlot* pSlot) const
	{
		return -1.f;
	}

	//____ _skinState() _______________________________________________________

	State GridLine::_skinState(const SkinSlot* pSlot) const
	{
		return m_pDisplay->state();
	}

	//____ _skinSize() ________________________________________________________

	SizeSPX GridLine::_skinSize(const SkinSlot* pSlot) const
	{
		return m_labelGeo.size();
	}

	//____ _skinRequestRender() _______________________________________________

	void GridLine::_skinRequestRender(const SkinSlot* pSlot)
	{
		m_pDisplay->_requestRender(m_labelGeo);
	}

	void GridLine::_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect)
	{
		m_pDisplay->_requestRender(rect + m_labelGeo.pos());
	}
}
