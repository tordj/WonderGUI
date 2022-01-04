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

#ifndef WG_SLOTEXTRAS_DOT_H
#define WG_SLOTEXTRAS_DOT_H
#pragma once

#include <wg_slotiterator.h>
#include <wg_geo.h>


#define		SLOT_PADDING_METHODS	inline void		setPadding(Border padding) { _holder()->_repadSlots(this, 1, padding); } \
									inline Border	padding() const { return m_padding; }

#define		SLOT_HIDING_METHODS		inline void		hide() { _holder()->_hideSlots(this, 1); } \
									inline void		unhide() { _holder()->_unhideSlots(this, 1); } \
									inline void		setVisible(bool bVisible) { if (bVisible) _holder()->_unhideSlots(this, 1); else _holder()->_unhideSlots(this, 1); } \
									inline bool		isVisible() const { return m_bVisible; }

#define		SLOT_PADDING_HELPER_METHODS	SizeSPX		_paddedPreferredSize(int scale) const { return m_pWidget->_preferredSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); } \
										SizeSPX		_paddedMinSize(int scale) const { return m_pWidget->_minSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); } \
										SizeSPX		_paddedMaxSize(int scale) const { return m_pWidget->_maxSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); } \
										spx			_paddedMatchingWidth(spx paddedHeight, int scale) const \
										{ \
											SizeSPX paddingSize = Util::align(Util::ptsToSpx(m_padding, scale)); \
											return m_pWidget->_matchingWidth(paddedHeight - paddingSize.h) + paddingSize.w; \
										} \
										spx			_paddedMatchingHeight(spx paddedWidth, int scale) const \
										{ \
											SizeSPX paddingSize = Util::align(Util::ptsToSpx(m_padding, scale)); \
											return m_pWidget->_matchingHeight(paddedWidth - paddingSize.w) + paddingSize.h; \
										} 


namespace wg
{
	template<class SlotType, class IteratorType, class HolderType> class PaddedSlotCollectionMethods
	{
	public:

		bool		setPadding(int index, int amount, Border padding);
		bool		setPadding(const IteratorType& beg, const IteratorType& end, Border padding);
		bool		setPadding(int index, int amount, std::initializer_list<Border> padding);
		bool		setPadding(const IteratorType& beg, const IteratorType& end, std::initializer_list<Border> padding);

	protected:

		virtual HolderType* _holder() = 0;

		virtual SlotType* _slot(int index) = 0;

//		virtual SlotType* _begin() const = 0;
//		virtual SlotType* _end() const = 0;
	};

	template<class SlotType, class IteratorType, class HolderType> class SelectableSlotCollectionMethods
	{
	public:

		void	select(int index, int amount);
		void	select(const   IteratorType& beg, const   IteratorType& end);

		void	unselect(int index, int amount);
		void	unselect(const   IteratorType& beg, const   IteratorType& end);

		void	selectAll();
		void	unselectAll();
	protected:

		virtual HolderType* _holder() = 0;

		virtual SlotType*	_slot(int index) = 0;
		virtual int			_size() const = 0;

//		virtual SlotType* _begin() const = 0;
//		virtual SlotType* _end() const = 0;
	};


	template<class SlotType, class IteratorType, class HolderType> class HideableSlotCollectionMethods
	{
	public:

		void			hide(int index, int amount);
		void			hide(const IteratorType& beg, const IteratorType& end);

		void			unhide(int index, int amount);
		void			unhide(const IteratorType& beg, const IteratorType& end);

		void			hideAll();

		void			unhideAll();

	protected:

		virtual HolderType* _holder() = 0;

		virtual SlotType* _slot(int index) = 0;
		virtual int			_size() const = 0;

		//		virtual SlotType* _begin() const = 0;
		//		virtual SlotType* _end() const = 0;
	};

}



#endif //WG_SLOTEXTRAS_DOT_H
