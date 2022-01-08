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
#ifndef WG_PACKLIST_DOT_H
#define WG_PACKLIST_DOT_H
#pragma once

#include <functional>

#include <wg_list.h>
#include <wg_ccolumnheader.h>
#include <wg_cdynamicslotvector.h>
#include <wg_slotextras.h>


namespace wg
{

	class PackList;
	typedef	StrongPtr<PackList>		PackList_p;
	typedef	WeakPtr<PackList>		PackList_wp;

	//____ PackList ____________________________________________________________

	/**
	 * @brief	List widget aligning its entries horizontally or vertically.
	 *
	 * PackList provides a standard list, where all contained children are
	 * aligned ("packed") either vertically or horizontally and can be selected
	 * using mouse or keyboard.
	 *
	 */

	class PackList : public List
	{
		friend class Slot;
		friend class CSlots;

	public:

		//____ Slot ____________________________________________________________

		class Slot : public List::Slot
		{
			friend class CSlots;
			friend class PackList;
			template<class S> friend class SlotVector;
			friend class CDynamicSlotVector<Slot>;

		public:

			//.____ Identification ________________________________________________

			const static TypeInfo	TYPEINFO;

			//.____ State __________________________________________________________

			SLOT_SELECTING_METHODS(PackList)

			//.____ Appearance _________________________________________________

			SLOT_HIDING_METHODS(PackList)

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder *pHolder) : List::Slot(pHolder) {}

			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;

			spx				m_ofs;				// Offset for start of this list item.
			spx				m_length;			// Length of this list item. Includes widget padding.
			spx				m_prefBreadth;		// Prefereed breadth of this widget.
		};

		using		iterator = SlotArrayIterator<Slot>;

		//____ CSlots ______________________________________________________

		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CSlots_p;
		typedef	WeakComponentPtr<CSlots>	CSlots_wp;

		class CSlots : public CDynamicSlotVector<Slot>,
			 public SelectableSlotCollectionMethods< PackList::Slot, iterator, PackList >
		{
			friend class PackList;
		public:

			//.____ Misc __________________________________________________________

			inline CSlots_p	ptr() { return CSlots_p(this); }

			//.____ Content _______________________________________________________

			iterator	insertSorted(Widget * pWidget);

			//.____ Ordering ______________________________________________________

			void		sort();

		protected:

			//____ Holder _________________________________________________________

			CSlots(SlotHolder * pHolder) : CDynamicSlotVector<Slot>(pHolder) {}


			inline const PackList *	_holder() const { return static_cast<const PackList*>(CDynamicSlotVector<Slot>::_holder()); }
			inline PackList *		_holder() { return static_cast<PackList*>(CDynamicSlotVector<Slot>::_holder()); }

			inline Slot* _slot(int index) override { return CDynamicSlotVector::_slot(index); }
			inline const Slot* _slot(int index) const { return CDynamicSlotVector::_slot(index); }

			inline int _size() const override {	return CDynamicSlotVector<Slot>::size(); }

		};

		friend class SelectableSlotCollectionMethods<PackList::Slot, iterator, PackList>;


		//.____ Creation __________________________________________

		static PackList_p	create() { return PackList_p(new PackList()); }

		//.____ Components _______________________________________

		CColumnHeader		header;
		CSlots	slots;

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;
	
		//.____ Geometry ____________________________________________

		void				setAxis( Axis axis );
		Axis				axis() const { return m_bHorizontal?Axis::X:Axis::Y; }

		bool				setMinEntrySize(Size min);
		bool				setMaxEntrySize(Size max);
		Size				minEntrySize() const { return m_minEntrySize; }
		Size				maxEntrySize() const { return m_maxEntrySize; }

		//.____ Behavior ________________________________________________________

		void				setSortOrder( SortOrder order );
		SortOrder			sortOrder() const { return m_sortOrder; }

		void				setSortFunction( std::function<int(const Widget *, const Widget *)> func );
		std::function<int(const Widget *, const Widget *)> sortFunction() const { return m_sortFunc; }

		//.____ Internal _________________________________________________________

		SizeSPX				_preferredSize(int scale = -1) const override;
		spx					_matchingHeight(spx width, int scale = -1) const override;
		spx					_matchingWidth(spx height, int scale = -1) const override;


	protected:
		PackList();
		virtual ~PackList();
		Widget*			_newOfMyType() const override { return new PackList(); }

		// Overloaded from Widget

		void			_collectPatches( Patches& container, const RectSPX& geo, const RectSPX& clip ) override;
		void			_maskPatches( Patches& patches, const RectSPX& geo, const RectSPX& clip, BlendMode blendMode ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectSPX& _canvas, const RectSPX& _window ) override;
		void			_resize( const SizeSPX& size, int scale = -1) override;
		void			_refresh() override;

		void			_receive( Msg * pMsg ) override;
		SizeSPX			_windowPadding() const override;

		// Overloaded from List

		List::Slot *	_findEntry(const CoordSPX& ofs) override;
		void			_getEntryGeo(RectSPX& geo, const List::Slot * pSlot) const override;

		RectSPX			_listArea() const override;
		RectSPX			_listWindow() const override;
		RectSPX			_listCanvas() const override;

		void			_onEntrySkinChanged(SizeSPX oldPadding, SizeSPX newPadding) override;
		void			_onLassoUpdated(const RectSPX& oldLasso, const RectSPX& newLasso) override;

		List::Slot *	_beginSlots() const override;
		List::Slot *	_endSlots() const override;


		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		void			_firstSlotWithGeo(SlotWithGeo& package) const override;
		void			_nextSlotWithGeo(SlotWithGeo& package) const override;

		Widget * 		_findWidget(const CoordSPX& ofs, SearchMode mode) override;

		CoordSPX		_childPos(const StaticSlot * pSlot) const override;

		void			_childRequestRender(StaticSlot * pSlot) override;
		void			_childRequestRender(StaticSlot * pSlot, const RectSPX& rect) override;
		void			_childRequestResize(StaticSlot * pSlot) override;

		Widget *		_prevChild(const StaticSlot * pSlot) const override;
		Widget *		_nextChild(const StaticSlot * pSlot) const override;

		void			_releaseChild(StaticSlot * pSlot) override;
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;

		void			_hideSlots(StaticSlot * pSlot, int nb);
		void			_unhideSlots(StaticSlot * pSlot, int nb);

		void			_selectSlots(StaticSlot * pSlot, int nb);
		void			_unselectSlots(StaticSlot * pSlot, int nb);


		// Overloaded from WidgetComponent::Holder

		CoordSPX		_componentPos(const WidgetComponent * pComponent) const override;
		SizeSPX			_componentSize(const WidgetComponent * pComponent) const override;
		RectSPX			_componentGeo(const WidgetComponent * pComponent) const override;

		void			_receiveComponentNotif(WidgetComponent * pComponent, ComponentNotif notification, int value, void * pData) override;


		// Internal

		int				_getInsertionPoint(const Widget * pWidget) const;
		bool			_hasSortFunction() const { return m_sortFunc != nullptr; }
		bool			_sortEntries();


		void			_requestRenderChildren(Slot * pBegin, Slot * pEnd);

		void			_updateChildOfsFrom( Slot* pSlot );


		void			_getChildGeo( RectSPX& geo, const Slot * pSlot ) const;
		int				_getEntryAt( spx ofs ) const;
		RectSPX			_headerGeo() const;

		void			_refreshHeader();
		void			_refreshList();

		SizeSPX			_paddedLimitedPreferredSize( StaticSlot * pSlot );
		spx				_paddedLimitedMatchingHeight( StaticSlot * pSlot, spx paddedWidth );
		spx				_paddedLimitedMatchingWidth( StaticSlot * pSlot, spx paddedHeight );

		void			_addToContentPreferredSize(spx length, spx breadth);
		void			_subFromContentPreferredSize(spx length, spx breadth);

		class ColumnHeaderAccess : public CColumnHeader { friend class PackList; };
		const ColumnHeaderAccess& _header() const { return static_cast<const ColumnHeaderAccess&>(header); }
		ColumnHeaderAccess& _header() { return static_cast<ColumnHeaderAccess&>(header); }


		bool				m_bHorizontal;

		SortOrder			m_sortOrder;
		std::function<int(const Widget *,const Widget *)> m_sortFunc;

		spx					m_contentBreadth;
		spx					m_contentLength;

		SizeSPX				m_entryPadding;
		SizeSPX				m_minEntrySizeSPX = { 0,0 };
		SizeSPX				m_maxEntrySizeSPX = { spx_max, spx_max };


		Size				m_minEntrySize = { 0,0 };
		Size				m_maxEntrySize = { 1000000,1000000 };

		//----

		spx				m_contentPreferredLength;
		spx				m_contentPreferredBreadth;
		int				m_nbPreferredBreadthEntries;			// Number of entries whose preferred breadth are the same as m_preferredSize.
	};



} // namespace wg
#endif //WG_PACKLIST_DOT_H
