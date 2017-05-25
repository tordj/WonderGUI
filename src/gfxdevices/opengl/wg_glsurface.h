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
#pragma once

#define GL_GLEXT_PROTOTYPES 1



#ifdef WIN32
#	include <GL/glew.h>
#else
#	ifdef __APPLE__
#		include <OpenGL/gl.h>
#	else
#		include <GL/gl.h>
#	endif
#endif

#include <wg_surface.h>

namespace wg
{

	class GlSurface;
	typedef	StrongPtr<GlSurface>	GlSurface_p;
	typedef	WeakPtr<GlSurface>	GlSurface_wp;

	//____ GlSurface _____________________________________________________________

	class GlSurface : public Surface
	{
		friend class GlSurfaceFactory;

	public:

		//.____ Creation __________________________________________

        static GlSurface_p	create( Size size, PixelType type = PixelType::BGRA_8, SurfaceHint hint = SurfaceHint::Static );
        static GlSurface_p	create( Size size, PixelType type, Blob * pBlob, int pitch, SurfaceHint hint = SurfaceHint::Static );
        static GlSurface_p	create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat = 0, SurfaceHint hint = SurfaceHint::Static );
        static GlSurface_p	create( Surface * pOther, SurfaceHint hint = SurfaceHint::Static );
       
		//.____ Identification __________________________________________

		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static GlSurface_p	cast( Object * pObject );

		//.____ Geometry _________________________________________________

		Size		size() const;
		static Size	maxSize();

		//.____ Appearance ____________________________________________________

		void		setScaleMode(ScaleMode mode);
		bool		isOpaque() const;

		//.____ Content _______________________________________________________

		uint32_t	pixel(Coord coord) const;
		uint8_t		alpha(Coord coord) const;

		//.____ Control _______________________________________________________

		uint8_t *	lock(AccessMode mode);
		uint8_t *	lockRegion(AccessMode mode, const Rect& region);
		void		unlock();

		//.____ Misc __________________________________________________________

		inline	GLuint	getTexture() const { return m_texture; }

	private:
        GlSurface( Size size, PixelType type = PixelType::BGRA_8, SurfaceHint hint = SurfaceHint::Static );
        GlSurface( Size size, PixelType type, Blob * pBlob, int pitch, SurfaceHint hint = SurfaceHint::Static );
        GlSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat, SurfaceHint hint = SurfaceHint::Static );
        GlSurface( Surface * pOther, SurfaceHint hint = SurfaceHint::Static );
		~GlSurface();


		void		_setPixelDetails( PixelType type );
		void		_initBuffer();

        GLuint 		m_texture;			// GL texture handle.
        GLint       m_internalFormat;   // GL_RGB8 or GL_RGBA8.
        GLenum		m_accessFormat;		// GL_BGR or GL_BGRA.
        Blob_p      m_pBlob;
		bool		m_bDynamic;			// Set if surface is dynamic, thus having a GL pixel buffer.
		
		GLuint		m_buffer;			// Pointer at GL pixel buffer, if any.
		Size		m_size;				// Width and height in pixels.
		uint32_t	m_pixelSize;		// Size in bytes of a pixel.

	};
} // namespace wg
#endif //WG_GLSURFACE_DOT_H

