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
#ifndef WG_TIMER_DOT_H
#define WG_TIMER_DOT_H
#pragma once


#include <wg_widget.h>
#include <wg_skin.h>

namespace wg
{

	class Timer;
	typedef	StrongPtr<Timer>	Timer_p;
	typedef	WeakPtr<Timer>		Timer_wp;

	//____ Timer ____________________________________________________________
	/**
	 * @brief A simple Timer widget.
	 *
	 *
	 **/


	class Timer : public Widget
	{
	public:

		//.____ Creation __________________________________________

		static Timer_p	create() { return Timer_p(new Timer()); }

		//.____ Identification __________________________________________

		const TypeInfo&	typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;


		//.____ Control _______________________________________________________

		void			setDuration(int millisec);
		int				duration() const { return m_duration; }

		void			setPlayMode(PlayMode playMode);
		PlayMode		playMode() const { return m_playMode; }

		void			setStepSize(int millisec);
		inline int		stepSize() const { return m_stepSize; }


		//.____ State _________________________________________________________

		void			start();
		void			stop();
		void			reset();

		void			setOn(bool bOn);
		bool			isOn() const { return m_bOn; }


		//.____ Content _______________________________________________________

		void			setValue(int millisec);
		inline int		value() const { return m_value;	}


	protected:
		Timer();
		virtual ~Timer();
		virtual Widget* _newOfMyType() const override { return new Timer(); };
		void 			_receive(Msg* pMsg) override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_render(GfxDevice * pDevice, const Rect& _canvas, const Rect& _window) override;
		void		_setValue(int millisec);

		float		_skinValue(const SkinSlot* pSlot) const override;

	private:

		int			m_value;
		int			m_duration;
		int			m_stepSize;
		PlayMode	m_playMode;
		int			m_direction;			// 1 or -1, depending on the direction we go.
		bool		m_bOn;

		RouteId		m_tickRouteId;
		float		m_renderedFraction;

	};



} // namespace wg
#endif //WG_Timer_DOT_H
