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
#include <wg_dynamicslotvector.h>
#include <wg_slotextras.h>

namespace wg
{

	class LambdaPanel;
	typedef	StrongPtr<LambdaPanel>		LambdaPanel_p;
	typedef	WeakPtr<LambdaPanel>		LambdaPanel_wp;


	//____ LambdaPanel _________________________________________________________

	class LambdaPanel : public Panel
	{

	public:

		
		//____ Slot ____________________________________________________________

		class Slot : public DynamicSlot
		{
			friend class LambdaPanel;
			friend class DynamicSlotVector<Slot>;

		public:

			//.____ Blueprint _______________________________________________________

			struct Blueprint
			{
				std::function<Rect(Widget * pWidget, Size parentSize)> func;
			};
			
			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ Properties _________________________________________________

			inline void	setFunction(const std::function<Rect(Widget * pWidget, Size parentSize)>& func) { m_func = func; static_cast<LambdaPanel*>(_holder())->_updateSlotGeo(this, 1); }
			inline const std::function<Rect(Widget * pWidget, Size parentSize)>& function() const { return m_func; }

			//.____ Geometry _________________________________________________

			inline Coord	pos() const { return Util::spxToPts(m_geo.pos(), _holder()->_scale()); }
			inline Size		size() const { return Util::spxToPts(m_geo.size(), _holder()->_scale()); }
			inline Rect		geo() const { return Util::spxToPts(m_geo, _holder()->_scale()); }

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder * pHolder) : DynamicSlot(pHolder) {}
//			Slot(Slot&& o) noexcept = default;
//			Slot& operator=(Slot&& o) = default;

			const static bool safe_to_relocate = false;

			bool _setBlueprint( const Blueprint& bp );

			std::function<Rect(Widget * pWidget, Size parentSize)>	m_func = nullptr;
			bool			m_bVisible = false;
			RectSPX			m_geo;				// Widgets geo relative parent
		};


		using		iterator = DynamicSlotVector<Slot>::iterator;


		friend class Slot;
		
		
		//.____ Creation __________________________________________

		static LambdaPanel_p	create() { return LambdaPanel_p(new LambdaPanel()); }

		//.____ Components _______________________________________

		DynamicSlotVector<Slot>	slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry ____________________________________________

		bool				setMinSize(Size minSize);
		bool				setMaxSize(Size maxSize);
		bool				setSizeLimits( Size minSize, Size maxSize );
		bool				setDefaultSize(Size defaultSize);

		//.____ Misc ___________________________________________________________
		
		void			hideSlots(int index, int amount);
		void			hideSlots(iterator beg, iterator end);

		void			unhideSlots(int index, int amount);
		void			unhideSlots(iterator beg, iterator end);
		
	protected:
		LambdaPanel();
		virtual ~LambdaPanel();

		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		SizeSPX		_defaultSize(int scale) const override;
		SizeSPX		_minSize(int scale) const override;
		SizeSPX		_maxSize(int scale) const override;

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;

		CoordSPX	_childPos(const StaticSlot * pSlot) const override;

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectSPX& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild) override;

		void		_didAddSlots(StaticSlot * pSlot, int nb) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots(StaticSlot * pSlot, int nb) override;
		void		_hideSlots(StaticSlot * pSlot, int nb);
		void		_unhideSlots(StaticSlot * pSlot, int nb);

	private:
		void		_updateSlotGeo(StaticSlot * pSlot, int nb);

		void		_resize( const SizeSPX& size, int scale ) override;
		void		_updateGeo(Slot * pSlot, bool bForceResize = false);

		void		_onRequestRender( const RectSPX& rect, const Slot * pSlot );

		Size		m_minSize;
		Size		m_maxSize;
		Size		m_defaultSize;
	};



} // namespace wg
#endif //WG_LAMBDAPANEL_DOT_H
