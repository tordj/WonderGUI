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
#include <wg_cstaticvaluedisplay.h>

namespace wg
{
	const TypeInfo CStaticValueDisplay::TYPEINFO = { "CStaticValueDisplay", &CText::TYPEINFO };

	//____ constructor _____________________________________________________________

	CStaticValueDisplay::CStaticValueDisplay(GeoComponent::Holder * pHolder) : GeoComponent(pHolder)
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& CStaticValueDisplay::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ setPresenter() __________________________________________________________

	void CStaticValueDisplay::setPresenter(ValuePresenter* pPresenter)
	{
		if (m_pPresenter.rawPtr() != pPresenter)
		{
			m_pPresenter = pPresenter;
			_refresh();
		}
	}

	//____ clearPresenter() ________________________________________________________

	void CStaticValueDisplay::clearPresenter()
	{
		if (m_pPresenter)
		{
			m_pPresenter = 0;
			_refresh();
		}
	}

	//____ _set() ___________________________________________________________________

	bool CStaticValueDisplay::_set( double value )
	{
		if( m_value != value )
		{
			m_value = value;
			_refresh();
			return true;
		}
		return false;
	}

	//____ _refresh() _____________________________________________________________

	void CStaticValueDisplay::_refresh()
	{
		ValuePresenter* pPresenter = m_pPresenter ? m_pPresenter.rawPtr() : Base::defaultValuePresenter().rawPtr();

//		if (m_pPresenter)
//			m_charBuffer = pPresenter->format(m_value, m_scale);

		_requestRender();

		//TODO: Conditional call to _requestResize();
	}

} // namespace wg
