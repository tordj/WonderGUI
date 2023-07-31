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

	GraphDisplay::GraphDisplay(const Blueprint& bp) : graphs(this), xLines(this), yLines(this)
	{
		_initFromBlueprint(bp);

		m_displayCeiling = bp.displayCeiling;
		m_displayFloor = bp.displayFloor;
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
			_waveformNeedsRefresh(pEntry+i, true, true, true);
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


	void GraphDisplay::_didAddEntries(GridLine* pEntry, int nb)
	{
		
	}

	void GraphDisplay::_didMoveEntries(GridLine* pFrom, GridLine* pTo, int nb)
	{
	}

	void GraphDisplay::_willEraseEntries(GridLine* pEntry, int nb)
	{
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
		{
			_requestPreRenderCall();
			m_bPreRenderRequested = true;
		}
	}

	//____ _fullRefreshOfAllWaveforms() _______________________________________

	void GraphDisplay::_fullRefreshOfAllWaveforms()
	{
		for (auto& graph : graphs)
			graph.m_pWaveform = nullptr;

		if (!m_bPreRenderRequested)
		{
			_requestPreRenderCall();
			m_bPreRenderRequested = true;
		}
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

					graph.m_pSampleTransition->snapshot(timestamp, graph.m_topSamples.size(), 
														graph.m_startTopSamples.data(), 
														graph.m_endTopSamples.data(), 
														graph.m_topSamples.data());	

					graph.m_pSampleTransition->snapshot(timestamp, graph.m_bottomSamples.size(),
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
						_.size = rect.size(),
						_.bottomOutlineThickness = graph.m_bottomOutlineThickness,
						_.color = graph.m_fillColor,
						_.gradient = graph.m_fillGradient,
						_.origo = SampleOrigo::Top,
						_.outlineColor = graph.m_outlineColor,
						_.outlineGradient = graph.m_outlineGradient,
						_.topOutlineThickness = graph.m_topOutlineThickness 
					) );

					// Interpolate and set samples

					_updateWaveformEdge(graph.m_pWaveform, true, graph.m_topSamples.size(), graph.m_topSamples.data() );
					_updateWaveformEdge(graph.m_pWaveform, false, graph.m_bottomSamples.size(), graph.m_bottomSamples.data());

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
						_updateWaveformEdge(graph.m_pWaveform, true, graph.m_topSamples.size(), graph.m_topSamples.data());
						_updateWaveformEdge(graph.m_pWaveform, false, graph.m_bottomSamples.size(), graph.m_bottomSamples.data());
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

				float val1 = (pSamples[ofs] - m_displayFloor) * valueFactor;
				float val2 = (pSamples[ofs + 1] - m_displayFloor) * valueFactor;

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
		RectSPX graphCanvas = m_graphCanvas + _canvas.pos();

		// Render grid xLines

		{
			float	rangeMin = std::min(m_displayCeiling, m_displayFloor);
			float	rangeMax = std::max(m_displayCeiling, m_displayFloor);

			for (auto& line : xLines)
			{
				if (line.m_bVisible && line.m_value >= rangeMin && line.m_value <= rangeMax )
				{
					float valueFactor = (m_displayFloor - m_displayCeiling) * m_graphCanvas.h;

					CoordSPX pos = graphCanvas.pos();
					pos.y += line.m_value * valueFactor;

					pDevice->drawLine(pos, Direction::Right, graphCanvas.w, m_xLineColor, line.m_thickness * m_scale );
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

					pDevice->drawLine(pos, Direction::Down, graphCanvas.h, m_yLineColor, line.m_thickness * m_scale);
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
	}

	//____ _resize() __________________________________________________________

	void GraphDisplay::_resize(const SizeSPX& size, int scale)
	{
		Widget::_resize(size, scale);

		m_graphCanvas = { 0,0,size };

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
		return std::make_tuple(m_topSamples.size(), m_topSamples.data()); 
	}

	//____ bottomSamples() ____________________________________________________

	std::tuple<int, const float*>  Graph::bottomSamples() const 
	{ 
		return std::make_tuple(m_bottomSamples.size(), m_bottomSamples.data()); 
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





}