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

#include <wg_geo.h>


#define		SLOT_PADDING_METHODS(HOLDERTYPE) \
	inline void		setPadding(Border padding) { static_cast<HOLDERTYPE*>(_holder())->_repadSlots(this, 1, padding); } \
	inline Border	padding() const { return m_padding; }

#define		SLOT_HIDING_METHODS(HOLDERTYPE) \
	inline void		hide() { static_cast<HOLDERTYPE*>(_holder())->_hideSlots(this, 1); } \
	inline void		unhide() { static_cast<HOLDERTYPE*>(_holder())->_unhideSlots(this, 1); } \
	inline void		setVisible(bool bVisible) { if (bVisible) static_cast<HOLDERTYPE*>(_holder())->_unhideSlots(this, 1); else static_cast<HOLDERTYPE*>(_holder())->_hideSlots(this, 1); } \
	inline bool		isVisible() const { return m_bVisible; }

#define		SLOT_PADDING_HELPER_METHODS	SizeSPX		_paddedDefaultSize(int scale) const { return m_pWidget->_defaultSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); } \
										SizeSPX		_paddedMinSize(int scale) const { return m_pWidget->_minSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); } \
										SizeSPX		_paddedMaxSize(int scale) const { return m_pWidget->_maxSize(scale) + Util::align(Util::ptsToSpx(m_padding, scale)); } \
										spx			_paddedMatchingWidth(spx paddedHeight, int scale) const \
										{ \
											SizeSPX paddingSize = Util::align(Util::ptsToSpx(m_padding, scale)); \
											return m_pWidget->_matchingWidth(paddedHeight - paddingSize.h, scale) + paddingSize.w; \
										} \
										spx			_paddedMatchingHeight(spx paddedWidth, int scale) const \
										{ \
											SizeSPX paddingSize = Util::align(Util::ptsToSpx(m_padding, scale)); \
											return m_pWidget->_matchingHeight(paddedWidth - paddingSize.w, scale) + paddingSize.h; \
										} 

#define		SLOT_SELECTING_METHODS(HOLDERTYPE) \
	inline void	select() { static_cast<HOLDERTYPE*>(_holder())->_selectSlots(this, 1); } \
	inline void	unselect() { static_cast<HOLDERTYPE*>(_holder())->_selectSlots(this, 1); } \
	inline void	setSelected(bool bSelected) { if (bSelected) static_cast<HOLDERTYPE*>(_holder())->_selectSlots(this, 1); else static_cast<HOLDERTYPE*>(_holder())->_unselectSlots(this, 1); } \
	inline bool isSelected() const { return m_bSelected; }


#endif //WG_SLOTEXTRAS_DOT_H