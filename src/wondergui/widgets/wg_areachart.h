/*=========================================================================

						 >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI AreaChartEntryics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

							-----------

  The WonderGUI AreaChartEntryics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

							-----------

  The WonderGUI AreaChartEntryics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_AREACHART_DOT_H
#define WG_AREACHART_DOT_H
#pragma once

#include <wg_chart.h>
#include <wg_waveform.h>

namespace wg
{
	class AreaChart;

	class AreaChartEntry
	{
		friend class AreaChart;
	public:

		struct Blueprint
		{
			pts					bottomOutlineThickness = 1;
			HiColor				color = Color8::LightGrey;
			Gradient			gradient;								// Overrides color when set.
			HiColor				outlineColor = Color8::DarkGrey;
			Gradient			outlineGradient;						// Overrides outlineColor when set.
			float				rangeBegin = 0.f;
			float				rangeEnd = 1.f;
			pts					topOutlineThickness = 1;
			bool				visible = true;

		};

		AreaChartEntry() {}
		AreaChartEntry(const Blueprint& bp);

		bool	setColors(HiColor fill, HiColor outline, ColorTransition* pTransition = nullptr);
		bool	setGradients(Gradient fill, Gradient outline, ColorTransition* pTransition = nullptr);

		void	setRange(float begin, float end);

		void	setTopSamples(int nSamples, float* pSamples);
		void	setBottomSamples(int nSamples, float* pSamples);

		bool	transitionSamples(ArrayTransition* pTransition, int nTopSamples, float* pNewTopSamples, int nBottomSamples, float* pNewBottomSamples);

		void	setVisible(bool bVisible);
		bool	isVisible() const { return m_bVisible; }

		bool	isTransitioningColors() const { return m_pColorTransition; }
		bool	isTransitioningSamples() const { return m_pSampleTransition; }

		HiColor	color() const { return m_fillColor; }
		HiColor	outlineColor() const { return m_outlineColor; }

		std::tuple<int, const float*>	topSamples() const;
		std::tuple<int, const float*>	bottomSamples() const;

	protected:

		void				_endSampleTransition();
		void				_endColorTransition();


		AreaChart* m_pDisplay = nullptr;

		// Appearance

		bool				m_bVisible = true;

		HiColor				m_fillColor = Color::LightGray;
		HiColor				m_outlineColor = Color::Black;
		Gradient			m_fillGradient;
		Gradient			m_outlineGradient;

		pts					m_topOutlineThickness = 1;
		pts					m_bottomOutlineThickness = 1;

		// Transitions

		ColorTransition_p	m_pColorTransition;
		int					m_colorTransitionProgress = 0;

		HiColor				m_startFillColor;
		HiColor				m_endFillColor;

		HiColor				m_startOutlineColor;
		HiColor				m_endOutlineColor;

		Gradient			m_startFillGradient;
		Gradient			m_endFillGradient;

		Gradient			m_startOutlineGradient;
		Gradient			m_endOutlineGradient;

		ArrayTransition_p	m_pSampleTransition;
		int					m_sampleTransitionProgress = 0;	// In microseconds

		std::vector<float>	m_startTopSamples;
		std::vector<float>	m_endTopSamples;

		std::vector<float>	m_startBottomSamples;
		std::vector<float>	m_endBottomSamples;

		// AreaChartEntry samples

		float				m_begin = 0.f;			// Offset in AreaChart where this graph begins.
		float				m_end = 1.f;			// Offset in AreaChart where this graph ends.
		std::vector<float>	m_topSamples;			// Samples for top of this graph.
		std::vector<float>	m_bottomSamples;		// Samples for bottom of this graph.

		//		float				m_sampleMin = 0;
		//		float				m_sampleMax = 0;

				// Output

		bool				m_bSamplesChanged = false;
		bool				m_bColorsChanged = false;

		Waveform_p			m_pWaveform;
		CoordSPX			m_waveformPos;			// Only x has any impact for now, y should be fixed at 0.
	};




	class AreaChart;
	typedef	StrongPtr<AreaChart>	AreaChart_p;
	typedef	WeakPtr<AreaChart>		AreaChart_wp;

	class AreaChart : public Chart, private DynamicVector<AreaChartEntry>::Holder
	{
		friend class AreaChartEntry;
	public:

		struct Blueprint
		{
			Object_p		baggage;

			Placement		bottomLabelPlacement = Placement::South;
			pts				bottomLabelSpacing = 1;

			float			displayCeiling = 0.f;
			float			displayFloor = 1.f;
			Skin_p			displaySkin;

			bool			dropTarget = false;
			bool			disabled = false;
			Finalizer_p		finalizer = nullptr;

			HiColor			gridColor = Color::DarkGray;
			pts				gridThickness = 1;

			int				id = 0;
			Skin_p			labelSkin;
			MarkPolicy		markPolicy = MarkPolicy::AlphaTest;
			bool			pickable = false;
			int				pickCategory = 0;
			PointerStyle	pointer = PointerStyle::Undefined;
			bool			selectable = true;

			Placement		sideLabelPlacement = Placement::West;
			pts				sideLabelSpacing = 4;

			Skin_p			skin;
			bool			stickyFocus = false;
			bool			tabLock = false;

			TextLayout_p	textLayout;
			TextStyle_p		textStyle;

			String			tooltip;
		};


		//.____ Creation __________________________________________

		static AreaChart_p	create( const Blueprint& blueprint ) { return AreaChart_p(new AreaChart(blueprint)); }


		//.____ Components _______________________________________

		DynamicVector<AreaChartEntry>	entries;

		//.____ Identification __________________________________________

		const TypeInfo&			typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


	protected:
		AreaChart();
		
		template<class BP> AreaChart( const BP& bp ) : Chart(bp), entries(this)
		{
		}

		virtual ~AreaChart();

		void		_fullRefreshOfChart() override;
		void		_renderCharts(GfxDevice* pDevice, const RectSPX& canvas) override;

		void		_startedOrEndedTransition();
		void		_graphVisibilityChanged(AreaChartEntry* pAreaChartEntry);
		void		_setAreaChartEntryRange(AreaChartEntry* pAreaChartEntry, float newBegin, float newEnd);

		void		_didAddEntries(AreaChartEntry* pEntry, int nb) override;
		void		_didMoveEntries(AreaChartEntry* pFrom, AreaChartEntry* pTo, int nb) override;
		void		_willEraseEntries(AreaChartEntry* pEntry, int nb) override;

		void		_updateWaveformEdge(Waveform* pWaveform, bool bTopEdge, int nSamples, float* pSamples);

		void		_update(int microPassed, int64_t microsecTimestamp) override;

		void    	_preRender() override;

		void		_updateAreaChartEntrys();

		RectSPX		_graphRangeToRect( float begin, float end) const;

		void		_requestRenderAreaChartEntry(AreaChartEntry* pAreaChartEntry, float leftmost, float rightmost);

		void		_waveformNeedsRefresh(AreaChartEntry* pAreaChartEntry, bool bGeo, bool bSamples, bool bColor);

		//

	private:

		bool			m_bPreRenderRequested = false;
		bool			m_bTransitioning = false;
	};




} // namespace wg
#endif //WG_AREACHART_DOT_H
