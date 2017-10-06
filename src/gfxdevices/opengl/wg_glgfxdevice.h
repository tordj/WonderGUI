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
#ifndef WG_GLGFXDEVICE_DOT_H
#define WG_GLGFXDEVICE_DOT_H
#pragma once

#include <wg_geo.h>

#ifdef WIN32
#	include <GL/glew.h>
#else
#	ifdef __APPLE__
#		include <OpenGL/gl3.h>
#	else
#		include <GLES3/gl3.h>
#	endif
#endif

#include <wg_gfxdevice.h>


namespace wg
{
	class GlSurface;
	class GlGfxDevice;
	typedef	StrongPtr<GlGfxDevice> GlGfxDevice_p;
	typedef	WeakPtr<GlGfxDevice>	GlGfxDevice_wp;

	class GlGfxDevice : public GfxDevice
	{
	public:

		//.____ Creation __________________________________________

		static GlGfxDevice_p	create( Size canvasSize );

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static GlGfxDevice_p	cast( Object * pObject );
		const char *			surfaceClassName( void ) const;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory();

		//.____ Geometry _________________________________________________

		bool	setCanvas( Size dimensions );
		bool	setCanvas( Surface * pCanvas );

		//.____ State _________________________________________________

		void	setTintColor( Color color ) override;
		bool	setBlendMode( BlendMode blendMode ) override;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;;
		bool	endRender() override;;

		void	fill( const Rect& rect, const Color& col ) override;

		void	clipDrawHorrLine( const Rect& clip, const Coord& start, int length, const Color& col ) override;
		void	clipDrawVertLine( const Rect& clip, const Coord& start, int length, const Color& col ) override;
 
        
        void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors) override;
        void    clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors) override;

		void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f ) override;
		void	clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f ) override;

		void	clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine * pTopLine, const WaveLine * pBottomLine, Color front, Color back);


		void	blit( Surface * src, const Rect& srcrect, Coord dest  ) override;

		void	stretchBlitSubPixel( Surface * pSrc, float sx, float sy, float sw, float sh,
									 float dx, float dy, float dw, float dh ) override;

		void	fillSubPixel( const RectF& rect, const Color& col ) override;

		void	stretchBlitSubPixelWithInvert(Surface * pSrc, float sx, float sy, float sw, float sh,
			float dx, float dy, float dw, float dh);

	protected:
		GlGfxDevice( Size canvas );
		~GlGfxDevice();

        void	_initTables();
		void	_setBlendMode( BlendMode blendMode );

        GLuint  _createGLProgram( const char * pVertexShader, const char * pFragmentShader );
		void	_updateProgramDimensions();
		bool	_setFramebuffer();
       
        SurfaceFactory_p	m_pSurfaceFactory;
	    float	_scaleThickness( float thickeness, float slope );
        
	    bool	m_bRendering;

        float	m_lineThicknessTable[17];

		GLuint		m_framebufferId;
		bool		m_bFlipY;

		Size		m_defaultFramebufferSize;


        // Device programs
        
        GLuint  m_fillProg;
        GLint   m_fillProgColorLoc;

        GLuint  m_aaFillProg;
        GLint   m_aaFillProgColorLoc;
        GLint   m_aaFillProgFrameLoc;
        GLint   m_aaFillProgOutsideAALoc;

        GLuint  m_blitProg;
        GLint   m_blitProgTintLoc;
        GLint   m_blitProgTexIdLoc;
        
        GLuint  m_plotProg;
        GLint   m_plotProgTintLoc;

        GLuint  m_mildSlopeProg;
        GLint   m_mildSlopeProgColorLoc;
        GLint   m_mildSlopeProgSLoc;
        GLint   m_mildSlopeProgWLoc;
        GLint   m_mildSlopeProgSlopeLoc;

        GLuint  m_steepSlopeProg;
        GLint   m_steepSlopeProgColorLoc;
        GLint   m_steepSlopeProgSLoc;
        GLint   m_steepSlopeProgWLoc;
        GLint   m_steepSlopeProgSlopeLoc;

		GLuint  m_horrWaveProg;
		GLuint	m_horrWaveBufferTexture;
		GLuint	m_horrWaveBufferTextureData;
		GLint	m_horrWaveProgTexIdLoc;
		GLint	m_horrWaveProgWindowOfsLoc;
		GLint	m_horrWaveProgTopBorderColorLoc;
		GLint	m_horrWaveProgBottomBorderColorLoc;
		GLint	m_horrWaveProgFrontFillLoc;
		GLint	m_horrWaveProgBackFillLoc;

        GLuint  m_vertexArrayId;
        GLuint  m_vertexBufferId;
        GLfloat m_vertexBufferData[8];         // Space to store a quad (through triangle strip)
        
        GLuint  m_texCoordArrayId;
        GLuint  m_texCoordBufferId;
        GLfloat m_texCoordBufferData[8];         // Space to store UV for a quad
        
        
		// GL states saved between BeginRender() and EndRender().

		GLboolean	m_glDepthTest;
        GLboolean   m_glScissorTest;
        GLboolean	m_glBlendEnabled;
		GLint		m_glBlendSrc;
		GLint		m_glBlendDst;
		GLint		m_glViewport[4];
		GLint		m_glScissorBox[4];
		GLint		m_glReadFrameBuffer;
		GLint		m_glDrawFrameBuffer;
		Size		m_size;

	};
} // namespace wg
#endif //WG_GLGFXDEVICE_DOT_H

