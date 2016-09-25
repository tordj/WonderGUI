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
#	include <windows.h>
#endif

#ifdef __APPLE__
#	include <OpenGL/gl.h>
#else
#	include <GL/gl.h>
#endif

#ifndef WG_SURFACE_DOT_H
#	include <wg_surface.h>
#endif

namespace wg
{

	class GlSurface;
	typedef	StrongPtr<GlSurface,Surface_p>	GlSurface_p;
	typedef	WeakPtr<GlSurface,Surface_wp>	GlSurface_wp;

	//____ GlSurface _____________________________________________________________

	class GlSurface : public Surface
	{
		friend class GlSurfaceFactory;

	public:
        
        static GlSurface_p	create( Size size, PixelType type = PixelType::BGRA_8 );
        static GlSurface_p	create( Size size, PixelType type, const Blob_p& pBlob, int pitch );
        static GlSurface_p	create( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat = 0 );
        static GlSurface_p	create( const Surface_p& pOther );
       
        
		bool				isInstanceOf( const char * pClassName ) const;
		const char *		className( void ) const;
		static const char	CLASSNAME[];
		static GlSurface_p	cast( const Object_p& pObject );

		inline	GLuint	getTexture() const { return m_texture; }

		// Methods needed by Surface

		Size		size() const;
		bool		isOpaque() const;

		uint32_t	pixel( Coord coord ) const;
		uint8_t		alpha( Coord coord ) const;

		void *		lock( AccessMode mode );
		void *		lockRegion( AccessMode mode, const Rect& region );
		void		unlock();

	private:
        GlSurface( Size size, PixelType type = PixelType::BGRA_8 );
        GlSurface( Size size, PixelType type, const Blob_p& pBlob, int pitch );
        GlSurface( Size size, PixelType type, uint8_t * pPixels, int pitch, const PixelFormat * pPixelFormat );
        GlSurface( const Surface_p& pOther );
		~GlSurface();


		void		_setPixelDetails( PixelType type );
		void		_initBuffer();

        GLuint 		m_texture;			// GL texture handle.
        GLint       m_internalFormat;   // GL_RGB8 or GL_RGBA8.
        GLenum		m_accessFormat;		// GL_BGR or GL_BGRA.
        Blob_p      m_pBlob;
        
		GLuint		m_buffer;			// Pointer at GL pixel buffer, if any.
		Size		m_size;				// Width and height in pixels.
		uint32_t	m_pixelSize;		// Size in bytes of a pixel.

	};
} // namespace wg
#endif //WG_GLSURFACE_DOT_H

