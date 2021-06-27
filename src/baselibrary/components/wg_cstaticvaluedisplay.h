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
#ifndef	WG_CSTATICVALUEDISPLAY_DOT_H
#define WG_CSTATICVALUEDISPLAY_DOT_H
#pragma once

#include <wg_ctext.h>
#include <wg_valuepresenter.h>

namespace wg
{

	class CStaticValueDisplay;
	typedef	StrongComponentPtr<CStaticValueDisplay>	CStaticValueDisplay_p;
	typedef	WeakComponentPtr<CStaticValueDisplay>	CStaticValueDisplay_wp;

	//____ CStaticValueDisplay ____________________________________________________________

	class CStaticValueDisplay : public GeoComponent
	{
	public:
		CStaticValueDisplay(Holder * pHolder);


		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _____________________________________________

		void				setPresenter(ValuePresenter* pFormatter);
		void				clearPresenter();
		inline ValuePresenter_p	presenter() const { return m_pPresenter; }

		//.____ Content _______________________________________________

		inline double		value() const { return m_value; }

		//.____ Misc __________________________________________________

		inline CStaticValueDisplay_p	ptr() { return CStaticValueDisplay_p(this); }

		//.____ Internal __________________________________________________

		virtual bool		_set(double value);

	protected:

		void				_refresh();

		double				m_value = 0.0;

		ValuePresenter_p	m_pPresenter;
	};



} // namespace wg
#endif //WG_CSTATICVALUEDISPLAY_DOT_H
