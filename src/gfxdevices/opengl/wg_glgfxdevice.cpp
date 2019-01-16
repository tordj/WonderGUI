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

	//____ Vertex and Fragment shaders ____________________________________________


	const char fillVertexShader[] =

		"#version 330 core\n"
		"uniform vec2 dimensions;                                  "
		"uniform int yOfs;										   "
		"uniform int yMul;										   "
		"layout(location = 0) in ivec2 pos;                        "
		"layout(location = 1) in vec4 color;                       "
		"out vec4 fragColor;                                       "
		"void main()                                               "
		"{                                                         "
		"   gl_Position.x = pos.x*2/dimensions.x - 1.0;            "
		"   gl_Position.y = (yOfs + yMul*pos.y)*2/dimensions.y - 1.0;    "
		"   gl_Position.z = 0.0;                                   "
		"   gl_Position.w = 1.0;                                   "
		"   fragColor = color;						"
		"}                                                         ";


	const char fillFragmentShader[] =

		"#version 330 core\n"
		"out vec4 outColor;                     "
		"in vec4 fragColor;                         "
		"void main()                            "
		"{                                      "
		"   outColor = fragColor;                   "
		"}                                      ";

	const char blitVertexShader[] =

		"#version 330 core\n"
		"uniform vec2 dimensions;                                  "
		"uniform int yOfs;										   "
		"uniform int yMul;										   "
		"uniform ivec2 texSize;									   "
		"uniform samplerBuffer extrasId;						   "
		"layout(location = 0) in ivec2 pos;                        "
		"layout(location = 1) in vec4 color;                       "
		"layout(location = 2) in int extrasOfs;                    "
		"out vec2 texUV;                                           "
		"out vec4 fragColor;                                       "
		"void main()                                               "
		"{                                                         "
		"   gl_Position.x = pos.x*2/dimensions.x - 1.0;            "
		"   gl_Position.y = (yOfs + yMul*pos.y)*2/dimensions.y - 1.0;            "
		"   gl_Position.z = 0.0;                                   "
		"   gl_Position.w = 1.0;                                   "
		"   vec4 srcDst = texelFetch(extrasId, extrasOfs);		   "
		"   vec4 transform = texelFetch(extrasId, extrasOfs+1);	   "
		"   vec2 src = srcDst.xy;                                  "
		"   vec2 dst = srcDst.zw;                                  "
		"   texUV.x = (src.x + (pos.x - dst.x) * transform.x + (pos.y - dst.y) * transform.z) / texSize.x; "
		"   texUV.y = (src.y + (pos.x - dst.x) * transform.y + (pos.y - dst.y) * transform.w) / texSize.y; "
		"   fragColor = color;						               "
		"}                                                         ";

	const char blitFragmentShader[] =

		"#version 330 core\n"

		"uniform sampler2D texId;						"
		"in vec2 texUV;									"
		"in vec4 fragColor;								"
		"out vec4 color;								"
		"void main()									"
		"{												"
		"   color = texture(texId, texUV) * fragColor;  "
		"}												";

	const char plotVertexShader[] =

		"#version 330 core\n"
		"uniform vec2 dimensions;                               "
		"uniform int yOfs;                                "
		"uniform int yMul;                                "
		"layout(location = 0) in ivec2 pos;                     "
		"layout(location = 1) in vec4 color;                       "
		"out vec4 fragColor;										"
		"void main()                                            "
		"{                                                      "
		"   gl_Position.x = (pos.x+0.5)*2.0/dimensions.x - 1.0; "
		"   gl_Position.y = ((yOfs + yMul*pos.y)+0.5)*2.0/dimensions.y - 1,0;	"
		"   gl_Position.z = 0.0;                                "
		"   gl_Position.w = 1.0;                                "
		"   fragColor = color;									"
		"}                                                      ";


	const char plotFragmentShader[] =

		"#version 330 core\n"
		"in vec4 fragColor;                     "
		"out vec4 outColor;                     "
		"void main()                            "
		"{                                      "
		"   outColor = fragColor;				"
		"}                                      ";



	const char lineFromToVertexShader[] =

		"#version 330 core\n"
		"uniform vec2 dimensions;                                   "
		"uniform int yOfs;                                    "
		"uniform int yMul;                                    "
		"uniform samplerBuffer extrasId;								"
		"layout(location = 0) in ivec2 pos;                         "
		"layout(location = 1) in vec4 color;                        "
		"layout(location = 2) in int extrasOfs;                       "
		"out vec4 fragColor;                                        "
		"flat out float s;												"
		"flat out float w;												"
		"flat out float slope;											"
		"flat out float ifSteep;											"
		"flat out float ifMild;											"
		"void main()                                                "
		"{                                                          "
		"   gl_Position.x = pos.x*2/dimensions.x - 1.0;             "
		"   gl_Position.y = (yOfs + yMul*pos.y)*2/dimensions.y - 1.0;             "
		"   gl_Position.z = 0.0;                                    "
		"   gl_Position.w = 1.0;                                    "
		"   fragColor = color;										"
		"   int ofs = extrasOfs;									"
		"   vec4 x = texelFetch(extrasId, ofs);						"
		"   s = x.x;												"
		"   w = x.y;												"
		"   slope = yMul*x.z;										"
		"   ifSteep = x.w;											"
		"   ifMild = 1.0 - ifSteep;									"
		"}                                                          ";
	

	const char lineFromToFragmentShader[] =

		"#version 330 core\n"
		"in vec4 fragColor;                     "
		"flat in float s;							"
		"flat in float w;							"
		"flat in float slope;						"
		"flat in float ifSteep;						"
		"flat in float ifMild;						"
		"out vec4 outColor;                     "
		"void main()                            "
		"{										"
		"   outColor.rgb = fragColor.rgb;		"
		"   outColor.a = fragColor.a * clamp(w - abs(gl_FragCoord.x*ifSteep + gl_FragCoord.y*ifMild - s - (gl_FragCoord.x*ifMild + gl_FragCoord.y*ifSteep) * slope), 0.0, 1.0); "
		"}                                      ";

	const char aaFillVertexShader[] =

		"#version 330 core\n"
		"uniform vec2 dimensions;                                   "
		"uniform int yOfs;                                    "
		"uniform int yMul;                                    "
		"uniform samplerBuffer extrasId;								"
		"layout(location = 0) in ivec2 pos;                         "
		"layout(location = 1) in vec4 color;                        "
		"layout(location = 2) in int extrasOfs;                       "
		"out vec4 fragColor;                                        "
		"flat out vec4 rect;										"
		"void main()                                                "
		"{                                                          "
		"   gl_Position.x = pos.x*2/dimensions.x - 1.0;             "
		"   gl_Position.y = (yOfs + yMul*pos.y)*2/dimensions.y - 1.0;             "
		"   gl_Position.z = 0.0;                                    "
		"   gl_Position.w = 1.0;                                    "
		"   fragColor = color;										"
		"   rect = texelFetch(extrasId, extrasOfs);					"
		"   rect.y = yOfs + yMul*rect.y;							"
		"   rect.zw += vec2(0.5f,0.5f);								"		// Adding offset here so we don't have to do it in pixel shader.
		"}                                                          ";


	const char aaFillFragmentShader[] =

		"#version 330 core\n"
//		"layout(pixel_center_integer) in vec4 gl_FragCoord;"
		"in vec4 fragColor;						"
		"flat in vec4 rect;						"
		"out vec4 outColor;                     "
		"void main()                            "
		"{										"
		"   outColor.rgb = fragColor.rgb;             "
		"	vec2 middleofs = abs(gl_FragCoord.xy - rect.xy);   "
		"	vec2 alphas = clamp(rect.zw  - middleofs, 0.f, 1.f);  "
		"	outColor.a = fragColor.a * alphas.x * alphas.y;  "
		"}                                      ";

	


	//____ create() _______________________________________________________________
	
	GlGfxDevice_p GlGfxDevice::create( const Rect& viewport )
	{
		GlGfxDevice_p p(new GlGfxDevice( viewport ));
		
		GLenum err = glGetError();
		if( err != 0 )
			return GlGfxDevice_p(nullptr);
		
		return p;
	}

	GlGfxDevice_p GlGfxDevice::create( GlSurface * pSurface )
	{
		GlGfxDevice_p p(new GlGfxDevice(pSurface));

		GLenum err = glGetError();
		if (err != 0)
			return GlGfxDevice_p(nullptr);

		return p;
	}
	//____ Constructor _____________________________________________________________

	GlGfxDevice::GlGfxDevice(GlSurface * pSurface) : GlGfxDevice(pSurface->size())
	{
		setCanvas(pSurface);
	}

	GlGfxDevice::GlGfxDevice(const Rect& viewport) : GlGfxDevice(viewport.size())
	{
		setCanvas(viewport);
	}

	GlGfxDevice::GlGfxDevice( Size viewportSize ) : GfxDevice(viewportSize)
	{
		m_canvasYstart = viewportSize.h;
		m_canvasYmul = -1;

		_initTables();

		GLint	texIdLoc, extrasIdLoc;

		// Create and init our shader programs

		m_fillProg			= _createGLProgram(fillVertexShader, fillFragmentShader);
		m_fillProgDimLoc	= glGetUniformLocation(m_fillProg, "dimensions");
		m_fillProgYofsLoc   = glGetUniformLocation(m_fillProg, "yOfs");
		m_fillProgYmulLoc   = glGetUniformLocation(m_fillProg, "yMul");

		assert(glGetError() == 0);

		m_aaFillProg = _createGLProgram(aaFillVertexShader, aaFillFragmentShader);
		m_aaFillProgDimLoc = glGetUniformLocation(m_aaFillProg, "dimensions");
		m_aaFillProgYofsLoc = glGetUniformLocation(m_aaFillProg, "yOfs");
		m_aaFillProgYmulLoc = glGetUniformLocation(m_aaFillProg, "yMul");
		extrasIdLoc = glGetUniformLocation(m_aaFillProg, "extrasId");

		glUseProgram(m_aaFillProg);
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

		assert(glGetError() == 0);

		m_blitProg			= _createGLProgram(blitVertexShader, blitFragmentShader);
		m_blitProgDimLoc	= glGetUniformLocation(m_blitProg, "dimensions");
		m_blitProgYofsLoc	= glGetUniformLocation(m_blitProg, "yOfs");
		m_blitProgYmulLoc	= glGetUniformLocation(m_blitProg, "yMul");
		m_blitProgTexSizeLoc = glGetUniformLocation(m_blitProg, "texSize");
		texIdLoc			= glGetUniformLocation(m_blitProg, "texId");
		extrasIdLoc			= glGetUniformLocation(m_blitProg, "extrasId");

		glUseProgram(m_blitProg);
		glUniform1i(texIdLoc, 0);			// Needs to be set. Texture unit 0 is used for textures.
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

		assert(glGetError() == 0);

		m_plotProg			= _createGLProgram(plotVertexShader, plotFragmentShader);
		m_plotProgDimLoc	= glGetUniformLocation(m_blitProg, "dimensions");
		m_plotProgYofsLoc = glGetUniformLocation(m_plotProg, "yOfs");
		m_plotProgYmulLoc = glGetUniformLocation(m_plotProg, "yMul");

		assert(glGetError() == 0);

		m_lineFromToProg = _createGLProgram(lineFromToVertexShader, lineFromToFragmentShader);
		m_lineFromToProgDimLoc = glGetUniformLocation(m_lineFromToProg, "dimensions");
		m_lineFromToProgYofsLoc = glGetUniformLocation(m_lineFromToProg, "yOfs");
		m_lineFromToProgYmulLoc = glGetUniformLocation(m_lineFromToProg, "yMul");
		extrasIdLoc = glGetUniformLocation(m_lineFromToProg, "extrasId");

		glUseProgram(m_lineFromToProg);
		glUniform1i(extrasIdLoc, 1);		// Needs to be set. Texture unit 1 is used for extras buffer.

		assert(glGetError() == 0);


		// Create the buffers we need for FrameBuffer and Vertices

		glGenFramebuffers(1, &m_framebufferId);

		glGenBuffers(1, &m_vertexBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, c_vertexBufferSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

		// Create and initialize our VertexArra

		glGenVertexArrays(1, &m_vertexArrayId);
		glBindVertexArray(m_vertexArrayId);

		glVertexAttribIPointer(
			0,						// attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,						// size
			GL_INT,					// type
			sizeof(Vertex),			// stride
			(void*)0				// array buffer offset
		);

		glVertexAttribPointer(
			1,						// attribute 0. No particular reason for 0, but must match the layout in the shader.
			GL_BGRA,				// size
			GL_UNSIGNED_BYTE,		// type
			GL_TRUE,				// normalized?
			sizeof(Vertex),			// stride
			(void*)sizeof(Coord)	// array buffer offset
		);

		assert(glGetError() == 0);

		glVertexAttribIPointer(
			2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			1,                  // size
			GL_INT,           // type
			sizeof(Vertex),		// stride
			(void*)(sizeof(Coord)+sizeof(Color))  // array buffer offset
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
		glDeleteProgram(m_blitProg);

		glDeleteFramebuffers(1, &m_framebufferId);

		glDeleteBuffers(1, &m_vertexBufferId);

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
		m_clip = m_canvasSize;

		m_canvasYstart = canvasSize.h;
		m_canvasYmul = -1;


		if (m_bRendering)
		{
			_setCanvas();
			_setClip();
		}
	
		return true;
	}

	bool GlGfxDevice::setCanvas( Surface * pSurface )
	{
		// Do NOT add any gl-calls here, INCLUDING glGetError()!!!
		// This method can be called without us having our GL-context.

		if (!pSurface || pSurface->className() != GlSurface::CLASSNAME)
			return false;

		m_pCanvas = pSurface;
		m_canvasSize = pSurface->size();
		m_clip = m_canvasSize;

		m_canvasYstart = 0;
		m_canvasYmul = 1;

		if (m_bRendering)
		{
			_setCanvas();
			_setClip();
		}

		return true;
	}

	//____ setClip() __________________________________________________________________

	void GlGfxDevice::setClip(const Rect& clip)
	{
		GfxDevice::setClip(clip);

		if( m_bRendering )
			_setClip();
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
			_setBlendMode();

		return true;
	}

	//____ setBlitSource() __________________________________________________________________

	bool GlGfxDevice::setBlitSource(Surface * pSource)
	{
		if (!pSource || pSource->className() != GlSurface::CLASSNAME)
			return false;

		m_pBlitSource = pSource;

		if (m_bRendering)
			_setBlitSource();

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

		//

		m_bRendering = true;
		m_op = BaseOperation::None;
		m_pFlushOp = &GlGfxDevice::_flushNone;
		m_vertexOfs = 0;
		m_extrasOfs = 0;

		_setCanvas();
		_setClip();
		_setBlendMode();
		_setBlitSource();

        // Prepare for rendering

		glBindVertexArray(m_vertexArrayId);


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

		(this->*m_pFlushOp)();

		//

        glFlush();

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

		glBindVertexArray(NULL);


		//

		assert(glGetError() == 0);
		m_bRendering = false;
		return true;
	}

	//____ fillPatches() ____ [standard] __________________________________________________

	void GlGfxDevice::fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches)
	{
		assert(glGetError() == 0);

		// Skip calls that won't affect destination

		if (col.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		// Clip our rectangle

		Rect clip(rect, m_clip);
		if (clip.w == 0 || clip.h == 0)
			return;

		//

		Color fillColor = col * m_tintColor;

		if (m_op != BaseOperation::Fill)
		{
			(this->*m_pFlushOp)();

			m_op = BaseOperation::Fill;
			m_pFlushOp = &GlGfxDevice::_flushTriangles;

			glUseProgram(m_fillProg);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

			assert(glGetError() == 0);
		}
		else if( m_vertexOfs >= c_vertexBufferSize - 6*nPatches )
		{
			_flushTriangles();
			assert(glGetError() == 0);
		}

		for (int i = 0; i < nPatches; i++)
		{
			Rect patch(pPatches[i], clip);
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

	//____ fillPatches() ____ [subpixel] __________________________________________________

	void GlGfxDevice::fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches)
	{
		assert(glGetError() == 0);

		// Skip calls that won't affect destination

		if (col.a == 0 && (m_blendMode == BlendMode::Blend))
			return;

		// Create our outer rectangle

		Rect outerRect( (int) rect.x, (int) rect.y, ((int) (rect.x+rect.w+0.999f)) - (int) rect.x, ((int) (rect.y + rect.h + 0.999f)) - (int) rect.y );

		// Clip our rectangle

		Rect clip(outerRect, m_clip);
		if (clip.w == 0 || clip.h == 0)
			return;

		//

		Color fillColor = col * m_tintColor;
		if (m_op != BaseOperation::FillSubPixel)
		{
			(this->*m_pFlushOp)();

			m_op = BaseOperation::FillSubPixel;
			m_pFlushOp = &GlGfxDevice::_flushTrianglesWithExtras;

			glUseProgram(m_aaFillProg);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}
		else if (m_vertexOfs >= c_vertexBufferSize - 6 * nPatches)
		{
			_flushTrianglesWithExtras();
			assert(glGetError() == 0);
		}

		for (int i = 0; i < nPatches; i++)
		{
			Rect patch(pPatches[i], clip);
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

	//____ plotPixelPatches() ______________________________________________________

	void GlGfxDevice::plotPixelPatches(int nPixels, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches)
	{
		assert(glGetError() == 0);

		if (nPixels == 0)
			return;

		if (m_op != BaseOperation::Plot)
		{
			(this->*m_pFlushOp)();

			m_op = BaseOperation::Plot;
			m_pFlushOp = &GlGfxDevice::_flushPoints;

			glUseProgram(m_plotProg);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}

		for (int i = 0; i < nPatches; i++)
		{
			Rect patch(pPatches[i], m_clip);
			if (patch.w > 0 && patch.h > 0)
			{
				for (int pixel = 0; pixel < nPixels; pixel++)
				{
					if (patch.contains(pCoords[pixel]))
					{
						m_vertexBufferData[m_vertexOfs].coord = pCoords[pixel];
						m_vertexBufferData[m_vertexOfs].color = pColors[pixel] * m_tintColor;
						m_vertexOfs++;

						if (m_vertexOfs == c_vertexBufferSize)
							_flushPoints();
					}
				}
			}
		}

		assert(glGetError() == 0);
	}

	//____ drawLinePatches() ____ [from/to] __________________________________________________

	void GlGfxDevice::drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches)
	{
		assert(glGetError() == 0);

		if (m_op != BaseOperation::LineFromTo)
		{
			(this->*m_pFlushOp)();

			m_op = BaseOperation::LineFromTo;
			m_pFlushOp = &GlGfxDevice::_flushTrianglesWithExtras;

			glUseProgram(m_lineFromToProg);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
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

			float   x1 = (float)begin.x;
			float   y1 = begin.y - width / 2;
			float   x2 = (float)end.x;
			float   y2 = end.y - width / 2;

			c1.x = x1;
			c1.y = y1 - 1;
			c2.x = x2;
			c2.y = y2 - 1;
			c3.x = x2;
			c3.y = y2 + width + 2;
			c4.x = x1;
			c4.y = y1 + width + 2;

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
			float   y1 = (float)begin.y;
			float   x2 = end.x - width / 2;
			float   y2 = (float)end.y;

			c1.x = x1 - 1;
			c1.y = y1;
			c2.x = x1 + width + 2;
			c2.y = y1;
			c3.x = x2 + width + 2;
			c3.y = y2;
			c4.x = x2 - 1;
			c4.y = y2;
		}

		// TODO: Flush the buffer if it gets full!
		// TODO: Add clipping!

		for (int i = 0; i < nPatches; i++)
		{
			Rect patch(pPatches[i], m_clip);
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

	//____ drawLinePatches() ____ [start/direction] __________________________________________________

	void GlGfxDevice::drawLinePatches(Coord begin, Direction dir, int length, Color col, float thickness, int nPatches, const Rect * pPatches)
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
				rect.y = begin.y - length;
				rect.w = thickness;
				rect.h = length;
				break;

			case Direction::Down:
				rect.x = begin.x + 0.5f - thickness/2;
				rect.y = begin.y;
				rect.w = thickness;
				rect.h = length;
				break;

			case Direction::Left:
				rect.x = begin.x - length;
				rect.y = begin.y + 0.5f - thickness/2;
				rect.w = length;
				rect.h = thickness;
				break;

			case Direction::Right:
				rect.x = begin.x;
				rect.y = begin.y + 0.5f - thickness/2;
				rect.w = length;
				rect.h = thickness;
				break;
		}


		// Create our outer rectangle

		Rect outerRect((int)rect.x, (int)rect.y, ((int)(rect.x + rect.w + 0.999f)) - (int)rect.x, ((int)(rect.y + rect.h + 0.999f)) - (int)rect.y);

		// Clip our rectangle

		Rect clip(outerRect, m_clip);
		if (clip.w == 0 || clip.h == 0)
			return;

		//

		Color fillColor = col * m_tintColor;
		if (m_op != BaseOperation::FillSubPixel)
		{
			(this->*m_pFlushOp)();

			m_op = BaseOperation::FillSubPixel;
			m_pFlushOp = &GlGfxDevice::_flushTrianglesWithExtras;

			glUseProgram(m_aaFillProg);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}
		else if (m_vertexOfs >= c_vertexBufferSize - 6 * nPatches)
		{
			_flushTrianglesWithExtras();
			assert(glGetError() == 0);
		}

		// Provide the patches

		for (int i = 0; i < nPatches; i++)
		{
			Rect patch(pPatches[i], clip);
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

	//____ transformBlitPatches() ____ [simple] __________________________________________________

	void GlGfxDevice::transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{
		assert(glGetError() == 0);

		if ( m_pBlitSource == nullptr )
			return;

		// Clip our rectangle

		Rect clip(dest, m_clip);
		if (clip.w == 0 || clip.h == 0)
			return;

		if (m_op != BaseOperation::Blit)
		{
			(this->*m_pFlushOp)();

			m_op = BaseOperation::Blit;
			m_pFlushOp = &GlGfxDevice::_flushTrianglesWithExtras;

			glUseProgram(m_blitProg);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}
		else if (m_vertexOfs >= c_vertexBufferSize - 6 * nPatches)
		{
			_flushTrianglesWithExtras();
		}

		float sw = (float)m_pBlitSource->width();
		float sh = (float)m_pBlitSource->height();

		for (int i = 0; i < nPatches; i++)
		{
			Rect patch(pPatches[i], clip);
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

		m_extrasBufferData[m_extrasOfs++] = src.x;
		m_extrasBufferData[m_extrasOfs++] = src.y;
		m_extrasBufferData[m_extrasOfs++] = dest.x;
		m_extrasBufferData[m_extrasOfs++] = dest.y;

		m_extrasBufferData[m_extrasOfs++] = simpleTransform[0][0];
		m_extrasBufferData[m_extrasOfs++] = simpleTransform[0][1];
		m_extrasBufferData[m_extrasOfs++] = simpleTransform[1][0];
		m_extrasBufferData[m_extrasOfs++] = simpleTransform[1][1];

		assert(glGetError() == 0);
	}

	//____ transformBlitPatches() ____ [complex] __________________________________________________

	void GlGfxDevice::transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches)
	{
		assert(glGetError() == 0);

		if (m_pBlitSource == nullptr)
			return;

		// Clip our rectangle

		Rect clip(dest, m_clip);
		if (clip.w == 0 || clip.h == 0)
			return;

		if (m_op != BaseOperation::Blit)
		{
			(this->*m_pFlushOp)();

			m_op = BaseOperation::Blit;
			m_pFlushOp = &GlGfxDevice::_flushTrianglesWithExtras;

			glUseProgram(m_blitProg);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
		}
		else if (m_vertexOfs >= c_vertexBufferSize - 12 * nPatches)
		{
			_flushTrianglesWithExtras();
		}

		float sw = (float)m_pBlitSource->width();
		float sh = (float)m_pBlitSource->height();

		for (int i = 0; i < nPatches; i++)
		{
			Rect patch(pPatches[i], clip);
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
		m_extrasBufferData[m_extrasOfs++] = dest.x;
		m_extrasBufferData[m_extrasOfs++] = dest.y;

		m_extrasBufferData[m_extrasOfs++] = complexTransform[0][0];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[0][1];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[1][0];
		m_extrasBufferData[m_extrasOfs++] = complexTransform[1][1];


		assert(glGetError() == 0);


	}

	//____ transformDrawSegmentPatches() ______________________________________________________

	void GlGfxDevice::transformDrawSegmentPatches(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{

	}

	//____ _flushNone() __________________________________________________________

	void GlGfxDevice::_flushNone()
	{
	}

	//____ _flushTriangles() __________________________________________________________

	void GlGfxDevice::_flushTriangles()
	{
		assert(glGetError() == 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexOfs*sizeof(Vertex), m_vertexBufferData);

		glDrawArrays(GL_TRIANGLES, 0, m_vertexOfs);
		m_vertexOfs = 0;

		assert(glGetError() == 0);

	}

	//____ _flushPoints() __________________________________________________________

	void GlGfxDevice::_flushPoints()
	{
		assert(glGetError() == 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexOfs * sizeof(Vertex), m_vertexBufferData);

		glDrawArrays(GL_POINTS, 0, m_vertexOfs);
		m_vertexOfs = 0;

		assert(glGetError() == 0);
	}

	//____ _flushTrianglesWithExtras() __________________________________________________________

	void GlGfxDevice::_flushTrianglesWithExtras()
	{
		assert(glGetError() == 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexOfs * sizeof(Vertex), m_vertexBufferData);

		glBindBuffer(GL_TEXTURE_BUFFER, m_extrasBufferId);
		glBufferSubData(GL_TEXTURE_BUFFER, 0, m_extrasOfs * sizeof(GLfloat), m_extrasBufferData);

		glDrawArrays(GL_TRIANGLES, 0, m_vertexOfs);
		m_vertexOfs = 0;
		m_extrasOfs = 0;

		assert(glGetError() == 0);
	}


	//____ _setCanvas() _______________________________________________________

	void GlGfxDevice::_setCanvas()
	{
		assert(glGetError() == 0);

		(this->*m_pFlushOp)();
		m_op = BaseOperation::None;							// We will loose our program so we need to reset these...
		m_pFlushOp = &GlGfxDevice::_flushNone;

		if (m_pCanvas)
		{
			auto pCanvas = GlSurface::cast(m_pCanvas);
			pCanvas->m_bBackingBufferStale = true;

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

		glViewport(0, 0, m_canvasSize.w, m_canvasSize.h);

		glUseProgram(m_fillProg);
		glUniform2f(m_fillProgDimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);
		glUniform1i(m_fillProgYofsLoc, m_canvasYstart);
		glUniform1i(m_fillProgYmulLoc, m_canvasYmul);

		glUseProgram(m_aaFillProg);
		glUniform2f(m_aaFillProgDimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);
		glUniform1i(m_aaFillProgYofsLoc, m_canvasYstart);
		glUniform1i(m_aaFillProgYmulLoc, m_canvasYmul);

		glUseProgram(m_blitProg);
		glUniform2f(m_blitProgDimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);
		glUniform1i(m_blitProgYofsLoc, m_canvasYstart);
		glUniform1i(m_blitProgYmulLoc, m_canvasYmul);

		glUseProgram(m_plotProg);
		glUniform2f(m_plotProgDimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);
		glUniform1i(m_plotProgYofsLoc, m_canvasYstart);
		glUniform1i(m_plotProgYmulLoc, m_canvasYmul);

		glUseProgram(m_lineFromToProg);
		glUniform2f(m_lineFromToProgDimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);
		glUniform1i(m_lineFromToProgYofsLoc, m_canvasYstart);
		glUniform1i(m_lineFromToProgYmulLoc, m_canvasYmul);

		assert(glGetError() == 0);
	}

	//____ _setClip() _______________________________________________________

	void GlGfxDevice::_setClip()
	{
		assert(glGetError() == 0);

		(this->*m_pFlushOp)();
		glScissor(m_clip.x, m_clip.y, m_clip.w, m_clip.h);

		assert(glGetError() == 0);
	}

	//____ _setBlendMode() _______________________________________________________

	void GlGfxDevice::_setBlendMode()
	{
		assert(glGetError() == 0);

		(this->*m_pFlushOp)();
		switch (m_blendMode)
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
			break;
		}
		assert(glGetError() == 0);
	}

	//____ _setBlitSource() _______________________________________________________

	void GlGfxDevice::_setBlitSource()
	{
		assert(glGetError() == 0);

		(this->*m_pFlushOp)();
		glActiveTexture(GL_TEXTURE0);

		if( m_pBlitSource.rawPtr() )
		{
			glBindTexture(GL_TEXTURE_2D, ((GlSurface*)m_pBlitSource.rawPtr())->getTexture());

			m_op = BaseOperation::Blit;
			m_pFlushOp = &GlGfxDevice::_flushTrianglesWithExtras;
			glUseProgram(m_blitProg);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glUniform2i(m_blitProgTexSizeLoc, m_pBlitSource->size().w, m_pBlitSource->size().h);
		}
		else
			glBindTexture(GL_TEXTURE_2D, 0 );

		assert(glGetError() == 0);
	}

	//____ _createGlProgram() ___________________________________________________

	GLuint GlGfxDevice::_createGLProgram(const char * pVertexShader, const char * pFragmentShader)
	{
		char log[1024];
		GLsizei logLen;

		GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vertexShaderID, 1, &pVertexShader, NULL);
		glCompileShader(vertexShaderID);

		glShaderSource(fragmentShaderID, 1, &pFragmentShader, NULL);
		glCompileShader(fragmentShaderID);

		glGetShaderInfoLog(fragmentShaderID, 1023, &logLen, log);

		GLuint  programID = glCreateProgram();
		glAttachShader(programID, vertexShaderID);
		glAttachShader(programID, fragmentShaderID);
		glLinkProgram(programID);

		// glLinkProgram doesn't use glGetError
		int mess = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &mess);
		//		assert(mess == GL_TRUE);

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

