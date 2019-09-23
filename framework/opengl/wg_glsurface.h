/*=========================================================================

                         >>> WonderGUI <<<

  This file is part of Tord Jansson's WonderGUI Graphics Toolkit
  and copyright (c) Tord Jansson, Sweden [tord.jansson@gmail.com].

                            -----------

  The WonderGUI Graphics Toolkit is free Glware; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Glware Foundation; either
  version 2 of the License, or (at your option) any later version.

                            -----------

  The WonderGUI Graphics Toolkit is also available for use in commercial
  closed-source projects under a separate license. Interested parties
  should contact Tord Jansson [tord.jansson@gmail.com] for details.

=========================================================================*/

#ifndef	WG_GLSURFACE_DOT_H
#define	WG_GLSURFACE_DOT_H

#define GL_GLEXT_PROTOTYPES 1

#ifdef WIN32
#	include <gl/glew.h>
#	include <gl/gl.h>
#else
#	ifdef __APPLE__
#		include <OpenGL/gl3.h>
#	else
#		include <GL/gl.h>
#	endif
#endif


#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

#ifndef WG_BLOB_DOT_H
#   include <wg_blob.h>
#endif


//____ WgGlSurface _____________________________________________________________

class WgGlSurface : public WgSurface
{
    friend class WgGlSurfaceFactory;

public:
    WgGlSurface( WgSize size, WgPixelType type = WgPixelType::BGRA_8, int flags = wg::SurfaceFlag::Static );
    WgGlSurface( WgSize size, WgPixelType type, uint8_t * pPixels, int pitch, const WgPixelFormat& pixelFormat, int flags = wg::SurfaceFlag::Static );
    WgGlSurface( WgSurface * pOther, int flags = wg::SurfaceFlag::Static );
    ~WgGlSurface();

    const char *Type() const;
    static const char * GetClass();
    
    
//    inline	GLuint	GetTexture() const { return m_texture; }


	static WgSize	MaxSize();
/*
	bool		unload();
	bool		isLoaded();
	void		reload();
*/


};

#endif //WG_GLSURFACE_DOT_H

