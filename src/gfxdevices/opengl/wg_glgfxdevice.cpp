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

#include <cmath>
#include <cstdlib>

#include <wg_glgfxdevice.h>
#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>
#include <wg_base.h>
#include <wg_util.h>
#include <assert.h>

using namespace std;

namespace wg
{
	const char GlGfxDevice::CLASSNAME[] = { "GlGfxDevice" };

	GlGfxDevice *	GlGfxDevice::s_pActiveDevice = nullptr;


	//____ create() _______________________________________________________________

	GlGfxDevice_p GlGfxDevice::create( const Rect& viewport, int uboBindingPoint)
	{
		GlGfxDevice_p p(new GlGfxDevice( viewport, uboBindingPoint ));

		GLenum err = glGetError();
		if( err != 0 )
			return GlGfxDevice_p(nullptr);

		return p;
	}

	GlGfxDevice_p GlGfxDevice::create( GlSurface * pSurface, int uboBindingPoint)
	{
		GlGfxDevice_p p(new GlGfxDevice(pSurface, uboBindingPoint ));

		GLenum err = glGetError();
		if (err != 0)
			return GlGfxDevice_p(nullptr);

		return p;
	}
	//____ Constructor _____________________________________________________________

	GlGfxDevice::GlGfxDevice(GlSurface * pSurface, int uboBindingPoint) : GlGfxDevice(pSurface->size(), uboBindingPoint)
	{
		setCanvas(pSurface);
	}

	GlGfxDevice::GlGfxDevice(const Rect& viewport, int uboBindingPoint) : GlGfxDevice(viewport.size(), uboBindingPoint)
	{
		setCanvas(viewport);
	}

	GlGfxDevice::GlGfxDevice( Size viewportSize, int uboBindingPoint ) : GfxDevice(viewportSize)
	{
		m_canvasYstart = viewportSize.h;
		m_canvasYmul = -1;

		_initTables();

		GLint	texIdLoc, extrasIdLoc, clutIdLoc;

		// Create and init Fill shader

		m_fillProg			= _createGLProgram(fillVertexShader, fillFragmentShader);

		unsigned int canvasIndex = glGetUniformBlockIndex(m_fillProg, "Canvas");
		glUniformBlockBinding(m_fillProg, canvasIndex, uboBindingPoint);

		assert(glGetError() == 0);

		// Create and init AA-Fill shader

		m_aaFillProg		= _createGLProgram(aaFillVertexShader, aaFillFragmentShader);
		canvasIndex			= glGetUniformBlockIndex(m_aaFillProg, "Canvas");
		glUniformBlockBinding(m_aaFillProg, canvasIndex, uboBindingPoint);

		extrasIdLoc			= glGetUniformLocation(m_aaFillProg, "extrasId");
		glUseProgram(m_aaFillProg);
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

		assert(glGetError() == 0);

		// Create and init Blit shader

		m_blitProg			= _createGLProgram(blitVertexShader, blitFragmentShader);
		canvasIndex			= glGetUniformBlockIndex(m_blitProg, "Canvas");
		glUniformBlockBinding(m_blitProg, canvasIndex, uboBindingPoint);
		m_blitProgTexSizeLoc = glGetUniformLocation(m_blitProg, "texSize");

		texIdLoc			= glGetUniformLocation(m_blitProg, "texId");
		extrasIdLoc			= glGetUniformLocation(m_blitProg, "extrasId");
		glUseProgram(m_blitProg);
		glUniform1i(texIdLoc, 0);			// Needs to be set. Texture unit 0 is used for textures.
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

		assert(glGetError() == 0);

		// Create and init AlphaBlit shader (shader program for blitting from alpha-only texture)

		m_alphaBlitProg = _createGLProgram(blitVertexShader, alphaBlitFragmentShader);
		canvasIndex = glGetUniformBlockIndex(m_alphaBlitProg, "Canvas");
		glUniformBlockBinding(m_alphaBlitProg, canvasIndex, uboBindingPoint);
		m_alphaBlitProgTexSizeLoc = glGetUniformLocation(m_alphaBlitProg, "texSize");

		texIdLoc = glGetUniformLocation(m_alphaBlitProg, "texId");
		extrasIdLoc = glGetUniformLocation(m_alphaBlitProg, "extrasId");
		glUseProgram(m_alphaBlitProg);
		glUniform1i(texIdLoc, 0);			// Needs to be set. Texture unit 0 is used for textures.
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

		assert(glGetError() == 0);


		// Create and init Clut Blit shaders

		m_clutBlitNearestProg = _createGLProgram(clutBlitNearestVertexShader, clutBlitNearestFragmentShader);
		canvasIndex = glGetUniformBlockIndex(m_clutBlitNearestProg, "Canvas");
		glUniformBlockBinding(m_clutBlitNearestProg, canvasIndex, uboBindingPoint);
		m_clutBlitNearestProgTexSizeLoc = glGetUniformLocation(m_clutBlitNearestProg, "texSize");

		texIdLoc = glGetUniformLocation(m_clutBlitNearestProg, "texId");
		extrasIdLoc = glGetUniformLocation(m_clutBlitNearestProg, "extrasId");
		clutIdLoc = glGetUniformLocation(m_clutBlitNearestProg, "clutId");

		glUseProgram(m_clutBlitNearestProg);
		glUniform1i(texIdLoc, 0);			// Needs to be set. Texture unit 0 is used for textures.
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.
		glUniform1i(clutIdLoc, 2);			// Needs to be set. Texture unit 2 is used for CLUT.

		assert(glGetError() == 0);

		m_clutBlitInterpolateProg = _createGLProgram(clutBlitInterpolateVertexShader, clutBlitInterpolateFragmentShader);
		canvasIndex = glGetUniformBlockIndex(m_clutBlitInterpolateProg, "Canvas");
		glUniformBlockBinding(m_clutBlitInterpolateProg, canvasIndex, uboBindingPoint);
		m_clutBlitInterpolateProgTexSizeLoc = glGetUniformLocation(m_clutBlitInterpolateProg, "texSize");

		texIdLoc = glGetUniformLocation(m_clutBlitInterpolateProg, "texId");
		extrasIdLoc = glGetUniformLocation(m_clutBlitInterpolateProg, "extrasId");
		clutIdLoc = glGetUniformLocation(m_clutBlitInterpolateProg, "clutId");

		glUseProgram(m_clutBlitInterpolateProg);
		glUniform1i(texIdLoc, 0);			// Needs to be set. Texture unit 0 is used for textures.
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.
		glUniform1i(clutIdLoc, 2);			// Needs to be set. Texture unit 2 is used for CLUT.

		assert(glGetError() == 0);




		// Create and init Plot shader

		m_plotProg			= _createGLProgram(plotVertexShader, plotFragmentShader);
		canvasIndex			= glGetUniformBlockIndex(m_plotProg, "Canvas");
		glUniformBlockBinding(m_plotProg, canvasIndex, uboBindingPoint);

		assert(glGetError() == 0);

		// Create and init Line shader

		m_lineFromToProg	= _createGLProgram(lineFromToVertexShader, lineFromToFragmentShader);
		canvasIndex			= glGetUniformBlockIndex(m_lineFromToProg, "Canvas");
		glUniformBlockBinding(m_lineFromToProg, canvasIndex, uboBindingPoint);

		extrasIdLoc = glGetUniformLocation(m_lineFromToProg, "extrasId");
		glUseProgram(m_lineFromToProg);
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

		assert(glGetError() == 0);

		// Create and init Segment shaders

		for (int i = 1; i < c_maxSegments; i++)
		{
			GLuint prog = _createGLProgram(segmentVertexShaders[i], segmentFragmentShaders[i]);
			m_segmentsProg[i] = prog;
			unsigned int canvasIndex = glGetUniformBlockIndex(prog, "Canvas");
			glUniformBlockBinding(prog, canvasIndex, uboBindingPoint);

			extrasIdLoc = glGetUniformLocation(prog, "extrasId");
			GLint colorsIdLoc = glGetUniformLocation(prog, "colorsId");
			GLint stripesIdLoc = glGetUniformLocation(prog, "stripesId");

			glUseProgram(prog);
			glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.
			glUniform1i(colorsIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer, which doubles as the colors buffer.
			glUniform1i(stripesIdLoc, 1);		// Needs to be set. Texture unit 2 is used for segment stripes buffer.
		}

		assert(glGetError() == 0);

		// Create our Uniform Buffer

		glGenBuffers(1, &m_canvasUBOId);
		glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(canvasUBO), NULL, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, uboBindingPoint, m_canvasUBOId);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// Create the buffers we need for FrameBuffer and Vertices

		glGenFramebuffers(1, &m_framebufferId);

		glGenBuffers(1, &m_vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, c_vertexBufferSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

		// Create and initialize our VertexArray

		glGenVertexArrays(1, &m_vertexArrayId);
		glBindVertexArray(m_vertexArrayId);

		glVertexAttribIPointer(
			0,						// attribute number, must match the layout in the shader.
			2,						// size
			GL_INT,					// type
			sizeof(Vertex),			// stride
			(void*)0				// array buffer offset
		);

		glVertexAttribPointer(
			1,						// attribute number, must match the layout in the shader.
			GL_BGRA,				// size
			GL_UNSIGNED_BYTE,		// type
			GL_TRUE,				// normalized?
			sizeof(Vertex),			// stride
			(void*)sizeof(Coord)	// array buffer offset
		);

		assert(glGetError() == 0);

		glVertexAttribIPointer(
			2,                  // attribute number, must match the layout in the shader.
			1,                  // size
			GL_INT,           // type
			sizeof(Vertex),		// stride
			(void*)(sizeof(Coord)+sizeof(Color))  // array buffer offset
		);

		glVertexAttribPointer(
			3,						// attribute number, must match the layout in the shader.
			2,						// size
			GL_FLOAT,				// type
			GL_TRUE,				// normalized?
			sizeof(Vertex),			// stride
			(void*)(sizeof(Coord) + sizeof(Color) + sizeof(int) )  // array buffer offset
		);


		assert(glGetError() == 0);

		// Create a TextureBufferObject for providing extra data to our shaders

		glGenBuffers(1, &m_extrasBufferId);
		glBindBuffer(GL_TEXTURE_BUFFER, m_extrasBufferId);
		glBufferData(GL_TEXTURE_BUFFER, c_extrasBufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

		assert(glGetError() == 0);

		glGenTextures(1, &m_extrasBufferTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, m_extrasBufferTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_extrasBufferId);

		assert(glGetError() == 0);
	}

	//____ Destructor ______________________________________________________________

	GlGfxDevice::~GlGfxDevice()
	{
		glDeleteProgram(m_fillProg);
		glDeleteProgram(m_aaFillProg);
		glDeleteProgram(m_blitProg);
		glDeleteProgram(m_plotProg);
		glDeleteProgram(m_lineFromToProg);

		for( int i = 1 ; i < c_maxSegments ; i++ )
			glDeleteProgram(m_segmentsProg[i]);

		glDeleteFramebuffers(1, &m_framebufferId);
		glDeleteTextures(1, &m_extrasBufferTex);

		glDeleteBuffers(1, &m_vertexBufferId);
		glDeleteBuffers(1, &m_extrasBufferId);

		glDeleteVertexArrays(1, &m_vertexArrayId);
	}

	//____ isInstanceOf() _________________________________________________________

	bool GlGfxDevice::isInstanceOf( const char * pClassName ) const
	{
		if( pClassName==CLASSNAME )
			return true;

		return GfxDevice::isInstanceOf(pClassName);
	}

	//____ className() ____________________________________________________________

	const char * GlGfxDevice::className( void ) const
	{
		return CLASSNAME;
	}

	//____ cast() _________________________________________________________________

	GlGfxDevice_p GlGfxDevice::cast( Object * pObject )
	{
		if( pObject && pObject->isInstanceOf(CLASSNAME) )
			return GlGfxDevice_p( static_cast<GlGfxDevice*>(pObject) );

		return 0;
	}

	//____ surfaceClassName() _______________________________________________________

	const char * GlGfxDevice::surfaceClassName( void ) const
	{
		return GlSurface::CLASSNAME;
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p GlGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = GlSurfaceFactory::create();

		return m_pSurfaceFactory;
	}

	//____ setCanvas() __________________________________________________________________

	bool GlGfxDevice::setCanvas( Size canvasSize )
	{
		// Do NOT add any gl-calls here, INCLUDING glGetError()!!!
		// This method can be called without us having our GL-context.

		if (canvasSize.w < 1 || canvasSize.h < 1)
			return false;

		m_pCanvas = nullptr;
		m_canvasSize = canvasSize;
		m_clipCanvas = m_canvasSize;
		m_clipBounds = m_canvasSize;
		m_pClipRects = &m_clipCanvas;
		m_nClipRects = 1;

		m_canvasYstart = canvasSize.h;
		m_canvasYmul = -1;

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::SetCanvas, 2);
			m_commandBuffer[m_commandOfs++] = m_canvasSize.w;
			m_commandBuffer[m_commandOfs++] = m_canvasSize.h;
			m_surfaceBuffer[m_surfaceOfs++] = nullptr;
		}

        m_emptyCanvasSize = canvasSize;
        
		return true;
	}

	bool GlGfxDevice::setCanvas( Surface * pSurface )
	{
		// Do NOT add any gl-calls here, INCLUDING glGetError()!!!
		// This method can be called without us having our GL-context.

        if( pSurface == nullptr )
            return setCanvas( m_emptyCanvasSize );
        
		if (!pSurface || pSurface->className() != GlSurface::CLASSNAME)
			return false;

		m_pCanvas = pSurface;
		m_canvasSize = pSurface->size();
		m_clipCanvas = m_canvasSize;
		m_clipBounds = m_canvasSize;
		m_pClipRects = &m_clipCanvas;
		m_nClipRects = 1;

		m_canvasYstart = 0;
		m_canvasYmul = 1;

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::SetCanvas, 2);
			m_commandBuffer[m_commandOfs++] = m_canvasSize.w;
			m_commandBuffer[m_commandOfs++] = m_canvasSize.h;
			m_surfaceBuffer[m_surfaceOfs++] = static_cast<GlSurface*>(pSurface);
		}

		return true;
	}

	//____ setClipList() ______________________________________________________

	bool GlGfxDevice::setClipList(int nRectangles, const Rect * pRectangles)
	{
		if (GfxDevice::setClipList(nRectangles, pRectangles))
		{
			m_clipCurrOfs = -1;
			return true;
		}
		return false;
	}

	//____ clearClipList() ____________________________________________________

	void GlGfxDevice::clearClipList()
	{
		GfxDevice::clearClipList();
		m_clipCurrOfs = -1;
	}

	//____ setTintColor() __________________________________________________________________

	void GlGfxDevice::setTintColor(Color color)
	{
		GfxDevice::setTintColor(color);
	}

	//____ setBlendMode() __________________________________________________________________

	bool GlGfxDevice::setBlendMode(BlendMode blendMode)
	{
		if (blendMode == m_blendMode)
			return true;

		GfxDevice::setBlendMode(blendMode);

		if( m_bRendering )
		{
			_endCommand();
			_beginStateCommand(Command::SetBlendMode, 1);
			m_commandBuffer[m_commandOfs++] = (int) blendMode;
		}

		return true;
	}

	//____ setBlitSource() __________________________________________________________________

	bool GlGfxDevice::setBlitSource(Surface * pSource)
	{
		if (!pSource || pSource->className() != GlSurface::CLASSNAME)
			return false;

		m_pBlitSource = pSource;

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::SetBlitSource, 0);
			m_surfaceBuffer[m_surfaceOfs++] = static_cast<GlSurface*>(pSource);
		}

		return true;
	}


	//____ beginRender() ___________________________________________________________

	bool GlGfxDevice::beginRender()
	{
		assert( glGetError() == 0 );

		if( m_bRendering == true )
			return false;

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		// Remember GL states so we can restore in EndRender()

		m_glDepthTest 		= glIsEnabled(GL_DEPTH_TEST);
		m_glScissorTest 	= glIsEnabled(GL_SCISSOR_TEST);
		m_glBlendEnabled  	= glIsEnabled(GL_BLEND);
		glGetIntegerv(GL_BLEND_SRC, &m_glBlendSrc);
		glGetIntegerv(GL_BLEND_DST, &m_glBlendDst);
		glGetIntegerv(GL_VIEWPORT, m_glViewport);
		glGetIntegerv(GL_SCISSOR_BOX, m_glScissorBox);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_glReadFrameBuffer);
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_glDrawFrameBuffer);


		//  Modify states

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);


		// If there already is an active device, that needs to be flushed before we
		// take over the role as the active device.

		if (s_pActiveDevice)
			s_pActiveDevice->flush();

		m_pPrevActiveDevice = s_pActiveDevice;
		s_pActiveDevice = this;

		//

		m_bRendering = true;
		m_cmd = Command::None;
		m_pCmdFinalizer = &GlGfxDevice::_dummyFinalizer;
		m_vertexOfs = 0;
		m_extrasOfs = 0;
		m_commandOfs = 0;
		m_surfaceOfs = 0;
		m_clipWriteOfs = 0;
		m_clipCurrOfs = -1;

		_setCanvas( static_cast<GlSurface*>(m_pCanvas.rawPtr()), m_canvasSize.w, m_canvasSize.h );
		_setBlendMode(m_blendMode);
		_setBlitSource( static_cast<GlSurface*>(m_pBlitSource.rawPtr()) );

		// Prepare for rendering

		glBindVertexArray(m_vertexArrayId);


		glFinish();


		//

		assert( glGetError() == 0 );
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool GlGfxDevice::endRender()
	{
		assert( glGetError() == 0 );
		if( m_bRendering == false )
			return false;

		// Finalize any ongoing operation

		_endCommand();
		_executeBuffer();

		//

//        glFlush();
		glFinish();

		// Restore render states from before beginRender()

		if( m_glDepthTest )
			glEnable(GL_DEPTH_TEST);

		if( !m_glScissorTest )
			glDisable(GL_SCISSOR_TEST);

		if( m_glBlendEnabled )
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		glBlendFunc( m_glBlendSrc, m_glBlendDst );
		glViewport(m_glViewport[0], m_glViewport[1], m_glViewport[2], m_glViewport[3]);
		glScissor(m_glScissorBox[0], m_glScissorBox[1], m_glScissorBox[2], m_glScissorBox[3]);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_glReadFrameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_glDrawFrameBuffer);

		glBindVertexArray(0);


		//

		assert(glGetError() == 0);
		m_bRendering = false;

		// Restore previously active device and exit

		s_pActiveDevice = m_pPrevActiveDevice;
		return true;
	}

	//____ flush() _______________________________________________________________

	void GlGfxDevice::flush()
	{
		// Finalize any ongoing operation

		_endCommand();
		_executeBuffer();

		//

		glFlush();
	}



	//____ fill() ____ [standard] __________________________________________________

	void GlGfxDevice::fill(const Rect& rect, const Color& col)
	{
		assert(glGetError() == 0);

		// Skip calls that won't affect destination

		if (col.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		if (!rect.intersectsWith(m_clipBounds))
			return;

		//

		Color fillColor = col * m_tintColor;

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects )
		{
			_endCommand();
			_executeBuffer();
			_beginDrawCommand(Command::Fill);
		}
		else if (m_cmd != Command::Fill)
		{
			_endCommand();
			_beginDrawCommand(Command::Fill);
		}

		for (int i = 0; i < m_nClipRects; i++)
		{
			Rect patch(m_pClipRects[i], rect);
			if (patch.w > 0 && patch.h > 0)
			{
				int	dx1 = patch.x;
				int	dy1 = patch.y;
				int dx2 = patch.x + patch.w;
				int dy2 = patch.y + patch.h;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexOfs++;
			}
		}
		assert(glGetError() == 0);
	}

	//____ fill() ____ [subpixel] __________________________________________________

	void GlGfxDevice::fill(const RectF& rect, const Color& col)
	{
		assert(glGetError() == 0);

		// Skip calls that won't affect destination

		if (col.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		// Create our outer rectangle

		Rect outerRect( (int) rect.x, (int) rect.y, ((int) (rect.x+rect.w+0.999f)) - (int) rect.x, ((int) (rect.y + rect.h + 0.999f)) - (int) rect.y );


		Rect clip(outerRect, m_clipBounds);
		if (clip.w == 0 || clip.h == 0)
			return;

		//

		Color fillColor = col * m_tintColor;
		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 4)
		{
			_endCommand();
			_executeBuffer();
			_beginDrawCommand(Command::FillSubPixel);
		}
		else if (m_cmd != Command::FillSubPixel)
		{
			_endCommand();
			_beginDrawCommand(Command::FillSubPixel);
		}

		for (int i = 0; i < m_nClipRects; i++)
		{
			Rect patch(m_pClipRects[i], outerRect);
			if (patch.w > 0 && patch.h > 0)
			{
				int	dx1 = patch.x;
				int	dy1 = patch.y;
				int dx2 = patch.x + patch.w;
				int dy2 = patch.y + patch.h;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;
			}
		}

		// Provide rectangle center and radius

		SizeF	radius(rect.w / 2, rect.h / 2);
		CoordF	center(rect.x + radius.w, rect.y + radius.h);

		m_extrasBufferData[m_extrasOfs++] = center.x;
		m_extrasBufferData[m_extrasOfs++] = center.y;
		m_extrasBufferData[m_extrasOfs++] = radius.w;
		m_extrasBufferData[m_extrasOfs++] = radius.h;

		assert(glGetError() == 0);
	}

	//____ plotPixels() ______________________________________________________

	void GlGfxDevice::plotPixels(int nPixels, const Coord * pCoords, const Color * pColors)
	{
		assert(glGetError() == 0);

		if (nPixels == 0)
			return;

		if (m_vertexOfs > c_vertexBufferSize - 1 )
		{
			_endCommand();
			_executeBuffer();
			_beginDrawCommand(Command::Plot);
		}
		if (m_cmd != Command::Plot)
		{
			_endCommand();
			_beginDrawCommand(Command::Plot);
		}

		for (int i = 0; i < m_nClipRects; i++)
		{
			const Rect& clip = m_pClipRects[i];
			for (int pixel = 0; pixel < nPixels; pixel++)
			{
				if (clip.contains(pCoords[pixel]))
				{
					m_vertexBufferData[m_vertexOfs].coord = pCoords[pixel];
					m_vertexBufferData[m_vertexOfs].color = pColors[pixel] * m_tintColor;
					m_vertexOfs++;

					if (m_vertexOfs == c_vertexBufferSize)
					{
						_endCommand();
						_executeBuffer();
						_beginDrawCommand(Command::Plot);
					}
				}
			}
		}

		assert(glGetError() == 0);
	}

	//____ drawLine() ____ [from/to] __________________________________________________

	void GlGfxDevice::drawLine(Coord begin, Coord end, Color color, float thickness)
	{
		assert(glGetError() == 0);

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 4 || m_clipCurrOfs == -1 )
		{
			_endCommand();
			_executeBuffer();
			_beginClippedDrawCommand(Command::LineFromTo);
		}
		else if (m_cmd != Command::LineFromTo)
		{
			_endCommand();
			_beginClippedDrawCommand(Command::LineFromTo);
		}

		int 	length;
		float   width;

		float	slope;
		float	s, w;
		bool	bSteep;

		Color fillColor = color * m_tintColor;

		Coord	c1, c2, c3, c4;

		if (std::abs(begin.x - end.x) > std::abs(begin.y - end.y))
		{
			// Prepare mainly horizontal line segment

			if (begin.x > end.x)
				swap(begin, end);

			length = end.x - begin.x;
			if (length == 0)
				return;											// TODO: Should stil draw the caps!

			slope = ((float)(end.y - begin.y)) / length;
			width = _scaleThickness(thickness, slope);
			bSteep = false;

			s = m_canvasYstart + m_canvasYmul * ((begin.y + 0.5f) - (begin.x + 0.5f)*slope);
			w =  width / 2 + 0.5f;

			float   y1 = begin.y - width / 2;
			float   y2 = end.y - width / 2;

			c1.x = begin.x;
			c1.y = int(y1) - 1;
			c2.x = end.x;
			c2.y = int(y2) - 1;
			c3.x = end.x;
			c3.y = int(y2 + width) + 2;
			c4.x = begin.x;
			c4.y = int(y1 + width) + 2;
		}
		else
		{
			// Prepare mainly vertical line segment

			if (begin.y > end.y)
				swap(begin, end);

			length = end.y - begin.y;
			if (length == 0)
				return;											// TODO: Should stil draw the caps!

			slope = ((float)(end.x - begin.x)) / length;
			width = _scaleThickness(thickness, slope);
			bSteep = true;

//			s = (begin.x + 0.5f) - (m_canvasYstart + m_canvasYmul * (begin.y + 0.5f))*slope*m_canvasYmul;
			s = (begin.x + 0.5f) - (m_canvasYstart*m_canvasYmul + (begin.y + 0.5f))*slope;
			w = width / 2 + 0.5f;

			float   x1 = begin.x - width / 2;
			float   x2 = end.x - width / 2;

			c1.x = int(x1) - 1;
			c1.y = begin.y;
			c2.x = int(x1 + width) + 2;
			c2.y = begin.y;
			c3.x = int(x2 + width) + 2;
			c3.y = end.y;
			c4.x = int(x2) - 1;
			c4.y = end.y;


		}

		for (int i = 0; i < m_nClipRects; i++)
		{
			const Rect& patch = m_pClipRects[i];
			if (patch.w > 0 && patch.h > 0)
			{
				m_vertexBufferData[m_vertexOfs].coord = c1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord = c2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord = c3;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord = c1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord = c3;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord = c4;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
				m_vertexOfs++;
			}
		}

		m_extrasBufferData[m_extrasOfs++] = s;
		m_extrasBufferData[m_extrasOfs++] = w;
		m_extrasBufferData[m_extrasOfs++] = slope;
		m_extrasBufferData[m_extrasOfs++] = bSteep;

		assert(glGetError() == 0);
	}

	//____ drawLine() ____ [start/direction] __________________________________________________

	void GlGfxDevice::drawLine(Coord begin, Direction dir, int length, Color col, float thickness)
	{
		assert(glGetError() == 0);

		// Skip calls that won't affect destination

		if (col.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		// Create a rectangle from the line

		RectF rect;

		switch (dir)
		{
			case Direction::Up:
				rect.x = begin.x + 0.5f - thickness/2;
				rect.y = float(begin.y - length);
				rect.w = thickness;
				rect.h = float(length);
				break;

			case Direction::Down:
				rect.x = begin.x + 0.5f - thickness/2;
				rect.y = float(begin.y);
				rect.w = thickness;
				rect.h = float(length);
				break;

			case Direction::Left:
				rect.x = float(begin.x - length);
				rect.y = begin.y + 0.5f - thickness/2;
				rect.w = float(length);
				rect.h = thickness;
				break;

			case Direction::Right:
				rect.x = float(begin.x);
				rect.y = begin.y + 0.5f - thickness/2;
				rect.w = float(length);
				rect.h = thickness;
				break;
		}


		// Create our outer rectangle

		Rect outerRect((int)rect.x, (int)rect.y, ((int)(rect.x + rect.w + 0.999f)) - (int)rect.x, ((int)(rect.y + rect.h + 0.999f)) - (int)rect.y);

		// Clip our rectangle

		if (!outerRect.intersectsWith(m_clipBounds))
			return;

		//

		Color fillColor = col * m_tintColor;

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 4)
		{
			_endCommand();
			_executeBuffer();
			_beginDrawCommand(Command::FillSubPixel);
		}
		else if (m_cmd != Command::FillSubPixel)
		{
			_endCommand();
			_beginDrawCommand(Command::FillSubPixel);
		}


		// Provide the patches

		for (int i = 0; i < m_nClipRects; i++)
		{
			Rect patch(m_pClipRects[i], outerRect);
			if (patch.w > 0 && patch.h > 0)
			{
				int	dx1 = patch.x;
				int	dy1 = patch.y;
				int dx2 = patch.x + patch.w;
				int dy2 = patch.y + patch.h;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].color = fillColor;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;
			}
		}

		// Provide rectangle center and raidus.

		SizeF	radius(rect.w / 2, rect.h / 2);
		CoordF	center(rect.x + radius.w, rect.y + radius.h);

		m_extrasBufferData[m_extrasOfs++] = center.x;
		m_extrasBufferData[m_extrasOfs++] = center.y;
		m_extrasBufferData[m_extrasOfs++] = radius.w;
		m_extrasBufferData[m_extrasOfs++] = radius.h;


	}

	//____ transformBlit() ____ [simple] __________________________________________________

	void GlGfxDevice::transformBlit(const Rect& dest, Coord src, const int simpleTransform[2][2])
	{
		assert(glGetError() == 0);

		if (m_pBlitSource == nullptr)
			return;

		if (!dest.intersectsWith(m_clipBounds))
			return;

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 8 )
		{
			_endCommand();
			_executeBuffer();
			_beginDrawCommandWithSource(Command::Blit);
		}
		else if (m_cmd != Command::Blit)
		{
			_endCommand();
			_beginDrawCommandWithSource(Command::Blit);
		}

		for (int i = 0; i < m_nClipRects; i++)
		{
			Rect patch(m_pClipRects[i], dest);
			if (patch.w > 0 && patch.h > 0)
			{
				Vertex * pVertex = m_vertexBufferData + m_vertexOfs;

				int		dx1 = patch.x;
				int		dx2 = patch.x + patch.w;
				int		dy1 = patch.y;
				int		dy2 = patch.y + patch.h;
/*
				float	sx1 = (src.x + (patch.x - dest.x) * simpleTransform[0][0] + (patch.y - dest.y) * simpleTransform[1][0]) / sw;
				float	sy1 = (src.y + (patch.x - dest.x) * simpleTransform[0][1] + (patch.y - dest.y) * simpleTransform[1][1]) / sh;

				float	sx2 = (src.x + (patch.x + patch.w - dest.x) * simpleTransform[0][0] + (patch.y - dest.y) * simpleTransform[1][0]) / sw;
				float	sy2 = (src.y + (patch.x + patch.w - dest.x) * simpleTransform[0][1] + (patch.y - dest.y) * simpleTransform[1][1]) / sh;

				float	sx3 = (src.x + (patch.x + patch.w - dest.x) * simpleTransform[0][0] + (patch.y + patch.h - dest.y) * simpleTransform[1][0]) / sw;
				float	sy3 = (src.y + (patch.x + patch.w - dest.x) * simpleTransform[0][1] + (patch.y + patch.h - dest.y) * simpleTransform[1][1]) / sh;

				float	sx4 = (src.x + (patch.x - dest.x) * simpleTransform[0][0] + (patch.y + patch.h - dest.y) * simpleTransform[1][0]) / sw;
				float	sy4 = (src.y + (patch.x - dest.x) * simpleTransform[0][1] + (patch.y + patch.h - dest.y) * simpleTransform[1][1]) / sh;
*/

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs/4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				m_vertexOfs += 6;
			}
		}

		m_extrasBufferData[m_extrasOfs++] = (GLfloat) src.x;
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) src.y;
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) dest.x;
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) dest.y;

		m_extrasBufferData[m_extrasOfs++] = (GLfloat) simpleTransform[0][0];
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) simpleTransform[0][1];
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) simpleTransform[1][0];
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) simpleTransform[1][1];

		assert(glGetError() == 0);
	}

	//____ transformBlit() ____ [complex] __________________________________________________

	void GlGfxDevice::transformBlit(const Rect& dest, CoordF src, const float complexTransform[2][2])
	{
		assert(glGetError() == 0);

		if (m_pBlitSource == nullptr)
			return;

		if (!dest.intersectsWith(m_clipBounds))
			return;

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 8)
		{
			_endCommand();
			_executeBuffer();
			_beginDrawCommandWithSource(Command::Blit);
		}
		else if (m_cmd != Command::Blit)
		{
			_endCommand();
			_beginDrawCommandWithSource(Command::Blit);
		}

		//

		for (int i = 0; i < m_nClipRects; i++)
		{
			Rect patch(m_pClipRects[i], dest);
			if (patch.w > 0 && patch.h > 0)
			{
				Vertex * pVertex = m_vertexBufferData + m_vertexOfs;

				int		dx1 = patch.x;
				int		dx2 = patch.x + patch.w;
				int		dy1 = patch.y;
				int		dy2 = patch.y + patch.h;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				m_vertexOfs += 6;
			}
		}

		m_extrasBufferData[m_extrasOfs++] = src.x;
		m_extrasBufferData[m_extrasOfs++] = src.y;
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) dest.x;
		m_extrasBufferData[m_extrasOfs++] = (GLfloat) dest.y;

		m_extrasBufferData[m_extrasOfs++] = complexTransform[0][0];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[0][1];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[1][0];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[1][1];

		assert(glGetError() == 0);
	}

	//____ transformDrawSegments() ______________________________________________________

	void GlGfxDevice::transformDrawSegments( const Rect& _dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, const int simpleTransform[2][2] )
	{
		assert(glGetError() == 0);

		if (!_dest.intersectsWith(m_clipBounds))
			return;

		//

		int extrasSpaceNeeded = (4 + 4 * nSegments + 4 * (nEdgeStrips - 1)*(nSegments - 1) + 3) & 0xFFFFFFFC;		// Various data + colors + strips + alignment + margin for

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - extrasSpaceNeeded )			// varios data, transform , colors, edgestrips
		{
			m_nSegments = nSegments;
			_endCommand();
			_executeBuffer();
			_beginDrawCommandWithInt(Command::Segments, m_nSegments);

		}
		else if (m_cmd != Command::Segments || m_nSegments != nSegments )
		{
			m_nSegments = nSegments;

			_endCommand();
			_beginDrawCommandWithInt(Command::Segments, m_nSegments);
		}

		// Do transformations

		Rect dest = _dest;

		int uIncX = simpleTransform[0][0];
		int vIncX = simpleTransform[0][1];
		int uIncY = simpleTransform[1][0];
		int vIncY = simpleTransform[1][1];

		// Possibly clip the destination rectangle if we have space for more columns than we have

		int maxCol = (nEdgeStrips - 1);
		if (uIncX != 0)								// Columns are aligned horizontally
		{
			if (dest.w > maxCol)
			{
				if (uIncX < 0)
					dest.x += dest.w - maxCol;
				dest.w = maxCol;
			}
		}
		else										// Columns are aligned vertically
		{
			if (dest.h > maxCol)
			{
				if (uIncY < 0)
					dest.y += dest.h - maxCol;
				dest.h = maxCol;
			}
		}

		// Calc topLeft UV values

		int uTopLeft = 0;
		int vTopLeft = 0;

		if (uIncX + uIncY < 0)
			uTopLeft = maxCol - 1;

		if (vIncX < 0)
			vTopLeft = dest.w - 1;
		else if (vIncY < 0)
			vTopLeft = dest.h - 1;

		// Setup vertices

		for (int i = 0; i < m_nClipRects; i++)
		{
			Rect patch(m_pClipRects[i], dest);
			if (patch.w > 0 && patch.h > 0)
			{
				Vertex * pVertex = m_vertexBufferData + m_vertexOfs;

				int		dx1 = patch.x;
				int		dx2 = patch.x + patch.w;
				int		dy1 = patch.y;
				int		dy2 = patch.y + patch.h;

				// Calc UV-coordinates. U is edge offset, V is pixel offset from begin in column.

				float	u1 = (float) (uTopLeft + (patch.x - dest.x) * simpleTransform[0][0] + (patch.y - dest.y) * simpleTransform[1][0]);
				float	v1 = (float) (vTopLeft + (patch.x - dest.x) * simpleTransform[0][1] + (patch.y - dest.y) * simpleTransform[1][1]);

				float	u2 = (float) (uTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][0] + (patch.y - dest.y) * simpleTransform[1][0]);
				float	v2 = (float) (vTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][1] + (patch.y - dest.y) * simpleTransform[1][1]);

				float	u3 = (float) (uTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][0] + (patch.y + patch.h - dest.y) * simpleTransform[1][0]);
				float	v3 = (float) (vTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][1] + (patch.y + patch.h - dest.y) * simpleTransform[1][1]);

				float	u4 = (float) (uTopLeft + (patch.x - dest.x) * simpleTransform[0][0] + (patch.y + patch.h - dest.y) * simpleTransform[1][0]);
				float	v4 = (float) (vTopLeft + (patch.x - dest.x) * simpleTransform[0][1] + (patch.y + patch.h - dest.y) * simpleTransform[1][1]);

				CoordF	uv1 = { u1, v1 - 0.5f };
				CoordF	uv2 = { u2, v2 - 0.5f };
				CoordF	uv3 = { u3, v3 - 0.5f };
				CoordF	uv4 = { u4, v4 - 0.5f };


				//

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv1;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv2;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv3;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv1;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv3;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy2;
				pVertex->color = m_tintColor;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv4;
				pVertex++;

				m_vertexOfs += 6;
			}
		}

		// Setup extras data

		GLfloat * pExtras = m_extrasBufferData + m_extrasOfs;

		// Add various data to extras

		int edgeStripOfs = (m_extrasOfs + 4 + nSegments * 4);	// Offset for edgestrips in buffer.

		pExtras[0] = (GLfloat) nSegments;
		pExtras[1] = (GLfloat) edgeStripOfs/4;
		pExtras += 4;												// Alignment for vec4 reads.

		// Add segment colors to extras

		for (int i = 0; i < nSegments; i++)
		{
			*pExtras++ = pSegmentColors[i].r/256.f;
			*pExtras++ = pSegmentColors[i].g/256.f;
			*pExtras++ = pSegmentColors[i].b/256.f;
			*pExtras++ = pSegmentColors[i].a/256.f;
		}

		// Add edgestrips to extras

		pExtras = m_extrasBufferData + edgeStripOfs;

		const int * pEdges = pEdgeStrips;

		for (int i = 0; i < nEdgeStrips-1; i++)
		{
			for (int j = 0; j < nSegments - 1; j++)
			{
				int edgeIn = pEdges[j];
				int edgeOut = pEdges[edgeStripPitch+j];

				if (edgeIn > edgeOut)
					std::swap(edgeIn, edgeOut);

				float increment = edgeOut == edgeIn ? 100.f : 256.f / (edgeOut - edgeIn);
				float beginAdder;
				float endAdder;

				if ((edgeOut & 0xFFFFFF00) <= (unsigned int) edgeIn)
				{
					float firstPixelCoverage = ((256 - (edgeOut & 0xFF)) + (edgeOut - edgeIn) / 2) / 256.f;

					beginAdder = increment * (edgeIn & 0xFF)/256.f + firstPixelCoverage;
					endAdder = beginAdder;
				}
				else
				{
					int height = 256 - (edgeIn & 0xFF);
					int width = (int)(increment * height);
					float firstPixelCoverage = (height * width) / (2 * 65536.f);
					float lastPixelCoverage = 1.f - (edgeOut & 0xFF)*increment*(edgeOut & 0xFF) / (2*65536.f);

					beginAdder = increment * (edgeIn & 0xFF) / 256.f + firstPixelCoverage;
					endAdder = lastPixelCoverage - (1.f - (edgeOut & 0xFF)*increment / 256.f);
// 					endAdder = lastPixelCoverage - ((edgeOut & 0xFFFFFF00)-edgeIn)*increment / 256.f;
				}

				*pExtras++ = edgeIn/256.f;					// Segment begin pixel
				*pExtras++ = increment;						// Segment increment
				*pExtras++ = beginAdder;					// Segment begin adder
				*pExtras++ = endAdder;						// Segment end adder
			}

			pEdges += edgeStripPitch;
		}

		m_extrasOfs += extrasSpaceNeeded;

		assert(glGetError() == 0);


	}

	//____ _dummyFinalizer() __________________________________________________________

	void GlGfxDevice::_dummyFinalizer()
	{
	}

	//____ _drawCmdFinalizer() __________________________________________________________

	void GlGfxDevice::_drawCmdFinalizer()
	{
		m_commandBuffer[m_commandOfs++] = m_vertexOfs - m_cmdBeginVertexOfs;
	}

	//____ _executeBuffer() ___________________________________________________

	void GlGfxDevice::_executeBuffer()
	{
		assert(glGetError() == 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, c_vertexBufferSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);		// Orphan current buffer if still in use.
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexOfs * sizeof(Vertex), m_vertexBufferData);			// Fill new buffer with as much data as needed.

		glBindBuffer(GL_TEXTURE_BUFFER, m_extrasBufferId);
		glBufferData(GL_TEXTURE_BUFFER, c_extrasBufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, m_extrasOfs * sizeof(GLfloat), m_extrasBufferData);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		int * pCmd = m_commandBuffer;
		int * pCmdEnd = &m_commandBuffer[m_commandOfs];

		int vertexOfs = 0;
		int surfaceOfs = 0;

		// Clear pending flags of active BlitSource and Canvas.

		if (m_pActiveBlitSource)
			m_pActiveBlitSource->m_bPendingReads = false;

		while (pCmd < pCmdEnd)
		{
			Command cmd = (Command) * pCmd++;

			switch (cmd)
			{
				case Command::SetCanvas:
				{
					_setCanvas(m_surfaceBuffer[surfaceOfs], pCmd[0], pCmd[1]);
					m_surfaceBuffer[surfaceOfs++] = nullptr;
					pCmd += 2;
					break;
				}
				case Command::SetBlendMode:
				{
					_setBlendMode((BlendMode)* pCmd++);
					break;
				}
				case Command::SetBlitSource:
				{
					_setBlitSource(m_surfaceBuffer[surfaceOfs]);
					m_surfaceBuffer[surfaceOfs++] = nullptr;
					break;
				}
				case Command::Blit:
				{
					glUseProgram(m_cmdBlitProgram);
					glEnableVertexAttribArray(2);

					int nVertices = *pCmd++;


					glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
					vertexOfs += nVertices;
					break;
				}
				case Command::Fill:
				{
					glUseProgram(m_fillProg);
					glDisableVertexAttribArray(2);

					int nVertices = *pCmd++;
					glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
					vertexOfs += nVertices;
					break;
				}
				case Command::FillSubPixel:
				{
					glUseProgram(m_aaFillProg);
					glEnableVertexAttribArray(2);

					int nVertices = *pCmd++;
					glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
					vertexOfs += nVertices;
					break;
				}
				case Command::LineFromTo:
				{
					glUseProgram(m_lineFromToProg);
					glEnableVertexAttribArray(2);

					int clipListOfs = *pCmd++;
					int clipListLen = *pCmd++;
					int nVertices = *pCmd++;

					for (int i = 0; i < clipListLen; i++)
					{
						Rect& clip = m_clipListBuffer[clipListOfs++];
						glScissor(clip.x, clip.y, clip.w, clip.h);
						glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
					}

					glScissor(0, 0, m_canvasSize.w, m_canvasSize.h);
					vertexOfs += nVertices;
					break;
				}
				case Command::Plot:
				{

					glUseProgram(m_plotProg);
					glDisableVertexAttribArray(2);

					int nVertices = *pCmd++;
					glDrawArrays(GL_POINTS, vertexOfs, nVertices);
					vertexOfs += nVertices;
					break;
				}
				case Command::Segments:
				{
					int nEdges = (*pCmd++)-1;
					glUseProgram(m_segmentsProg[nEdges]);
					glEnableVertexAttribArray(2);
					glEnableVertexAttribArray(3);

					int nVertices = *pCmd++;
					glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
					vertexOfs += nVertices;

					glDisableVertexAttribArray(3);
					break;
				}
				default:
					assert(false);
			}
		}

		m_vertexOfs = 0;
		m_extrasOfs = 0;
		m_commandOfs = 0;
		m_surfaceOfs = 0;
		m_clipWriteOfs = 0;
		m_clipCurrOfs = -1;

		assert(glGetError() == 0);
	}


	//____ _setCanvas() _______________________________________________________

	void GlGfxDevice::_setCanvas( GlSurface * pCanvas, int width, int height )
	{
		assert(glGetError() == 0);

		if (pCanvas)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pCanvas->getTexture(), 0);

			GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				//TODO: Signal error that we could not set the specified canvas.

				return;
			}
		}
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		int canvasYstart	= pCanvas ? 0 : height;
		int canvasYmul		= pCanvas ? 1 : -1;

		glViewport(0, 0, width, height);
		glScissor(0, 0, width, height);

		// Updating canvas info for our shaders

		m_canvasUBOBuffer.dimX = (GLfloat) width;
		m_canvasUBOBuffer.dimY = (GLfloat) height;
		m_canvasUBOBuffer.yOfs = canvasYstart;
		m_canvasUBOBuffer.yMul = canvasYmul;

		glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(canvasUBO), &m_canvasUBOBuffer);

		assert(glGetError() == 0);
	}

	//____ _setBlendMode() _______________________________________________________

	void GlGfxDevice::_setBlendMode( BlendMode mode )
	{
		assert(glGetError() == 0);

		switch (mode)
		{
		case BlendMode::Replace:
			glBlendEquation(GL_FUNC_ADD);
			glDisable(GL_BLEND);
			break;

		case BlendMode::Blend:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			//				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendMode::Add:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;

		case BlendMode::Subtract:
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;

		case BlendMode::Multiply:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			glBlendFunc(GL_DST_COLOR, GL_ZERO);
			break;

		case BlendMode::Invert:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			break;

		case BlendMode::Ignore:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO, GL_ONE);
			break;

		default:
			assert(false);
			break;
		}
		assert(glGetError() == 0);
	}

	//____ _setBlitSource() _______________________________________________________

	void GlGfxDevice::_setBlitSource( GlSurface * pSurf )
	{
		assert(glGetError() == 0);

		glActiveTexture(GL_TEXTURE0);

		if( pSurf )
		{
			glBindTexture(GL_TEXTURE_2D, pSurf->getTexture());

			m_pActiveBlitSource = pSurf;
			pSurf->m_bPendingReads = false;			// Clear this as we pass it by...

			if (pSurf->m_pClut)
			{
				if (pSurf->scaleMode() == ScaleMode::Interpolate)
				{
					glUseProgram(m_clutBlitInterpolateProg);
					glUniform2i(m_clutBlitInterpolateProgTexSizeLoc, pSurf->size().w, pSurf->size().h);

					m_cmdBlitProgram = m_clutBlitInterpolateProg;
				}
				else
				{
					glUseProgram(m_clutBlitNearestProg);
					glUniform2i(m_clutBlitNearestProgTexSizeLoc, pSurf->size().w, pSurf->size().h);

					m_cmdBlitProgram = m_clutBlitNearestProg;
				}

				glActiveTexture(GL_TEXTURE2);
				GLuint clutTex = pSurf->getClutTexture();
				glBindTexture(GL_TEXTURE_BUFFER, clutTex);

				assert(glGetError() == 0);
			}
			else if (pSurf->m_pixelDescription.format == PixelFormat::A8)
			{
				glUseProgram(m_alphaBlitProg);
				glUniform2i(m_alphaBlitProgTexSizeLoc, pSurf->size().w, pSurf->size().h);

				m_cmdBlitProgram = m_alphaBlitProg;
			}
			else
			{
				glUseProgram(m_blitProg);
				glUniform2i(m_blitProgTexSizeLoc, pSurf->size().w, pSurf->size().h);

				m_cmdBlitProgram = m_blitProg;
			}
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0 );

		assert(glGetError() == 0);
	}

	//____ _createGlProgram() ___________________________________________________

	GLuint GlGfxDevice::_createGLProgram(const char * pVertexShader, const char * pFragmentShader)
	{
		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShaderID, 1, &pVertexShader, NULL);
		glCompileShader(vertexShaderID);

		glShaderSource(fragmentShaderID, 1, &pFragmentShader, NULL);
		glCompileShader(fragmentShaderID);

		GLuint  programID = glCreateProgram();
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);
		glLinkProgram(programID);

		// glLinkProgram doesn't use glGetError
		int mess = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &mess);
		if (mess != GL_TRUE)
		{
			GLchar	vertexShaderLog[1024];
			GLchar	fragmentShaderLog[1024];
			GLsizei vertexShaderLogLength;
			GLsizei fragmentShaderLogLength;

			glGetShaderInfoLog(vertexShaderID, 1024, &vertexShaderLogLength, vertexShaderLog );
			glGetShaderInfoLog(fragmentShaderID, 1024, &fragmentShaderLogLength, fragmentShaderLog);

			assert(false);
		}

		glDetachShader(programID, vertexShaderID);
		glDetachShader(programID, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		return programID;
	}

	//____ _initTables() ___________________________________________________________

	void GlGfxDevice::_initTables()
	{
		// Init lineThicknessTable

		for (int i = 0; i < 17; i++)
		{
			double b = i / 16.0;
			m_lineThicknessTable[i] = (float)Util::squareRoot(1.0 + b * b);
		}
	}


	//____ _scaleThickness() ___________________________________________________

	float GlGfxDevice::_scaleThickness(float thickness, float slope)
	{
		slope = std::abs(slope);

		float scale = m_lineThicknessTable[(int)(slope * 16)];

		if (slope < 1.f)
		{
			float scale2 = m_lineThicknessTable[(int)(slope * 16) + 1];
			scale += (scale2 - scale)*((slope * 16) - ((int)(slope * 16)));
		}

		return thickness * scale;
	}

} // namespace wg

