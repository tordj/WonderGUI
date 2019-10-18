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
#include <wg_icolumnheader.h>
#include <wg_iselectablechildren.h>



namespace wg
{

	class PackList;
	typedef	StrongPtr<PackList>		PackList_p;
	typedef	WeakPtr<PackList>		PackList_wp;



	//____ PackListSlot ____________________________________________________________

	class PackListSlot : public ListSlot	/** @private */
	{
	public:
		PackListSlot() {}

		int				ofs;				// Offset in pixels for start of this list item.
		int				length;				// Length in pixels of this list item. Includes widget padding.
		int				prefBreadth;		// Prefereed breadth of this widget.
	};

	//____ PackListChildrenHolder() ___________________________________________

	class PackListChildrenHolder : public SelectableChildrenHolder /** @private */
	{
	public:
		virtual int		_getInsertionPoint(const Widget * pWidget) const = 0;
		virtual bool	_sortEntries() = 0;
		virtual bool	_hasSortFunction() const = 0;
	};


	class IPackListChildren;
	typedef	StrongInterfacePtr<IPackListChildren>	IPackListChildren_p;
	typedef	WeakInterfacePtr<IPackListChildren>		IPackListChildren_wp;

	//____ IPackListChildren ______________________________________________________

	class IPackListChildren : public ISelectableChildren<PackListSlot, PackListChildrenHolder>
	{
	public:

		/** @private */

		IPackListChildren(SlotArray<PackListSlot> * pSlotArray, PackListChildrenHolder * pHolder) : ISelectableChildren<PackListSlot, PackListChildrenHolder>(pSlotArray, pHolder) {}

		//.____ Misc __________________________________________________________

		inline IPackListChildren_p	ptr() { return IPackListChildren_p(this); }

		//.____ Content _______________________________________________________

		iterator	insertSorted(Widget * pWidget);

		//.____ Ordering ______________________________________________________

		void		sort();

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

	class PackList : public List, protected PackListChildrenHolder
	{
		friend class IPackListChildren;
		friend class ISelectableChildren<PackListSlot, PackList>;
		friend class IHideableChildren<PackListSlot,PackList>;
		friend class IDynamicChildren<PackListSlot,PackList>;
		friend class IChildrenSubclass<PackListSlot,PackList>;
//		template<class T, class P> friend class Children;
	public:

		//.____ Creation __________________________________________

		static PackList_p	create() { return PackList_p(new PackList()); }

		//.____ Interfaces _______________________________________

		IColumnHeader		header;
		IPackListChildren	children;

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
		void			_setSize( const SizeI& size ) override;
		void			_refresh() override;

		void			_receive( Msg * pMsg ) override;
		SizeI			_windowPadding() const override;


		// Overloaded from PackListChildrenHolder

		Slot *			_incSlot(Slot * pSlot) const override;
		Slot *			_decSlot(Slot * pSlot) const override;

		void			_didAddSlots(Slot * pSlot, int nb) override;
		void			_didMoveSlots(Slot * pFrom, Slot * pTo, int nb) override;
		void			_willRemoveSlots(Slot * pSlot, int nb) override;

		void			_hideSlots(Slot * pSlot, int nb) override;
		void			_unhideSlots(Slot * pSlot, int nb) override;

		void			_selectSlots(Slot * pSlot, int nb) override;
		void			_unselectSlots(Slot * pSlot, int nb) override;
		Object *		_object() override { return this; }
		const Object *	_object() const override { return this; }

		WidgetHolder *	_widgetHolder() override { return this; }
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


		// Overloaded from WidgetHolder

		CoordI		_childPos(Slot * pSlot) const override;
		SizeI		_childSize(Slot * pSlot) const override;

		void		_childRequestRender(Slot * pSlot) override;
		void		_childRequestRender(Slot * pSlot, const RectI& rect) override;
		void		_childRequestResize(Slot * pSlot) override;

		Widget *	_prevChild(const Slot * pSlot) const override;
		Widget *	_nextChild(const Slot * pSlot) const override;

		void		_releaseChild(Slot * pSlot) override;

		// Overloaded from ComponentHolder

		CoordI	_componentPos(const Component * pComponent) const override;
		SizeI	_componentSize(const Component * pComponent) const override;
		RectI	_componentGeo(const Component * pComponent) const override;

		void	_receiveComponentNotif(Component * pComponent, ComponentNotif notification, int value, void * pData) override;


		// Internal

		void			_requestRenderChildren(PackListSlot * pBegin, PackListSlot * pEnd);

		void			_updateChildOfsFrom( PackListSlot* pSlot );


		void			_getChildGeo( RectI& geo, const PackListSlot * pSlot ) const;
		int				_getEntryAt( int pixelofs ) const;
		RectI			_headerGeo() const;

		void			_refreshHeader();
		void			_refreshList();

		SizeI			_paddedLimitedPreferredSize( Slot * pSlot );
		int				_paddedLimitedMatchingHeight( Slot * pSlot, int paddedWidth );
		int				_paddedLimitedMatchingWidth( Slot * pSlot, int paddedHeight );

		void			_addToContentPreferredSize(int length, int breadth);
		void			_subFromContentPreferredSize(int length, int breadth);

		CColumnHeader		m_header;
		SlotArray<PackListSlot>	m_children;

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
