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

#ifndef	WG_SURFACE_GL_DOT_H
#define	WG_SURFACE_GL_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#ifdef WIN32
#	include <windows.h>
#endif

#include <GL/gl.h>

//____ WgSurfaceGL _____________________________________________________________

class WgSurfaceGL : public WgSurface
{
public:
	WgSurfaceGL( GLint format, Uint32 width, Uint32 height, void * pPixels );
	WgSurfaceGL( GLuint texture, Uint8 * pAlpha = 0 );
	~WgSurfaceGL();

	inline	GLuint	GetTexture() const { return m_texture; };

	// Methods needed by WgSurface

	Uint32		GetWidth() const;
	Uint32		GetHeight() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( Uint32 x, Uint32 y ) const;
	Uint8		GetOpacity( Uint32 x, Uint32 y ) const;
	Uint32		Col2Pixel( const WgColor& col ) const;
	WgColor		Pixel2Col( Uint32 pixel ) const;

	void *		Lock( LockStatus mode );
	void		Unlock();

private:
	static bool	InitGlExtensions();

	void		SetPixelSize( GLint _format );
	void		InitBuffer();

	GLuint		m_buffer;			// Pointer at GL pixel buffer, if any.
	GLuint 		m_texture;			// GL texture handle.
	GLint		m_format;			// GL-enum for the texture format or 0 if unknown.
	Uint32		m_width;			// Width in pixels.
	Uint32		m_height;			// Height in pixels.
	Uint8 *		m_pAlpha;			// Pointer at supplementary alpha map, if any.
	Uint32		m_pixelSize;		// Size in bytes of a pixel.

	Uint8 *		m_pPixels;			// Pointer at pixels when surface locked.

};

#endif //WG_SURFACE_GL_DOT_H

