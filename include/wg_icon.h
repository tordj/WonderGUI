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

#ifndef	WG_ICON_DOT_H
#define	WG_ICON_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
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

#ifndef WG_ICONFIELD_DOT_H
#	include <wg_iconfield.h>
#endif

class WgIcon;
typedef	WgIStrongPtr<WgIcon,WgInterfacePtr>		WgIconPtr;
typedef	WgIWeakPtr<WgIcon,WgInterfacePtr>		WgIconWeakPtr;

class WgIcon : public WgInterface
{
public:
	WgIcon(WgIconField* pField) : m_pField(pField) {}

	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgIconPtr			Cast( const WgInterfacePtr& pInterface );
	inline WgIconPtr			Ptr() { return WgIconPtr(_object(),this); }

	inline bool			Set( const WgSkinPtr& pIconGfx, WgOrigo origo = WG_WEST, WgBorder padding = WgBorder(0),
							 float scale = 0.f, bool bOverlap = false ) { return m_pField->Set(pIconGfx,origo,padding,scale,bOverlap); }
	inline void			Clear() { m_pField->Clear(); }

	inline bool			SetScale( float scaleFactor ) { return m_pField->SetScale(scaleFactor); }
	inline void			SetOrigo( WgOrigo origo ) { m_pField->SetOrigo(origo); }
	inline void			SetPadding( WgBorder padding ) { m_pField->SetPadding(padding); }
	inline void			SetOverlap( bool bOverlap ) { m_pField->SetOverlap(bOverlap); }
	inline void			SetSkin( const WgSkinPtr& pSkin ) { m_pField->SetSkin(pSkin); }

	inline float		Scale() const { return m_pField->Scale(); }
	inline WgOrigo		Origo() const { return m_pField->Origo(); }
	inline WgBorder		Padding() const { return m_pField->Padding(); }
	inline bool			Overlap() const { return m_pField->Overlap(); }
	inline WgSkinPtr	Skin() const { return m_pField->Skin(); }
protected:
	WgObject * 			_object() const;

	WgIconField *		m_pField;
};

#endif	// WG_ICON_DOT_H
