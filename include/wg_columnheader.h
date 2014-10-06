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

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_INTERFACE_DOT_H
#	include <wg_interface.h>
#endif

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_SKIN_DOT_H
#	include <wg_skin.h>
#endif

#ifndef WG_COLUMNHEADERFIELD_DOT_H
#	include <wg_columnheaderfield.h>
#endif

#ifndef WG_ICON_DOT_H
#	include <wg_icon.h>
#endif

#ifndef WG_MODTEXT_DOT_H
#	include <wg_modtext.h>
#endif

class WgColumnHeader;
typedef	WgIStrongPtr<WgColumnHeader,WgInterfacePtr>		WgColumnHeaderPtr;
typedef	WgIWeakPtr<WgColumnHeader,WgInterfacePtr>		WgColumnHeaderWeakPtr;

class WgColumnHeader : public WgInterface
{
public:
	WgColumnHeader(WgColumnHeaderField* pField);

	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgColumnHeaderPtr	Cast( const WgInterfacePtr& pInterface );
	inline WgColumnHeaderPtr	Ptr() { return WgColumnHeaderPtr(_object(),this); }

	//____ Interfaces ______________________________________

	WgIcon				icon;
	WgIcon				arrow;
	WgModText			label;

	//____ Methods _________________________________________

	inline void			SetSkin( const WgSkinPtr& pSkin ) { m_pField->SetSkin(pSkin); }
	inline WgSkinPtr	Skin() const { return m_pField->Skin(); }


protected:
	WgObject * 			_object() const;

	WgColumnHeaderField *		m_pField;
};

#endif	// WG_COLUMNHEADER_DOT_H
