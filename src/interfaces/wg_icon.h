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

namespace wg 
{
	
	class Icon;
	typedef	WgIStrongPtr<Icon,Interface_p>		Icon_p;
	typedef	WgIWeakPtr<Icon,Interface_wp>	Icon_wp;
	
	class Icon : public Interface
	{
	public:
		Icon(IconField* pField) : m_pField(pField) {}
	
		virtual bool				isInstanceOf( const char * pClassName ) const;
		virtual const char *		className( void ) const;
		static const char			CLASSNAME[];
		static Icon_p			cast( const Interface_p& pInterface );
		inline Icon_p			ptr() { return Icon_p(_object(),this); }
	
		inline bool			set( const Skin_p& pIconGfx, Origo origo = WG_WEST, Border padding = Border(0),
								 float scale = 0.f, bool bOverlap = false ) { return m_pField->set(pIconGfx,origo,padding,scale,bOverlap); }
		inline void			clear() { m_pField->clear(); }
	
		inline bool			setScale( float scaleFactor ) { return m_pField->setScale(scaleFactor); }
		inline void			setOrigo( Origo origo ) { m_pField->setOrigo(origo); }
		inline void			setPadding( Border padding ) { m_pField->setPadding(padding); }
		inline void			setOverlap( bool bOverlap ) { m_pField->setOverlap(bOverlap); }
		inline void			setSkin( const Skin_p& pSkin ) { m_pField->setSkin(pSkin); }
	
		inline float		scale() const { return m_pField->scale(); }
		inline Origo		origo() const { return m_pField->origo(); }
		inline Border		padding() const { return m_pField->padding(); }
		inline bool			overlap() const { return m_pField->overlap(); }
		inline Skin_p	skin() const { return m_pField->skin(); }
	protected:
		Object * 			_object() const;
	
		IconField *		m_pField;
	};
	

} // namespace wg
#endif	// WG_ICON_DOT_H
