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

#ifndef	WG_ICOLUMNHEADER_DOT_H
#define	WG_ICOLUMNHEADER_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_interface.h>
#include <wg_pointers.h>
#include <wg_skin.h>
#include <wg_ccolumnheader.h>
#include <wg_iicondisplay.h>
#include <wg_itextdisplay.h>

namespace wg 
{
	
	class IColumnHeader;
	typedef	StrongInterfacePtr<IColumnHeader>		IColumnHeader_p;
	typedef	WeakInterfacePtr<IColumnHeader>	IColumnHeader_wp;
	
	class IColumnHeader : public Interface
	{
	public:
		/** @private */

		IColumnHeader(CColumnHeader* pComponent);
		
		//.____ Interfaces _______________________________________
	
		IIconDisplay			icon;
		IIconDisplay			arrow;
		ITextDisplay	label;
	
		//.____ Appearance ____________________________________________
	
		inline void		setSkin( Skin * pSkin ) { m_pComponent->setSkin(pSkin); }
		inline Skin_p	skin() const { return m_pComponent->skin(); }

		//.____ Misc __________________________________________________

		inline IColumnHeader_p	ptr() { return IColumnHeader_p(this); }

	
	protected:
		Object * 			_object() const;
	
		CColumnHeader *	m_pComponent;
	};
	

} // namespace wg
#endif	// WG_ICOLUMNHEADER_DOT_H
