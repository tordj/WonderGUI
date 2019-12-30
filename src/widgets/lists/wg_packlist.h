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
#include <wg_cselectableslotarray.h>



namespace wg
{

	class PackList;
	typedef	StrongPtr<PackList>		PackList_p;
	typedef	WeakPtr<PackList>		PackList_wp;


	//____ PackListSlot ____________________________________________________________

	class PackListSlot : public ListSlot	/** @private */
	{
		friend class CPackListSlotArray;
		friend class PackList;
		template<class S> friend class SlotArray;

	public:
		class Holder : public ListSlot::Holder
		{
		};

		PackListSlot(Holder *pHolder) : ListSlot(pHolder) {}

	protected:
		int				m_ofs;				// Offset in pixels for start of this list item.
		int				m_length;				// Length in pixels of this list item. Includes widget padding.
		int				m_prefBreadth;		// Prefereed breadth of this widget.
	};


	//____ CPackListSlotArray ______________________________________________________

	class CPackListSlotArray;
	typedef	StrongComponentPtr<CPackListSlotArray>	CPackListSlotArray_p;
	typedef	WeakComponentPtr<CPackListSlotArray>	CPackListSlotArray_wp;

	class CPackListSlotArray : public CSelectableSlotArray<PackListSlot>
	{
		friend class PackList;
	public:

		//____ Holder _________________________________________________________

		class Holder : public CSelectableSlotArray<PackListSlot>::Holder /** @private */
		{
		public:
			virtual int		_getInsertionPoint(const Widget * pWidget) const = 0;
			virtual bool	_sortEntries() = 0;
			virtual bool	_hasSortFunction() const = 0;
		};


		using		iterator = SlotArrayIterator<PackListSlot>;

		/** @private */

		CPackListSlotArray(Holder * pHolder) : CSelectableSlotArray<PackListSlot>(pHolder) {}

		//.____ Misc __________________________________________________________

		inline CPackListSlotArray_p	ptr() { return CPackListSlotArray_p(this); }

		//.____ Content _______________________________________________________

		iterator	insertSorted(Widget * pWidget);

		//.____ Ordering ______________________________________________________

		void		sort();
	protected:

		inline const Holder *	_holder() const { return static_cast<const Holder*>(CSelectableSlotArray<PackListSlot>::_holder()); }
		inline Holder *			_holder() { return static_cast<Holder*>(CSelectableSlotArray<PackListSlot>::_holder()); }
	};


	//____ PackList ____________________________________________________________

	/**
	 * @brief	List widget aligning its entries horizontally or vertically.
	 *
	 * PackList provides a standard list, where all contained children are
	 * aligned ("packed") either vertically or horizontally and can be selected
	 * using mouse or keyboard.
	 *
	 */

	class PackList : public List, protected CPackListSlotArray::Holder
	{
		friend class CPackListSlotArray;

	public:

		//.____ Creation __________________________________________

		static PackList_p	create() { return PackList_p(new PackList()); }

		//.____ Components _______________________________________

		CColumnHeader		header;
		CPackListSlotArray	slots;

		//.____ Identification __________________________________________

		virtual bool		isInstanceOf( const char * pClassName ) const override;
		virtual const char *className( void ) const override;
		static const char	CLASSNAME[];
		static PackList_p	cast( Object * pObject );

		//.____ Geometry ____________________________________________

		void				setOrientation( Orientation orientation );
		Orientation			orientation() const { return m_bHorizontal?Orientation::Horizontal:Orientation::Vertical; }

		bool				setMinEntrySize(Size min);
		bool				setMaxEntrySize(Size max);
		Size				minEntrySize() const { return Util::rawToQpix(m_minEntrySize); }
		Size				maxEntrySize() const { return Util::rawToQpix(m_maxEntrySize); }

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

		SizeI			_preferredSize() const override;
		int				_matchingHeight(int width) const override;
		int				_matchingWidth(int height) const override;

		void			_collectPatches( Patches& container, const RectI& geo, const RectI& clip ) override;
		void			_maskPatches( Patches& patches, const RectI& geo, const RectI& clip, BlendMode blendMode ) override;
		void			_cloneContent( const Widget * _pOrg ) override;
		void			_render( GfxDevice * pDevice, const RectI& _canvas, const RectI& _window ) override;
		void			_resize( const SizeI& size ) override;
		void			_refresh() override;

		void			_receive( Msg * pMsg ) override;
		SizeI			_windowPadding() const override;


		// Overloaded from PackListCSlotArray::Holder

		void			_didAddSlots(StaticSlot * pSlot, int nb) override;
		void			_didMoveSlots(StaticSlot * pFrom, StaticSlot * pTo, int nb) override;
		void			_willEraseSlots(StaticSlot * pSlot, int nb) override;

		void			_hideSlots(StaticSlot * pSlot, int nb) override;
		void			_unhideSlots(StaticSlot * pSlot, int nb) override;

		void			_selectSlots(StaticSlot * pSlot, int nb) override;
		void			_unselectSlots(StaticSlot * pSlot, int nb) override;
		Object *		_object() override { return this; }
		const Object *	_object() const override { return this; }

		int				_getInsertionPoint(const Widget * pWidget) const override;
		bool			_hasSortFunction() const override { return m_sortFunc != nullptr; }
		bool			_sortEntries() override;


		// Overloaded from List

		ListSlot *		_findEntry(const CoordI& ofs) override;
		void			_getEntryGeo(RectI& geo, const ListSlot * pSlot) const override;

		RectI			_listArea() const override;
		RectI			_listWindow() const override;
		RectI			_listCanvas() const override;

		void			_onEntrySkinChanged(SizeI oldPadding, SizeI newPadding) override;
		void			_onLassoUpdated(const RectI& oldLasso, const RectI& newLasso) override;

		ListSlot *		_beginSlots() const override;
		ListSlot *		_endSlots() const override;


		// Overloaded from Container

		void			_firstSlotWithGeo(SlotWithGeo& package) const override;
		void			_nextSlotWithGeo(SlotWithGeo& package) const override;

		Widget * 		_findWidget(const CoordI& ofs, SearchMode mode) override;


		// Overloaded from PackListSlotHolder

		Container *	_container() override { return this; }
		RootPanel *	_root() override { return Container::_root(); }

		CoordI		_childPos(const StaticSlot * pSlot) const override;
		CoordI		_childGlobalPos(const StaticSlot * pSlot) const override { return Container::_childGlobalPos(pSlot); }
		bool		_isChildVisible(const StaticSlot * pSlot) const override { return Container::_isChildVisible(pSlot); }
		RectI		_childWindowSection(const StaticSlot * pSlot) const override { return Container::_childWindowSection(pSlot); }

		bool		_childRequestFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childRequestFocus(pSlot, pWidget); }
		bool		_childReleaseFocus(StaticSlot * pSlot, Widget * pWidget) override { return Container::_childReleaseFocus(pSlot, pWidget); }

		void		_childRequestInView(StaticSlot * pSlot) override { return Container::_childRequestInView(pSlot); }
		void		_childRequestInView(StaticSlot * pSlot, const RectI& mustHaveArea, const RectI& niceToHaveArea) override { return Container::_childRequestInView(pSlot, mustHaveArea, niceToHaveArea); }


		void		_childRequestRender(StaticSlot * pSlot) override;
		void		_childRequestRender(StaticSlot * pSlot, const RectI& rect) override;
		void		_childRequestResize(StaticSlot * pSlot) override;

		Widget *	_prevChild(const StaticSlot * pSlot) const override;
		Widget *	_nextChild(const StaticSlot * pSlot) const override;

		void		_releaseChild(StaticSlot * pSlot) override;
		void		_replaceChild(StaticSlot * pSlot, Widget * pNewChild ) override;

		// Overloaded from GeoComponent::Holder

		CoordI	_componentPos(const GeoComponent * pComponent) const override;
		SizeI	_componentSize(const GeoComponent * pComponent) const override;
		RectI	_componentGeo(const GeoComponent * pComponent) const override;

		void	_receiveComponentNotif(GeoComponent * pComponent, ComponentNotif notification, int value, void * pData) override;


		// Internal

		void			_requestRenderChildren(PackListSlot * pBegin, PackListSlot * pEnd);

		void			_updateChildOfsFrom( PackListSlot* pSlot );


		void			_getChildGeo( RectI& geo, const PackListSlot * pSlot ) const;
		int				_getEntryAt( int pixelofs ) const;
		RectI			_headerGeo() const;

		void			_refreshHeader();
		void			_refreshList();

		SizeI			_paddedLimitedPreferredSize( StaticSlot * pSlot );
		int				_paddedLimitedMatchingHeight( StaticSlot * pSlot, int paddedWidth );
		int				_paddedLimitedMatchingWidth( StaticSlot * pSlot, int paddedHeight );

		void			_addToContentPreferredSize(int length, int breadth);
		void			_subFromContentPreferredSize(int length, int breadth);

		class ColumnHeaderAccess : public CColumnHeader { friend class PackList; };
		const ColumnHeaderAccess& _header() const { return static_cast<const ColumnHeaderAccess&>(header); }
		ColumnHeaderAccess& _header() { return static_cast<ColumnHeaderAccess&>(header); }


		bool				m_bHorizontal;

		SortOrder			m_sortOrder;
		std::function<int(const Widget *,const Widget *)> m_sortFunc;

		int					m_contentBreadth;
		int					m_contentLength;

		SizeI				m_entryPadding;
		SizeI				m_minEntrySize;
		SizeI				m_maxEntrySize;

		//----

		int				m_contentPreferredLength;
		int				m_contentPreferredBreadth;
		int				m_nbPreferredBreadthEntries;			// Number of entries whose preferred breadth are the same as m_preferredSize.
	};



} // namespace wg
#endif //WG_PACKLIST_DOT_H
