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
typedef	WgStrongPtr<WgNullGfxDevice,WgGfxDevicePtr>	WgNullGfxDevicePtr;
typedef	WgWeakPtr<WgNullGfxDevice,WgGfxDeviceWeakPtr>	WgNullGfxDeviceWeakPtr;

class WgNullGfxDevice : public WgGfxDevice
{
public:
	static WgNullGfxDevicePtr	Create( WgSize size );

	bool						IsInstanceOf( const char * pClassName ) const;
	const char *				ClassName( void ) const;
	static const char			CLASSNAME[];
	static WgNullGfxDevicePtr	Cast( const WgObjectPtr& pObject );

	//

	void	Fill( const WgRect& rect, const WgColor& col );
	void	Blit( const WgSurfacePtr& src, const WgRect& srcrect, int dx, int dy  );
	void	TileBlit( const WgSurfacePtr& src, const WgRect& srcrect, const WgRect& dest );

	void	FillSubPixel( const WgRectF& rect, const WgColor& col );
	void	StretchBlitSubPixel( const WgSurfacePtr& pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );

	void	ClipDrawHorrLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	ClipDrawVertLine( const WgRect& clip, const WgCoord& start, int length, const WgColor& col );
	void	ClipPlotSoftPixels( const WgRect& clip, int nCoords, const WgCoord * pCoords, const WgColor& col, float thickness );

	void	DrawArcNE( const WgRect& rect, WgColor color );
	void	DrawElipse( const WgRect& rect, WgColor color );
	void	DrawFilledElipse( const WgRect& rect, WgColor color );

	void	ClipDrawArcNE( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawElipse( const WgRect& clip, const WgRect& rect, WgColor color );
	void	ClipDrawFilledElipse( const WgRect& clip, const WgRect& rect, WgColor color );

protected:
	WgNullGfxDevice( WgSize size );
	~WgNullGfxDevice();

};

#endif //WG_NULLGFXDEVICE_DOT_H

