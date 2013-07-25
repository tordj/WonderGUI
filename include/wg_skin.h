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

#ifndef WG_SMARTPTR_DOT_H
#	include <wg_smartptr.h>
#endif

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

class WgSkin;
class WgGfxDevice;

typedef	WgSmartPtr<WgSkin,WgObjectPtr>	WgSkinPtr;


class WgSkin : public WgObject
{
public:
	virtual ~WgSkin() {};
	
	virtual void Render( WgGfxDevice * pDevice, const WgRect& canvas, WgState state, const WgRect& clip ) const = 0;

	virtual WgSize	MinSize() const = 0;
	virtual WgSize	PreferredSize() const = 0;

	virtual WgSize	ContentPadding() const = 0;
	virtual WgSize	SizeForContent( const WgSize contentSize ) const = 0;
	virtual WgRect	ContentRect( const WgRect& canvas, WgState state ) const = 0;

	virtual bool	MarkTest( const WgCoord& ofs, const WgRect& canvas, WgState state, int opacityTreshold ) const = 0;

	virtual bool	IsOpaque() const = 0;
	virtual bool	IsOpaque( WgState state ) const = 0;
	virtual bool	IsOpaque( const WgRect& rect, const WgSize& canvasSize, WgState state ) const = 0;

	virtual bool	IsStateIdentical( WgState state, WgState comparedTo ) const = 0;

protected:
	WgSkin() {};
};

#endif //WG_SKIN_DOT_H