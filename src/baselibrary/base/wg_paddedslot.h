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

#ifndef WG_PADDEDSLOT_DOT_H
#define WG_PADDEDSLOT_DOT_H
#pragma once

#include <wg_slot.h>

namespace wg
{

	//____ PaddedSlot ____________________________________________________________

	class PaddedSlot : public DynamicSlot
	{
	public:

//		void		setPadding(const BorderI& padding);
//		inline const BorderI& padding() const { return m_padding; }

		/** @private */
		PaddedSlot(SlotHolder* pHolder) : DynamicSlot(pHolder), m_bVisible(false) {} 

		//.____ Identification ________________________________________________

		const static TypeInfo	TYPEINFO;

		//.____ Appearance ____________________________________________________

		inline void		hide() { _holder()->_hideSlots(this, 1); }
		inline void		unhide() { _holder()->_unhideSlots(this, 1); }
		inline void		setVisible(bool bVisible) { if (bVisible) _holder()->_unhideSlots(this, 1); else _holder()->_unhideSlots(this, 1); }
		inline bool		isVisible() const { return m_bVisible; }

		//.____ Geometry ______________________________________________________

		void			setPadding( Border padding) { static_cast<SlotHolder*>(_holder())->_repadSlots(this, 1, padding); }
		inline Border	padding() const { return m_padding; }

		//.____ Operators __________________________________________

		inline void operator=(Widget * pWidget) { setWidget(pWidget); }

	protected:
		PaddedSlot(PaddedSlot&& o) = default;
		PaddedSlot& operator=(PaddedSlot&& o) = default;

		inline SlotHolder * _holder() { return static_cast<SlotHolder*>(m_pHolder); }
		inline const SlotHolder * _holder() const { return static_cast<SlotHolder*>(m_pHolder); }

		SizeSPX		_paddedPreferredSize(int scale) const { return m_pWidget->_preferredSize(scale) + Util::align(Util::ptsToSpx(m_padding,scale)); }
		SizeSPX		_paddedMinSize(int scale) const { return m_pWidget->_minSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); }
		SizeSPX		_paddedMaxSize(int scale) const { return m_pWidget->_maxSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); }
		spx			_paddedMatchingWidth(spx paddedHeight, int scale) const 
					{ 
						SizeSPX paddingSize = Util::align(Util::ptsToSpx(m_padding, scale));
						return m_pWidget->_matchingWidth(paddedHeight - paddingSize.h) + paddingSize.w; 
					}
		spx			_paddedMatchingHeight(spx paddedWidth, int scale) const 
					{ 
						SizeSPX paddingSize = Util::align(Util::ptsToSpx(m_padding, scale));
						return m_pWidget->_matchingHeight(paddedWidth - paddingSize.w) + paddingSize.h; 
					}

		Border		m_padding;
		bool		m_bVisible;
	};
}
#endif //WG_PADDEDSLOT_DOT_H