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
#include <wg_waveform.h>
#include <wg_text.h>

namespace wg
{
class GraphDisplay;

	class Graph
	{
		friend class GraphDisplay;
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

		Graph() {}
		Graph(const Blueprint& bp);

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



		GraphDisplay*		m_pDisplay = nullptr;

		// Appearance

		bool				m_bVisible = true;

		HiColor				m_fillColor = Color::LightGray;
		HiColor				m_outlineColor = Color::Black;
		Gradient			m_fillGradient;
		Gradient			m_outlineGradient;

		pts					m_topOutlineThickness		= 1;
		pts					m_bottomOutlineThickness	= 1;

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

		// Graph samples

		float				m_begin = 0.f;			// Offset in GraphDisplay where this graph begins.
		float				m_end = 1.f;			// Offset in GraphDisplay where this graph ends.
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


	class GridLine : protected TextItem, protected SkinSlot::Holder
	{
		friend class GraphDisplay;
	public:

		struct Blueprint
		{
			HiColor			color = HiColor::Undefined;
			
			String			label;
			Coord			labelAdjustment;
			Placement		labelPlacement;
			Skin_p			labelSkin;

			float			pos = 0.f;

			TextLayout_p	textLayout;
			TextStyle_p		textStyle;

			pts				thickness = 0;					// 0 = Undefined.
			bool			visible = true;
		};


		GridLine(const Blueprint& bp)
			: m_labelSkin(this),
			m_value(bp.pos),
			m_color(bp.color),
			m_thickness(bp.thickness),
			m_bVisible(bp.visible),
			m_label(bp.label),
			m_labelAdjustment(bp.labelAdjustment),
			m_pTextLayout(bp.textLayout),
			m_labelPlacement(bp.labelPlacement),
			m_pTextStyle(bp.textStyle)
		{
			m_labelSkin.set(bp.labelSkin);
		};

		GridLine()
			: m_labelSkin(this)
		{
			// This constructor is only used to expand vector at insertion of multiple elements.
			// We don't care about setting default values, they should be overwritten immediately anyway.
			
		};

	protected:

		SizeSPX			_textSize() const override;
		State			_textState() const override;
		TextStyle*		_textStyle() const override;

		const Char*		_textBegin() const override;
		int 			_textLength() const override;

		bool			_caretVisible() const override;
		int				_caretOffset() const override;
		std::tuple<int, int> _selectedText() const override;		// Begin/end of selection

		void			_mapperRequestRender() override;
		void			_mapperRequestRender(const RectSPX& rect) override;
		void			_mapperRequestResize() override;

		int				_scale() const override;

		float			_skinValue(const SkinSlot* pSlot) const override;
		float			_skinValue2(const SkinSlot* pSlot) const override;
		State			_skinState(const SkinSlot* pSlot) const override;

		SizeSPX			_skinSize(const SkinSlot* pSlot) const override;

		void			_skinRequestRender(const SkinSlot* pSlot) override;
		void			_skinRequestRender(const SkinSlot* pSlot, const RectSPX& rect) override;



		//

		void			_resizeLabel(int scale, int oldScale );


		GraphDisplay*	m_pDisplay = nullptr;

		RectSPX			m_labelGeo;

		float			m_value;
		HiColor			m_color;
		pts				m_thickness;

		bool			m_bVisible;

		String			m_label;
		Coord			m_labelAdjustment;
		TextLayout_p	m_pTextLayout;
		Placement		m_labelPlacement = Placement::Undefined;	// Defaults to West or South depending on x- or y-line.
		TextStyle_p		m_pTextStyle;
		SkinSlot		m_labelSkin;
	};


	class GraphDisplay;
	typedef	StrongPtr<GraphDisplay>		GraphDisplay_p;
	typedef	WeakPtr<GraphDisplay>	GraphDisplay_wp;

	class GraphDisplay : public Widget , private DynamicVector<Graph>::Holder, private DynamicVector<GridLine>::Holder
	{
		friend class Graph;
		friend class GridLine;
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

		static GraphDisplay_p	create( const Blueprint& blueprint ) { return GraphDisplay_p(new GraphDisplay(blueprint)); }


		//.____ Components _______________________________________

		DynamicVector<Graph>	graphs;
		DynamicVector<GridLine>	xLines;
		DynamicVector<GridLine>	yLines;

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _______________________________________________

		void	setDisplayRange(float ceiling, float floor);

		float	displayCeiling() const { return m_displayCeiling;  }
		float	displayFloor() const { return m_displayFloor; }


		//.____ Internal _________________________________________________

	protected:
		GraphDisplay();
		
		template<class BP> GraphDisplay( const BP& bp ) : graphs(this), xLines(this), yLines(this),
			m_displaySkin(this), m_labelSkin(this), Widget(bp)
		{
			m_displayCeiling = bp.displayCeiling;
			m_displayFloor = bp.displayFloor;
			m_displaySkin.set(bp.displaySkin);

			m_gridColor		= bp.gridColor;
			m_gridThickness = bp.gridThickness;
			m_pTextStyle	= bp.textStyle;
			m_labelSkin.set(bp.labelSkin);

			m_sideLabelPlacement = bp.sideLabelPlacement;
			m_bottomLabelPlacement = bp.bottomLabelPlacement;

			m_sideLabelSpacing = bp.sideLabelSpacing;
			m_bottomLabelSpacing = bp.bottomLabelSpacing;
		}

		virtual ~GraphDisplay();

		void		_startedOrEndedTransition();
		void		_graphVisibilityChanged(Graph* pGraph);
		void		_setGraphRange(Graph* pGraph, float newBegin, float newEnd);

		void		_didAddEntries(Graph* pEntry, int nb) override;
		void		_didMoveEntries(Graph* pFrom, Graph* pTo, int nb) override;
		void		_willEraseEntries(Graph* pEntry, int nb) override;

		void		_updateWaveformEdge(Waveform* pWaveform, bool bTopEdge, int nSamples, float* pSamples);

		void		_update(int microPassed, int64_t microsecTimestamp) override;

		void    	_preRender() override;
		void		_render(GfxDevice* pDevice, const RectSPX& _canvas, const RectSPX& _window) override;
		void		_resize(const SizeSPX& size, int scale) override;

		void		_updateGraphs();

		RectSPX		_graphRangeToRect( float begin, float end) const;

		void		_requestRenderGraph(Graph* pGraph, float leftmost, float rightmost);

		void		_waveformNeedsRefresh(Graph* pGraph, bool bGeo, bool bSamples, bool bColor);
		void		_fullRefreshOfAllWaveforms();

		//

		void		_didAddEntries(GridLine* pEntry, int nb) override;
		void		_didMoveEntries(GridLine* pFrom, GridLine* pTo, int nb) override;
		void		_willEraseEntries(GridLine* pEntry, int nb) override;

		bool		_recalcGraphCanvas();
		void		_repositionAllLabels();
		CoordSPX	_sideLabelOffset(GridLine* pLine);
		CoordSPX	_bottomLabelOffset(GridLine* pLine);


	private:

		bool			m_bPreRenderRequested = false;

		RectSPX			m_graphCanvas;

		float			m_displayCeiling;
		float			m_displayFloor;
		SkinSlot		m_displaySkin;

		HiColor			m_gridColor = Color::DarkGray;
		pts				m_gridThickness = 1;
		TextStyle_p		m_pTextStyle;
		TextLayout_p	m_pTextLayout;

		SkinSlot		m_labelSkin;
		Placement		m_sideLabelPlacement = Placement::West;
		Placement		m_bottomLabelPlacement = Placement::South;
		pts				m_sideLabelSpacing = 1;
		pts				m_bottomLabelSpacing = 1;

		//

//		float		m_sampleMin = 0;	// Minimum sample value of all graphs.
//		float		m_sampleMax = 0;


	};




} // namespace wg
#endif //GRAPHDISPLAY_DOT_H
