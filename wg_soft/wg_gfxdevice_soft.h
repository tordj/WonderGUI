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
#ifndef WG_GFXDEVICE_SOFT_DOT_H
#define WG_GFXDEVICE_SOFT_DOT_H

#ifndef WG_GEO_DOT_H
#	include <wg_geo.h>
#endif

#ifndef	WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif


class WgSurfaceSoft;


class WgGfxDeviceSoft : public WgGfxDevice
{
public:
	WgGfxDeviceSoft();
	WgGfxDeviceSoft( WgSurfaceSoft * pCanvas );
	~WgGfxDeviceSoft();

	void	SetCanvas( WgSurfaceSoft * pCanvas );

	//

	void	Fill( const WgRect& rect, const WgColor& col );
	void	Blit( const WgSurface* pSrc, const WgRect& srcrect, int dx, int dy  );

	void	StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );

protected:

	void	_initTables();

	WgSurfaceSoft * m_pCanvas;
	Uint8			m_limitTable[512];
};

#endif //WG_GFXDEVICE_SOFT_DOT_H

