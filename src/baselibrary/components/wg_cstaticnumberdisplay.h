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
#ifndef	WG_CSTATICNUMBERDISPLAY_DOT_H
#define WG_CSTATICNUMBERDISPLAY_DOT_H
#pragma once

#include <wg_ctext.h>
#include <wg_numberlayout.h>

namespace wg
{
	class CStaticNumberDisplay;
	typedef	StrongComponentPtr<CStaticNumberDisplay>	CStaticNumberDisplay_p;
	typedef	WeakComponentPtr<CStaticNumberDisplay>	CStaticNumberDisplay_wp;

	//____ CStaticNumberDisplay ____________________________________________________________

	class CStaticNumberDisplay : public WidgetComponent
	{
	public:
		CStaticNumberDisplay(Widget * pHolder);


		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance _____________________________________________

		void				setLayout(NumberLayout* pLayout);
		void				clearLayout();
		inline NumberLayout_p	layout() const { return m_pLayout; }

		//.____ Content _______________________________________________

		inline double		value() const { return m_value; }

		//.____ Misc __________________________________________________

		inline CStaticNumberDisplay_p	ptr() { return CStaticNumberDisplay_p(this); }

		//.____ Internal __________________________________________________


		virtual bool		_set(double value);
		void 				_render(GfxDevice* pDevice, const RectSPX& canvas, int scale, State state);
		SizeSPX				_preferredSize(int scale) const;
		void				_refresh();
		bool				_stateChangeNeedsRender(State newState, State oldState) const;

	protected:
		
		NumberLayout* _layout() const { return m_pLayout ? m_pLayout.rawPtr() : Base::defaultNumberLayout().rawPtr(); }


		double				m_value = 0.0;

		NumberLayout_p	m_pLayout;
	};



} // namespace wg
#endif //WG_CSTATICNUMBERDISPLAY_DOT_H
