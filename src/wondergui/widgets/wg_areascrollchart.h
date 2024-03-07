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

#ifndef WG_AREASCROLLCHART_DOT_H
#define WG_AREASCROLLCHART_DOT_H
#pragma once

#include <wg_scrollchart.h>
#include <wg_waveform.h>

namespace wg
{

    class AreScrollChart;
    typedef StrongPtr<AreaScrollChart> AreaScrollChart_p;
    typedef WeakPtr<AreaScrollChart> AreaScrollChart_wp;

    class AreaScrollChartEntry
    {
        friend class AreaChart;
    public:

    };

    class AreaScrollChart: public ScrollChart, private DynamicVector<AreaScrollChartEntry>::Holder
    {
        friend class AreaScrollChartEntry;

    public:
        struct Blueprint
        {
            Object_p baggage;

            Placement bottomLabelPlacement = Placement::South;
            pts bottomLabelSpacing = 1;

            float displayCeiling = 0.f;
            float displayFloor = 1.f;
            Skin_p displaySkin;

            bool dropTarget = false;
            bool disabled = false;
            Finalizer_p finalizer = nullptr;

            Glow::Blueprint glow;

            HiColor gridColor = Color::DarkGray;
            pts gridThickness = 1;

            int id = 0;
            Skin_p labelSkin;
            MarkPolicy markPolicy = MarkPolicy::AlphaTest;
            bool pickable = false;
            int pickCategory = 0;
            PointerStyle pointer = PointerStyle::Undefined;
            bool selectable = true;

            Placement sideLabelPlacement = Placement::West;
            pts sideLabelSpacing = 4;

            Skin_p skin;
            bool stickyFocus = false;
            bool tabLock = false;

            TextLayout_p textLayout;
            TextStyle_p textStyle;

            String tooltip;

            int displayTime = 4000000; // microsec of samples displayed.
        };

		//.____ Creation __________________________________________

        static AreaScrollChart_p create(const Blueprint& blueprint) { return AreaScrollChart_p(new AreaScrollChart(blueprint)); }

        //.____ Identification __________________________________________

        const TypeInfo& typeInfo(void) const override;
        const static TypeInfo TYPEINFO;

    protected:
        AreaScrollChart();

        template<class BP>
        AreaScrollChart(const BP& bp): ScrollChart(bp)
        {
        }

        virtual ~AreaScrollChart();

        void _update(int microPassed, int64_t microsecTimestamp) override;
        void _renderCharts(GfxDevice* pDevice, const RectSPX& canvas) override;

        //

   		void _didAddEntries(AreaScrollChartEntry* pEntry, int nb) override;
        void _didMoveEntries(AreaScrollChartEntry* pFrom, AreaScrollChartEntry* pTo, int nb) override;
        void _willEraseEntries(AreaScrollChartEntry* pEntry, int nb) override;

        void _renderOnScrollSurface(spx leftEdgeOfs, int64_t leftEdgeTimestamp) override;


    private:
    };

} // namespace wg
#endif // WG_AREASCROLLCHART_DOT_H