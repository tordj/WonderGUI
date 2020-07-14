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
#ifndef WG_SLIDER_DOT_H
#define WG_SLIDER_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

namespace wg
{

	class Slider;
	typedef	StrongPtr<Slider>	Slider_p;
	typedef	WeakPtr<Slider>		Slider_wp;

	//____ Slider ____________________________________________________________
	/**
	 * @brief A simple slider widget.
	 *
	 *
	 **/


	class Slider : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static Slider_p	create() { return Slider_p(new Slider()); }

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		Size			preferredSize() const override;

		void			setPreferredSlideLength(MU length);
		inline MU		preferredSlideLength() const { return m_preferredSlideLength; }

		//.____ Appearance _____________________________________________

		void			setSkin(Skin * pSkin) override;

		void			setHandleSkin(const Skin_p& pSkin);
		inline Skin_p	handleSkin() const { return m_pHandleSkin; }

		void			setAxis(Axis axis);
		Axis			axis() const { return m_axis; }


		//.____ Control _______________________________________________________

		void			setSteps(int nbSteps );
		inline int		steps() const { return m_nbSteps; }

		//.____ Content _______________________________________________________

		void			setValue(float value);
		inline float	value() const { return m_value;	}


	protected:
		Slider();
		virtual ~Slider();
		virtual Widget* _newOfMyType() const override { return new Slider(); };
		void 			_receive(Msg* pMsg) override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		bool		_alphaTest(const Coord& ofs) override;

		void		_updatePreferredSize();

		void		_setValue(float value, bool bPostMsg = true);
		void		_setHandleState(State state);
		Rect		_handleGeo(const Rect& widgetGeo);


	private:
		Size		m_preferredSize;
		MU			m_preferredSlideLength;
		Skin_p		m_pHandleSkin;
		float		m_value = 0.f;
		float		m_valueAtPress = 0.f;
		int			m_nbSteps = 0;
		Axis		m_axis = Axis::X;
		State		m_handleState;
	};



} // namespace wg
#endif //WG_SLIDER_DOT_H
