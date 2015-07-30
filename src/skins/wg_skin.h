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
#ifndef WG_SKIN_DOT_H
#define WG_SKIN_DOT_H

#ifndef WG_POINTERS_DOT_H
#	include <wg_pointers.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgSkin;
class WgGfxDevice;

typedef	WgStrongPtr<WgSkin,WgObjectPtr>	WgSkinPtr;


class WgSkin : public WgObject
{
public:
	virtual ~WgSkin() {};

	bool		isInstanceOf( const char * pClassName ) const;
	const char *className( void ) const;
	static const char	CLASSNAME[];
	static WgSkinPtr	cast( const WgObjectPtr& pObject );


	virtual void render( WgGfxDevice * pDevice, const WgRect& canvas, WgState state, const WgRect& clip ) const = 0;

	virtual WgSize	minSize() const = 0;
	virtual WgSize	preferredSize() const = 0;

	virtual WgSize	contentPadding() const = 0;
	virtual WgSize	sizeForContent( const WgSize contentSize ) const = 0;
	virtual WgRect	contentRect( const WgRect& canvas, WgState state ) const = 0;

	virtual bool	markTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const = 0;

	virtual bool	isOpaque() const = 0;
	virtual bool	isOpaque( WgState state ) const = 0;
	virtual bool	isOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const = 0;

	virtual bool	isStateIdentical( WgState state, WgState comparedTo ) const = 0;

protected:
	WgSkin() {};
};

#endif //WG_SKIN_DOT_H
