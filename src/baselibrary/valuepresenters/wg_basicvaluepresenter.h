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
#ifndef	WG_BASICVALUEPRESENTER_DOT_H
#define WG_BASICVALUEPRESENTER_DOT_H
#pragma once

#include <wg_valuepresenter.h>

namespace wg
{


	class BasicValuePresenter;
	typedef	StrongPtr<BasicValuePresenter>		BasicValuePresenter_p;
	typedef	WeakPtr<BasicValuePresenter>	BasicValuePresenter_wp;

	//____ BasicValuePresenter __________________________________________________________

	class BasicValuePresenter : public ValuePresenter
	{
	public:
		//.____ Creation __________________________________________

		static BasicValuePresenter_p	create() { return BasicValuePresenter_p(new BasicValuePresenter()); }
		static BasicValuePresenter_p	create( const CharSeq& format ) { return BasicValuePresenter_p(new BasicValuePresenter(format)); }

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Appearance ____________________________________________________

		void	setPlacement(Placement placement);
		inline Placement placement() const { return m_placement; }

		void	setStyle(TextStyle* pStyle);
		void	setStyles(	TextStyle* pPositiveIntegers, TextStyle* pNegativeIntegers,
							TextStyle* pPositiveDecimals, TextStyle* pNegativeDecimals );

		void	setNumberBase(int base);
		void	setDecimals(int maxDecimals, int minDecimals = 0, uint16_t decimalSign = '.');
		void	setIntegers(int grouping, int minIntegers = 1, uint16_t groupSeparator = ' ');
		void	setPrefix(String prefix);
		void	setSuffix(String suffix);

		//    ,1-3.0-2


		//.____ Misc __________________________________________________

		void 	render(double value, GfxDevice* pDevice, const RectSPX& canvas) override;
		SizeSPX	preferredSize(double value, int scale) const override;

	protected:
		BasicValuePresenter();
		BasicValuePresenter( const CharSeq& format );
		~BasicValuePresenter();

		Placement	m_placement = Placement::East;

		TextStyle_p	m_pStylePosInt;
		TextStyle_p	m_pStyleNegInt;
		TextStyle_p	m_pStylePosDec;
		TextStyle_p	m_pStyleNegDec;

		int			m_numberBase = 10;
		int			m_minDecimals = 0;
		int			m_maxDecimals = 6;
		int			m_minIntegers = 1;
		int			m_integerGrouping = 3;
		uint16_t	m_decimalSign = '.';
		uint16_t	m_groupSeparator = ' ';
		String		m_prefix;
		String		m_suffix;
	};


} // namespace wg
#endif //WG_BASICVALUEPRESENTER_DOT_H
