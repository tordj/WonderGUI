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
#include <wg_cselectableslotvector.h>



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

			//.____ Operators __________________________________________

			inline void operator=(Widget * pWidget) { setWidget(pWidget); }

		protected:

			Slot(SlotHolder *pHolder) : List::Slot(pHolder) {}

			Slot(Slot&& o) = default;
			Slot& operator=(Slot&& o) = default;

			MU				m_ofs;				// Offset for start of this list item.
			MU				m_length;			// Length of this list item. Includes widget padding.
			MU				m_prefBreadth;		// Prefereed breadth of this widget.
		};


		//____ CSlots ______________________________________________________

		class CSlots;
		typedef	StrongComponentPtr<CSlots>	CSlots_p;
		typedef	WeakComponentPtr<CSlots>	CSlots_wp;

		class CSlots : public CSelectableSlotVector<Slot>
		{
			friend class PackList;
		public:

			using		iterator = SlotArrayIterator<Slot>;

			//.____ Misc __________________________________________________________

			inline CSlots_p	ptr() { return CSlots_p(this); }

			//.____ Content _______________________________________________________

			iterator	insertSorted(Widget * pWidget);

			//.____ Ordering ______________________________________________________

			void		sort();

		protected:

			//____ Holder _________________________________________________________

			CSlots(SlotHolder * pHolder) : CSelectableSlotVector<Slot>(pHolder) {}


			inline const PackList *	_holder() const { return static_cast<const PackList*>(CSelectableSlotVector<Slot>::_holder()); }
			inline PackList *		_holder() { return static_cast<PackList*>(CSelectableSlotVector<Slot>::_holder()); }
		};


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

		Size				preferredSize() const override;
		MU					matchingHeight(MU width) const override;
		MU					matchingWidth(MU height) const override;


		bool				setMinEntrySize(Size min);
		bool				setMaxEntrySize(Size max);
		Size				minEntrySize() const { return m_minEntrySize; }
		Size				maxEntrySize() const { return m_maxEntrySize; }

		//.____ Behavior ________________________________________________________

		void				setSortOrder( SortOrder order );
		SortOrder			sortOrder() const { return m_sortOrder; }

		void				setSortFunction( std::function<int(const Widget *, const Widget *)> func );
		std::function<int(const Widget *, const Widget *)> sortFunction() const { return m_sortFunc; }


	protected:
		PackList();
		virtual ~PackList();
		Widget*			_newOfMyType() const override { return new PackList(); }

		// Overloaded from Widget

		void			_collectPatches( Patches& container, const Rect& geo, const Rect& clip ) override;
		void			_maskPatches( Patches& patches, const Rect& geo, const Rect& clip, BlendMode blendMode ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const Rect& _canvas, const Rect& _window ) override;
		void			_resize( const Size& size ) override;
		void			_refresh() override;

		void			_receive( Msg * pMsg ) override;
		Size			_windowPadding() const override;

		// Overloaded from List

		List::Slot *	_findEntry(const Coord& ofs) override;
		void			_getEntryGeo(Rect& geo, const List::Slot * pSlot) const override;

		Rect			_listArea() const override;
		Rect			_listWindow() const override;
		Rect			_listCanvas() const override;

		void			_onEntrySkinChanged(Size oldPadding, Size newPadding) override;
		void			_onLassoUpdated(const Rect& oldLasso, const Rect& newLasso) override;

		List::Slot *	_beginSlots() const override;
		List::Slot *	_endSlots() const override;


		// Overloaded from Container

		const TypeInfo&	_slotTypeInfo(const StaticSlot * pSlot) const override;

		void			_firstSlotWithGeo(SlotWithGeo& package) const override;
		void			_nextSlotWithGeo(SlotWithGeo& package) const override;

		Widget * 		_findWidget(const Coord& ofs, SearchMode mode) override;

		Coord			_childPos(const StaticSlot * pSlot) const override;

		void			_childRequestRender(StaticSlot * pSlot) override;
		void			_childRequestRender(StaticSlot * pSlot, const Rect& rect) override;
		void			_childRequestResize(StaticSlot * pSlot) override;

		Widget *		_prevChild(const StaticSlot * pSlot) const override;
		Widget *		_nextChild(const StaticSlot * pSlot) const override;

		void			_releaseChild(StaticSlot * pSlot) override;
		void			_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;

		void			_hideSlots(StaticSlot * pSlot, int nb) override;
		void			_unhideSlots(StaticSlot * pSlot, int nb) override;

		void			_selectSlots(StaticSlot * pSlot, int nb) override;
		void			_unselectSlots(StaticSlot * pSlot, int nb) override;


		// Overloaded from GeoComponent::Holder

		Coord	_componentPos(const GeoComponent * pComponent) const override;
		Size	_componentSize(const GeoComponent * pComponent) const override;
		Rect	_componentGeo(const GeoComponent * pComponent) const override;

		void	_receiveComponentNotif(GeoComponent * pComponent, ComponentNotif notification, int value, void * pData) override;


		// Internal

		int				_getInsertionPoint(const Widget * pWidget) const;
		bool			_hasSortFunction() const { return m_sortFunc != nullptr; }
		bool			_sortEntries();


		void			_requestRenderChildren(Slot * pBegin, Slot * pEnd);

		void			_updateChildOfsFrom( Slot* pSlot );


		void			_getChildGeo( Rect& geo, const Slot * pSlot ) const;
		int				_getEntryAt( MU ofs ) const;
		Rect			_headerGeo() const;

		void			_refreshHeader();
		void			_refreshList();

		Size			_paddedLimitedPreferredSize( StaticSlot * pSlot );
		MU				_paddedLimitedMatchingHeight( StaticSlot * pSlot, MU paddedWidth );
		MU				_paddedLimitedMatchingWidth( StaticSlot * pSlot, MU paddedHeight );

		void			_addToContentPreferredSize(MU length, MU breadth);
		void			_subFromContentPreferredSize(MU length, MU breadth);

		class ColumnHeaderAccess : public CColumnHeader { friend class PackList; };
		const ColumnHeaderAccess& _header() const { return static_cast<const ColumnHeaderAccess&>(header); }
		ColumnHeaderAccess& _header() { return static_cast<ColumnHeaderAccess&>(header); }


		bool				m_bHorizontal;

		SortOrder			m_sortOrder;
		std::function<int(const Widget *,const Widget *)> m_sortFunc;

		MU					m_contentBreadth;
		MU					m_contentLength;

		Size				m_entryPadding;
		Size				m_minEntrySize;
		Size				m_maxEntrySize;

		//----

		MU				m_contentPreferredLength;
		MU				m_contentPreferredBreadth;
		int				m_nbPreferredBreadthEntries;			// Number of entries whose preferred breadth are the same as m_preferredSize.
	};



} // namespace wg
#endif //WG_PACKLIST_DOT_H
