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
#include <assert.h>
#include <math.h>
#include <algorithm>

using namespace std;

namespace wg
{
	const char GlGfxDevice::CLASSNAME[] = {"GlGfxDevice"};

    
    
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
    "layout(location = 0) in vec2 pos;                         "
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
	
	//____ surfaceClassName() _______________________________________________________

	SurfaceFactory_p GlGfxDevice::surfaceFactory()
	{
		if( !m_pSurfaceFactory )
			m_pSurfaceFactory = GlSurfaceFactory::create();
	
		return m_pSurfaceFactory;
	}

	//____ setCanvas() __________________________________________________________________

	void GlGfxDevice::setCanvas( Size canvas )
	{
		m_canvasSize 	= canvas;
        
        glUseProgram( m_fillProg );
        GLint dimLoc = glGetUniformLocation( m_fillProg, "dimensions");
        glUniform2f(dimLoc, (GLfloat) canvas.w, (GLfloat) canvas.h);
        
        glUseProgram( m_aaFillProg );
        dimLoc = glGetUniformLocation( m_aaFillProg, "dimensions");
        glUniform2f(dimLoc, (GLfloat) canvas.w, (GLfloat) canvas.h);
        
        glUseProgram( m_mildSlopeProg );
        dimLoc = glGetUniformLocation( m_mildSlopeProg, "dimensions");
        glUniform2f(dimLoc, (GLfloat) canvas.w, (GLfloat) canvas.h);
        
        glUseProgram( m_steepSlopeProg );
        dimLoc = glGetUniformLocation( m_steepSlopeProg, "dimensions");
        glUniform2f(dimLoc, (GLfloat) canvas.w, (GLfloat) canvas.h);
        
        glUseProgram( m_blitProg );
        dimLoc = glGetUniformLocation( m_blitProg, "dimensions");
        glUniform2f(dimLoc, (GLfloat) canvas.w, (GLfloat) canvas.h);
        glUniform1i( m_blitProgTexIdLoc, 0 );
        
        glUseProgram( m_plotProg );
        dimLoc = glGetUniformLocation( m_plotProg, "dimensions");
        glUniform2f(dimLoc, (GLfloat) canvas.w, (GLfloat) canvas.h);
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

		//  Modify states

		glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glScissor( 0, 0, m_canvasSize.w, m_canvasSize.h );
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
        GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        
        glShaderSource(vertexShaderID, 1, &pVertexShader, NULL);
        glCompileShader(vertexShaderID);
        
        glShaderSource(fragmentShaderID, 1, &pFragmentShader, NULL);
        glCompileShader(fragmentShaderID);
        
        GLuint  programID = glCreateProgram();
        glAttachShader( programID, vertexShaderID );
        glAttachShader( programID, fragmentShaderID );
        glLinkProgram( programID );
        
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
        glVertexAttribPointer(
                              0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                              2,                  // size
                              GL_INT,             // type
                              GL_FALSE,           // normalized?
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

