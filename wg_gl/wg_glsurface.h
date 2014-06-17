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

#ifndef	WG_GLSURFACE_DOT_H
#define	WG_GLSURFACE_DOT_H

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#define GL_GLEXT_PROTOTYPES 1

#ifdef WIN32
#	include <windows.h>
#endif

#include <GL/gl.h>

class WgGLSurface;
typedef	WgStrongPtr<WgGLSurface,WgSurfacePtr>	WgGLSurfacePtr;
typedef	WgWeakPtr<WgGLSurface,WgSurfaceWeakPtr>	WgGLSurfaceWeakPtr;

//____ WgGLSurface _____________________________________________________________

class WgGLSurface : public WgSurface
{
	friend class WgGLSurfaceFactory;

public:
	static WgGLSurfacePtr	Create( WgSize dimensions, GLint format, void * pPixels ) { return new WgGLSurface(dimensions, format, pPixels); }
	static WgGLSurfacePtr	Create( GLuint texture, Uint8 * pAlpha = 0 ) { return new WgGLSurface(texture,pAlpha); }

	inline	GLuint	GetTexture() const { return m_texture; };

	bool					IsInstanceOf( const char * pClassName ) const;
	const char *			ClassName( void ) const;
	static const char		CLASSNAME[];
	static WgGLSurfacePtr	Cast( const WgObjectPtr& pObject );

	// Methods needed by WgSurface

	WgSize		Size() const;
	bool		IsOpaque() const;

	Uint32		GetPixel( WgCoord coord ) const;
	Uint8		GetOpacity( WgCoord coord ) const;

	void *		Lock( WgAccessMode mode );
	void *		LockRegion( WgAccessMode mode, const WgRect& region );
	void		Unlock();

private:
	WgGLSurface( WgSize dimensions, GLint format, void * pPixels );
	WgGLSurface( GLuint texture, Uint8 * pAlpha = 0 );
	~WgGLSurface();

	static bool	_initGlExtensions();

	void		_setPixelFormat( GLint _format );
	void		_initBuffer();


	GLuint		m_buffer;			// Pointer at GL pixel buffer, if any.
	GLuint 		m_texture;			// GL texture handle.
	GLint		m_format;			// GL-enum for the texture format or 0 if unknown.
	WgSize		m_size;				// Width and height in pixels.
	Uint8 *		m_pAlpha;			// Pointer at supplementary alpha map, if any.
	Uint32		m_pixelSize;		// Size in bytes of a pixel.

};

#endif //WG_GLSURFACE_DOT_H

