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

#ifndef WG_LAMBDAPANEL_DOT_H
#define WG_LAMBDAPANEL_DOT_H
#pragma once


#include <wg_panel.h>
#include <wg_cdynamicslotvector.h>

namespace wg
{

	class LambdaPanel;
	typedef	StrongPtr<LambdaPanel>		LambdaPanel_p;
	typedef	WeakPtr<LambdaPanel>		LambdaPanel_wp;


	//____ LambdaPanel _________________________________________________________

	class LambdaPanel : public Panel
	{
		friend class Slot;
		friend class CSlots;

	public:

		//____ Slot ____________________________________________________________

		class Slot : public DynamicSlot
		{
			friend class LambdaPanel;
			friend class CSlots;
			friend class CDynamicSlotVector<Slot>;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Properties _________________________________________________

			inline void	setFunction(const std::function<Rect(Widget * pWidget, Size parentSize)>& func) { m_func = func; static_cast<LambdaPanel*>(_holder())->_updateSlotGeo(this, 1); }
			inline const std::function<Rect(Widget * pWidget, Size parentSize)>& function() const { return m_func; }

			//.____ Geometry _________________________________________________

			inline Coord	pos() const { return Util::qpixToMU(m_geo.pos()); }
			inline Size		size() const { return Util::qpixToMU(m_geo.size()); }
			inline Rect		geo() const { return Util::qpixToMU(m_geo); }

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder * pHolder) : DynamicSlot(pHolder) {}
			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;

			const static bool safe_to_relocate = false;


			std::function<Rect(Widget * pWidget, Size parentSize)>	m_func = nullptr;
			bool			m_bVisible = false;
			RectI			m_geo;				// Widgets geo relative parent
		};


		class CSlots;
		typedef	StrongComponentPtr<CSlots>		CSlots_p;
		typedef	WeakComponentPtr<CSlots>		CSlots_wp;

		//____ CSlots ________________________________________________________

		class CSlots : public CDynamicSlotVector<Slot>
		{
			friend class LambdaPanel;
		public:


			//.____ Content _______________________________________________________

			iterator	pushFront(const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parenSize)> func);
			iterator	pushBack(const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parenSize)> func);

			iterator	insert(int index, const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func);
			iterator	insert(iterator pos, const Widget_p& pWidget, std::function<Rect(Widget * pWidget, Size parentSize)> func);

			//.____ Misc __________________________________________________________

			inline CSlots_p	ptr() { return CSlots_p(this); }

		protected:

			CSlots(SlotHolder * pHolder) : CDynamicSlotVector<Slot>(pHolder) {}

			LambdaPanel *	_holder() { return static_cast<LambdaPanel*>(CDynamicSlotVector<Slot>::_holder()); }
			const LambdaPanel *	_holder() const { return static_cast<const LambdaPanel*>(CDynamicSlotVector<Slot>::_holder()); }
		};


		//.____ Creation __________________________________________

		static LambdaPanel_p	create() { return LambdaPanel_p(new LambdaPanel()); }

		//.____ Components _______________________________________

		CSlots	slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		bool				setMinSize(Size min);
		bool				setMaxSize(Size max);
		bool				setSizeLimits( Size min, Size max );
		bool				setPreferredSize(Size pref);

	protected:
		LambdaPanel();
		virtual ~LambdaPanel();
		virtual Widget* _newOfMyType() const override { return new LambdaPanel(); };

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		CoordI		_childPos(const StaticSlot * pSlot) const override;

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		void		_didAddSlots(StaticSlot * pSlot, int nb) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void		_hideSlots(StaticSlot * pSlot, int nb) override;
		void		_unhideSlots(StaticSlot * pSlot, int nb) override;

	private:
		void		_updateSlotGeo(StaticSlot * pSlot, int nb);

		SizeI		_preferredSize() const override;

		void		_cloneContent( const Widget * _pOrg ) override;
		void		_resize( const SizeI& size ) override;
		void		_updateGeo(Slot * pSlot, bool bForceResize = false);

		void		_onRequestRender( const RectI& rect, const Slot * pSlot );

		SizeI		m_minSize;
		SizeI		m_maxSize;
		SizeI		m_preferredSize;
	};



} // namespace wg
#endif //WG_LAMBDAPANEL_DOT_H
