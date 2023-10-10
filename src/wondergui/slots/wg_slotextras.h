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


#define		SLOT_HIDING_METHODS(HOLDERTYPE) \
	inline void		hide() { static_cast<HOLDERTYPE*>(_holder())->_hideSlots(this, 1); } \
	inline void		unhide() { static_cast<HOLDERTYPE*>(_holder())->_unhideSlots(this, 1); } \
	inline void		setVisible(bool bVisible) { if (bVisible) static_cast<HOLDERTYPE*>(_holder())->_unhideSlots(this, 1); else static_cast<HOLDERTYPE*>(_holder())->_hideSlots(this, 1); } \
	inline bool		isVisible() const { return m_bVisible; }


#define		SLOT_SELECTING_METHODS(HOLDERTYPE) \
	inline void	select() { static_cast<HOLDERTYPE*>(_holder())->_selectSlots(this, 1); } \
	inline void	unselect() { static_cast<HOLDERTYPE*>(_holder())->_selectSlots(this, 1); } \
	inline void	setSelected(bool bSelected) { if (bSelected) static_cast<HOLDERTYPE*>(_holder())->_selectSlots(this, 1); else static_cast<HOLDERTYPE*>(_holder())->_unselectSlots(this, 1); } \
	inline bool isSelected() const { return m_bSelected; }


#endif //WG_SLOTEXTRAS_DOT_H
