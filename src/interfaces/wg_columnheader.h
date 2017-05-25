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

#ifndef	WG_COLUMNHEADER_DOT_H
#define	WG_COLUMNHEADER_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_skin.h>
#include <wg_columnheaderitem.h>
#include <wg_icon.h>
#include <wg_modtext.h>

namespace wg 
{
	
	class ColumnHeader;
	typedef	StrongInterfacePtr<ColumnHeader>		ColumnHeader_p;
	typedef	WeakInterfacePtr<ColumnHeader,Interface_wp>	ColumnHeader_wp;
	
	class ColumnHeader : public Interface
	{
	public:
		//.____ Creation __________________________________________

		ColumnHeader(ColumnHeaderItem* pItem);
		
		//.____ Components _______________________________________
	
		Icon			icon;
		Icon			arrow;
		ModText			label;
	
		//.____ Appearance ____________________________________________
	
		inline void		setSkin( Skin * pSkin ) { m_pItem->setSkin(pSkin); }
		inline Skin_p	skin() const { return m_pItem->skin(); }

		//.____ Misc __________________________________________________

		inline ColumnHeader_p	ptr() { return ColumnHeader_p(this); }

	
	protected:
		Object * 			_object() const;
	
		ColumnHeaderItem *	m_pItem;
	};
	

} // namespace wg
#endif	// WG_COLUMNHEADER_DOT_H
