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

#ifndef WG_GFXDEVICE_NULL_DOT_H
#define WG_GFXDEVICE_NULL_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

class WgSurface;
class WgRect;
class WgColor;

class WgGfxDeviceNull : public WgGfxDevice
{
public:
	WgGfxDeviceNull( WgSize size );
	~WgGfxDeviceNull();

	//

	void	Fill( const WgRect& rect, const WgColor& col );
	void	Blit( const WgSurface* src, const WgRect& srcrect, int dx, int dy  );
	void	TileBlit( const WgSurface* src, const WgRect& srcrect, const WgRect& dest );

	void	FillSubPixel( const WgRectF& rect, const WgColor& col );
	void	StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );


	void	DrawArcNE( const WgRect& rect, WgColor color );
	void	DrawElipse( const WgRect& rect, WgColor color );
	void	DrawFilledElipse( const WgRect& rect, WgColor color );

	void	ClipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color );

protected:

};

#endif //WG_GFXDEVICE_NULL_DOT_H

