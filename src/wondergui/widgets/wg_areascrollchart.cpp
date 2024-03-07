/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI ScrollChartEntryics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI ScrollChartEntryics Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI ScrollChartEntryics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#include <wg_base.h>
#include <wg_gfxdevice.h>
#include <wg_areascrollchart.h>

#include <cstring>

namespace wg
{

    const TypeInfo AreaScrollChart::TYPEINFO = {"AreaScrollChart", &Widget::TYPEINFO};

    //____ constructor ____________________________________________________________

    AreaScrollChart::AreaScrollChart()
    {
    }

    //____ destructor _____________________________________________________________

    AreaScrollChart::~AreaScrollChart()
    {
    }

    //____ typeInfo() _________________________________________________________

    const TypeInfo& AreaScrollChart::typeInfo(void) const
    {
        return TYPEINFO;
    }

    //____ _update() ____________________________________________________________

    void AreaScrollChart::_update(int microPassed, int64_t microsecTimestamp)
    {
        ScrollChart::_update(microPassed, microsecTimestamp);
    }

    //____ _renderCharts() ____________________________________________________

    void AreaScrollChart::_renderCharts(GfxDevice* pDevice, const RectSPX& canvas)
    {

    }

    //____ _didAddEntries() ___________________________________________________

    void AreaScrollChart::_didAddEntries(AreaScrollChartEntry* pEntry, int nb)
    {

    }

    //____ _didMoveEntries() __________________________________________________

    void AreaScrollChart::_didMoveEntries(AreaScrollChartEntry* pFrom, AreaScrollChartEntry* pTo, int nb)
    {

    }

    //____ _willEraseEntries() ________________________________________________

    void AreaScrollChart::_willEraseEntries(AreaScrollChartEntry* pEntry, int nb)
    {

    }

    //____ _renderOnScrollSurface() ___________________________________________

    void AreaScrollChart::_renderOnScrollSurface(spx leftEdgeOfs, int64_t leftEdgeTimestamp)
    {

    }



} // namespace wg