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
#ifndef WG_RANGESLIDER_DOT_H
#define WG_RANGESLIDER_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

namespace wg
{

	class RangeSlider;
	typedef	StrongPtr<RangeSlider>	RangeSlider_p;
	typedef	WeakPtr<RangeSlider>		RangeSlider_wp;

	//____ RangeSlider ____________________________________________________________
	/**
	 * @brief A simple slider widget.
	 *
	 *
	 **/


	class RangeSlider : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static RangeSlider_p	create() { return RangeSlider_p(new RangeSlider()); }

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		Size			preferredSize() const override;

		void			setPreferredSlideLength(MU length);
		inline MU		preferredSlideLength() const { return m_preferredSlideLength; }

		//.____ Appearance _____________________________________________

		void			setSkin(Skin * pSkin) override;

		void			setBeginHandleSkin(Skin * pSkin);
		inline Skin_p	beginHandleSkin() const { return m_beginHandleSkin.skin(); }

		void			setEndHandleSkin(Skin * pSkin);
		inline Skin_p	endHandleSkin() const { return m_endHandleSkin.skin(); }

		void			setAxis(Axis axis);
		Axis			axis() const { return m_axis; }


		//.____ Control _______________________________________________________

		void			setSteps(int nbSteps );
		inline int		steps() const { return m_nbSteps; }

		//.____ Content _______________________________________________________

		void			setRange(float begin, float end);

		inline float	rangeBegin() const { return m_rangeBegin; }
		inline float	rangeEnd() const { return m_rangeEnd; }


	protected:
		RangeSlider();
		virtual ~RangeSlider();
		virtual Widget* _newOfMyType() const override { return new RangeSlider(); };
		void 			_receive(Msg* pMsg) override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		bool		_alphaTest(const Coord& ofs) override;

		void		_updatePreferredSize();

		void		_setRange(float begin, float end, bool bPostMsg = true);
		void		_setHandleState(State state, bool isBeginHandle);
		Rect		_handleGeo(const Rect& widgetGeo, bool isbBeginHandle) const;

		void	_skinRequestRender(const SkinSlot* pSlot) override;
		void	_skinRequestRender(const SkinSlot* pSlot, const Rect& rect) override;

		Size	_skinSize(const SkinSlot* pSlot) const override;
		Coord	_skinGlobalPos(const SkinSlot* pSlot) const override;

		State	_skinState(const SkinSlot* pSlot) const override;
		float	_skinValue(const SkinSlot* pSlot) const override;
		float	_skinValue2(const SkinSlot* pSlot) const override;


	private:
		Size		m_preferredSize;
		MU			m_preferredSlideLength;
		SkinSlot	m_beginHandleSkin;
		SkinSlot	m_endHandleSkin;
		float		m_rangeBegin = 0.f;
		float		m_rangeEnd = 1.f;
		float		m_minRange = 0.f;
		float		m_valueAtPress = 0.f;
		int			m_nbSteps = 0;
		Axis		m_axis = Axis::X;
		State		m_beginHandleState;
		State		m_endHandleState;
	};



} // namespace wg
#endif //WG_RANGESLIDER_DOT_H
