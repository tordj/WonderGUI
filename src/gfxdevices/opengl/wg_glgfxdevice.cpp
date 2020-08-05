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
#include <cstdio>

#include <wg_glgfxdevice.h>
#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_util.h>

using namespace std;

namespace wg
{
	const TypeInfo GlGfxDevice::TYPEINFO = { "GlGfxDevice", &GfxDevice::TYPEINFO };

	GlGfxDevice *	GlGfxDevice::s_pActiveDevice = nullptr;


#define LOG_GLERROR(check) { GLenum err = check; if(err != 0) onGlError(err, this, TYPEINFO, __func__, __FILE__, __LINE__ ); }

#define LOG_INIT_GLERROR(check) { GLenum err = check; if(err != 0) { onGlError(err, this, TYPEINFO, __func__, __FILE__, __LINE__ ); m_bFullyInitialized = false; } }


	//____ onGlError() _______________________________________________________________

	void GlGfxDevice::onGlError(GLenum errorCode, const Object * pObject, const TypeInfo& pClassType, const char * func, const char * file, int line)
	{
		char	buffer[1024];

		const char * pErrorName;

		switch( errorCode )
		{
			case GL_NO_ERROR:
				pErrorName = "GL_NO_ERROR";
				break;

			case GL_INVALID_ENUM:
				pErrorName = "GL_INVALID_ENUM";
				break;

			case GL_INVALID_VALUE:
				pErrorName = "GL_INVALID_VALUE";
				break;

			case GL_INVALID_OPERATION:
				pErrorName = "GL_INVALID_OPERATION";
				break;

			case GL_INVALID_FRAMEBUFFER_OPERATION:
				pErrorName = "GL_INVALID_FRAMEBUFFER_OPERATION";
				break;

			case GL_OUT_OF_MEMORY:
				pErrorName = "GL_OUT_OF_MEMORY";
				break;
/*
			case GL_STACK_UNDERFLOW:
				pErrorName = "GL_STACK_UNDERFLOW";
				break;

			case GL_STACK_OVERFLOW:
				pErrorName = "GL_STACK_OVERFLOW";
				break;
*/
			default:
				pErrorName = "UNKNOWN GL ERROR! (should not happen)";
				break;
		}


		sprintf( buffer, "OpenGL error 0x%x: %s", errorCode, pErrorName );
		Base::handleError(ErrorSeverity::SilentFail, ErrorCode::OpenGL, buffer, pObject, pClassType, func, file, line);
	}


	//____ create() _______________________________________________________________

	GlGfxDevice_p GlGfxDevice::create( const RectI& viewport, int uboBindingPoint)
	{
		GlGfxDevice_p p(new GlGfxDevice( viewport, uboBindingPoint ));

		if( !p->m_bFullyInitialized )
			return GlGfxDevice_p(nullptr);

		return p;
	}

	GlGfxDevice_p GlGfxDevice::create( GlSurface * pSurface, int uboBindingPoint)
	{
		GlGfxDevice_p p(new GlGfxDevice(pSurface, uboBindingPoint ));

		if ( !p->m_bFullyInitialized )
			return GlGfxDevice_p(nullptr);

		return p;
	}
	//____ constructor _____________________________________________________________

	GlGfxDevice::GlGfxDevice(GlSurface * pSurface, int uboBindingPoint) : GlGfxDevice(pSurface->size(), uboBindingPoint)
	{
		setCanvas(pSurface);
	}

	GlGfxDevice::GlGfxDevice(const RectI& viewport, int uboBindingPoint) : GlGfxDevice(viewport.size(), uboBindingPoint)
	{
		setCanvas(viewport);
	}

	//____ _setDrawUniforms() __________________________________________________

	void GlGfxDevice::_setDrawUniforms(GLuint progId, int uboBindingPoint)
	{
		unsigned int canvasIndex = glGetUniformBlockIndex(progId, "Canvas");
		glUniformBlockBinding(progId, canvasIndex, uboBindingPoint);

		GLint extrasIdLoc = glGetUniformLocation(progId, "extrasId");
		glUseProgram(progId);
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

	}

	//____ _setBlitUniforms() __________________________________________________

	void GlGfxDevice::_setBlitUniforms(GLuint progId, int uboBindingPoint)
	{
		unsigned int canvasIndex = glGetUniformBlockIndex(progId, "Canvas");
		glUniformBlockBinding(progId, canvasIndex, uboBindingPoint);

		GLint extrasIdLoc = glGetUniformLocation(progId, "extrasId");
		GLint texIdLoc = glGetUniformLocation(progId, "texId");

		glUseProgram(progId);
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.
		glUniform1i(texIdLoc, 0);			// Needs to be set. Texture unit 0 is used for textures.
	}

	//____ _setClutBlitUniforms() __________________________________________________

	void GlGfxDevice::_setClutBlitUniforms(GLuint progId, int uboBindingPoint)
	{
		unsigned int canvasIndex = glGetUniformBlockIndex(progId, "Canvas");
		glUniformBlockBinding(progId, canvasIndex, uboBindingPoint);

		GLint extrasIdLoc = glGetUniformLocation(progId, "extrasId");
		GLint texIdLoc = glGetUniformLocation(progId, "texId");
		GLint clutIdLoc = glGetUniformLocation(progId, "clutId");

		glUseProgram(progId);
		glUniform1i(texIdLoc, 0);			// Needs to be set. Texture unit 0 is used for textures.
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.
		glUniform1i(clutIdLoc, 2);			// Needs to be set. Texture unit 2 is used for CLUT.
	}

	//____ Constructor _____________________________________________________________________

	GlGfxDevice::GlGfxDevice( SizeI viewportSize, int uboBindingPoint ) : GfxDevice(viewportSize)
	{
		m_bFullyInitialized = true;

		m_canvasYstart = viewportSize.h;
		m_canvasYmul = -1;

		_initTables();

		// Create and init Fill shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId			= _createGLProgram(fillVertexShader, i == 0 ? fillFragmentShader : fillFragmentShader_A8);
			_setDrawUniforms(progId, uboBindingPoint);
			m_fillProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init Fill Gradient shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(fillGradientVertexShader, i == 0 ? fillFragmentShader : fillFragmentShader_A8);
			_setDrawUniforms(progId, uboBindingPoint);
			m_fillGradientProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init AA-Fill shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(aaFillVertexShader, i == 0 ? aaFillFragmentShader : aaFillFragmentShader_A8);
			_setDrawUniforms(progId, uboBindingPoint);
			m_aaFillProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init AA-Fill gradient shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(aaFillGradientVertexShader, i == 0 ? aaFillFragmentShader : aaFillFragmentShader_A8);
			_setDrawUniforms(progId, uboBindingPoint);
			m_aaFillGradientProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init Blit shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(blitVertexShader, i == 0 ? blitFragmentShader : blitFragmentShader_A8);
			_setBlitUniforms(progId, uboBindingPoint);
			m_blitProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init Gradient Blit shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(blitGradientVertexShader, i == 0 ? blitFragmentShader : blitFragmentShader_A8);
			_setBlitUniforms(progId, uboBindingPoint);
			m_blitGradientProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init AlphaBlit shader (shader program for blitting from alpha-only texture)

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(blitVertexShader, i == 0 ? alphaBlitFragmentShader : alphaBlitFragmentShader_A8);
			_setBlitUniforms(progId, uboBindingPoint);
			m_alphaBlitProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init AlphaBlit gradient shader (shader program for blitting from alpha-only texture)

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(blitGradientVertexShader, i == 0 ? alphaBlitFragmentShader : alphaBlitFragmentShader_A8);
			_setBlitUniforms(progId, uboBindingPoint);
			m_alphaBlitGradientProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init Clut Blit shaders

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(clutBlitNearestVertexShader, i == 0 ? clutBlitNearestFragmentShader : clutBlitNearestFragmentShader_A8);
			_setClutBlitUniforms(progId, uboBindingPoint);
			m_clutBlitNearestProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(clutBlitInterpolateVertexShader, i == 0 ? clutBlitInterpolateFragmentShader : clutBlitInterpolateFragmentShader_A8);
			_setClutBlitUniforms(progId, uboBindingPoint);
			m_clutBlitInterpolateProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init Clut Blit gradient shaders

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(clutBlitNearestGradientVertexShader, i == 0 ? clutBlitNearestFragmentShader : clutBlitNearestFragmentShader_A8);
			_setClutBlitUniforms(progId, uboBindingPoint);
			m_clutBlitNearestGradientProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(clutBlitInterpolateGradientVertexShader, i == 0 ? clutBlitInterpolateFragmentShader : clutBlitInterpolateFragmentShader_A8);
			_setClutBlitUniforms(progId, uboBindingPoint);
			m_clutBlitInterpolateGradientProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Fill in our m_blitProgMatrix

		for (int i = 0; i < PixelFormat_size; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				m_blitProgMatrix[i][0][0][j] = m_blitProg[j];
				m_blitProgMatrix[i][0][0][j] = m_blitProg[j];
				m_blitProgMatrix[i][1][0][j] = m_blitProg[j];
				m_blitProgMatrix[i][0][1][j] = m_blitGradientProg[j];
				m_blitProgMatrix[i][1][1][j] = m_blitGradientProg[j];
			}
		}


		m_blitProgMatrix[(int)PixelFormat::A_8][0][0][0] = m_alphaBlitProg[0];
		m_blitProgMatrix[(int)PixelFormat::A_8][1][0][0] = m_alphaBlitProg[0];

		m_blitProgMatrix[(int)PixelFormat::A_8][0][1][0] = m_alphaBlitGradientProg[0];
		m_blitProgMatrix[(int)PixelFormat::A_8][1][1][0] = m_alphaBlitGradientProg[0];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][0][0][0] = m_clutBlitNearestProg[0];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][1][0][0] = m_clutBlitInterpolateProg[0];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][0][1][0] = m_clutBlitNearestGradientProg[0];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][1][1][0] = m_clutBlitInterpolateGradientProg[0];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][0][0][0] = m_clutBlitNearestProg[0];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][1][0][0] = m_clutBlitInterpolateProg[0];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][0][1][0] = m_clutBlitNearestGradientProg[0];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][1][1][0] = m_clutBlitInterpolateGradientProg[0];


		m_blitProgMatrix[(int)PixelFormat::A_8][0][0][1] = m_alphaBlitProg[1];
		m_blitProgMatrix[(int)PixelFormat::A_8][1][0][1] = m_alphaBlitProg[1];

		m_blitProgMatrix[(int)PixelFormat::A_8][0][1][1] = m_alphaBlitGradientProg[1];
		m_blitProgMatrix[(int)PixelFormat::A_8][1][1][1] = m_alphaBlitGradientProg[1];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][0][0][1] = m_clutBlitNearestProg[1];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][1][0][1] = m_clutBlitInterpolateProg[1];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][0][1][1] = m_clutBlitNearestGradientProg[1];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_linear][1][1][1] = m_clutBlitInterpolateGradientProg[1];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][0][0][1] = m_clutBlitNearestProg[1];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][1][0][1] = m_clutBlitInterpolateProg[1];

		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][0][1][1] = m_clutBlitNearestGradientProg[1];
		m_blitProgMatrix[(int)PixelFormat::CLUT_8_sRGB][1][1][1] = m_clutBlitInterpolateGradientProg[1];


		// Create and init Plot shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(plotVertexShader, i == 0 ? plotFragmentShader : plotFragmentShader_A8);
			_setDrawUniforms(progId, uboBindingPoint);
			m_plotProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init Line shader

		for (int i = 0; i < 2; i++)
		{
			GLuint progId = _createGLProgram(lineFromToVertexShader, i == 0 ? lineFromToFragmentShader : lineFromToFragmentShader_A8);
			_setDrawUniforms(progId, uboBindingPoint);
			m_lineFromToProg[i] = progId;
			LOG_INIT_GLERROR(glGetError());
		}

		// Create and init Segment shaders

		for (int i = 1; i < c_maxSegments ; i++)
		{
			for (int canvType = 0; canvType < 2; canvType++)
			{
				std::string fragShader = canvType == 0 ? segmentsFragmentShader : segmentsFragmentShader_A8;
				auto edgesPos = fragShader.find("$EDGES");
				fragShader.replace(edgesPos, 6, std::to_string(i));

				auto maxsegPos = fragShader.find("$MAXSEG");
				fragShader.replace(maxsegPos, 7, std::to_string(c_maxSegments));
			
				const char* pVertexShader = segmentsVertexShader;
				for (int j = 0; j < 2; j++)
				{
					GLuint prog = _createGLProgram(pVertexShader, fragShader.c_str());
					m_segmentsProg[i][j][canvType] = prog;
					unsigned int canvasIndex = glGetUniformBlockIndex(prog, "Canvas");
					glUniformBlockBinding(prog, canvasIndex, uboBindingPoint);

					GLint extrasIdLoc = glGetUniformLocation(prog, "extrasId");
					GLint colorsIdLoc = glGetUniformLocation(prog, "colorsId");
					GLint stripesIdLoc = glGetUniformLocation(prog, "stripesId");
					GLint paletteIdLoc = glGetUniformLocation(prog, "paletteId");

					glUseProgram(prog);
					glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.
					glUniform1i(colorsIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer, which doubles as the colors buffer.
					glUniform1i(stripesIdLoc, 1);		// Needs to be set. Texture unit 1 is used for segment stripes buffer.
					glUniform1i(paletteIdLoc, 3);		// Needs to be set. Texture unit 3 is used for segment stripes buffer.

					pVertexShader = segmentsVertexShaderGradient;
				}
			}
		}

		LOG_INIT_GLERROR(glGetError());

		// Create texture for segment colors

		for (int seg = 0; seg < c_maxSegments; seg++)
		{
			for (int i = 0; i < 16 * 2 * 2 * 4; i++)
			{
				m_segmentsTintTexMap[seg][i] = 65535;
			}
		}


		glGenTextures(1, &m_segmentsTintTexId);
		glBindTexture(GL_TEXTURE_2D, m_segmentsTintTexId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 2 * c_maxSegments, 2 * c_segmentsTintTexMapSize, 0, GL_BGRA, GL_UNSIGNED_SHORT, nullptr);


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

		LOG_INIT_GLERROR(glGetError());

		glVertexAttribIPointer(
			1,                  // attribute number, must match the layout in the shader.
			1,                  // size
			GL_INT,           // type
			sizeof(Vertex),		// stride
			(void*)(sizeof(CoordI))  // array buffer offset
		);

		glVertexAttribPointer(
			2,						// attribute number, must match the layout in the shader.
			2,						// size
			GL_FLOAT,				// type
			GL_TRUE,				// normalized?
			sizeof(Vertex),			// stride
			(void*)(sizeof(CoordI) + sizeof(int) )  // array buffer offset
		);


		LOG_INIT_GLERROR(glGetError());

		// Create a TextureBufferObject for providing extra data to our shaders

		glGenBuffers(1, &m_extrasBufferId);
		glBindBuffer(GL_TEXTURE_BUFFER, m_extrasBufferId);
		glBufferData(GL_TEXTURE_BUFFER, c_extrasBufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);

		LOG_INIT_GLERROR(glGetError());

		glGenTextures(1, &m_extrasBufferTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, m_extrasBufferTex);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_extrasBufferId);
		glActiveTexture(GL_TEXTURE0);

		LOG_INIT_GLERROR(glGetError());
	}

	//____ Destructor ______________________________________________________________

	GlGfxDevice::~GlGfxDevice()
	{
		LOG_GLERROR(glGetError());

		glDeleteProgram(m_fillProg[0]);
		glDeleteProgram(m_fillProg[1]);
		glDeleteProgram(m_fillGradientProg[0]);
		glDeleteProgram(m_fillGradientProg[1]);
		glDeleteProgram(m_aaFillProg[0]);
		glDeleteProgram(m_aaFillProg[1]);
		glDeleteProgram(m_aaFillGradientProg[0]);
		glDeleteProgram(m_aaFillGradientProg[1]);
		glDeleteProgram(m_blitProg[0]);
		glDeleteProgram(m_blitProg[1]);
		glDeleteProgram(m_blitGradientProg[0]);
		glDeleteProgram(m_blitGradientProg[1]);
		glDeleteProgram(m_alphaBlitProg[0]);
		glDeleteProgram(m_alphaBlitProg[1]);
		glDeleteProgram(m_alphaBlitGradientProg[0]);
		glDeleteProgram(m_alphaBlitGradientProg[1]);
		glDeleteProgram(m_clutBlitNearestProg[0]);
		glDeleteProgram(m_clutBlitNearestProg[1]);
		glDeleteProgram(m_clutBlitNearestGradientProg[0]);
		glDeleteProgram(m_clutBlitNearestGradientProg[1]);
		glDeleteProgram(m_clutBlitInterpolateProg[0]);
		glDeleteProgram(m_clutBlitInterpolateProg[1]);
		glDeleteProgram(m_clutBlitInterpolateGradientProg[0]);
		glDeleteProgram(m_clutBlitInterpolateGradientProg[1]);

		glDeleteProgram(m_plotProg[0]);
		glDeleteProgram(m_plotProg[1]);
		glDeleteProgram(m_lineFromToProg[0]);
		glDeleteProgram(m_lineFromToProg[1]);

		for (int i = 1; i < c_maxSegments; i++)
		{
			glDeleteProgram(m_segmentsProg[i][0][0]);
			glDeleteProgram(m_segmentsProg[i][0][1]);
			glDeleteProgram(m_segmentsProg[i][1][0]);
			glDeleteProgram(m_segmentsProg[i][1][1]);
		}

		glDeleteFramebuffers(1, &m_framebufferId);
		glDeleteTextures(1, &m_extrasBufferTex);
		glDeleteTextures(1, &m_segmentsTintTexId);

		glDeleteBuffers(1, &m_vertexBufferId);
		glDeleteBuffers(1, &m_extrasBufferId);

		glDeleteVertexArrays(1, &m_vertexArrayId);

		if( m_idleSync != 0 )
			glDeleteSync(m_idleSync);

		LOG_GLERROR(glGetError());
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& GlGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& GlGfxDevice::surfaceType( void ) const
	{
		return GlSurface::TYPEINFO;
	}

	//____ surfaceFactory() ______________________________________________________

	SurfaceFactory_p GlGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = GlSurfaceFactory::create();

		return m_pSurfaceFactory;
	}

	//____ setCanvas() __________________________________________________________________

	bool GlGfxDevice::setCanvas( SizeI canvasSize, CanvasInit initOperation, bool bResetClipRects )
	{
		// Do NOT add any gl-calls here, INCLUDING glGetError()!!!
		// This method can be called without us having our GL-context.

		if (canvasSize.w < 1 || canvasSize.h < 1)
			return false;

		m_pCanvas = nullptr;
		m_canvasSize = canvasSize;
		m_clipCanvas = m_canvasSize;

		if( bResetClipRects )
		{
			m_clipBounds = m_canvasSize;
			m_pClipRects = &m_clipCanvas;
			m_nClipRects = 1;
		}

		m_canvasYstart = canvasSize.h;
		m_canvasYmul = -1;

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::SetCanvas, 3);
			m_commandBuffer[m_commandOfs++] = m_canvasSize.w;
			m_commandBuffer[m_commandOfs++] = m_canvasSize.h;
			m_commandBuffer[m_commandOfs++] = (int) initOperation;
			m_surfaceBuffer[m_surfaceOfs++] = nullptr;
		}
		else
			m_beginRenderOp = initOperation;

		m_emptyCanvasSize = canvasSize;

		return true;
	}

	bool GlGfxDevice::setCanvas( Surface * pSurface, CanvasInit initOperation, bool bResetClipRects )
	{
		// Do NOT add any gl-calls here, INCLUDING glGetError()!!!
		// This method can be called without us having our GL-context.

		if( pSurface == nullptr )
			return setCanvas( m_emptyCanvasSize, initOperation, bResetClipRects );

		if (!pSurface || pSurface->typeInfo() != GlSurface::TYPEINFO)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "Provided surface is NOT a GlSurface", this, TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		if (pSurface->pixelFormat() == PixelFormat::CLUT_8_sRGB || pSurface->pixelFormat() == PixelFormat::CLUT_8_linear)
		{
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::InvalidParam, "A CLUT-based surface can not be used as canvas", this, TYPEINFO, __func__, __FILE__, __LINE__);
			return false;
		}

		m_pCanvas = pSurface;
		m_canvasSize = pSurface->size();
		m_clipCanvas = m_canvasSize;

		if( bResetClipRects )
		{
			m_clipBounds = m_canvasSize;
			m_pClipRects = &m_clipCanvas;
			m_nClipRects = 1;
		}

		m_canvasYstart = 0;
		m_canvasYmul = 1;

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::SetCanvas, 3);
			m_commandBuffer[m_commandOfs++] = m_canvasSize.w;
			m_commandBuffer[m_commandOfs++] = m_canvasSize.h;
			m_commandBuffer[m_commandOfs++] = (int) initOperation;
			m_surfaceBuffer[m_surfaceOfs++] = static_cast<GlSurface*>(pSurface);
		}
		else
			m_beginRenderOp = initOperation;

		return true;
	}

	//____ setClipList() ______________________________________________________

	bool GlGfxDevice::setClipList(int nRectangles, const RectI * pRectangles)
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

	//____ setClearColor() _____________________________________________________

	void GlGfxDevice::setClearColor( Color color )
	{
		GfxDevice::setClearColor(color);
		
		if( m_bRendering )
		{
			_endCommand();
			_beginStateCommand(Command::SetClearColor, 1);
			m_commandBuffer[m_commandOfs++] = color.argb;
		}
	}


	//____ setTintColor() __________________________________________________________________

	void GlGfxDevice::setTintColor(Color color)
	{
		GfxDevice::setTintColor(color);

		if( m_bRendering )
		{
			_endCommand();
			_beginStateCommand(Command::SetTintColor, 1);
			m_commandBuffer[m_commandOfs++] = color.argb;
		}
	}

	//____ setTintGradient() __________________________________________________________________

	void GlGfxDevice::setTintGradient(const RectI& rect, Color topLeft, Color topRight, Color bottomRight, Color bottomLeft)
	{
		GfxDevice::setTintGradient(rect, topLeft, topRight, bottomRight, bottomLeft);

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::SetTintGradient, 8);
			m_commandBuffer[m_commandOfs++] = rect.x;
			m_commandBuffer[m_commandOfs++] = rect.y;
			m_commandBuffer[m_commandOfs++] = rect.w;
			m_commandBuffer[m_commandOfs++] = rect.h;
			m_commandBuffer[m_commandOfs++] = topLeft.argb;
			m_commandBuffer[m_commandOfs++] = topRight.argb;
			m_commandBuffer[m_commandOfs++] = bottomRight.argb;
			m_commandBuffer[m_commandOfs++] = bottomLeft.argb;
		}

	}

	//____ clearTintGradient() __________________________________________________________________

	void GlGfxDevice::clearTintGradient()
	{
		GfxDevice::clearTintGradient();

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::ClearTintGradient, 0);
		}
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
		if (!pSource || pSource->typeInfo() != GlSurface::TYPEINFO)
			return false;

		m_pBlitSource = pSource;

		if (m_bRendering)
		{
            //TODO: Check so that we don't overrun m_surfaceBuffer;

			_endCommand();
			_beginStateCommand(Command::SetBlitSource, 0);
			m_surfaceBuffer[m_surfaceOfs++] = static_cast<GlSurface*>(pSource);
		}

		return true;
	}

	//____ setMorphFactor() ______________________________________________________

	void GlGfxDevice::setMorphFactor(float factor)
	{
		limit(factor, 0.f, 1.f);

		m_morphFactor = factor;

		if (m_bRendering)
		{
			_endCommand();
			_beginStateCommand(Command::SetMorphFactor, 1);
			m_commandBuffer[m_commandOfs++] = (int)(factor*1024);
		}
	}

	//____ isCanvasReady() ___________________________________________________________

	bool GlGfxDevice::isCanvasReady() const
	{
		return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}

	//____ beginRender() ___________________________________________________________

	bool GlGfxDevice::beginRender()
	{
		LOG_GLERROR(glGetError());

		if( m_bRendering == true )
			return false;

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return false;

		// Remember GL states so we can restore in EndRender()

		m_glDepthTest 		= glIsEnabled(GL_DEPTH_TEST);
		m_glScissorTest 	= glIsEnabled(GL_SCISSOR_TEST);
		m_glBlendEnabled  	= glIsEnabled(GL_BLEND);
		m_glSRGBEnabled		= glIsEnabled(GL_FRAMEBUFFER_SRGB);
		glGetIntegerv(GL_BLEND_SRC, &m_glBlendSrc);
		glGetIntegerv(GL_BLEND_DST, &m_glBlendDst);
		glGetIntegerv(GL_VIEWPORT, m_glViewport);
		glGetIntegerv(GL_SCISSOR_BOX, m_glScissorBox);
		glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_glReadFrameBuffer);
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_glDrawFrameBuffer);
		glGetFloatv(GL_COLOR_CLEAR_VALUE, m_glClearColor);

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
		m_segmentsTintTexOfs = 0;
		m_clipWriteOfs = 0;
		m_clipCurrOfs = -1;

		_setCanvas( static_cast<GlSurface*>(m_pCanvas.rawPtr()), m_canvasSize.w, m_canvasSize.h );

		_setBlendMode(m_blendMode);
		_setMorphFactor(m_morphFactor);
		_setClearColor(m_clearColor);

		if( m_beginRenderOp == CanvasInit::Clear )
			glClear( GL_COLOR_BUFFER_BIT );
		m_beginRenderOp = CanvasInit::Keep;

		if (m_bTintGradient)
			_setTintGradient(m_tintGradientRect, m_tintGradient);
		else
			_setTintColor(m_tintColor);
		
		_setBlitSource( static_cast<GlSurface*>(m_pBlitSource.rawPtr()) );

		
		// Set our extras buffer and segments palette textures.

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_BUFFER, m_extrasBufferTex);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_segmentsTintTexId);
		glActiveTexture(GL_TEXTURE0);


		// Prepare for rendering

		glBindVertexArray(m_vertexArrayId);


		glFinish();  //TODO: Remove.

		//

		LOG_GLERROR(glGetError());
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool GlGfxDevice::endRender()
	{
		LOG_GLERROR(glGetError());
		if( m_bRendering == false )
			return false;

		// Finalize any ongoing operation

		_endCommand();
		_executeBuffer();

		//

 		glFinish(); //TODO: Remove.

		// Restore render states from before beginRender()

		if( m_glDepthTest )
			glEnable(GL_DEPTH_TEST);

		if( !m_glScissorTest )
			glDisable(GL_SCISSOR_TEST);

		if( m_glBlendEnabled )
			glEnable(GL_BLEND);
		else
			glDisable(GL_BLEND);

		if( m_glSRGBEnabled )
			glEnable(GL_FRAMEBUFFER_SRGB);
		else
			glDisable(GL_FRAMEBUFFER_SRGB);

		glClearColor( m_glClearColor[0], m_glClearColor[1], m_glClearColor[2], m_glClearColor[3] );
		glBlendFunc( m_glBlendSrc, m_glBlendDst );
		glViewport(m_glViewport[0], m_glViewport[1], m_glViewport[2], m_glViewport[3]);
		glScissor(m_glScissorBox[0], m_glScissorBox[1], m_glScissorBox[2], m_glScissorBox[3]);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_glReadFrameBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_glDrawFrameBuffer);

		glBindVertexArray(0);

		//TODO: Reenable

		//if( m_idleSync != 0 )
		//	glDeleteSync(m_idleSync);

		//m_idleSync = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

		//glFlush();

//

		LOG_GLERROR(glGetError());
		m_bRendering = false;

		// Restore previously active device and exit

		s_pActiveDevice = m_pPrevActiveDevice;
		return true;
	}

	//____ isIdle() _____________________________________________________________________

	bool GlGfxDevice::isIdle()
	{
		if( m_bRendering )
			return false;

		//TODO: Reenable
/*
		if (m_idleSync == 0)
			return true;

		GLint isSignaled = 0;
		glGetSynciv(m_idleSync, GL_SYNC_STATUS, 1, NULL, &isSignaled);

		if(isSignaled == GL_UNSIGNALED)
			return false;

		glDeleteSync(m_idleSync);
		m_idleSync = 0;
*/
		return true;
	}

	//____ flush() _______________________________________________________________

	void GlGfxDevice::flush()
	{
		LOG_GLERROR(glGetError());

		// Finalize any ongoing operation

		_endCommand();
		_executeBuffer();

		//

		glFlush();

		LOG_GLERROR(glGetError());
	}



	//____ fill() ____ [standard] __________________________________________________

	void GlGfxDevice::fill(const RectI& rect, const Color& col)
	{
		// Skip calls that won't affect destination

		if (col.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		if (!rect.intersectsWith(m_clipBounds))
			return;

		//

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 4)
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
			RectI patch(m_pClipRects[i], rect);
			if (patch.w > 0 && patch.h > 0)
			{
				int	dx1 = patch.x;
				int	dy1 = patch.y;
				int dx2 = patch.x + patch.w;
				int dy2 = patch.y + patch.h;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;
			}
		}

		float * pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		m_extrasBufferData[m_extrasOfs++] = pConv[col.r];
		m_extrasBufferData[m_extrasOfs++] = pConv[col.g];
		m_extrasBufferData[m_extrasOfs++] = pConv[col.b];
		m_extrasBufferData[m_extrasOfs++] = col.a / 255.f;
	}

	//____ fill() ____ [subpixel] __________________________________________________

	void GlGfxDevice::fill(const RectF& rect, const Color& col)
	{
		// Skip calls that won't affect destination

		if (col.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		// Create our outer rectangle

		RectI outerRect( (int) rect.x, (int) rect.y, ((int) (rect.x+rect.w+0.999f)) - (int) rect.x, ((int) (rect.y + rect.h + 0.999f)) - (int) rect.y );


		RectI clip(outerRect, m_clipBounds);
		if (clip.w == 0 || clip.h == 0)
			return;

		//

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 8)
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
			RectI patch(m_pClipRects[i], outerRect);
			if (patch.w > 0 && patch.h > 0)
			{
				int	dx1 = patch.x;
				int	dy1 = patch.y;
				int dx2 = patch.x + patch.w;
				int dy2 = patch.y + patch.h;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;
			}
		}

		// Provide color	

		float * pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		m_extrasBufferData[m_extrasOfs++] = pConv[col.r];
		m_extrasBufferData[m_extrasOfs++] = pConv[col.g];
		m_extrasBufferData[m_extrasOfs++] = pConv[col.b];
		m_extrasBufferData[m_extrasOfs++] = col.a / 255.f;

		// Provide rectangle center and radius

		SizeF	radius(rect.w / 2, rect.h / 2);
		CoordF	center(rect.x + radius.w, rect.y + radius.h);

		m_extrasBufferData[m_extrasOfs++] = center.x;
		m_extrasBufferData[m_extrasOfs++] = center.y;
		m_extrasBufferData[m_extrasOfs++] = radius.w;
		m_extrasBufferData[m_extrasOfs++] = radius.h;
	}

	//____ plotPixels() ______________________________________________________

	void GlGfxDevice::plotPixels(int nPixels, const CoordI * pCoords, const Color * pColors)
	{
		if (nPixels == 0)
			return;

		if (m_vertexOfs > c_vertexBufferSize - 1 || m_extrasOfs > c_extrasBufferSize - 4)
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

		float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		for (int i = 0; i < m_nClipRects; i++)
		{
			const RectI& clip = m_pClipRects[i];
			for (int pixel = 0; pixel < nPixels; pixel++)
			{
				if (clip.contains(pCoords[pixel]))
				{
					m_vertexBufferData[m_vertexOfs].coord = pCoords[pixel];
					m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
					m_vertexOfs++;

					Color color = pColors[pixel];

					m_extrasBufferData[m_extrasOfs++] = pConv[color.r];
					m_extrasBufferData[m_extrasOfs++] = pConv[color.g];
					m_extrasBufferData[m_extrasOfs++] = pConv[color.b];
					m_extrasBufferData[m_extrasOfs++] = color.a / 255.f;

					if (m_vertexOfs == c_vertexBufferSize || m_extrasOfs == c_extrasBufferSize)
					{
						_endCommand();
						_executeBuffer();
						_beginDrawCommand(Command::Plot);
					}
				}
			}
		}
	}

	//____ drawLine() ____ [from/to] __________________________________________________

	void GlGfxDevice::drawLine(CoordI begin, CoordI end, Color color, float thickness)
	{
		if (m_vertexOfs > c_vertexBufferSize - 6 || m_extrasOfs > c_extrasBufferSize - 8 || m_clipCurrOfs == -1 )
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

		CoordI	c1, c2, c3, c4;

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

		m_vertexBufferData[m_vertexOfs].coord = c1;
		m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
		m_vertexOfs++;

		m_vertexBufferData[m_vertexOfs].coord = c2;
		m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
		m_vertexOfs++;

		m_vertexBufferData[m_vertexOfs].coord = c3;
		m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
		m_vertexOfs++;

		m_vertexBufferData[m_vertexOfs].coord = c1;
		m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
		m_vertexOfs++;

		m_vertexBufferData[m_vertexOfs].coord = c3;
		m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
		m_vertexOfs++;

		m_vertexBufferData[m_vertexOfs].coord = c4;
		m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs/4;
		m_vertexOfs++;

		float * pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		m_extrasBufferData[m_extrasOfs++] = pConv[color.r];
		m_extrasBufferData[m_extrasOfs++] = pConv[color.g];
		m_extrasBufferData[m_extrasOfs++] = pConv[color.b];
		m_extrasBufferData[m_extrasOfs++] = color.a / 255.f;

		m_extrasBufferData[m_extrasOfs++] = s;
		m_extrasBufferData[m_extrasOfs++] = w;
		m_extrasBufferData[m_extrasOfs++] = slope;
		m_extrasBufferData[m_extrasOfs++] = bSteep;
	}

	//____ drawLine() ____ [start/direction] __________________________________________________

	void GlGfxDevice::drawLine(CoordI begin, Direction dir, int length, Color color, float thickness)
	{
		// Skip calls that won't affect destination

		if (color.a == 0 && (m_blendMode == BlendMode::Blend))
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

		RectI outerRect((int)rect.x, (int)rect.y, ((int)(rect.x + rect.w + 0.999f)) - (int)rect.x, ((int)(rect.y + rect.h + 0.999f)) - (int)rect.y);

		// Clip our rectangle

		if (!outerRect.intersectsWith(m_clipBounds))
			return;

		//

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - 8)
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
			RectI patch(m_pClipRects[i], outerRect);
			if (patch.w > 0 && patch.h > 0)
			{
				int	dx1 = patch.x;
				int	dy1 = patch.y;
				int dx2 = patch.x + patch.w;
				int dy2 = patch.y + patch.h;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy1;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx2;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;

				m_vertexBufferData[m_vertexOfs].coord.x = dx1;
				m_vertexBufferData[m_vertexOfs].coord.y = dy2;
				m_vertexBufferData[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
				m_vertexOfs++;
			}
		}

		// Provide color

		float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		m_extrasBufferData[m_extrasOfs++] = pConv[color.r];
		m_extrasBufferData[m_extrasOfs++] = pConv[color.g];
		m_extrasBufferData[m_extrasOfs++] = pConv[color.b];
		m_extrasBufferData[m_extrasOfs++] = color.a / 255.f;

		// Provide rectangle center and raidus.

		SizeF	radius(rect.w / 2, rect.h / 2);
		CoordF	center(rect.x + radius.w, rect.y + radius.h);

		m_extrasBufferData[m_extrasOfs++] = center.x;
		m_extrasBufferData[m_extrasOfs++] = center.y;
		m_extrasBufferData[m_extrasOfs++] = radius.w;
		m_extrasBufferData[m_extrasOfs++] = radius.h;
	}

	//____ _transformBlit() ____ [simple] __________________________________________________

	void GlGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
	{
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
			RectI patch(m_pClipRects[i], dest);
			if (patch.w > 0 && patch.h > 0)
			{
				int		dx1 = patch.x;
				int		dx2 = patch.x + patch.w;
				int		dy1 = patch.y;
				int		dy2 = patch.y + patch.h;

				Vertex * pVertex = m_vertexBufferData + m_vertexOfs;

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
				pVertex->extrasOfs = m_extrasOfs/4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy1;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy2;
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
	}

	//____ _transformBlit() ____ [complex] __________________________________________________

	void GlGfxDevice::_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2])
	{
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
			RectI patch(m_pClipRects[i], dest);
			if (patch.w > 0 && patch.h > 0)
			{
				Vertex * pVertex = m_vertexBufferData + m_vertexOfs;

				int		dx1 = patch.x;
				int		dx2 = patch.x + patch.w;
				int		dy1 = patch.y;
				int		dy2 = patch.y + patch.h;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy1;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex++;

				m_vertexOfs += 6;
			}
		}

		if (m_pBlitSource->scaleMode() == ScaleMode::Interpolate)
		{
			m_extrasBufferData[m_extrasOfs++] = src.x + 0.5f;
			m_extrasBufferData[m_extrasOfs++] = src.y + 0.5f;
			m_extrasBufferData[m_extrasOfs++] = GLfloat(dest.x) + 0.5f;
			m_extrasBufferData[m_extrasOfs++] = GLfloat(dest.y) + 0.5f;
		}
		else
		{
			m_extrasBufferData[m_extrasOfs++] = src.x;
			m_extrasBufferData[m_extrasOfs++] = src.y;
			m_extrasBufferData[m_extrasOfs++] = GLfloat(dest.x) +0.5f;
			m_extrasBufferData[m_extrasOfs++] = GLfloat(dest.y) +0.5f;
		}

		m_extrasBufferData[m_extrasOfs++] = complexTransform[0][0];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[0][1];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[1][0];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[1][1];
	}

	//____ _transformDrawSegments() ______________________________________________________

	void GlGfxDevice::_transformDrawSegments( const RectI& _dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2] )
	{
		if (!_dest.intersectsWith(m_clipBounds))
			return;

		//

		int extrasSpaceNeeded = (8 + 4 * (nEdgeStrips - 1)*(nSegments - 1) + 3) & 0xFFFFFFFC;		// Various data + colors + strips + alignment + margin for

		assert( extrasSpaceNeeded <= c_extrasBufferSize );               // EXTRAS BUFFER IS SET TOO SMALL!

		if (m_vertexOfs > c_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > c_extrasBufferSize - extrasSpaceNeeded || m_segmentsTintTexOfs == c_segmentsTintTexMapSize)			// varios data, transform , colors, edgestrips
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

		RectI dest = _dest;

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
			uTopLeft = maxCol;

		if (vIncX < 0)
			vTopLeft = dest.w;
		else if (vIncY < 0)
			vTopLeft = dest.h;

		// Setup vertices

		for (int i = 0; i < m_nClipRects; i++)
		{
			RectI patch(m_pClipRects[i], dest);
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
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv1;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy1;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv2;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv3;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy1;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv1;
				pVertex++;

				pVertex->coord.x = dx2;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv3;
				pVertex++;

				pVertex->coord.x = dx1;
				pVertex->coord.y = dy2;
				pVertex->extrasOfs = m_extrasOfs / 4;
				pVertex->uv = uv4;
				pVertex++;

				m_vertexOfs += 6;
			}
		}

		// Setup extras data

		GLfloat * pExtras = m_extrasBufferData + m_extrasOfs;

		// Add various data to extras

		int edgeStripOfs = (m_extrasOfs + 8);	// Offset for edgestrips in buffer.

		pExtras[0] = (GLfloat) nSegments;
		pExtras[1] = (GLfloat) edgeStripOfs/4;
		pExtras[2] = (GLfloat)((_dest.w) * abs(simpleTransform[0][0]) + (_dest.h) * abs(simpleTransform[1][0]));
		pExtras[3] = (GLfloat)((_dest.w) * abs(simpleTransform[0][1]) + (_dest.h) * abs(simpleTransform[1][1]));
		pExtras[4] = GLfloat( 0.25f/c_maxSegments );
		pExtras[5] = GLfloat(m_segmentsTintTexOfs + 0.25f) / c_segmentsTintTexMapSize;
		pExtras[6] = GLfloat(c_maxSegments*2);
		pExtras[7] = GLfloat(c_segmentsTintTexMapSize*2);

		pExtras += 8;												// Alignment for vec4 reads.

		// Add colors to segmentsTintTexMap

		float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;
		const Color* pSegCol = pSegmentColors;

		uint16_t* pMapRow = m_segmentsTintTexMap[m_segmentsTintTexOfs];
		int			mapPitch = c_maxSegments * 4 * 2;

		switch (tintMode)
		{
			case TintMode::None:
			case TintMode::Flat:
			{
				for (int i = 0; i < nSegments; i++)
				{
					uint16_t r = uint16_t(pConv[pSegCol->r] * 65535);
					uint16_t g = uint16_t(pConv[pSegCol->g] * 65535);
					uint16_t b = uint16_t(pConv[pSegCol->b] * 65535);
					uint16_t a = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);

					pMapRow[i * 8 + 0] = b;
					pMapRow[i * 8 + 1] = g;
					pMapRow[i * 8 + 2] = r;
					pMapRow[i * 8 + 3] = a;
					pMapRow[i * 8 + 4] = b;
					pMapRow[i * 8 + 5] = g;
					pMapRow[i * 8 + 6] = r;
					pMapRow[i * 8 + 7] = a;

					pMapRow[mapPitch + i * 8 + 0] = b;
					pMapRow[mapPitch + i * 8 + 1] = g;
					pMapRow[mapPitch + i * 8 + 2] = r;
					pMapRow[mapPitch + i * 8 + 3] = a;
					pMapRow[mapPitch + i * 8 + 4] = b;
					pMapRow[mapPitch + i * 8 + 5] = g;
					pMapRow[mapPitch + i * 8 + 6] = r;
					pMapRow[mapPitch + i * 8 + 7] = a;
					pSegCol++;
				}
				break;
			}

			case TintMode::GradientX:
			{
				for (int i = 0; i < nSegments; i++)
				{
					int r1 = uint16_t(pConv[pSegCol->r] * 65535);
					int g1 = uint16_t(pConv[pSegCol->g] * 65535);
					int b1 = uint16_t(pConv[pSegCol->b] * 65535);
					int a1 = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;

					int r2 = uint16_t(pConv[pSegCol->r] * 65535);
					int g2 = uint16_t(pConv[pSegCol->g] * 65535);
					int b2 = uint16_t(pConv[pSegCol->b] * 65535);
					int a2 = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;

					pMapRow[i * 8 + 0] = b1;
					pMapRow[i * 8 + 1] = g1;
					pMapRow[i * 8 + 2] = r1;
					pMapRow[i * 8 + 3] = a1;
					pMapRow[i * 8 + 4] = b2;
					pMapRow[i * 8 + 5] = g2;
					pMapRow[i * 8 + 6] = r2;
					pMapRow[i * 8 + 7] = a2;

					pMapRow[mapPitch + i * 8 + 0] = b1;
					pMapRow[mapPitch + i * 8 + 1] = g1;
					pMapRow[mapPitch + i * 8 + 2] = r1;
					pMapRow[mapPitch + i * 8 + 3] = a1;
					pMapRow[mapPitch + i * 8 + 4] = b2;
					pMapRow[mapPitch + i * 8 + 5] = g2;
					pMapRow[mapPitch + i * 8 + 6] = r2;
					pMapRow[mapPitch + i * 8 + 7] = a2;
				}
				break;
			}

			case TintMode::GradientY:
			{
				for (int i = 0; i < nSegments; i++)
				{
					int r1 = uint16_t(pConv[pSegCol->r] * 65535);
					int g1 = uint16_t(pConv[pSegCol->g] * 65535);
					int b1 = uint16_t(pConv[pSegCol->b] * 65535);
					int a1 = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;

					int r2 = uint16_t(pConv[pSegCol->r] * 65535);
					int g2 = uint16_t(pConv[pSegCol->g] * 65535);
					int b2 = uint16_t(pConv[pSegCol->b] * 65535);
					int a2 = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;

					pMapRow[i * 8 + 0] = b1;
					pMapRow[i * 8 + 1] = g1;
					pMapRow[i * 8 + 2] = r1;
					pMapRow[i * 8 + 3] = a1;
					pMapRow[i * 8 + 4] = b1;
					pMapRow[i * 8 + 5] = g1;
					pMapRow[i * 8 + 6] = r1;
					pMapRow[i * 8 + 7] = a1;

					pMapRow[mapPitch + i * 8 + 0] = b2;
					pMapRow[mapPitch + i * 8 + 1] = g2;
					pMapRow[mapPitch + i * 8 + 2] = r2;
					pMapRow[mapPitch + i * 8 + 3] = a2;
					pMapRow[mapPitch + i * 8 + 4] = b2;
					pMapRow[mapPitch + i * 8 + 5] = g2;
					pMapRow[mapPitch + i * 8 + 6] = r2;
					pMapRow[mapPitch + i * 8 + 7] = a2;
				}
				break;
			}

			case TintMode::GradientXY:
			{
				for (int i = 0; i < nSegments; i++)
				{
					pMapRow[i * 8 + 0] = uint16_t(pConv[pSegCol->b] * 65535);
					pMapRow[i * 8 + 1] = uint16_t(pConv[pSegCol->g] * 65535);
					pMapRow[i * 8 + 2] = uint16_t(pConv[pSegCol->r] * 65535);
					pMapRow[i * 8 + 3] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;

					pMapRow[i * 8 + 4] = uint16_t(pConv[pSegCol->b] * 65535);
					pMapRow[i * 8 + 5] = uint16_t(pConv[pSegCol->g] * 65535);
					pMapRow[i * 8 + 6] = uint16_t(pConv[pSegCol->r] * 65535);
					pMapRow[i * 8 + 7] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;

					pMapRow[mapPitch + i * 8 + 4] = uint16_t(pConv[pSegCol->b] * 65535);
					pMapRow[mapPitch + i * 8 + 5] = uint16_t(pConv[pSegCol->g] * 65535);
					pMapRow[mapPitch + i * 8 + 6] = uint16_t(pConv[pSegCol->r] * 65535);
					pMapRow[mapPitch + i * 8 + 7] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;

					pMapRow[mapPitch + i * 8 + 0] = uint16_t(pConv[pSegCol->b] * 65535);
					pMapRow[mapPitch + i * 8 + 1] = uint16_t(pConv[pSegCol->g] * 65535);
					pMapRow[mapPitch + i * 8 + 2] = uint16_t(pConv[pSegCol->r] * 65535);
					pMapRow[mapPitch + i * 8 + 3] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
					pSegCol++;
				}
				break;
			}
		}

		m_segmentsTintTexOfs++;

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
		LOG_GLERROR(glGetError());

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, c_vertexBufferSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);		// Orphan current buffer if still in use.
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexOfs * sizeof(Vertex), m_vertexBufferData);			// Fill new buffer with as much data as needed.

		glBindBuffer(GL_TEXTURE_BUFFER, m_extrasBufferId);
		glBufferData(GL_TEXTURE_BUFFER, c_extrasBufferSize * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, m_extrasOfs * sizeof(GLfloat), m_extrasBufferData);

		if (m_segmentsTintTexOfs > 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_segmentsTintTexId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, c_maxSegments * 2, m_segmentsTintTexOfs * 2, GL_BGRA, GL_UNSIGNED_SHORT, m_segmentsTintTexMap);
		}
		

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		LOG_GLERROR(glGetError());

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
					
					CanvasInit initOp = (CanvasInit) pCmd[2];
					if( initOp == CanvasInit::Clear )
						glClear( GL_COLOR_BUFFER_BIT);
					pCmd += 3;
					break;
				}
				case Command::SetBlendMode:
				{
					_setBlendMode((BlendMode)* pCmd++);
					break;
				}
				case Command::SetMorphFactor:
				{
					_setMorphFactor((*pCmd++) / 1024.f);
					break;
				}
				case Command::SetClearColor:
				{
					_setClearColor(*(Color*)(pCmd++));
					break;
				}
				case Command::SetTintColor:
				{
					_setTintColor(*(Color*)(pCmd++));
					break;
				}
				case Command::SetTintGradient:
				{
					RectI& rect = *(RectI*)pCmd;
					pCmd += 4;
					Color* pColors = (Color*)pCmd;
					pCmd += 4;

					_setTintGradient(rect, pColors);
					break;
				}
				case Command::ClearTintGradient:
				{
					_clearTintGradient();
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
					int nVertices = *pCmd++;
					if (nVertices > 0 && m_pActiveBlitSource)
					{
						GlSurface* pSurf = m_pActiveBlitSource;
						glUseProgram(m_blitProgMatrix[(int)pSurf->m_pixelDescription.format][(int)pSurf->scaleMode()][m_bGradientActive][m_bActiveCanvasIsA8]);

						glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
						vertexOfs += nVertices;

						if( m_bMipmappedActiveCanvas )
							m_pActiveCanvas->m_bMipmapStale = true;
					}
					break;
				}
				case Command::Fill:
				{
					int nVertices = *pCmd++;
					if( nVertices > 0 )
					{
						if( m_bGradientActive )
							glUseProgram(m_fillGradientProg[m_bActiveCanvasIsA8]);
						else
							glUseProgram(m_fillProg[m_bActiveCanvasIsA8]);

						glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
						vertexOfs += nVertices;

						if( m_bMipmappedActiveCanvas )
							m_pActiveCanvas->m_bMipmapStale = true;
					}
					break;
				}
				case Command::FillSubPixel:
				{
					int nVertices = *pCmd++;
					if( nVertices > 0 )
					{
						if (m_bGradientActive)
							glUseProgram(m_aaFillGradientProg[m_bActiveCanvasIsA8]);
						else
							glUseProgram(m_aaFillProg[m_bActiveCanvasIsA8]);

						glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
						vertexOfs += nVertices;

						if( m_bMipmappedActiveCanvas )
							m_pActiveCanvas->m_bMipmapStale = true;
					}
					break;
				}
				case Command::LineFromTo:
				{
					int clipListOfs = *pCmd++;
					int clipListLen = *pCmd++;
					int nVertices = *pCmd++;
					if( nVertices > 0 )
					{
						glUseProgram(m_lineFromToProg[m_bActiveCanvasIsA8]);

						for (int i = 0; i < clipListLen; i++)
						{
							RectI& clip = m_clipListBuffer[clipListOfs++];
							glScissor(clip.x, clip.y, clip.w, clip.h);
							glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
						}

						glScissor(0, 0, m_canvasSize.w, m_canvasSize.h);
						vertexOfs += nVertices;

						if( m_bMipmappedActiveCanvas )
							m_pActiveCanvas->m_bMipmapStale = true;
					}
					break;
				}
				case Command::Plot:
				{
					int nVertices = *pCmd++;
					if( nVertices > 0 )
					{
						glUseProgram(m_plotProg[m_bActiveCanvasIsA8]);

						glDrawArrays(GL_POINTS, vertexOfs, nVertices);
						vertexOfs += nVertices;

						if( m_bMipmappedActiveCanvas )
							m_pActiveCanvas->m_bMipmapStale = true;
					}
					break;
				}
				case Command::Segments:
				{
					int nSegments = (*pCmd++);
					if (nSegments > c_maxSegments)
						nSegments = c_maxSegments;
					int nEdges = nSegments-1;
					int nVertices = *pCmd++;
					if( nVertices > 0 )
					{
						glUseProgram(m_segmentsProg[nEdges][m_bGradientActive][m_bActiveCanvasIsA8]);
						glEnableVertexAttribArray(2);

						glDrawArrays(GL_TRIANGLES, vertexOfs, nVertices);
						vertexOfs += nVertices;

						glDisableVertexAttribArray(2);

						if( m_bMipmappedActiveCanvas )
							m_pActiveCanvas->m_bMipmapStale = true;
					}
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
		m_segmentsTintTexOfs = 0;
		m_clipWriteOfs = 0;
		m_clipCurrOfs = -1;

		LOG_GLERROR(glGetError());
	}


	//____ _setCanvas() _______________________________________________________

	void GlGfxDevice::_setCanvas( GlSurface * pCanvas, int width, int height )
	{
		LOG_GLERROR(glGetError());

		bool bWasAlphaOnly = m_bActiveCanvasIsA8;

		if (pCanvas)
		{
			glEnable(GL_FRAMEBUFFER_SRGB);		// Always use SRGB on Canvas that is SRGB.

			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, pCanvas->getTexture(), 0);

			GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				m_pActiveCanvas = nullptr;
				m_bMipmappedActiveCanvas = false;

				//TODO: Signal error that we could not set the specified canvas.

				return;
			}

            glViewport(0, 0, width, height);
            glScissor(0, 0, width, height);
        }
		else
		{
			if( Base::activeContext()->gammaCorrection() )
				glEnable(GL_FRAMEBUFFER_SRGB);
			else
				glDisable(GL_FRAMEBUFFER_SRGB);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
 
			// Normally we would set the viewport and scissoring to the values given,
			// but a bug in Logic Pro X doesn't handle viewports correctly when moving
			// plugin window between screens with different scaling. Just maintaining
			// viewport value avoids the issue.
			// The patch seems to be work generally fine under OSX, but breaks resizing
			// of viewport on Windows (OSD and PresetManager).

#ifdef PATCH_LOGIC_PRO_X_VIEWPORT_BUG
            if( abs(m_glViewport[2] - width) > 10 && abs(m_glViewport[3] - height) > 10 )
            {
				glViewport(m_glViewport[0], m_glViewport[1], m_glViewport[2], m_glViewport[3]);
				glScissor(m_glViewport[0], m_glViewport[1], m_glViewport[2], m_glViewport[3]);
            }
            else
            {
				glViewport(0, 0, width, height);
				glScissor(0, 0, width, height);
            }
#else
            glViewport(0, 0, width, height);
            glScissor(0, 0, width, height);
#endif
		}

		int canvasYstart	= pCanvas ? 0 : height;
		int canvasYmul		= pCanvas ? 1 : -1;


		// Updating canvas info for our shaders

		m_canvasUBOBuffer.canvasDimX = (GLfloat) width;
		m_canvasUBOBuffer.canvasDimY = (GLfloat) height;
		m_canvasUBOBuffer.canvasYOfs = canvasYstart;
		m_canvasUBOBuffer.canvasYMul = canvasYmul;

		glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, 4*4 /*sizeof(canvasUBO)*/, &m_canvasUBOBuffer);

		LOG_GLERROR(glGetError());

		m_pActiveCanvas = pCanvas;
		m_bMipmappedActiveCanvas = m_pActiveCanvas ? m_pActiveCanvas->m_bMipmapped : false;

		bool bIsAlphaOnly = m_pActiveCanvas ? m_pActiveCanvas->pixelFormat() == PixelFormat::A_8 : false;

		if (bIsAlphaOnly != bWasAlphaOnly)
			_setBlendMode(m_activeBlendMode);

		m_bActiveCanvasIsA8 = bIsAlphaOnly;
	}

	//____ _setBlendMode() _______________________________________________________

	void GlGfxDevice::_setBlendMode( BlendMode mode )
	{
		LOG_GLERROR(glGetError());

		bool bAlphaOnly = m_pActiveCanvas ? m_pActiveCanvas->pixelFormat() == PixelFormat::A_8 : false;


		switch (mode)
		{
		case BlendMode::Replace:
			glBlendEquation(GL_FUNC_ADD);
			glDisable(GL_BLEND);
			break;

		case BlendMode::Blend:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			if (bAlphaOnly)
				glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
			else
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
			break;

		case BlendMode::Morph:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
			break;

		case BlendMode::Add:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			if( bAlphaOnly )
				glBlendFunc(GL_ONE, GL_ONE);
			else
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
			break;

		case BlendMode::Subtract:
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
			if (bAlphaOnly)
				glBlendFunc(GL_ONE, GL_ONE);
			else
				glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ZERO, GL_ONE);
			break;

		case BlendMode::Multiply:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			if( bAlphaOnly )
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
			else
				glBlendFuncSeparate(GL_DST_COLOR, GL_ZERO, GL_ZERO, GL_ONE);
			break;

		case BlendMode::Invert:
			glBlendEquation(GL_FUNC_ADD);
			glEnable(GL_BLEND);
			if( bAlphaOnly )
				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
			else
				glBlendFuncSeparate(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ONE);
			break;

		case BlendMode::Min:
			glEnable(GL_BLEND);
			if (bAlphaOnly)
			{
				glBlendEquation(GL_MIN);
				glBlendFunc(GL_ONE, GL_ONE);
			}
			else
			{
				glBlendEquationSeparate(GL_MIN, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
			}
			break;

		case BlendMode::Max:
			glEnable(GL_BLEND);
			if (bAlphaOnly)
			{
				glBlendEquation(GL_MAX);
				glBlendFunc(GL_ONE, GL_ONE);
			}
			else
			{
				glBlendEquationSeparate(GL_MAX, GL_FUNC_ADD);
				glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE);
			}
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

		m_activeBlendMode = mode;

		LOG_GLERROR(glGetError());
	}

	//____ _setMorphFactor() __________________________________________________

	void GlGfxDevice::_setMorphFactor(float morphFactor)
	{
		glBlendColor(1.f, 1.f, 1.f, morphFactor);
	}

	//____ _setBlitSource() _______________________________________________________

	void GlGfxDevice::_setBlitSource( GlSurface * pSurf )
	{
		LOG_GLERROR(glGetError());

		glActiveTexture(GL_TEXTURE0);

		if( pSurf )
		{
			glBindTexture(GL_TEXTURE_2D, pSurf->getTexture());

			if( pSurf->m_bMipmapStale )
			{
				glGenerateMipmap(GL_TEXTURE_2D);
				pSurf->m_bMipmapStale = false;
			}

			m_pActiveBlitSource = pSurf;
			pSurf->m_bPendingReads = false;			// Clear this as we pass it by...

			m_canvasUBOBuffer.textureSize = pSurf->size();
			glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
			glBufferSubData(GL_UNIFORM_BUFFER, ((char*)&m_canvasUBOBuffer.textureSize) - ((char*)&m_canvasUBOBuffer), sizeof(SizeI), &m_canvasUBOBuffer.textureSize);

			if (pSurf->m_pClut)
			{
				glActiveTexture(GL_TEXTURE2);
				GLuint clutTex = pSurf->getClutTexture();
				glBindTexture(GL_TEXTURE_2D, clutTex);
				glActiveTexture(GL_TEXTURE0);

				assert(glGetError() == 0);
			}
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0 );

		LOG_GLERROR(glGetError());
	}

	//____ _setClearColor() ___________________________________________________

	void GlGfxDevice::_setClearColor(Color color)
	{
		float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		glClearColor( pConv[color.r]/4096.f, pConv[color.g]/4096.f, pConv[color.b]/4096.f, color.a/255.f );
	}

	//____ _setTintColor() ____________________________________________________

	void GlGfxDevice::_setTintColor(Color color)
	{
		float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		float r, g, b, a;

		m_canvasUBOBuffer.flatTint[0] = r = pConv[color.r];
		m_canvasUBOBuffer.flatTint[1] = g = pConv[color.g];
		m_canvasUBOBuffer.flatTint[2] = b = pConv[color.b];
		m_canvasUBOBuffer.flatTint[3] = a = m_linearToLinearTable[color.a];

		glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
		glBufferSubData(GL_UNIFORM_BUFFER, 16, 4 * 4 /*sizeof(canvasUBO)*/, &m_canvasUBOBuffer.flatTint);
	}

	//____ _setTintGradient() _________________________________________________

	void GlGfxDevice::_setTintGradient(const RectI& rect, const Color colors[4])
	{
		m_bGradientActive = true;

		m_canvasUBOBuffer.tintRect = rect;

		float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

		m_canvasUBOBuffer.topLeftTint[0] = pConv[colors[0].r];
		m_canvasUBOBuffer.topLeftTint[1] = pConv[colors[0].g];
		m_canvasUBOBuffer.topLeftTint[2] = pConv[colors[0].b];
		m_canvasUBOBuffer.topLeftTint[3] = m_linearToLinearTable[colors[0].a];

		m_canvasUBOBuffer.topRightTint[0] = pConv[colors[1].r];
		m_canvasUBOBuffer.topRightTint[1] = pConv[colors[1].g];
		m_canvasUBOBuffer.topRightTint[2] = pConv[colors[1].b];
		m_canvasUBOBuffer.topRightTint[3] = m_linearToLinearTable[colors[1].a];

		m_canvasUBOBuffer.bottomRightTint[0] = pConv[colors[2].r];
		m_canvasUBOBuffer.bottomRightTint[1] = pConv[colors[2].g];
		m_canvasUBOBuffer.bottomRightTint[2] = pConv[colors[2].b];
		m_canvasUBOBuffer.bottomRightTint[3] = m_linearToLinearTable[colors[2].a];

		m_canvasUBOBuffer.bottomLeftTint[0] = pConv[colors[3].r];
		m_canvasUBOBuffer.bottomLeftTint[1] = pConv[colors[3].g];
		m_canvasUBOBuffer.bottomLeftTint[2] = pConv[colors[3].b];
		m_canvasUBOBuffer.bottomLeftTint[3] = m_linearToLinearTable[colors[3].a];

		glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
		glBufferSubData(GL_UNIFORM_BUFFER, 16+16, 20 * 4 /*sizeof(canvasUBO)*/, &m_canvasUBOBuffer.tintRect);
	}

	//____ _clearTintGradient() _________________________________________________

	void GlGfxDevice::_clearTintGradient()
	{
		m_bGradientActive = false;
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
			GLchar	vertexShaderLog[4096];
			GLchar	fragmentShaderLog[4096];
			GLchar	programInfoLog[4096];

			GLsizei vertexShaderLogLength;
			GLsizei fragmentShaderLogLength;
			GLsizei programInfoLogLength;

			glGetShaderInfoLog(vertexShaderID, 4096, &vertexShaderLogLength, vertexShaderLog );
			glGetShaderInfoLog(fragmentShaderID, 4096, &fragmentShaderLogLength, fragmentShaderLog);
			glGetProgramInfoLog(programID, 4096, &programInfoLogLength, programInfoLog);

			char	buffer[4096*3+256];

			sprintf(buffer, "Failed compiling OpenGL shader\nVertexShaderLog: %s\nFragmentShaderLog: %s\nProgramInfoLog: %s", vertexShaderLog, fragmentShaderLog, programInfoLog);
			Base::handleError(ErrorSeverity::SilentFail, ErrorCode::OpenGL, buffer, this, TYPEINFO, __func__, __FILE__, __LINE__);
		}

		glDetachShader(programID, vertexShaderID);
		glDetachShader(programID, fragmentShaderID);

		glDeleteShader(vertexShaderID);
		glDeleteShader(fragmentShaderID);

		LOG_GLERROR(glGetError());

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

		// Init sRGBtoLinearTable

		float max = powf(255, 2.2f);

		for (int i = 0; i < 256; i++)
		{
			m_sRGBtoLinearTable[i] = powf(float(i), 2.2f)/max;
			m_linearToLinearTable[i] = i / 255.f;
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

