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

#include <memory.h>

#include <wg_glsurface.h>
#include <wg_util.h>
#include <assert.h>

#ifdef __APPLE___
#	include <OpenGL/glu.h>
#endif

#ifdef WIN32
#	include <gl/glu.h>
#endif

#include <wg3_glsurface.h>
#include <wg_versionbridge.h>

static const char	c_surfaceType[] = {"OpenGL"};


//____ MaxSize() _______________________________________________________________

WgSize WgGlSurface::MaxSize()
{
	return wg::GlSurface::maxSize();
}


//____ Constructor _____________________________________________________________


WgGlSurface::WgGlSurface( WgSize size, WgPixelType type, int flags )
{
	m_pRealSurface = wg::GlSurface::create(size,type,flags);
}


WgGlSurface::WgGlSurface( WgSize size, WgPixelType type, uint8_t * pPixels, int pitch,  const WgPixelFormat& pixelFormat, int flags )
{
	m_pRealSurface = wg::GlSurface::create(size, type, pPixels, pitch, &pixelFormat,flags);
}


WgGlSurface::WgGlSurface( WgSurface * pOther, int flags )
{
	m_pRealSurface = wg::GlSurface::create( ((WgGlSurface*)pOther)->m_pRealSurface,flags);
}


//____ Destructor ______________________________________________________________

WgGlSurface::~WgGlSurface()
{
}

//____ Type() __________________________________________________________________

const char * WgGlSurface::Type() const
{
    return GetClass();
}

//____ GetClass() _____________________________________________________________

const char * WgGlSurface::GetClass()
{
    return c_surfaceType;
}



/*
bool WgGlSurface::unload()
{
	if( m_texture == 0 )
		return true;
	
    glDeleteTextures( 1, &m_texture );
	m_texture = 0;
	
	assert(glGetError() == 0);	
    return true;
}

bool WgGlSurface::isLoaded()
{
	return (m_texture == 0);
}

void WgGlSurface::reload()
{
	assert(glGetError() == 0);
 
    glGenTextures( 1, &m_texture );
    glBindTexture( GL_TEXTURE_2D, m_texture );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	
    glTexImage2D( GL_TEXTURE_2D, 0, m_internalFormat, m_size.w, m_size.h, 0,
                 m_accessFormat, GL_UNSIGNED_BYTE, m_pBlob->Content() );
    

	assert( glGetError() == 0);	
}
*/
