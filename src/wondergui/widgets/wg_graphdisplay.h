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

#ifndef	WG_GRAPHDISPLAY_DOT_H
#define WG_GRAPHDISPLAY_DOT_H
#pragma once

#include <wg_widget.h>
#include <wg_dynamicvector.h>
#include <wg_transitions.h>
#include <wg_gradient.h>

namespace wg
{


	class Graph
	{
		friend class GraphDisplay;
	public:

		struct Blueprint
		{
			pts					bottomOutlineThickness = 1;
			HiColor				color = Color8::LightGrey;
			Gradient			gradient;								// Overrides color when set.
			SampleOrigo			origo = SampleOrigo::Top;
			HiColor				outlineColor = Color8::DarkGrey;
			Gradient			outlineGradient;						// Overrides outlineColor when set.
			float				rangeBegin = 0.f;
			float				rangeEnd = 1.f;
			pts					topOutlineThickness = 1;
			bool				visible = true;

		};

		Graph() {}
		Graph(const Blueprint& bp);

		void	setColors(HiColor fill, HiColor outline, ColorTransition* pTransition = nullptr);
		void	setGradients(Gradient fill, Gradient outline);

		void	setRange(float begin, float end);

		void	setTopSamples(int nSamples, float* pSamples);
		void	setBottomSamples(int nSamples, float* pSamples);

		void	transitionSamples(ArrayTransition* pTransition, int nTopSamples, float* pNewTopSamples, int nBottomSamples, float* pNewBottomSamples);

		void	setVisible(bool bVisible);
		bool	isVisible() const { return m_bVisible; }

		bool	isTransitioningColors() const { return m_pColorTransition; }
		bool	isTransitioningSamples() const { return m_pSampleTransition; }

		HiColor	color() const { return m_fillColor; }
		HiColor	outlineColor() const { return m_outlineColor; }

		std::tuple<int, const float*>	topSamples() const;
		std::tuple<int, const float*>	bottomSamples() const;

	protected:

		GraphDisplay*		m_pDisplay = nullptr;

		// Appearance

		bool				m_bVisible = true;

		HiColor				m_fillColor = Color::LightGray;
		HiColor				m_outlineColor = Color::Black;
		pts					m_topOutlineThickness		= 1;
		pts					m_bottomOutlineThickness	= 1;

		// Transitions

		ColorTransition_p	m_pColorTransition;
		int					m_colorTransitionProgress = 0;

		HiColor				m_startFillColor;
		HiColor				m_endFillColor;

		HiColor				m_startOutlineColor;
		HiColor				m_endOutlineColor;

		ArrayTransition_p	m_pSampleTransition;
		int					m_sampleTransitionProgress = 0;

		std::vector<float>	m_startTopSamples;
		std::vector<float>	m_endTopSamples;

		std::vector<float>	m_startBottomSamples;
		std::vector<float>	m_endBottomSamples;

		// Graph samples

		float				m_begin = 0.f;			// Offset in GraphDisplay where this graph begins.
		float				m_end = 1.f;			// Offset in GraphDisplay where this graph ends.
		std::vector<float>	m_topSamples;			// Samples for top of this graph.
		std::vector<float>	m_bottomSamples;		// Samples for bottom of this graph.

		float				m_sampleMin = 0;
		float				m_sampleMax = 0;

		// Output

		bool				m_bSamplesChanged = false;
		bool				m_bColorsChanged = false;

		SampleOrigo			m_pWaveform;

	};





	class GraphDisplay;
	typedef	StrongPtr<GraphDisplay>		GraphDisplay_p;
	typedef	WeakPtr<GraphDisplay>	GraphDisplay_wp;

	class GraphDisplay : public Widget , private DynamicVector<Graph>::Holder
	{
	public:

		struct Blueprint
		{
			Object_p		baggage;
			bool			dropTarget = false;
			bool			enabled = true;
			Finalizer_p		finalizer = nullptr;
			int				id = 0;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Default;
			bool			selectable = true;
			Skin_p			skin;
			bool			tabLock = false;
			String			tooltip;
		};


		//.____ Creation __________________________________________

		static GraphDisplay_p	create( const Blueprint& blueprint ) { return GraphDisplay_p(new GraphDisplay()); }


		//.____ Components _______________________________________

		DynamicVector<Graph>	graphs;


		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _______________________________________________

		SampleOrigo origo() const { return m_origo; }



		//.____ Internal _________________________________________________

	protected:
		GraphDisplay();
		virtual ~GraphDisplay();

		void		_didAddEntries(Graph* pEntry, int nb) override;
		void		_didMoveEntries(Graph* pFrom, Graph* pTo, int nb) override;
		void		_willEraseEntries(Graph* pEntry, int nb) override;

		void		_setState(State state) override;
		void		_update(int microPassed, int64_t microsecTimestamp) override;
		void		_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window) override;

	private:

		SampleOrigo	m_origo = SampleOrigo::MiddleUp;

	};




} // namespace wg
#endif //GRAPHDISPLAY_DOT_H
