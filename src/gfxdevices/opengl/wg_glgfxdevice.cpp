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

#include <wg_glgfxdevice.h>
#include <wg_glsurface.h>
#include <wg_glsurfacefactory.h>
#include <wg_base.h>
#include <assert.h>
#include <math.h>
#include <algorithm>

using namespace std;

namespace wg
{
	const char GlGfxDevice::CLASSNAME[] = { "GlGfxDevice" };



	//____ Vertex and Fragment shaders ____________________________________________

	const char fillVertexShader[] =

		"#version 330 core\n"
		"uniform vec2 dimensions;                                  "
		"layout(location = 0) in vec2 pos;                          "
		"void main()                                                "
		"{                                                          "
		"   gl_Position.x = pos.x*2/dimensions.x - 1.0;             "
		"   gl_Position.y = pos.y*2/dimensions.y - 1.0;             "
		"   gl_Position.z = 0.0;                                    "
		"   gl_Position.w = 1.0;                                    "
		"}                                                          ";


	const char fillFragmentShader[] =

		"#version 330 core\n"
		"uniform vec4 color;                    "
		"out vec4 outColor;                     "
		"void main()                            "
		"{                                      "
		"   outColor = color;                   "
		"}                                      ";

	/*
	while (amount[1] < 65536)
	{
	int aFrac = amount[0];
	int bFrac = amount[1];
	int cFrac = amount[2];
	int dFrac = amount[3];
	limit(aFrac, 0, 65536);
	limit(bFrac, 0, 65536);
	limit(cFrac, 0, 65536);
	limit(dFrac, 0, 65536);

	aFrac = ((aFrac - bFrac)*col[0].a) / 255;
	bFrac = ((bFrac - cFrac)*col[1].a) / 255;
	cFrac = ((cFrac - dFrac)*col[2].a) / 255;

	int backFraction = 65536 - aFrac - bFrac - cFrac;

	pDst[0] = (pDst[0] * backFraction + col[0].b * aFrac + col[1].b * bFrac + col[2].b * cFrac) >> 16;
	pDst[1] = (pDst[1] * backFraction + col[0].g * aFrac + col[1].g * bFrac + col[2].g * cFrac) >> 16;
	pDst[2] = (pDst[2] * backFraction + col[0].r * aFrac + col[1].r * bFrac + col[2].r * cFrac) >> 16;
	pDst += linePitch;

	for (int i = 0; i < 4; i++)
	amount[i] += inc[i];
	}


	*/



	const char horrWaveFragmentShader[] =

		"#version 330 core\n"
		"layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;"
		"uniform isamplerBuffer texId;          "
		"uniform vec2 windowOfs;				"
		"uniform vec4 topBorderColor;           "
		"uniform vec4 bottomBorderColor;        "
		"uniform vec4 frontColor;               "
		"uniform vec4 backColor;                "
		"out vec4 outColor;                     "
		"void main()                            "
		"{										"
		"	vec2 ofs = gl_FragCoord.xy - windowOfs;"
		"   int column = int(ofs.x)*9;			"
		"  float aFrac = texelFetch(texId, column).r/65536.0;"
		"  float aInc = texelFetch(texId, column+1).r/65536.0;"
		"  float dFrac = texelFetch(texId, column+6).r/65536.0;"
		"  float dInc = texelFetch(texId, column+7).r/65536.0;"

		"  aFrac = clamp(aFrac + aInc*ofs.y,0.0,1.0);"
		"  dFrac = clamp(dFrac + dInc*ofs.y,0.0,1.0);"

		"  if (aFrac == 0.0 || dFrac == 1.0) "
		"	discard; "

		"  float bFrac = texelFetch(texId, column+2).r/65536.0;"
		"  float bInc = texelFetch(texId, column+3).r/65536.0;"
		"  float cFrac = texelFetch(texId, column+4).r/65536.0;"
		"  float cInc = texelFetch(texId, column+5).r/65536.0;"
		"  int bFlipped = texelFetch(texId, column+8).r;"

		"  bFrac = clamp(bFrac + bInc*ofs.y,0.0,1.0);"
		"  cFrac = clamp(cFrac + cInc*ofs.y,0.0,1.0);"


		" aFrac -= bFrac;"
		" bFrac -= cFrac;"
		" cFrac -= dFrac;"
		" dFrac = 0.0;"

		" if(bFlipped != 0) "
		" { "
		"	float x = aFrac;"
		"	aFrac = cFrac;"
		"	cFrac = x;"
		"	dFrac = bFrac;"
		"	bFrac = 0.0;"
		" }"

		"  aFrac *= topBorderColor.a;"
		"  bFrac *= frontColor.a;"
		"  cFrac *= bottomBorderColor.a;"
		"  dFrac *= backColor.a;"

		"  float totalAlpha = aFrac + bFrac + cFrac + dFrac;"
		"  aFrac /= totalAlpha; "
		"  bFrac /= totalAlpha; "
		"  cFrac /= totalAlpha; "
		"  dFrac /= totalAlpha; "

		"   outColor.a = totalAlpha;  "
		"   outColor.r = aFrac*topBorderColor.r + bFrac*frontColor.r + cFrac*bottomBorderColor.r + dFrac*backColor.r;  "
		"   outColor.g = aFrac*topBorderColor.g + bFrac*frontColor.g + cFrac*bottomBorderColor.g + dFrac*backColor.g;  "
		"   outColor.b = aFrac*topBorderColor.b + bFrac*frontColor.b + cFrac*bottomBorderColor.b + dFrac*backColor.b;  "
		"}                                      ";


/* Original, unoptimized versions
    
    const char mildSlopeFragmentShader[] =
    
    "#version 330 core\n"
    "uniform vec4 color;                    "
    "uniform vec2 start;                   "
    "uniform float slope;                   "
    "uniform float width;                   "
    "out vec4 outColor;                     "
    "void main()                            "
    "{                                      "
    "   float center = start.y - (gl_FragCoord.x - start.x) * slope;    "
    "   float edgeDist = width - abs(gl_FragCoord.y - center); "        // Distance to line edge from fragCoords perspective, += fragCoord is inside line.
    "   float a = clamp(edgeDist+0.5, 0.0, 1.0);"
    "   outColor = color * a;               "
    "}                                      ";
    
    const char steepSlopeFragmentShader[] =
    
    "#version 330 core\n"
    "uniform vec4 color;                    "
    "uniform vec2 start;                   "
    "uniform float slope;                   "
    "uniform float width;                   "
    "out vec4 outColor;                     "
    "void main()                            "
    "{                                      "
    "   float center = start.x + (start.y - gl_FragCoord.y) * slope;    "
    "   float edgeDist = width - abs(gl_FragCoord.x - center); "        // Distance to line edge from fragCoords perspective, += fragCoord is inside line.
    "   float a = clamp(edgeDist+0.5, 0.0, 1.0);"
    "   outColor = color * a;               "
    "}                                      ";
*/
 
    
    const char steepSlopeFragmentShader[] =
    
    "#version 330 core\n"
    "uniform vec4 color;                    "
    "uniform float s;                       "       // start.x + start.y*slope
    "uniform float slope;                   "
    "uniform float w;                       "       // Horizontal line segment width + 0.5
    "out vec4 outColor;                     "
    "void main()                            "
    "{                                      "
    "   outColor.rgb = color.rgb;			"
	"	outColor.a = color.a * clamp(w - abs(gl_FragCoord.x - s + gl_FragCoord.y * slope), 0.0, 1.0); "
    "}                                      ";

    
    const char mildSlopeFragmentShader[] =
    
    "#version 330 core\n"
    "uniform vec4 color;                    "
    "uniform float s;                       "       // start.y + start.x*slope
    "uniform float slope;                   "
    "uniform float w;                       "       // Vertical line segment width + 0.5
    "out vec4 outColor;                     "
    "void main()                            "
    "{                                      "
	"   outColor.rgb = color.rgb;           "
    "   outColor.a = color.a * clamp(w - abs(gl_FragCoord.y - s + gl_FragCoord.x * slope), 0.0, 1.0); "
    "}                                      ";
    
    
    const char aaFillFragmentShader[] =
    
    "#version 330 core\n"
    "uniform vec4 color;                    "
    "uniform vec4 frame;                    "
    "uniform vec4 outsideAA;                "
    "out vec4 outColor;                     "
    "void main()                            "
    "{                                      "
    "   vec4 col = color;                   "
    "   if( gl_FragCoord.x < frame.x )      "
    "        col.a *= outsideAA.x;       "
    "   if( gl_FragCoord.y > frame.y )      "
    "        col.a *= outsideAA.y;       "
    "   if( gl_FragCoord.x > frame.z )      "
    "        col.a *= outsideAA.z;       "
    "   if( gl_FragCoord.y < frame.w )      "
    "        col.a *= outsideAA.w;       "
    "   outColor = col;                     "
    "}                                      ";

    
    
    const char blitVertexShader[] =
    
    "#version 330 core\n"
    "uniform vec2 dimensions;                                   "
    "layout(location = 0) in vec2 pos;                          "
    "in vec2 texPos;                                             "
    "out vec2 texUV;                                             "
    "void main()                                                "
    "{                                                          "
    "   gl_Position.x = pos.x*2/dimensions.x - 1.0;             "
    "   gl_Position.y = pos.y*2/dimensions.y - 1.0;             "
    "   gl_Position.z = 0.0;                                    "
    "   gl_Position.w = 1.0;                                    "
    "   texUV = texPos;                                         "
    "}                                                          ";


    const char blitFragmentShader[] =
    
    "#version 330 core\n"
    
    "uniform vec4 tint;                     "
    "uniform sampler2D texId;               "
    "in vec2 texUV;                         "
    "out vec4 color;                        "
    "void main()                            "
    "{                                      "
    "   color = texture(texId, texUV) * tint;     "
    "}                                      ";

    const char plotVertexShader[] =
    
    "#version 330 core\n"
    "uniform vec2 dimensions;                                  "
    "layout(location = 0) in ivec2 pos;                         "
    "in vec4 inColor;                                       "
    "out vec4 color;"
    "void main()                                                "
    "{                                                          "
    "   gl_Position.x = (pos.x+0.5)*2.0/dimensions.x - 1.0;             "
    "   gl_Position.y = 1.0 -(pos.y+0.5)*2.0/dimensions.y;             "
    "   gl_Position.z = 0.0;                                    "
    "   gl_Position.w = 1.0;                                    "
    "   color = inColor;"
    "}                                                          ";
    
    
    const char plotFragmentShader[] =
    
    "#version 330 core\n"
    "uniform vec4 tint;                     "
    "in vec4 color;                         "
    "out vec4 outColor;                     "
    "void main()                            "
    "{                                      "
    "   outColor = color * tint;            "
    "}                                      ";

    
    
    
	//____ create() _______________________________________________________________
	
	GlGfxDevice_p GlGfxDevice::create( Size canvasSize )
	{
		GlGfxDevice_p p(new GlGfxDevice( canvasSize ));
		
		GLenum err = glGetError();
		if( err != 0 )
			return GlGfxDevice_p(nullptr);
		
		return p;
	}


	//____ Constructor _____________________________________________________________

	GlGfxDevice::GlGfxDevice( Size canvas ) : GfxDevice(canvas)
	{
		m_bRendering = false;
		m_bFlipY = true;
        _initTables();
        
        
        m_fillProg = _createGLProgram( fillVertexShader, fillFragmentShader );
        m_fillProgColorLoc = glGetUniformLocation( m_fillProg, "color");

        m_aaFillProg = _createGLProgram( fillVertexShader, aaFillFragmentShader );
        m_aaFillProgColorLoc = glGetUniformLocation( m_aaFillProg, "color");
        m_aaFillProgFrameLoc = glGetUniformLocation( m_aaFillProg, "frame");
        m_aaFillProgOutsideAALoc = glGetUniformLocation( m_aaFillProg, "outsideAA");
        
        m_mildSlopeProg = _createGLProgram( fillVertexShader, mildSlopeFragmentShader );
        m_mildSlopeProgColorLoc = glGetUniformLocation( m_mildSlopeProg, "color");
        m_mildSlopeProgSlopeLoc = glGetUniformLocation( m_mildSlopeProg, "slope");
        m_mildSlopeProgSLoc     = glGetUniformLocation( m_mildSlopeProg, "s");
        m_mildSlopeProgWLoc     = glGetUniformLocation( m_mildSlopeProg, "w");

        m_steepSlopeProg = _createGLProgram( fillVertexShader, steepSlopeFragmentShader );
        m_steepSlopeProgColorLoc = glGetUniformLocation( m_steepSlopeProg, "color");
        m_steepSlopeProgSlopeLoc = glGetUniformLocation( m_steepSlopeProg, "slope");
        m_steepSlopeProgSLoc = glGetUniformLocation( m_steepSlopeProg, "s");
        m_steepSlopeProgWLoc = glGetUniformLocation( m_steepSlopeProg, "w");
        
        m_blitProg = _createGLProgram( blitVertexShader, blitFragmentShader );
        m_blitProgTintLoc = glGetUniformLocation( m_blitProg, "tint" );
        m_blitProgTexIdLoc = glGetUniformLocation( m_blitProg, "texId" );

		m_horrWaveProg = _createGLProgram(fillVertexShader, horrWaveFragmentShader);
		m_horrWaveProgTexIdLoc = glGetUniformLocation(m_horrWaveProg, "texId");
		m_horrWaveProgWindowOfsLoc = glGetUniformLocation(m_horrWaveProg, "windowOfs");
		m_horrWaveProgTopBorderColorLoc = glGetUniformLocation(m_horrWaveProg, "topBorderColor");
		m_horrWaveProgBottomBorderColorLoc = glGetUniformLocation(m_horrWaveProg, "bottomBorderColor");
		m_horrWaveProgFrontFillLoc = glGetUniformLocation(m_horrWaveProg, "frontColor");
		m_horrWaveProgBackFillLoc = glGetUniformLocation(m_horrWaveProg, "backColor");

        assert( glGetError() == 0 );
        m_plotProg = _createGLProgram( plotVertexShader, plotFragmentShader );
        m_plotProgTintLoc = glGetUniformLocation( m_plotProg, "tint" );
        assert( glGetError() == 0 );
        
        glGenVertexArrays(1, &m_vertexArrayId);
        glBindVertexArray(m_vertexArrayId);
        glGenBuffers(1, &m_vertexBufferId);
        
        glGenVertexArrays(1, &m_texCoordArrayId);
        glBindVertexArray(m_texCoordArrayId);
        glGenBuffers(1, &m_texCoordBufferId);
        glBindVertexArray(0);
 
		glGenFramebuffers(1, &m_framebufferId);

		glGenTextures(1, &m_horrWaveBufferTexture);
		glGenBuffers(1, &m_horrWaveBufferTextureData);

        setCanvas( canvas );
        setTintColor( Color::White );        
    }

	//____ Destructor ______________________________________________________________

	GlGfxDevice::~GlGfxDevice()
	{
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

	bool GlGfxDevice::setCanvas( Size dimensions )
	{
		m_pCanvas					= nullptr;
		m_bFlipY					= true;
		m_canvasSize 				= dimensions; 
		m_defaultFramebufferSize	= dimensions;
		_updateProgramDimensions();

		if (m_bRendering)
			return _setFramebuffer();

		return true;
	}

	bool GlGfxDevice::setCanvas( Surface * _pSurface )
	{
		if (!_pSurface)
			return setCanvas(m_defaultFramebufferSize);		// Revert back to default frame buffer.

		GlSurface * pSurface = GlSurface::cast(_pSurface);
		if (!pSurface)
			return false;			// Surface must be of type GlSurface!

		m_pCanvas		= pSurface;
		m_bFlipY		= false;
		m_canvasSize	= pSurface->size();
		_updateProgramDimensions();

		if (m_bRendering)
			return _setFramebuffer();

		return true;
	}

	//____ _setFramebuffer() ____________________________________________________

	bool GlGfxDevice::_setFramebuffer()
	{
		if (m_pCanvas)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferId);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GlSurface::cast(m_pCanvas)->getTexture(), 0);

			GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, drawBuffers);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				return false;
			}
		}
		else
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}


	//____ _updateProgramDimensions() ____________________________________________

	void GlGfxDevice::_updateProgramDimensions()
	{
		glUseProgram(m_fillProg);
		GLint dimLoc = glGetUniformLocation(m_fillProg, "dimensions");
		glUniform2f(dimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);

		glUseProgram(m_aaFillProg);
		dimLoc = glGetUniformLocation(m_aaFillProg, "dimensions");
		glUniform2f(dimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);

		glUseProgram(m_mildSlopeProg);
		dimLoc = glGetUniformLocation(m_mildSlopeProg, "dimensions");
		glUniform2f(dimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);

		glUseProgram(m_steepSlopeProg);
		dimLoc = glGetUniformLocation(m_steepSlopeProg, "dimensions");
		glUniform2f(dimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);

		glUseProgram(m_blitProg);
		dimLoc = glGetUniformLocation(m_blitProg, "dimensions");
		glUniform2f(dimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);
		glUniform1i(m_blitProgTexIdLoc, 0);

		glUseProgram(m_plotProg);
		dimLoc = glGetUniformLocation(m_plotProg, "dimensions");
		glUniform2f(dimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);

		glUseProgram(m_horrWaveProg);
		dimLoc = glGetUniformLocation(m_horrWaveProg, "dimensions");
		glUniform2f(dimLoc, (GLfloat)m_canvasSize.w, (GLfloat)m_canvasSize.h);

	}


	//____ setTintColor() __________________________________________________________

	void GlGfxDevice::setTintColor( Color color )
	{
		GfxDevice::setTintColor(color);

        glUseProgram( m_blitProg );
        glUniform4f( m_blitProgTintLoc, m_tintColor.r/255.f, m_tintColor.g/255.f, m_tintColor.b/255.f, m_tintColor.a/255.f );
        glUseProgram( m_plotProg );
        glUniform4f( m_plotProgTintLoc, m_tintColor.r/255.f, m_tintColor.g/255.f, m_tintColor.b/255.f, m_tintColor.a/255.f );
    }

	//____ setBlendMode() __________________________________________________________

	bool GlGfxDevice::setBlendMode( BlendMode blendMode )
	{
		if( blendMode != BlendMode::Blend && blendMode != BlendMode::Replace && 
			blendMode != BlendMode::Add && blendMode != BlendMode::Subtract && blendMode != BlendMode::Multiply &&
			blendMode != BlendMode::Invert )
				return false;
	 
		GfxDevice::setBlendMode(blendMode);
		if( m_bRendering )
			_setBlendMode(blendMode);

		return true;
	}

	//____ beginRender() ___________________________________________________________

	bool GlGfxDevice::beginRender()
	{

        if( m_bRendering == true )
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

		// Set correct framebuffer

		_setFramebuffer();

		//  Modify states

		glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glScissor( 0, 0, m_canvasSize.w, m_canvasSize.h );
		glViewport(0, 0, m_canvasSize.w, m_canvasSize.h);

		// Set correct blend mode

		_setBlendMode(m_blendMode);

        //

		m_bRendering = true;
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool GlGfxDevice::endRender()
	{
		if( m_bRendering == false )
			return false;

        glFlush();

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

		m_bRendering = false;
		return true;
	}



	//____ fill() __________________________________________________________________

	void GlGfxDevice::fill( const Rect& _rect, const Color& _col )
	{
		if( _col.a  == 0 || _rect.w < 1 || _rect.h < 1 )
			return;
 
        Color fillColor = _col * m_tintColor;
        
        int	dx1 = _rect.x;
		int	dy1 = m_canvasSize.h - _rect.y;
		int dx2 = _rect.x + _rect.w;
		int dy2 = m_canvasSize.h - (_rect.y + _rect.h);

        m_vertexBufferData[0] = (GLfloat) dx1;
        m_vertexBufferData[1] = (GLfloat) dy1;
        m_vertexBufferData[2] = (GLfloat) dx2;
        m_vertexBufferData[3] = (GLfloat) dy1;
        m_vertexBufferData[4] = (GLfloat) dx2;
        m_vertexBufferData[5] = (GLfloat) dy2;
        m_vertexBufferData[6] = (GLfloat) dx1;
        m_vertexBufferData[7] = (GLfloat) dy2;
        
        glUseProgram( m_fillProg );
        glUniform4f( m_fillProgColorLoc, fillColor.r/255.f, fillColor.g/255.f, fillColor.b/255.f, fillColor.a/255.f );
        
        glBindVertexArray(m_vertexArrayId);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
        glDisableVertexAttribArray(0);
  
		return;
	}

	//____ blit() __________________________________________________________________

	void GlGfxDevice::blit( Surface * _pSrc, const Rect& _src, Coord dest  )
	{
        if( !_pSrc )
			return;

		float sw = (float) _pSrc->width();
		float sh = (float) _pSrc->height();

		float	sx1 = _src.x/sw;
		float	sx2 = (_src.x+_src.w)/sw;
		float	sy1 = _src.y/sh;
		float	sy2 = (_src.y+_src.h)/sh;

		int		dx1 = dest.x;
		int		dx2 = dest.x + _src.w;
		int		dy1 = m_canvasSize.h - dest.y;
		int		dy2 = dy1 - _src.h;

        m_vertexBufferData[0] = (GLfloat) dx1;
        m_vertexBufferData[1] = (GLfloat) dy1;
        m_vertexBufferData[2] = (GLfloat) dx2;
        m_vertexBufferData[3] = (GLfloat) dy1;
        m_vertexBufferData[4] = (GLfloat) dx2;
        m_vertexBufferData[5] = (GLfloat) dy2;
        m_vertexBufferData[6] = (GLfloat) dx1;
        m_vertexBufferData[7] = (GLfloat) dy2;
        
        m_texCoordBufferData[0] = (GLfloat) sx1;
        m_texCoordBufferData[1] = (GLfloat) sy1;
        m_texCoordBufferData[2] = (GLfloat) sx2;
        m_texCoordBufferData[3] = (GLfloat) sy1;
        m_texCoordBufferData[4] = (GLfloat) sx2;
        m_texCoordBufferData[5] = (GLfloat) sy2;
        m_texCoordBufferData[6] = (GLfloat) sx1;
        m_texCoordBufferData[7] = (GLfloat) sy2;
        
        glActiveTexture(GL_TEXTURE0 );
        glBindTexture(GL_TEXTURE_2D, ((GlSurface*)(_pSrc))->getTexture());

        glUseProgram( m_blitProg );
        
        glBindVertexArray(m_vertexArrayId);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );

        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordArrayId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_texCoordBufferData), m_texCoordBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );

        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
       
	}

	//____ fillSubPixel() ______________________________________________________

	void GlGfxDevice::fillSubPixel( const RectF& rect, const Color& col )
	{
        if( col.a  == 0 )
            return;
        
        glUseProgram( m_aaFillProg );

        // Set color
        
        Color fillColor = col * m_tintColor;
        glUniform4f( m_aaFillProgColorLoc, fillColor.r/255.f, fillColor.g/255.f, fillColor.b/255.f, fillColor.a/255.f );

        // Calc frame coordinates and outside frame AA while we are still in normal coordinate space
        
        float frameX1 = (float)((int)(rect.x+0.999f));
        float frameY1 = (float)((int)(rect.y+0.999f));
        float frameX2 = (float)((int)(rect.x + rect.w));
        float frameY2 = (float)((int)(rect.y + rect.h));
        
        glUniform4f( m_aaFillProgOutsideAALoc, frameX1 - rect.x, frameY1 - rect.y, rect.x + rect.w - frameX2, rect.y + rect.h - frameY2 );

        // Set frame coords in GL coordinate space
        
        glUniform4f( m_aaFillProgFrameLoc, frameX1, m_canvasSize.h - frameY1, frameX2, m_canvasSize.h - frameY2 );
       
        
        // Convert rect to topLeft and bottomRight coordinates in GL coordinate space
        
        int	dx1 = (int) rect.x;
        int	dy1 = m_canvasSize.h - (int)rect.y;
        int dx2 = (int) (rect.x + rect.w+0.999f);
        int dy2 = m_canvasSize.h - (int)(rect.y + rect.h + 0.999f);
        
        // Set vertices
        
        m_vertexBufferData[0] = (GLfloat) dx1;
        m_vertexBufferData[1] = (GLfloat) dy1;
        m_vertexBufferData[2] = (GLfloat) dx2;
        m_vertexBufferData[3] = (GLfloat) dy1;
        m_vertexBufferData[4] = (GLfloat) dx2;
        m_vertexBufferData[5] = (GLfloat) dy2;
        m_vertexBufferData[6] = (GLfloat) dx1;
        m_vertexBufferData[7] = (GLfloat) dy2;

        // Set buffer and draw
        
        glBindVertexArray(m_vertexArrayId);
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
        glDisableVertexAttribArray(0);
        
        return;
	}


	//____ stretchBlitSubPixel() ___________________________________________________

	void GlGfxDevice::stretchBlitSubPixel( Surface * pSrc, float sx, float sy,
											 float sw, float sh,
											 float dx, float dy, float dw, float dh )
	{
		if( pSrc->scaleMode() == ScaleMode::Interpolate )
		{
			if( sw < dw )
				sx += 0.5f;
	
			if( sh < dh )
				sy += 0.5f;
		}


		if( !pSrc )
			return;

		float tw = (float) pSrc->width();
		float th = (float) pSrc->height();

		float	sx1 = sx/tw;
		float	sx2 = (sx+sw)/tw;
		float	sy1 = sy/th;
		float	sy2 = (sy+sh)/th;

		float	dx1 = dx;
		float	dx2 = dx + dw;
		float	dy1 = m_canvasSize.h - dy;
		float	dy2 = dy1 - dh;

        m_vertexBufferData[0] = dx1;
        m_vertexBufferData[1] = dy1;
        m_vertexBufferData[2] = dx2;
        m_vertexBufferData[3] = dy1;
        m_vertexBufferData[4] = dx2;
        m_vertexBufferData[5] = dy2;
        m_vertexBufferData[6] = dx1;
        m_vertexBufferData[7] = dy2;
        
        m_texCoordBufferData[0] = sx1;
        m_texCoordBufferData[1] = sy1;
        m_texCoordBufferData[2] = sx2;
        m_texCoordBufferData[3] = sy1;
        m_texCoordBufferData[4] = sx2;
        m_texCoordBufferData[5] = sy2;
        m_texCoordBufferData[6] = sx1;
        m_texCoordBufferData[7] = sy2;
        
        glActiveTexture(GL_TEXTURE0 );
        glBindTexture(GL_TEXTURE_2D, ((GlSurface*)(pSrc))->getTexture());
        
        glUseProgram( m_blitProg );
        
        glBindVertexArray(m_vertexArrayId);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordArrayId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_texCoordBufferData), m_texCoordBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
    }

	//____ stretchBlitSubPixelWithInvert() ___________________________________________________

	void GlGfxDevice::stretchBlitSubPixelWithInvert(Surface * pSrc, float sx, float sy,
		float sw, float sh, float dx, float dy, float dw, float dh)
	{
		if (pSrc->scaleMode() == ScaleMode::Interpolate)
		{
			if (sw < dw)
				sx += 0.5f;

			if (sh < dh)
				sy += 0.5f;
		}

		if (!pSrc)
			return;

		float tw = (float)pSrc->width();
		float th = (float)pSrc->height();

		float	sx1 = sx / tw;
		float	sx2 = (sx + sw) / tw;
		float	sy1 = 1.f - (sy / th);
		float	sy2 = 1.f - ((sy + sh) / th);

		float	dx1 = dx;
		float	dx2 = dx + dw;
		float	dy1 = m_canvasSize.h - dy;
		float	dy2 = dy1 - dh;

		m_vertexBufferData[0] = dx1;
		m_vertexBufferData[1] = dy1;
		m_vertexBufferData[2] = dx2;
		m_vertexBufferData[3] = dy1;
		m_vertexBufferData[4] = dx2;
		m_vertexBufferData[5] = dy2;
		m_vertexBufferData[6] = dx1;
		m_vertexBufferData[7] = dy2;

		m_texCoordBufferData[0] = sx1;
		m_texCoordBufferData[1] = sy1;
		m_texCoordBufferData[2] = sx2;
		m_texCoordBufferData[3] = sy1;
		m_texCoordBufferData[4] = sx2;
		m_texCoordBufferData[5] = sy2;
		m_texCoordBufferData[6] = sx1;
		m_texCoordBufferData[7] = sy2;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ((GlSurface*)(pSrc))->getTexture());

		glUseProgram(m_blitProg);

		glBindVertexArray(m_vertexArrayId);


		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glBindBuffer(GL_ARRAY_BUFFER, m_texCoordArrayId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_texCoordBufferData), m_texCoordBufferData, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);


		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
	}


	//____ _setBlendMode() _________________________________________________________

	void GlGfxDevice::_setBlendMode( BlendMode blendMode )
	{
		switch( blendMode )
		{
			case BlendMode::Replace:
                glBlendEquation( GL_FUNC_ADD );
				glDisable(GL_BLEND);
				break;

			case BlendMode::Blend:
                glBlendEquation( GL_FUNC_ADD );
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
				break;

			case BlendMode::Add:
                glBlendEquation( GL_FUNC_ADD );
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;

            case BlendMode::Subtract:
                glBlendEquation( GL_FUNC_REVERSE_SUBTRACT );
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                break;
                
			case BlendMode::Multiply:
                glBlendEquation( GL_FUNC_ADD );
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				break;

			case BlendMode::Invert:
                glBlendEquation( GL_FUNC_ADD );
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
				break;

			default:
				break;
		}
	}


    //____ _createGlProgram() ___________________________________________________
    
    GLuint GlGfxDevice::_createGLProgram( const char * pVertexShader, const char * pFragmentShader )
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
        glAttachShader( programID, vertexShaderID );
        glAttachShader( programID, fragmentShaderID );
        glLinkProgram( programID );
        
		// glLinkProgram doesn't use glGetError
		int mess = 0;
		glGetProgramiv(programID, GL_LINK_STATUS, &mess);
//		assert(mess == GL_TRUE);

        glDetachShader( programID, vertexShaderID );
        glDetachShader( programID, fragmentShaderID );
        
        glDeleteShader( vertexShaderID );
        glDeleteShader( fragmentShaderID );
        
        return programID;
    }
    
    
	//____ clipDrawHorrLine() __________________________________________________
	
	void GlGfxDevice::clipDrawHorrLine( const Rect& clip, const Coord& _start, int length, const Color& col )
	{
        if( col.a  == 0 || _start.y < clip.y || _start.y >= clip.y + clip.h )
            return;
        
        Coord start = _start;
        
        if( start.x < clip.x )
        {
            length -= clip.x - start.x;
            start.x = clip.x;
        }
        
        if( start.x + length > clip.x + clip.w )
            length = clip.x + clip.w - start.x;
        
        Color fillColor = col * m_tintColor;
        
        float	dx1 = start.x + 0.5f;
        float	dy = m_canvasSize.h - (start.y +0.5f);
        float   dx2 = start.x + length +0.5f;
        
        m_vertexBufferData[0] = dx1;
        m_vertexBufferData[1] = dy;
        m_vertexBufferData[2] = dx2;
        m_vertexBufferData[3] = dy;
        
        glUseProgram( m_fillProg );
        glUniform4f( m_fillProgColorLoc, fillColor.r/255.f, fillColor.g/255.f, fillColor.b/255.f, fillColor.a/255.f );
        
        glBindVertexArray(m_vertexArrayId);
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glDrawArrays(GL_LINES, 0, 2); // Starting from vertex 0; 2 vertices total -> 1 line
        glDisableVertexAttribArray(0);
        return;
		
	}


	//____ clipDrawVertLine() __________________________________________________
	
	void GlGfxDevice::clipDrawVertLine( const Rect& clip, const Coord& _start, int length, const Color& col )
	{
        if( col.a  == 0 || _start.x < clip.x || _start.x >= clip.x + clip.w )
            return;
        
        Coord start = _start;
        
        if( start.y < clip.y )
        {
            length -= clip.y - start.y;
            start.y = clip.y;
        }
        
        if( start.y + length > clip.y + clip.h )
            length = clip.y + clip.h - start.y;
        
        Color fillColor = col * m_tintColor;
        
        float	dx = start.x + 0.5f;
        float	dy1 = m_canvasSize.h - (start.y +0.5f);
        float   dy2 = m_canvasSize.h - (start.y + length +0.5f);
        
        m_vertexBufferData[0] = dx;
        m_vertexBufferData[1] = dy1;
        m_vertexBufferData[2] = dx;
        m_vertexBufferData[3] = dy2;
        
        glUseProgram( m_fillProg );
        glUniform4f( m_fillProgColorLoc, fillColor.r/255.f, fillColor.g/255.f, fillColor.b/255.f, fillColor.a/255.f );
        
        glBindVertexArray(m_vertexArrayId);
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glDrawArrays(GL_LINES, 0, 2); // Starting from vertex 0; 2 vertices total -> 1 line
        glDisableVertexAttribArray(0);
        return;
	}
	
	
	//____ clipPlotPixels() ________________________________________________________
	
	void GlGfxDevice::clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors)
    {
        glScissor( clip.x, m_canvasSize.h - clip.y - clip.h, clip.w, clip.h );
        plotPixels( nCoords, pCoords, pColors );
        glScissor( 0, 0, m_canvasSize.w, m_canvasSize.h );
    }
    

	//____ plotPixels() ________________________________________________________
    
    void GlGfxDevice::plotPixels( int nCoords, const Coord * pCoords, const Color * pColors)
    {
        if( nCoords == 0 )
            return;
        
        glUseProgram( m_plotProg );
        
        glBindVertexArray(m_vertexArrayId);
        
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Coord)*nCoords, pCoords, GL_DYNAMIC_DRAW);
        glVertexAttribIPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_INT,             // type
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glBindBuffer(GL_ARRAY_BUFFER, m_texCoordArrayId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Color)*nCoords, pColors, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              GL_BGRA,            // size
                              GL_UNSIGNED_BYTE,   // type
                              GL_TRUE,            // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_POINTS, 0, nCoords); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(0);
	}


	//____ drawLine() __________________________________________________________

	void GlGfxDevice::drawLine( Coord beg, Coord end, Color color, float thickness )
	{
        int 	length;
        float   width;
        float	slope;
        
        if( abs(beg.x-end.x) > abs(beg.y-end.y) )
        {
            // Prepare mainly horizontal line segment
            
            if( beg.x > end.x )
                swap( beg, end );
            
            length = end.x - beg.x;
            slope = ((float)(end.y - beg.y)) / length;
            
            width = _scaleThickness( thickness, slope );
 
            
            glUseProgram( m_mildSlopeProg );
            
            Color fillColor = color * m_tintColor;
            glUniform4f( m_mildSlopeProgColorLoc, fillColor.r/255.f, fillColor.g/255.f, fillColor.b/255.f, fillColor.a/255.f );            
//            glUniform2f( m_mildSlopeProgStartLoc, beg.x + 0.5, m_canvasSize.h - (beg.y + 0.5));
            glUniform1f( m_mildSlopeProgSLoc, (beg.x + 0.5f)*slope + (m_canvasSize.h - (beg.y + 0.5f)));
            glUniform1f( m_mildSlopeProgWLoc, width/2 + 0.5f );
            glUniform1f( m_mildSlopeProgSlopeLoc, slope );
            
            
            float   x1 = (float) beg.x;
            float   y1 = m_canvasSize.h - (beg.y - width/2);
            float   x2 = (float) end.x;
            float   y2 = m_canvasSize.h - (end.y - width/2);
            
            m_vertexBufferData[0] = x1;
            m_vertexBufferData[1] = y1 +1;
            m_vertexBufferData[2] = x2;
            m_vertexBufferData[3] = y2 +1;
            m_vertexBufferData[4] = x2;
            m_vertexBufferData[5] = y2 - width -2;
            m_vertexBufferData[6] = x1;
            m_vertexBufferData[7] = y1 - width -2;
        }
        else
        {
            // Prepare mainly vertical line segment
            
            if( beg.y > end.y )
                swap( beg, end );
            
            length = end.y - beg.y;
            if( length == 0 )
                return;											// TODO: Should stil draw the caps!
            
            slope = ((float)(end.x - beg.x)) / length;
            width = _scaleThickness( thickness, slope );
 
            glUseProgram( m_steepSlopeProg );
            
            Color fillColor = color * m_tintColor;
            glUniform4f( m_steepSlopeProgColorLoc, fillColor.r/255.f, fillColor.g/255.f, fillColor.b/255.f, fillColor.a/255.f );
//            glUniform2f( m_steepSlopeProgStartLoc, beg.x + 0.5, m_canvasSize.h - (beg.y + 0.5));
            glUniform1f( m_steepSlopeProgSLoc, (beg.x + 0.5f) + (m_canvasSize.h - (beg.y + 0.5f))*slope );
            glUniform1f( m_steepSlopeProgWLoc, width/2 + 0.5f );
            glUniform1f( m_steepSlopeProgSlopeLoc, slope );
            
            
            float   x1 = beg.x - width/2;
            float   y1 = (float) (m_canvasSize.h - beg.y);
            float   x2 = end.x - width/2;
            float   y2 = (float) (m_canvasSize.h - end.y);

            m_vertexBufferData[0] = x1 -1;
            m_vertexBufferData[1] = y1;
            m_vertexBufferData[2] = x1 + width + 2;
            m_vertexBufferData[3] = y1;
            m_vertexBufferData[4] = x2 + width + 2;
            m_vertexBufferData[5] = y2;
            m_vertexBufferData[6] = x2 -1;
            m_vertexBufferData[7] = y2;
        }



        
        // Set buffer and draw
        

        glBindVertexArray(m_vertexArrayId);
        
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
        glDisableVertexAttribArray(0);

	}
	
	//____ clipDrawLine() ______________________________________________________
	
	void GlGfxDevice::clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness )
	{
        glScissor( clip.x, m_canvasSize.h - clip.y - clip.h, clip.w, clip.h );
        drawLine( begin, end, color, thickness );
        glScissor( 0, 0, m_canvasSize.w, m_canvasSize.h );
	}

	//____ clipDrawHorrWave() _____________________________________________________

	void GlGfxDevice::clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine& topBorder, const WaveLine& bottomBorder, Color frontFill, Color backFill)
	{
		// Do early rough X-clipping with margin (need to trace lines with margin of thickest line).

		int ofs = 0;
		if (clip.x > begin.x || clip.x + clip.w < begin.x + length)
		{
			int margin = (int)(max(topBorder.thickness, bottomBorder.thickness) / 2 + 0.99);

			if (clip.x > begin.x + margin)
			{
				ofs = clip.x - begin.x - margin;
				begin.x += ofs;
				length -= ofs;
			}

			if (begin.x + length - margin > clip.x + clip.w)
				length = clip.x + clip.w - begin.x + margin;

			if (length <= 0)
				return;
		}

		// Generate line traces

		int	traceBufferSize = (length + 1) * 2 * sizeof(int) * 2;	// length+1 * values per point * sizeof(int) * 2 separate traces.
		char * pTraceBuffer = Base::memStackAlloc(traceBufferSize);
		int * pTopBorderTrace = (int*)pTraceBuffer;
		int * pBottomBorderTrace = (int*)(pTraceBuffer + traceBufferSize / 2);

		_traceLine(pTopBorderTrace, length + 1, topBorder, ofs);
		_traceLine(pBottomBorderTrace, length + 1, bottomBorder, ofs);

		// Do proper X-clipping

		int startColumn = 0;
		if (begin.x < clip.x)
		{
			startColumn = clip.x - begin.x;
			length -= startColumn;
			begin.x += startColumn;
		}

		if (begin.x + length > clip.x + clip.w)
			length = clip.x + clip.w - begin.x;

		// Box in drawing area

		Rect box;
		box.x = begin.x;
		box.w = length;

		int top = INT_MAX;
		int bottom = INT_MIN;

		for (int i = startColumn; i <= length+startColumn; i++)
		{
			int t = pTopBorderTrace[i * 2] < pBottomBorderTrace[i * 2] ? pTopBorderTrace[i * 2] : pBottomBorderTrace[i * 2];
			int b = pTopBorderTrace[i * 2 + 1] > pBottomBorderTrace[i * 2 + 1] ? pTopBorderTrace[i * 2 + 1] : pBottomBorderTrace[i * 2 + 1];

			if (t < top) top = t;
			if (b > bottom) bottom = b;
		}

		top = begin.y + (top >> 8);
		bottom = begin.y + ((bottom + 255) >> 8);


		box.y = top > clip.y ? top : clip.y;
		box.h = bottom < (clip.y + clip.h) ? bottom - box.y : clip.y + clip.h - box.y;

		if (box.w <= 0 || box.h <= 0)
			return;

		// Render columns

		int pos[2][4];						// Startpositions for the 4 fields of the column (topBorder, fill, bottomBorder, line end) for left and right edge of pixel column. 16 binals.

		int		textureBufferDataSize = (length) * sizeof(int) * 9;
		int * pTextureBufferData = (int*) Base::memStackAlloc( textureBufferDataSize );
		int * wpBuffer = pTextureBufferData;
		bool  bFlipped = false;

		for (int i = startColumn; i <= length+startColumn; i++)
		{
			// Old right pos becomes new left pos and old left pos will be reused for new right pos

			int * pLeftPos = pos[i % 2];
			int * pRightPos = pos[(i + 1) % 2];

			// Check if lines have intersected and in that case swap top and bottom lines and colors

			if (pTopBorderTrace[i * 2] > pBottomBorderTrace[i * 2])
			{
				swap(pTopBorderTrace, pBottomBorderTrace);
				bFlipped = !bFlipped;

				// We need to regenerate leftpos since we now have swapped top and bottom line.

				if (i > startColumn)
				{
					int j = i - 1;
					pLeftPos[0] = pTopBorderTrace[j * 2] << 8;
					pLeftPos[1] = pTopBorderTrace[j * 2 + 1] << 8;

					pLeftPos[2] = pBottomBorderTrace[j * 2] << 8;
					pLeftPos[3] = pBottomBorderTrace[j * 2 + 1] << 8;

					if (pLeftPos[2] < pLeftPos[1])
					{
						pLeftPos[2] = pLeftPos[1];
						if (pLeftPos[3] < pLeftPos[2])
							pLeftPos[3] = pLeftPos[2];
					}
				}
			}

			// Generate new rightpos table

			pRightPos[0] = pTopBorderTrace[i * 2] << 8;
			pRightPos[1] = pTopBorderTrace[i * 2 + 1] << 8;

			pRightPos[2] = pBottomBorderTrace[i * 2] << 8;
			pRightPos[3] = pBottomBorderTrace[i * 2 + 1] << 8;


			if (pRightPos[2] < pRightPos[1])
			{
				pRightPos[2] = pRightPos[1];
				if (pRightPos[3] < pRightPos[2])
					pRightPos[3] = pRightPos[2];
			}

			// Render the column

			if (i > startColumn)
			{
				// Calculate start amount and increment for our 4 fields

				for (int i = 0; i < 4; i++)
				{
					int yBeg;
					int64_t xInc;

					if (pLeftPos[i] < pRightPos[i])
					{
						yBeg = pLeftPos[i];
						xInc = (int64_t)65536 * 65536 / (pRightPos[i] - pLeftPos[i] + 1);
					}
					else
					{
						yBeg = pRightPos[i];
						xInc = (int64_t)65536 * 65536 / (pLeftPos[i] - pRightPos[i] + 1);
					}

					limit(xInc, (int64_t)0, (int64_t)65536);

					int64_t startAmount = -((xInc * yBeg) >> 16);
					
					* wpBuffer++ = (int)startAmount;
					* wpBuffer++ = (int)xInc;
				}
				*wpBuffer++ = (int) bFlipped;
			}
		}

		// Now we have the data generated, setup GL to operate on it

		glBindBuffer(GL_TEXTURE_BUFFER, m_horrWaveBufferTextureData);
		glBufferData(GL_TEXTURE_BUFFER, textureBufferDataSize, pTextureBufferData, GL_STREAM_DRAW);

		int	dx1 = box.x;
		int	dy1 = m_canvasSize.h - box.y;
		int dx2 = box.x + box.w;
		int dy2 = m_canvasSize.h - (box.y + box.h);

		m_vertexBufferData[0] = (GLfloat)dx1;
		m_vertexBufferData[1] = (GLfloat)dy1;
		m_vertexBufferData[2] = (GLfloat)dx2;
		m_vertexBufferData[3] = (GLfloat)dy1;
		m_vertexBufferData[4] = (GLfloat)dx2;
		m_vertexBufferData[5] = (GLfloat)dy2;
		m_vertexBufferData[6] = (GLfloat)dx1;
		m_vertexBufferData[7] = (GLfloat)dy2;

		glUseProgram(m_horrWaveProg);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_BUFFER, m_horrWaveBufferTexture);
		glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, m_horrWaveBufferTextureData);
		glUniform1i(m_horrWaveProgTexIdLoc, 0);
		glUniform2f(m_horrWaveProgWindowOfsLoc, begin.x, begin.y);
		glUniform4f(m_horrWaveProgTopBorderColorLoc, topBorder.color.r / 255.f, topBorder.color.g / 255.f, topBorder.color.b / 255.f, topBorder.color.a / 255.f);
		glUniform4f(m_horrWaveProgBottomBorderColorLoc, bottomBorder.color.r / 255.f, bottomBorder.color.g / 255.f, bottomBorder.color.b / 255.f, bottomBorder.color.a / 255.f);
		glUniform4f(m_horrWaveProgFrontFillLoc, frontFill.r / 255.f, frontFill.g / 255.f, frontFill.b / 255.f, frontFill.a / 255.f);
		glUniform4f(m_horrWaveProgBackFillLoc, backFill.r / 255.f, backFill.g / 255.f, backFill.b / 255.f, backFill.a / 255.f);

		glBindVertexArray(m_vertexArrayId);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData), m_vertexBufferData, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4); // Starting from vertex 0; 4 vertices total -> 2 triangles in the strip
		glDisableVertexAttribArray(0);

		// Free temporary work memory

		Base::memStackRelease(textureBufferDataSize);
		Base::memStackRelease(traceBufferSize);
	}


    //____ _initTables() ___________________________________________________________
    
    void GlGfxDevice::_initTables()
    {
       // Init lineThicknessTable
        
        for( int i = 0 ; i < 17 ; i++ )
        {
            double b = i/16.0;
            m_lineThicknessTable[i] = (float) sqrt( 1.0 + b*b );
        }
    }
	
	
	//____ _scaleThickness() ___________________________________________________
	
	float GlGfxDevice::_scaleThickness( float thickness, float slope )
	{
		slope = abs(slope);
		
		float scale = m_lineThicknessTable[(int)(slope*16)];
		
		if( slope < 1.f )
		{
			float scale2 = m_lineThicknessTable[(int)(slope*16)+1];
			scale += (scale2-scale)*((slope*16) - ((int)(slope*16)));
		}
		
		return thickness * scale;
	}
	
	
	
} // namespace wg

