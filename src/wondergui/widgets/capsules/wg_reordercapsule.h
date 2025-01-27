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
#ifndef WG_REORDERCAPSULE_DOT_H
#define WG_REORDERCAPSULE_DOT_H
#pragma once

#include <wg_capsule.h>
#include <wg_transitions.h>
#include <wg_filler.h>

namespace wg
{
	class ReorderCapsule;
	typedef	StrongPtr<ReorderCapsule>	ReorderCapsule_p;
	typedef	WeakPtr<ReorderCapsule>		ReorderCapsule_wp;

	//____ ReorderCapsule ______________________________________________________

	class ReorderCapsule : public Capsule
	{
	public:
		//____ Blueprint __________________________________________

		struct Blueprint
		{
			Object_p		baggage;
			Widget_p		child;
			bool			disabled		= false;
			bool			dropTarget		= true;
			Finalizer_p		finalizer		= nullptr;
			int				id				= 0;
			MarkPolicy		markPolicy		= MarkPolicy::AlphaTest;
			bool			pickable		= true;
			uint8_t			pickCategory	= 0;
			PointerStyle	pointer			= PointerStyle::Undefined;
			bool			selectable		= true;
			Skin_p			skin;
			bool			stickyFocus 	= false;
			bool			tabLock			= false;
			String			tooltip;
		};


		//.____ Creation __________________________________________

		static ReorderCapsule_p	create();
		static ReorderCapsule_p	create(const Blueprint& blueprint);

		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;



	protected:
		ReorderCapsule();
		void _init();

		template<class BP> ReorderCapsule( const BP& bp ) : Capsule(bp)
		{
			_init();

			if (bp.child)
				 slot.setWidget(bp.child);
		}

		virtual ~ReorderCapsule();

		void		_receive(Msg* pMsg) override;
		void		_update(int microPassed, int64_t microsecTimestamp) override;
		Container*	_ourContainer();
		void		_markPosition(int pos);
		void		_startTransition(int pos);
		void		_endTransition();


		uint8_t				m_dropCategory;


		CoordTransition_p	m_pTransition;
		bool				m_bTransitioning = false;
		int					m_transitionProgress;
		Size				m_transitionStartSize;

		int					m_prevPos = -1;
		int					m_hoveredPos = -1;
		int					m_dropPos = -1;
		int					m_markedPos = -1;

		Filler_p			m_pHoveredPosFiller;
		Filler_p			m_pPrevPosFiller;
		int					m_transitionDelay = 200000;


		Widget_p			m_pPicked;
		int					m_pickedPos;			// Offset in PackPanel
		float				m_pickedWeight;			// Weight of picked widget

		int					m_delayCountdown = 0;



		enum class TransitionState
		{
			Idle,
			Leaving,
			Entering,
			Reordering,
		};

		enum class PickState
		{
			Unpicked,
			Remaining,
			Leaving,
			Away,
			Returning,
			Canceled,
			Completed,
			Finishing
		};

		enum class DropState
		{
			Unprobed,
		};


		PickState			m_pickState = PickState::Unpicked;
		DropState			m_dropState = DropState::Unprobed;

	};


} // namespace wg
#endif //WG_REORDERCAPSULE_DOT_H

