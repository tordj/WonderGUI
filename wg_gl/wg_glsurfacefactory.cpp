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
#include <wg_glsurfacefactory.h>
#include <wg_util.h>

#ifdef WIN32
#	include <glext.h>
#else
#	include <GL/glext.h>
#	include <GL/glx.h>
#endif
//#include <GL/glu.h>


const char WgGLSurfaceFactory::CLASSNAME[] = {"GLSurfaceFactory"};

//____ IsInstanceOf() _________________________________________________________

bool WgGLSurfaceFactory::IsInstanceOf( const char * pClassName ) const
{ 
	if( pClassName==CLASSNAME )
		return true;

	return WgSurfaceFactory::IsInstanceOf(pClassName);
}

//____ ClassName() ____________________________________________________________

const char * WgGLSurfaceFactory::ClassName( void ) const
{ 
	return CLASSNAME; 
}

//____ Cast() _________________________________________________________________

WgGLSurfaceFactoryPtr WgGLSurfaceFactory::Cast( const WgObjectPtr& pObject )
{
	if( pObject && pObject->IsInstanceOf(CLASSNAME) )
		return WgGLSurfaceFactoryPtr( static_cast<WgGLSurfaceFactory*>(pObject.GetRealPtr()) );

	return 0;
}

//____ CreateSurface() ________________________________________________________

WgSurfacePtr WgGLSurfaceFactory::CreateSurface( const WgSize& size, WgPixelType type ) const
{
	GLint	format;
	int		buffSize;

	switch( type )
	{
	case WG_PIXEL_RGB_8:
		format = GL_RGB8;
		buffSize = 3*size.w*size.h;
		break;

	case WG_PIXEL_ARGB_8:
		format = GL_RGBA8;
		buffSize = 4*size.w*size.h;
		break;

	default:
		return 0;
	}

	char * pBuffer = new char[buffSize];
	memset( pBuffer, 0, buffSize );

	WgGLSurface * p = new WgGLSurface( size, format, pBuffer );

	delete pBuffer;
	return 	p;
}
