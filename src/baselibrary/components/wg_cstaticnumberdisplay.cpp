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

#include <wg_widget.h>
#include <wg_cstaticnumberdisplay.h>

namespace wg
{
	const TypeInfo CStaticNumberDisplay::TYPEINFO = { "CStaticNumberDisplay", &CText::TYPEINFO };

	//____ constructor ________________________________________________________

	CStaticNumberDisplay::CStaticNumberDisplay(Widget * pWidget) : WidgetComponent(pWidget)
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CStaticNumberDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setLayout() _____________________________________________________

	void CStaticNumberDisplay::setLayout(NumberLayout* pLayout)
	{
		if (m_pLayout.rawPtr() != pLayout)
		{
			m_pLayout = pLayout;
			_refresh();
		}
	}

	//____ clearLayout() ___________________________________________________

	void CStaticNumberDisplay::clearLayout()
	{
		if (m_pLayout)
		{
			m_pLayout = 0;
			_refresh();
		}
	}

	//____ _set() _____________________________________________________________

	bool CStaticNumberDisplay::_set( double value )
	{
		if( m_value != value )
		{
			m_value = value;
			_refresh();
			return true;
		}
		return false;
	}

	//____ _render() __________________________________________________________

	void CStaticNumberDisplay::_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state)
	{
		NumberLayout* pLayout = m_pLayout ? m_pLayout.rawPtr() : Base::defaultNumberLayout().rawPtr();

		pLayout->render(pDevice, canvas, m_value, scale, state);

	}


	//____ _defaultSize() ___________________________________________________

	SizeSPX CStaticNumberDisplay::_defaultSize(int scale) const
	{
		NumberLayout* pLayout = m_pLayout ? m_pLayout.rawPtr() : Base::defaultNumberLayout().rawPtr();

		return pLayout->defaultSize(m_value, scale, State::Normal);
	}


	//____ _refresh() _________________________________________________________

	void CStaticNumberDisplay::_refresh()
	{
		NumberLayout* pLayout = m_pLayout ? m_pLayout.rawPtr() : Base::defaultNumberLayout().rawPtr();

//		if (m_pLayout)
//			m_charBuffer = pLayout->format(m_value, m_scale);

		_requestRender();

		//TODO: Conditional call to _requestResize();
	}

	//____ _stateChangeNeedsRender() __________________________________________

	bool CStaticNumberDisplay::_stateChangeNeedsRender(State newState, State oldState) const
	{
		NumberLayout* pLayout = m_pLayout ? m_pLayout.rawPtr() : Base::defaultNumberLayout().rawPtr();

		return pLayout->stateChangeNeedsRender(newState, oldState);
	}

	//____ _initFromBlueprint() _______________________________________________

	void CStaticNumberDisplay::_initFromBlueprint(const Blueprint& blueprint)
	{
		m_pLayout = blueprint.layout;
	}


} // namespace wg
