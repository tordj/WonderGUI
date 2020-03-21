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

#ifndef	WG_CSTATICSLOTCOLLECTION_DOT_H
#define	WG_CSTATICSLOTCOLLECTION_DOT_H
#pragma once

#include <wg_component.h>
#include <wg_pointers.h>
#include <wg_container.h>
#include <wg_slot.h>
#include <wg_slotiterator.h>

namespace wg
{

	class CStaticSlotCollection;
	typedef	StrongComponentPtr<CStaticSlotCollection>	CStaticSlotCollection_p;
	typedef	WeakComponentPtr<CStaticSlotCollection>		CStaticSlotCollection_wp;

	//____ CStaticSlotCollection _________________________________________________________________

	class CStaticSlotCollection : public Component
	{
	public:
		using		iterator = SlotIterator;

		//.____ Identification _________________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Content _______________________________________________________

		virtual int		size() const = 0;
		virtual bool	isEmpty() const = 0;

		inline StaticSlot& at(int index) { return _at(index); }
		virtual int index(const Widget * pWidget) const = 0;

		//.____ Misc _______________________________________________________

		inline CStaticSlotCollection_p ptr() { return CStaticSlotCollection_p(this); }

		inline iterator	begin() { return _begin_iterator(); }
		inline iterator	end() { return _end_iterator(); }

	protected:

		// These are needed since we can't make begin/end virtual (shadowing related issues)

		virtual iterator	_begin_iterator() = 0;
		virtual iterator	_end_iterator() = 0;
		virtual StaticSlot&	_at(int index) = 0;
	};

} // namespace wg
#endif //WG_CSTATICSLOTCOLLECTION_DOT_H
