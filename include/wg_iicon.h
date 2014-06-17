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

#ifndef	WG_IICON_DOT_H
#define	WG_IICON_DOT_H

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

class WgIIcon;
typedef	WgIStrongPtr<WgIIcon,WgInterfacePtr>		WgIIconPtr;
typedef	WgWeakPtr<WgIIcon,WgInterfacePtr>		WgIIconWeakPtr;

class WgIIcon : public WgInterface
{
public:
	virtual bool				IsInstanceOf( const char * pClassName ) const;
	virtual const char *		ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgIIconPtr			Cast( const WgInterfacePtr& pInterface );

	virtual bool			Set( const WgSkinPtr& pIconGfx, WgOrigo origo = WG_WEST, WgBorders padding = WgBorders(0), 
								float _scale = 0.f, bool _bOverlap = false ) = 0;
	virtual void			Clear() = 0;

	virtual bool			SetScale( float scaleFactor ) = 0;
	virtual void			SetOrigo( WgOrigo origo ) = 0;
	virtual void			SetPadding( WgBorders padding ) = 0;
	virtual void			SetOverlap( bool bOverlap ) = 0;
	virtual void			SetSkin( const WgSkinPtr& pSkin ) = 0;

	virtual float			Scale() const = 0;
	virtual WgOrigo			Origo() const = 0;
	virtual WgBorders		Padding() const = 0;
	virtual bool			Overlap() const = 0;
	virtual WgSkinPtr		Skin() const = 0;

};

#endif	// WG_IICON_DOT_H
