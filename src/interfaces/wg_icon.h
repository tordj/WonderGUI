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
typedef	WgIStrongPtr<WgIcon,WgInterface_p>		WgIcon_p;
typedef	WgIWeakPtr<WgIcon,WgInterface_wp>	WgIcon_wp;

class WgIcon : public WgInterface
{
public:
	WgIcon(WgIconField* pField) : m_pField(pField) {}

	virtual bool				isInstanceOf( const char * pClassName ) const;
	virtual const char *		className( void ) const;
	static const char			CLASSNAME[];
	static WgIcon_p			cast( const WgInterface_p& pInterface );
	inline WgIcon_p			ptr() { return WgIcon_p(_object(),this); }

	inline bool			set( const WgSkin_p& pIconGfx, WgOrigo origo = WG_WEST, WgBorder padding = WgBorder(0),
							 float scale = 0.f, bool bOverlap = false ) { return m_pField->set(pIconGfx,origo,padding,scale,bOverlap); }
	inline void			clear() { m_pField->clear(); }

	inline bool			setScale( float scaleFactor ) { return m_pField->setScale(scaleFactor); }
	inline void			setOrigo( WgOrigo origo ) { m_pField->setOrigo(origo); }
	inline void			setPadding( WgBorder padding ) { m_pField->setPadding(padding); }
	inline void			setOverlap( bool bOverlap ) { m_pField->setOverlap(bOverlap); }
	inline void			setSkin( const WgSkin_p& pSkin ) { m_pField->setSkin(pSkin); }

	inline float		scale() const { return m_pField->scale(); }
	inline WgOrigo		origo() const { return m_pField->origo(); }
	inline WgBorder		padding() const { return m_pField->padding(); }
	inline bool			overlap() const { return m_pField->overlap(); }
	inline WgSkin_p	skin() const { return m_pField->skin(); }
protected:
	WgObject * 			_object() const;

	WgIconField *		m_pField;
};

#endif	// WG_ICON_DOT_H
