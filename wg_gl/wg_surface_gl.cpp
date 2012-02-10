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
#include <wg_util.h>

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

static const char	c_surfaceType[] = {"OpenGL"};


//____ _initGlExtensions() ______________________________________________________

bool WgSurfaceGL::_initGlExtensions()
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

WgSurfaceGL::WgSurfaceGL( WgSize dimensions, GLint _format, void * _pPixels )
{
	if( pglBufferDataARB == 0 )
		_initGlExtensions();

	_setPixelFormat( _format );

	m_format = _format;
	m_size	 = dimensions;
	m_pAlpha = 0;
	m_pPixels = (Uint8*)_pPixels;
	m_buffer = 0;
	m_pitch = dimensions.w*m_pixelFormat.bits/8;

	_initBuffer();


	glGenTextures( 1, &m_texture );
	glBindTexture( GL_TEXTURE_2D, m_texture );
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexImage2D( GL_TEXTURE_2D, 0, _format, m_size.w, m_size.h, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL );

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

}

WgSurfaceGL::WgSurfaceGL(GLuint _texture, Uint8 * _pAlpha )
{
	if( pglBufferDataARB == 0 )
		_initGlExtensions();

	_setPixelFormat( m_format );

	GLint width = 0;
	GLint height = 0;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );

	m_texture	= _texture;
	m_size = WgSize(width, height);
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &m_format );
	m_pAlpha	= _pAlpha;
	m_pPixels	= 0;
	m_buffer = 0;
	m_pitch = width*m_pixelFormat.bits/8;

	_initBuffer();

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
}

void WgSurfaceGL::_initBuffer()
{
	if( m_pPixels == 0 )
		return;

	int size = m_size.w*m_size.h;

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

void WgSurfaceGL::_setPixelFormat( GLint _format )
{
	WgPixelType pixeltype = WG_PIXEL_UNKNOWN;

	switch( _format )
	{
	case GL_RGB8:
		m_pixelSize = 3;
		pixeltype = WG_PIXEL_RGB_8;
		break;

	case GL_RGBA8:
		m_pixelSize = 4;
		pixeltype = WG_PIXEL_RGBA_8;
		break;

	default:
		m_pixelSize = 0;		// Signal unknown pixelsize.
		break;
	}

	WgUtil::PixelTypeToFormat(pixeltype, m_pixelFormat);
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

//____ Type() __________________________________________________________________

const char * WgSurfaceGL::Type() const
{
	return GetMyType();
}

//____ GetMyType() _____________________________________________________________

const char * WgSurfaceGL::GetMyType()
{
	return c_surfaceType;
}


//____ Size() ______________________________________________________________

WgSize WgSurfaceGL::Size() const
{
	return m_size;
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

void * WgSurfaceGL::Lock( WgAccessMode mode )
{
	if( m_format == 0 || m_accessMode != WG_NO_ACCESS || mode == WG_NO_ACCESS )
		return 0;

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );

	switch( mode )
	{
		case WG_READ_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );
			break;
		case WG_WRITE_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
			break;
		case WG_READ_WRITE:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB );
			break;
		default:
			break;	// Should never happen, just here to avoid compiler warnings...
	}

	m_lockRegion = WgRect(0,0,m_size);
	m_accessMode = mode;
	return m_pPixels;
}

//____ LockRegion() __________________________________________________________________

void * WgSurfaceGL::LockRegion( WgAccessMode mode, const WgRect& region )
{
	if( m_format == 0 || m_accessMode != WG_NO_ACCESS || mode == WG_NO_ACCESS )
		return 0;

	if( region.x + region.w > m_size.w || region.y + region.w > m_size.h || region.x < 0 || region.y < 0 )
		return 0;

	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );

	switch( mode )
	{
		case WG_READ_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );
			break;
		case WG_WRITE_ONLY:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB );
			break;
		case WG_READ_WRITE:
			m_pPixels = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_WRITE_ARB );
			break;
		default:
			break;	// Should never happen, just here to avoid compiler warnings...
	}

	m_lockRegion = region;
	m_accessMode = mode;
	return m_pPixels += (m_size.w*region.y+region.x)*m_pixelSize;
}


//____ Unlock() ________________________________________________________________

void WgSurfaceGL::Unlock()
{
	if(m_accessMode == WG_NO_ACCESS )
		return;

	pglUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );

	if( m_accessMode != WG_READ_ONLY )
	{
		glBindTexture( GL_TEXTURE_2D, m_texture );
//		pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );
		glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_size.w, m_size.h, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
//		glTexSubImage2D( GL_TEXTURE_2D, 0, m_lockRegion.x, m_lockRegion.y, m_lockRegion.w, m_lockRegion.h, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
	}
	pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );
	m_accessMode = WG_NO_ACCESS;
	m_pPixels = 0;
	m_lockRegion.w = 0;
	m_lockRegion.h = 0;
}


//____ GetPixel() ______________________________________________________________

Uint32 WgSurfaceGL::GetPixel( WgCoord coord ) const
{
	if( m_format != 0 && m_buffer && m_accessMode != WG_WRITE_ONLY )
	{
		Uint32 val;

		if( m_accessMode == WG_NO_ACCESS )
		{
			// Quick lock of surface

			pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );
			Uint8 * pPixel = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );

			//

			 pPixel += (m_size.w*coord.y+coord.x)*m_pixelSize;
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
			Uint8 * pPixel = m_pPixels + (m_size.w*coord.y+coord.x)*m_pixelSize;
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

Uint8 WgSurfaceGL::GetOpacity( WgCoord coord ) const
{
	return 255;

	if( m_buffer )
	{
		if( m_format == GL_RGBA8 && m_accessMode != WG_WRITE_ONLY )
		{
			Uint8 a;

			if( m_accessMode == WG_NO_ACCESS )
			{
				// Quick lock of surface

				pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_buffer );
				Uint8 * pPixel = (Uint8*) pglMapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, GL_READ_ONLY_ARB );

				//

				a = pPixel[(m_size.w*coord.y+coord.x)*m_pixelSize+3];

				// Quick unlock of surface

				pglUnmapBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB );
				pglBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

			}
			else
			{
				a = m_pPixels[(m_size.w*coord.y+coord.x)*m_pixelSize+3];
			}

			return a;
		}

	}
	else if( m_pAlpha )
	{
		return m_pAlpha[coord.y*m_size.w+coord.x];
	}

	return 255;
}


//____ WgSurfaceFactoryGL::CreateSurface() ___________________________________

WgSurface * WgSurfaceFactoryGL::CreateSurface( const WgSize& size, WgPixelType type ) const
{

	GLint	format;
	int		buffSize;



	switch( type )
	{
	case WG_PIXEL_RGB_8:
		format = GL_RGB8;
		buffSize = 3*size.w*size.h;
		break;

	case WG_PIXEL_RGBA_8:
		format = GL_RGBA8;
		buffSize = 4*size.w*size.h;
		break;

	default:
		return 0;

	}

	char * pBuffer = new char[buffSize];
	memset( pBuffer, 0, buffSize );

	WgSurfaceGL * p = new WgSurfaceGL( size, format, pBuffer );

	delete pBuffer;
	return 	p;


}
