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

#ifndef WG_GFXDEVICE_SDL_DOT_H
#define WG_GFXDEVICE_SDL_DOT_H

#ifndef	WG_GFXDEVICE_DOT_H
#	include <wg_gfxdevice.h>
#endif

class WgSurfaceSDL;


class WgGfxDeviceSDL : public WgGfxDevice
{
public:
	WgGfxDeviceSDL( WgSurfaceSDL * pCanvas );
	~WgGfxDeviceSDL();

	void	SetCanvas( WgSurfaceSDL * pCanvas );
	inline WgSurfaceSDL * GetCanvas( void ) { return m_pCanvas; };

	//

	void	SetTintColor( WgColor color );
	bool	SetBlendMode( WgBlendMode blendMode );

	void	Fill( const WgRect& rect, const WgColor& col );
	void	Blit( const WgSurface* src, const WgRect& srcrect, int dx, int dy  );


protected:
	void	StretchBlitSubPixel( const WgSurface * pSrc, float sx, float sy, float sw, float sh,
						   		 float dx, float dy, float dw, float dh, bool bTriLinear, float mipBias );

	WgSurfaceSDL *	m_pCanvas;
};

#endif //WG_GFXDEVICE_SDL_DOT_H

