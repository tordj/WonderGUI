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
#		include <OpenGL/gl3.h>
#	else
#		include <GL/glew.h>
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
		friend class GlGfxDevice;

	public:

		//.____ Creation __________________________________________

		static GlSurface_p	create( SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		static GlSurface_p	create( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		static GlSurface_p	create( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription = 0, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		static GlSurface_p	create( Surface * pOther, int flags = SurfaceFlag::Static );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Geometry _________________________________________________

		static SizeI	maxSize();

		//.____ Appearance ____________________________________________________

		void		setScaleMode(ScaleMode mode) override;
		bool		isOpaque() const override;

		void		setTiling(bool bTiling) override;

		//.____ Content _______________________________________________________

		uint8_t		alpha(CoordI coord) override;

		//.____ Control _______________________________________________________

		const PixelBuffer	allocPixelBuffer(const RectI& rect) override;
		bool				pushPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				pullPixels(const PixelBuffer& buffer, const RectI& bufferRect) override;
		void				freePixelBuffer(const PixelBuffer& buffer) override;

		bool		unload();
		bool		isLoaded();
		void		reload();


		//.____ Misc __________________________________________________________

		inline	GLuint	getTexture() const { return m_texture; }
		inline	GLuint	getClutTexture() const { return m_clutTexture; }

        static unsigned int totalTexturePixels() { return g_texturePixels; }
        static unsigned int totalBackingPixels() { return g_backingPixels; }

	private:
		GlSurface( SizeI size, PixelFormat format = PixelFormat::BGRA_8, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		GlSurface( SizeI size, PixelFormat format, Blob * pBlob, int pitch, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		GlSurface( SizeI size, PixelFormat format, uint8_t * pPixels, int pitch, const PixelDescription * pPixelDescription, int flags = SurfaceFlag::Static, const Color * pClut = nullptr);
		GlSurface( Surface * pOther, int flags = SurfaceFlag::Static );
		~GlSurface();


		void		_setPixelDetails( PixelFormat format );
		void		_setupGlTexture( void * pPixelsToUpload, int flags);
		void		_updateAlphaMap(const PixelBuffer& buffer, const RectI& bufferRect);

		int			m_bPendingReads = false;					// Set if there are queued GL commands that will use surface as source. Active GlGfxDevice needs to be flushed before we modify.
		bool		m_bBackingBufferStale = false;				// Set when there are modifications (in texture or queued GL commands) for this surface.
		bool        m_bMipmapStale = false;

		void		_readBackTexture( void * pDest);


		GLuint 		m_clutTexture = 0;	// GL clut texture handle.

		GLuint 		m_texture;			// GL texture handle.
		GLint       m_internalFormat;   // GL_RGB8 or GL_RGBA8.
		GLenum		m_accessFormat;		// GL_BGR or GL_BGRA.
		Blob_p      m_pBlob;
		int			m_pitch;

		uint32_t	m_pixelSize;		// Size in bytes of a pixel.
		GLenum		m_pixelDataType;
		static SizeI	s_maxSize;

        static unsigned int    g_texturePixels;
        static unsigned int    g_backingPixels;

        
		uint8_t*	m_pAlphaMap = nullptr;
	};
} // namespace wg
#endif //WG_GLSURFACE_DOT_H

