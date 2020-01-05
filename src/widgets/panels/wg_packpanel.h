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
#ifndef WG_PACKPANEL_DOT_H
#define WG_PACKPANEL_DOT_H
#pragma once

#include <wg_sizebroker.h>
#include <wg_panel.h>
#include <wg_paddedslot.h>
#include <wg_cpaddedslotarray.h>

namespace wg
{

	class PackPanel;
	typedef	StrongPtr<PackPanel>		PackPanel_p;
	typedef	WeakPtr<PackPanel>			PackPanel_wp;


	//____ PackSlot ____________________________________________________________

	class PackSlot : public PaddedSlot
	{
		friend class PackPanel;
		friend class CPackSlotArray;
		template<class S> friend class SlotArray;
		friend class CPaddedSlotArray<PackSlot>::Holder;
		friend class CDynamicSlotArray<PackSlot>::Holder;
		friend class CDynamicSlotArray<PackSlot>;

	public:


		//.____ Properties _________________________________________________

		inline float	weight() const { return m_weight; }
		inline void		setWeight(float weight) { if (weight != m_weight) static_cast<Holder*>(_holder())->_reweightSlots(this, 1, weight); }

		//.____ Geometry _________________________________________________

		inline Coord	pos() const { return Util::rawToQpix(m_geo.pos()); }
		inline Size		size() const { return Util::rawToQpix(m_geo.size()); }
		inline Rect		geo() const { return Util::rawToQpix(m_geo); }

	protected:

		//____ Holder ___________________________________________________________

		class Holder : public PaddedSlot::Holder
		{
		public:
			virtual void	_reweightSlots(PackSlot * pSlot, int nb, float weight) = 0;
		};

		PackSlot(Holder *pHolder) : PaddedSlot(pHolder) {}

		bool			m_bResizeRequired = false;
		float			m_weight = 1.f;				// Weight for space allocation.
		RectI			m_geo;						// Real geo of child (no padding included).
		SizeI			m_preferredSize;				// Cached padded preferred size from the child.
	};


	//____ CPackSlotArray ________________________________________________________

	class CPackSlotArray;
	typedef	StrongComponentPtr<CPackSlotArray>	CPackSlotArray_p;
	typedef	WeakComponentPtr<CPackSlotArray>	CPackSlotArray_wp;

	class CPackSlotArray : public CPaddedSlotArray<PackSlot>
	{
		friend class PackPanel;
	public:

		//____ Holder _________________________________________________________

		class Holder : public CPaddedSlotArray<PackSlot>::Holder 	/** @private */
		{
		public:
			virtual void	_reweightSlots(PackSlot * pSlot, int nb, float weight) = 0;
			virtual void	_reweightSlots(PackSlot * pSlot, int nb, const float * pWeights) = 0;
			virtual void	_refreshChildGeo() = 0;
		};

		//.____ Geometry _______________________________________________________

		bool		setWeight(int index, int amount, float weight);
		bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, float weight);
		bool		setWeight(int index, int amount, const std::initializer_list<float> weights);
		bool		setWeight(const SlotIterator&  beg, const SlotIterator&  end, const std::initializer_list<float> weights);

		//.____ Misc __________________________________________________________

		inline CPackSlotArray_p	ptr() { return CPackSlotArray_p(this); }

	protected:
		CPackSlotArray(Holder * pHolder) : CPaddedSlotArray<PackSlot>(pHolder) {}

		Holder *		_holder() { return static_cast<Holder*>(CPaddedSlotArray<PackSlot>::_holder()); }
		const Holder *	_holder() const { return static_cast<const Holder*>(CPaddedSlotArray<PackSlot>::_holder()); }
	};

	//____ PackPanel ________________________________________________________________
	/**
	 * @brief	A widget for arranging children horizontally or vertically.
	 *
	 * A widget for arranging children horizontally or vertically.
	 */

	class PackPanel : public Panel, protected CPackSlotArray::Holder
	{
		friend class CPackSlotArray;

	public:

		//.____ Creation __________________________________________

		static PackPanel_p	create() { return PackPanel_p(new PackPanel()); }

		//.____ Components _______________________________________

		CPackSlotArray		slots;

		//.____ Identification __________________________________________

		bool		isInstanceOf( const char * pClassName ) const override;
		const char *className( void ) const override;
		static const char	CLASSNAME[];
		static PackPanel_p	cast( Object * pObject );

		//.____ Geometry ____________________________________________

		void			setOrientation( Orientation orientaiton );
		Orientation		orientation() const { return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical; }

		//.____ Behavior ________________________________________________________

		void			setSizeBroker( SizeBroker * pBroker );
		SizeBroker_p	sizeBroker() const { return m_pSizeBroker; }


	protected:
		PackPanel();
		virtual ~PackPanel();
		virtual Widget* _newOfMyType() const  override { return new PackPanel(); };

		// Overloaded from Widget

		void			_resize( const SizeI& size ) override;

		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		SizeI			_preferredSize() const override;


		// Overloaded from Container

		Widget *	_firstChild() const override;
		Widget *	_lastChild() const override;

		void		_firstSlotWithGeo( SlotWithGeo& package ) const override;
		void		_nextSlotWithGeo( SlotWithGeo& package ) const override;


		// Overloaded from CPaddedSlotArray::Holder

		void		_didAddSlots( StaticSlot * pSlot, int nb ) override;
		void		_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void		_willEraseSlots( StaticSlot * pSlot, int nb ) override;
		void		_hideSlots( StaticSlot *, int nb ) override;
		void		_unhideSlots( StaticSlot *, int nb ) override;
		void		_repadSlots( StaticSlot *, int nb, BorderI padding ) override;
		void		_repadSlots(StaticSlot *, int nb, const BorderI * pPaddings) override;

		// Overloaded from CPackSlotArray::Holder

		void		_reweightSlots(PackSlot * pSlot, int nb, float weight) override;
		void		_reweightSlots(PackSlot * pSlot, int nb, const float * pWeights) override;
		void		_refreshChildGeo() override { _refreshChildGeo(true); }

		// Overloaded from PackSlotHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }
		Object *	_object() override { return this; }
		const Object *	_object() const override { return this; }

		CoordI		_childPos(const StaticSlot * pSlot) const override;
		CoordI		_childGlobalPos(const StaticSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(const StaticSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection(const StaticSlot * pSlot) const override { return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(StaticSlot * pSlot) override { return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }

		void		_childRequestRender( StaticSlot * pSlot ) override;
		void		_childRequestRender( StaticSlot * pSlot, const RectI& rect ) override;
		void		_childRequestResize( StaticSlot * pSlot ) override;

		Widget *	_prevChild( const StaticSlot * pSlot ) const override;
		Widget *	_nextChild( const StaticSlot * pSlot ) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewWidget ) override;

		//

		void		_refreshChildGeo(bool bRequestRender);

		void		_hideChildren(PackSlot * pSlot, int nb);
		void		_unhideChildren(PackSlot * pSlot, int nb);

		void		_refreshGeometries();
		SizeI		_calcPreferredSize();
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray ) const;
		int			_populateSizeBrokerArray( SizeBrokerItem * pArray, int forcedBreadth ) const;

		int			_setItemLengths(SizeBrokerItem * pItems, int nItems, int totalLength) const;
		int			_setPreferredLengths(SizeBrokerItem * pItems, int nItems) const;

		bool			m_bHorizontal;
		SizeBroker_p	m_pSizeBroker;
		SizeI			m_preferredContentSize;

	};



} // namespace wg
#endif //WG_PACKPANEL_DOT_H
