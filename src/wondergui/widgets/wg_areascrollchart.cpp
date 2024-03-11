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

	AreaScrollChart::AreaScrollChart() : entries(this)
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

	//____ _didAddEntries() ___________________________________________________

	void AreaScrollChart::_didAddEntries(AreaScrollChartEntry* pEntry, int nb)
	{
		_requestFullRedraw();
	}

	//____ _didMoveEntries() __________________________________________________

	void AreaScrollChart::_didMoveEntries(AreaScrollChartEntry* pFrom, AreaScrollChartEntry* pTo, int nb)
	{
		_requestFullRedraw();
	}

	//____ _willEraseEntries() ________________________________________________

	void AreaScrollChart::_willEraseEntries(AreaScrollChartEntry* pEntry, int nb)
	{
		_requestFullRedraw();
	}

	//____ _renderOnScrollSurface() ___________________________________________

	void AreaScrollChart::_renderOnScrollSurface( SizeSPX canvasSize, spx rightEdgeOfs, int64_t rightEdgeTimestamp, spx dirtLen )
	{
		
	}

	//____ _startedOrEndedTransition() ________________________________________

	void AreaScrollChart::_startedOrEndedTransition()
	{

	}

	//____ _entryVisibilityChanged() __________________________________________

	void AreaScrollChart::_entryVisibilityChanged(AreaScrollChartEntry* pAreaChartEntry)
	{
		_requestFullRedraw();
	}

	//____ AreaScrollChartEntry::constructor ___________________________________

	AreaScrollChartEntry::AreaScrollChartEntry()
	{
	}

	AreaScrollChartEntry::AreaScrollChartEntry(const AreaScrollChartEntry::Blueprint& bp)
	{

	}

	//____ AreaScrollChartEntry::setSampleRate() ______________________________

	void AreaScrollChartEntry::setSampleRate(int samplesPerSec, float rateTweak)
	{

	}

	//____ AreaScrollChartEntry::addSamples() _________________________________

	void AreaScrollChartEntry::addSamples(int nbSamples, const float* pTopSamples, const float* pBottomSamples)
	{

	}

	//____ AreaScrollChartEntry::setDefaultSample() _________________________________

	void AreaScrollChartEntry::setDefaultSample(float topSample, float bottomSample)
	{

	}

	//____ AreaScrollChartEntry::setColors() _________________________________

	bool AreaScrollChartEntry::setColors(HiColor fill, HiColor outline, ColorTransition* pTransition)
	{
		if (!fill.isValid() || !outline.isValid())
			return false;

		if (!m_fillGradient.isUndefined() || !m_outlineGradient.isUndefined())
		{
			m_fillGradient = Gradient::Undefined;
			m_outlineGradient = Gradient::Undefined;

			m_pDisplay->_requestFullRedraw();
		}

		if (fill == m_fillColor && outline == m_outlineColor)
			return true;

		if (pTransition)
		{
			m_pColorTransition = pTransition;
			m_colorTransitionProgress = 0;

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

			m_pDisplay->_requestFullRedraw();
		}

		return true;
	}

	//____ AreaScrollChartEntry::setGradients() _________________________________

	bool AreaScrollChartEntry::setGradients(Gradient fill, Gradient outline, ColorTransition* pTransition)
	{
		if (!fill.isValid() || !outline.isValid())
			return false;

		if (fill == m_fillGradient && outline == m_outlineGradient)
			return true;

		if (pTransition)
		{
			m_pColorTransition = pTransition;
			m_colorTransitionProgress = 0;

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

			m_pDisplay->_requestFullRedraw();
		}

		return true;
	}

	//____ AreaScrollChartEntry::setVisible() _________________________________

	void AreaScrollChartEntry::setVisible(bool bVisible)
	{
		if (bVisible != m_bVisible)
		{
			m_bVisible = bVisible;
			m_pDisplay->_entryVisibilityChanged(this);
		}
	}

	//____ AreaScrollChartEntry::_endColorTransition() ______________________________________________

	void AreaScrollChartEntry::_endColorTransition()
	{
		m_pColorTransition = nullptr;

		m_startFillColor = m_fillColor;
		m_endFillColor = m_fillColor;

		m_startOutlineColor = m_outlineColor;
		m_endOutlineColor = m_outlineColor;

		m_pDisplay->_startedOrEndedTransition();
	}


} // namespace wg