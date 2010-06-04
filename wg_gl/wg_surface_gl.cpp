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

#include <wg_surface_gl.h>

#ifdef WIN32
#	include <glext.h>
#else
#	include <GL/glext.h>
#	include <GL/glx.h>
#endif
//#include <GL/glu.h>

PFNGLBUFFERDATAARBPROC				pglBufferDataARB		= 0;
PFNGLBINDBUFFERARBPROC				pglBindBufferARB		= 0;
PFNGLGENBUFFERSARBPROC				pglGenBuffersARB		= 0;
PFNGLDELETEBUFFERSARBPROC			pglDeleteBuffersARB		= 0;
PFNGLMAPBUFFERARBPROC				pglMapBufferARB			= 0;
PFNGLUNMAPBUFFERARBPROC				pglUnmapBufferARB		= 0;


//____ InitGlExtensions() ______________________________________________________

bool WgSurfaceGL::InitGlExtensions()
{
#ifdef WIN32
	pglBufferDataARB = (PFNGLBUFFERDATAARBPROC) wglGetProcAddress("glBufferData");
	pglBindBufferARB = (PFNGLBINDBUFFERARBPROC) wglGetProcAddress("glBindBuffer");
	pglGenBuffersARB = (PFNGLGENBUFFERSARBPROC) wglGetProcAddress("glGenBuffers");
	pglDeleteBuffersARB	= (PFNGLDELETEBUFFERSARBPROC) wglGetProcAddress("glDeleteBuffers");
	pglMapBufferARB = (PFNGLMAPBUFFERARBPROC) wglGetProcAddress("glMapBuffer");
	pglUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) wglGetProcAddress("glUnmapBuffer");
#else
	pglBufferDataARB = (PFNGLBUFFERDATAARBPROC) glXGetProcAddress((const GLubyte *)"glBufferData");
	pglBindBufferARB = (PFNGLBINDBUFFERARBPROC) glXGetProcAddress((const GLubyte *)"glBindBuffer");
	pglGenBuffersARB = (PFNGLGENBUFFERSARBPROC) glXGetProcAddress((const GLubyte *)"glGenBuffers");
	pglDeleteBuffersARB	= (PFNGLDELETEBUFFERSARBPROC) glXGetProcAddress((const GLubyte *)"glDeleteBuffers");
	pglMapBufferARB = (PFNGLMAPBUFFERARBPROC) glXGetProcAddress((const GLubyte *)"glMapBuffer");
	pglUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC) glXGetProcAddress((const GLubyte *)"glUnmapBuffer");
#endif

	if( pglBufferDataARB == 0 || pglBindBufferARB == 0 || pglGenBuffersARB == 0 ||
		pglDeleteBuffersARB == 0 || pglMapBufferARB == 0 || pglUnmapBufferARB == 0 )
		return false;

	return true;
}


//____ Constructor _____________________________________________________________


WgSurfaceGL::WgSurfaceGL( GLint _format, Uint32 _width, Uint32 _height, void * _pPixels )
{
	if( pglBufferDataARB == 0 )
		InitGlExtensions();

	SetPixelFormat( _format );

	m_format = _format;
	m_width	 = _width;
	m_height = _height;
	m_pAlpha = 0;
	m_pPixels = (Uint8*)_pPixels;
	m_buffer = 0;
	m_pitch = _width*m_pixelFormat.bits/8;

	InitBuffer();


	glGenTextures( 1, &m_texture );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D( GL_TEXTURE_2D, 0, _format, _width, _height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

}

WgSurfaceGL::WgSurfaceGL(GLuint _texture, Uint8 * _pAlpha )
{
	if( pglBufferDataARB == 0 )
		InitGlExtensions();

	SetPixelFormat( m_format );

	GLint width = 0;
	GLint height = 0;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );

	m_texture	= _texture;
	m_width = width;
	m_height = height;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &m_format );
	m_pAlpha	= _pAlpha;
	m_pPixels	= 0;
	m_buffer = 0;
	m_pitch = width*m_pixelFormat.bits/8;

	InitBuffer();

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
}

void WgSurfaceGL::InitBuffer()
{
	if( m_pPixels == 0 )
		return;

	int size = m_width*m_height;

	size *= m_pixelSize;
	if( size == 0 )
	{
		// TODO: Error handling. Width or height were zero or format was unsupported.
		return;
	}

	pglGenBuffersARB( 1, &m_buffer );
	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );
	pglBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, size, m_pPixels, GL_STREAM_DRAW_ARB );
}

void WgSurfaceGL::SetPixelFormat( GLint _format )
{
	switch( _format )
	{
	case GL_RGB8:
		m_pixelSize = 3;

		m_pixelFormat.type = WgSurface::RGB_8;
		m_pixelFormat.bits = 24;

		m_pixelFormat.R_mask = 0xFF;
		m_pixelFormat.G_mask = 0xFF00;
		m_pixelFormat.B_mask = 0xFF0000;
		m_pixelFormat.A_mask = 0x0;

		m_pixelFormat.R_shift = 0;
		m_pixelFormat.G_shift = 8;
		m_pixelFormat.B_shift = 16;
		m_pixelFormat.A_shift = 0;

		m_pixelFormat.R_bits = 8;
		m_pixelFormat.G_bits = 8;
		m_pixelFormat.B_bits = 8;
		m_pixelFormat.A_bits = 0;

		break;

	case GL_RGBA8:
		m_pixelSize = 4;

		m_pixelFormat.type = WgSurface::RGBA_8;
		m_pixelFormat.bits = 32;

		m_pixelFormat.R_mask = 0xFF;
		m_pixelFormat.G_mask = 0xFF00;
		m_pixelFormat.B_mask = 0xFF0000;
		m_pixelFormat.A_mask = 0xFF000000;

		m_pixelFormat.R_shift = 0;
		m_pixelFormat.G_shift = 8;
		m_pixelFormat.B_shift = 16;
		m_pixelFormat.A_shift = 24;

		m_pixelFormat.R_bits = 8;
		m_pixelFormat.G_bits = 8;
		m_pixelFormat.B_bits = 8;
		m_pixelFormat.A_bits = 8;

		break;

	default:
		m_pixelFormat.type = WgSurface::UNSPECIFIED;
		m_pixelFormat.bits = 0;

		m_pixelFormat.R_mask = 0;
		m_pixelFormat.G_mask = 0;
		m_pixelFormat.B_mask = 0;
		m_pixelFormat.A_mask = 0;

		m_pixelFormat.R_shift = 0;
		m_pixelFormat.G_shift = 0;
		m_pixelFormat.B_shift = 0;
		m_pixelFormat.A_shift = 0;

		m_pixelFormat.R_bits = 0;
		m_pixelFormat.G_bits = 0;
		m_pixelFormat.B_bits = 0;
		m_pixelFormat.A_bits = 0;

		m_pixelSize = 0;		// Signal unknown pixelsize.
	}
}

//____ Destructor ______________________________________________________________

WgSurfaceGL::~WgSurfaceGL()
{
	// Free the stuff

	if( m_buffer )
		pglDeleteBuffersARB ( 1, &m_buffer );

	if( m_pAlpha )
		delete [] m_pAlpha;

	glDeleteTextures( 1, &m_texture );
}

//____ GetWidth() ______________________________________________________________

Uint32 WgSurfaceGL::GetWidth() const
{
	return m_width;
}

//____ GetHeight() _____________________________________________________________

Uint32 WgSurfaceGL::GetHeight() const
{
	return m_height;
}

//____ IsOpaque() ______________________________________________________________

bool WgSurfaceGL::IsOpaque() const
{
	if( m_pAlpha )
		return false;

	if( m_buffer && m_format == GL_RGBA8 )
		return false;

	return true;
}

//____ Lock() __________________________________________________________________

void * WgSurfaceGL::Lock( LockStatus mode )
{
	if( m_format == 0 || m_lockStatus != UNLOCKED || mode == UNLOCKED )
		return 0;

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );

	switch( mode )
	{
		case READ_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );
			break;
		case WRITE_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
			break;
		case READ_WRITE:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB );
			break;
		default:
			break;	// Should never happen, just here to avoid compiler warnings...
	}

	m_lockRegion = WgRect(0,0,m_width,m_height);
	m_lockStatus = mode;
	return m_pPixels;
}

//____ LockRegion() __________________________________________________________________

void * WgSurfaceGL::LockRegion( LockStatus mode, const WgRect& region )
{
	if( m_format == 0 || m_lockStatus != UNLOCKED || mode == UNLOCKED )
		return 0;

	if( region.x + region.w > m_width || region.y + region.w > m_height || region.x < 0 || region.y < 0 )
		return 0;

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );

	switch( mode )
	{
		case READ_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );
			break;
		case WRITE_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
			break;
		case READ_WRITE:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB );
			break;
		default:
			break;	// Should never happen, just here to avoid compiler warnings...
	}

	m_lockRegion = region;
	m_lockStatus = mode;
	return m_pPixels += (m_width*region.y+region.x)*m_pixelSize;
}


//____ Unlock() ________________________________________________________________

void WgSurfaceGL::Unlock()
{
	if(m_lockStatus == UNLOCKED )
		return;

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );	// In case we've been locking others inbetween...
	pglUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );

	if( m_lockStatus != READ_ONLY )
	{
		glBindTexture( GL_TEXTURE_2D, m_texture );
		glTexSubImage2D( GL_TEXTURE_2D, 0, m_lockRegion.x, m_lockRegion.y, m_lockRegion.w, m_lockRegion.h, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	}
	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	m_lockStatus = UNLOCKED;
	m_pPixels = 0;
	m_lockRegion.w = 0;
	m_lockRegion.h = 0;
}


//____ GetPixel() ______________________________________________________________

Uint32 WgSurfaceGL::GetPixel( Uint32 x, Uint32 y ) const
{
	if( m_format != 0 && m_buffer && m_lockStatus != WRITE_ONLY )
	{
		Uint32 val;

		if( m_lockStatus == UNLOCKED )
		{
			// Quick lock of surface

			pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );
			Uint8 * pPixel = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );

			//

			 pPixel += (m_width*y+x)*m_pixelSize;
			switch( m_pixelSize )
			{
				case 1:
					val = (Uint32) *pPixel;
				case 2:
					val = (Uint32) ((Uint16*) pPixel)[0];
				case 3:
					val = ((Uint32) pPixel[0]) + (((Uint32) pPixel[1]) << 8) + (((Uint32) pPixel[2]) << 16);
				default:
					val = *((Uint32*) pPixel);
			}

			// Quick unlock of surface

			pglUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );
			pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
		}
		else
		{
			Uint8 * pPixel = m_pPixels + (m_width*y+x)*m_pixelSize;
			switch( m_pixelSize )
			{
				case 1:
					val = (Uint32) *pPixel;
				case 2:
					val = (Uint32) ((Uint16*) pPixel)[0];
				case 3:
				   val = ((Uint32) pPixel[0]) + (((Uint32) pPixel[1]) << 8) + (((Uint32) pPixel[2]) << 16);
				default:
					val = *((Uint32*) pPixel);
			}
		}

		return val;
	}

	return 0;
}



//____ GetOpacity() ____________________________________________________________

Uint8 WgSurfaceGL::GetOpacity( Uint32 x, Uint32 y ) const
{
	return 255;

	if( m_buffer )
	{
		if( m_format == GL_RGBA8 && m_lockStatus != WRITE_ONLY )
		{
			Uint8 a;

			if( m_lockStatus == UNLOCKED )
			{
				// Quick lock of surface

				pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );
				Uint8 * pPixel = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );

				//

				a = pPixel[(m_width*y+x)*m_pixelSize+3];

				// Quick unlock of surface

				pglUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );
				pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

			}
			else
			{
				a = m_pPixels[(m_width*y+x)*m_pixelSize+3];
			}

			return a;
		}

	}
	else if( m_pAlpha )
	{
		return m_pAlpha[y*m_width+x];
	}

	return 255;
}


//____ WgSurfaceFactoryGL::CreateSurface() ___________________________________

WgSurface * WgSurfaceFactoryGL::CreateSurface( const WgSize& size, WgSurface::PixelType type )
{

	GLint	format;
	int		buffSize;



	switch( type )
	{
	case WgSurface::RGB_8:
		format = GL_RGB8;
		buffSize = 3*size.w*size.h;
		break;

	case WgSurface::RGBA_8:
		format = GL_RGBA8;
		buffSize = 4*size.w*size.h;
		break;

	default:
		return 0;

	}

	char * pBuffer = new char[buffSize];
	memset( pBuffer, 0, buffSize );

	WgSurfaceGL * p = new WgSurfaceGL( format, size.w, size.h, pBuffer );

	delete pBuffer;
	return 	p;


}
