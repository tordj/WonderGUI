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

#ifndef WG_NULLGFXDEVICE_DOT_H
#define WG_NULLGFXDEVICE_DOT_H

#ifndef WG_TYPES_DOT_H
#	include <wg_types.h>
#endif

#ifndef WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

class WgSurface;
class WgRect;
class WgColor;

class WgNullGfxDevice;
typedef	WgStrongPtr<WgNullGfxDevice,WgGfxDevice_p>	WgNullGfxDevice_p;
typedef	WgWeakPtr<WgNullGfxDevice,WgGfxDevice_wp>	WgNullGfxDevice_wp;

class WgNullGfxDevice : public WgGfxDevice
{
public:
	static WgNullGfxDevice_p	create( WgSize size );

	bool						isInstanceOf( const char * pClassName ) const;
	const char *				className( void ) const;
	static const char			CLASSNAME[];
	static WgNullGfxDevice_p	cast( const WgObject_p& pObject );

	//

	void	fill( const WgRect& rect, const WgColor& col );
	void	blit( const WgSurface_p& src, const WgRect& srcrect, int dx, int dy  );
	void	tileBlit( const WgSurface_p& src, const WgRect& srcrect, const WgRect& dest );

	void	fillSubPixel( const WgRectF& rect, const WgColor& col );
	void	stretchBlitSubPixel( const WgSurface_p& pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );

	void	clipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	clipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	clipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness );

	void	drawArcNE( const WgRect& rect, WgColor color );
	void	drawElipse( const WgRect& rect, WgColor color );
	void	drawFilledElipse( const WgRect& rect, WgColor color );

	void	clipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color );
	void	clipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color );
	void	clipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color );

protected:
	WgNullGfxDevice( WgSize size );
	~WgNullGfxDevice();

};

#endif //WG_NULLGFXDEVICE_DOT_H

