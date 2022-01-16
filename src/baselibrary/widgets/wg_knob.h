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
#ifndef WG_KNOB_DOT_H
#define WG_KNOB_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

namespace wg
{

	class Knob;
	typedef	StrongPtr<Knob>	Knob_p;
	typedef	WeakPtr<Knob>		Knob_wp;

	//____ Knob ____________________________________________________________
	/**
	 * @brief A simple knob widget.
	 *
	 *
	 **/


	class Knob : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static Knob_p	create() { return Knob_p(new Knob()); }

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Control _______________________________________________________

		void			setSteps(int nbSteps );
		inline int		steps() const { return m_nbSteps; }

		void			setDragAxis(Axis axis);
		Axis			dragAxis() const { return m_dragAxis; }

		void			setDragRange(pts length);
		pts				dragRange() const { return m_dragRange; }

		void			setWheelStepSize(float stepSize);
		float			wheelStepSize() const { return m_wheelStepSize; }


		//.____ Content _______________________________________________________

		void			setValue(float value);
		inline float	value() const { return m_value;	}


	protected:
		Knob();
		virtual ~Knob();

		void		_receive(Msg* pMsg) override;
		void 		_render(GfxDevice * pDevice, const RectSPX& canvas, const RectSPX& window) override;
		void		_setValue(float value, bool bPostMsg = true);

		float		_skinValue(const SkinSlot* pSlot) const override;

	private:
		pts			m_dragRange = 200;
		pts			m_dragged;
		float		m_value = 0.f;
		float		m_valueAtPress = 0.f;
		float		m_wheelStepSize = 0.01f;
		int			m_nbSteps = 0;
		Axis		m_dragAxis = Axis::Y;
	};



} // namespace wg
#endif //WG_KNOB_DOT_H
