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

	bool GlGfxDevice::setCanvas( const Rect& viewport )
	{
		// Do NOT add any gl-calls here, INCLUDING glGetError()!!!
		// This method can be called without us having our GL-context.

		return true;
	}

	bool GlGfxDevice::setCanvas( Surface * _pSurface )
	{
		// Do NOT add any gl-calls here, INCLUDING glGetError()!!!
		// This method can be called without us having our GL-context.

		return true;
	}

	//____ setClip() __________________________________________________________________

	void GlGfxDevice::setClip(const Rect& clip)
	{

	}

	//____ setTintColor() __________________________________________________________________

	void GlGfxDevice::setTintColor(Color color)
	{

	}

	//____ setBlendMode() __________________________________________________________________

	bool GlGfxDevice::setBlendMode(BlendMode blendMode)
	{

	}

	//____ setBlitSource() __________________________________________________________________

	bool GlGfxDevice::setBlitSource(Surface * pSource)
	{

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
/*
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

		// Update program dimensions

		_updateProgramDimensions();
        
		// Set correct framebuffer

		_setFramebuffer();

		// Set correct blend mode

		_setBlendMode(m_blendMode);
*/
        //

		assert( glGetError() == 0 );
		m_bRendering = true;
		return true;
	}

	//____ endRender() _____________________________________________________________

	bool GlGfxDevice::endRender()
	{
    	assert( glGetError() == 0 );
		if( m_bRendering == false )
			return false;

        glFlush();
/*
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
*/

		assert(glGetError() == 0);
		m_bRendering = false;
		return true;
	}

	//____ fillPatches() ____ [standard] __________________________________________________

	void GlGfxDevice::fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches)
	{
	}

	//____ fillPatches() ____ [subpixel] __________________________________________________

	void GlGfxDevice::fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches)
	{

	}

	//____ plotPixelPatches() ______________________________________________________

	void GlGfxDevice::plotPixelPatches(int nCoords, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches)
	{

	}

	//____ drawLinePatches() ____ [from/to] __________________________________________________

	void GlGfxDevice::drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches)
	{

	}

	//____ drawLinePatches() ____ [start/direction] __________________________________________________

	void GlGfxDevice::drawLinePatches(Coord begin, Direction dir, int length, Color col, float thickness, int nPatches, const Rect * pPatches)
	{

	}

	//____ transformBlitPatches() ____ [simple] __________________________________________________

	void GlGfxDevice::transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{

	}

	//____ transformBlitPatches() ____ [complex] __________________________________________________

	void GlGfxDevice::transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches)
	{

	}

	//____ transformDrawSegmentPatches() ______________________________________________________

	void GlGfxDevice::transformDrawSegmentPatches(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches)
	{

	}



	
	
} // namespace wg

