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

#import "Foundation/Foundation.h"

#include <wg_metalgfxdevice.h>
#include <wg_base.h>
#include <wg_context.h>
#include <wg_util.h>


namespace wg
{

	const TypeInfo MetalGfxDevice::TYPEINFO = { "MetalGfxDevice", &GfxDevice::TYPEINFO };

    MetalGfxDevice *    MetalGfxDevice::s_pActiveDevice = nullptr;

    id<MTLDevice>       MetalGfxDevice::s_metalDevice = nil;
    id<MTLCommandQueue> MetalGfxDevice::s_metalCommandQueue = nil;

    //____ setMetalDevice() ______________________________________________________

    void MetalGfxDevice::setMetalDevice( id<MTLDevice> device )
    {
        s_metalDevice = device;
        
        if( device == nil )
            s_metalCommandQueue = nil;
        else
            s_metalCommandQueue = [s_metalDevice newCommandQueue];
    }

	//____ create() _______________________________________________________________

	MetalGfxDevice_p MetalGfxDevice::create( SizeI size, MTLRenderPassDescriptor* passDesc )
	{
		return MetalGfxDevice_p(new MetalGfxDevice(size, passDesc));
    }

    //____ constructor() ______________________________________________________

	MetalGfxDevice::MetalGfxDevice( SizeI size, MTLRenderPassDescriptor* passDesc ) : GfxDevice(size)
	{
        m_bFullyInitialized = true;

        m_canvasYstart = size.h;
        m_canvasYmul = -1;

        _initTables();
        
        //
        
        m_viewportSize = size;
        m_renderPassDesc = passDesc;
        
        NSError *error = nil;
        NSString *shaderSource = [[NSString alloc] initWithUTF8String:shaders];
                
        m_library = [s_metalDevice newLibraryWithSource:shaderSource options:nil error:&error];

        // Create and init Fill shader
        
        id<MTLFunction> vertexFunction = [m_library newFunctionWithName:@"fillVertexShader"];
        id<MTLFunction> fragmentFunction = [m_library newFunctionWithName:@"fillFragmentShader"];

        // Configure a pipeline descriptor that is used to create a pipeline state.
        MTLRenderPipelineDescriptor *pipelineStateDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineStateDescriptor.label = @"Simple Pipeline";
        pipelineStateDescriptor.vertexFunction = vertexFunction;
        pipelineStateDescriptor.fragmentFunction = fragmentFunction;
        pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

        m_cmdFill = [s_metalDevice newRenderPipelineStateWithDescriptor:pipelineStateDescriptor error:&error];
        
        assert(m_cmdFill);
                
        // Initialize our buffers
        
        m_pCommandBuffer = new int[m_commandBufferSize];

        m_vertexBufferId = [s_metalDevice newBufferWithLength:m_vertexBufferSize*sizeof(float) options:MTLResourceStorageModeShared];
        m_pVertexBuffer = (Vertex *)[m_vertexBufferId contents];

        m_extrasBufferId = [s_metalDevice newBufferWithLength:m_extrasBufferSize*sizeof(float) options:MTLResourceStorageModeShared];
        m_pExtrasBuffer = (float *)[m_extrasBufferId contents];
     
        // Initialize our uniform buffer
        
        m_uniformBufferId = [s_metalDevice newBufferWithLength:sizeof(UniformBuffer) options:MTLResourceStorageModeShared];
        m_pUniformBuffer = (UniformBuffer *)[m_uniformBufferId contents];
        
        m_pUniformBuffer->canvasDimX = size.w;
        m_pUniformBuffer->canvasDimY = size.h;
        m_pUniformBuffer->canvasYOfs = m_canvasYstart;
        m_pUniformBuffer->canvasYMul = m_canvasYmul;

        m_pUniformBuffer->flatTint = { 1.f, 1.f, 1.f, 1.f };
    }

    //____ destructor() _______________________________________________________

	MetalGfxDevice::~MetalGfxDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& MetalGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& MetalGfxDevice::surfaceType( void ) const
	{
        //TODO: RETURN SURFACES TYPEINFO INSTEAD OF OURS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        
		return MetalGfxDevice::TYPEINFO;
	}

	//____ surfaceFactory() _______________________________________________________

	SurfaceFactory_p MetalGfxDevice::surfaceFactory()
	{
		return SurfaceFactory_p();
	}

	//____ setCanvas() ________________________________________________________

	bool MetalGfxDevice::setCanvas(Surface * pCanvas, bool bResetClipList )
	{
		m_pCanvas = pCanvas;
		return true;
	}

    //____ setClipList() _______________________________________________________

    bool MetalGfxDevice::setClipList(int nRectangles, const RectI * pRectangles)
    {
        if (GfxDevice::setClipList(nRectangles, pRectangles))
        {
            m_clipCurrOfs = -1;
            return true;
        }
        return false;
    }

    //____ clearClipList() _______________________________________________________

void MetalGfxDevice::clearClipList()
    {
        GfxDevice::clearClipList();
        m_clipCurrOfs = -1;
    }

    void MetalGfxDevice::setTintColor(Color color)
    {
    
    }

    void MetalGfxDevice::setTintGradient(const RectI& rect, Color topLeft, Color topRight, Color bottomRight, Color bottomLeft)
    {
    
    }

    void MetalGfxDevice::clearTintGradient()
    {
    
    }

    bool MetalGfxDevice::setBlendMode(BlendMode blendMode)
    {
        return false;
    }
    
    bool MetalGfxDevice::setBlitSource(Surface * pSource)
    {
        return false;
    }

    void MetalGfxDevice::setMorphFactor(float factor)
    {
    }

    bool MetalGfxDevice::isCanvasReady() const
    {
        return true;
    }
 
    void MetalGfxDevice::setRenderPassDescriptor( MTLRenderPassDescriptor* passDesc )
    {
        m_renderPassDesc = passDesc;
    }

    void MetalGfxDevice::setDrawableToAutopresent( id<MTLDrawable> drawable )
    {
        m_drawableToAutoPresent = drawable;
    }


    bool MetalGfxDevice::beginRender()
    {
        
        if( m_bRendering == true )
            return false;

//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//            return false;

        // If there already is an active device, that needs to be flushed before we
        // take over the role as the active device.

        if (s_pActiveDevice)
            s_pActiveDevice->flush();

        m_pPrevActiveDevice = s_pActiveDevice;
        s_pActiveDevice = this;

        //

        m_bRendering = true;
        m_cmd = Command::None;
        m_pCmdFinalizer = &MetalGfxDevice::_dummyFinalizer;
        m_vertexOfs = 0;
        m_extrasOfs = 0;
        m_commandOfs = 0;
        m_surfaceOfs = 0;
        m_segmentsTintTexOfs = 0;
        m_clipWriteOfs = 0;
        m_clipCurrOfs = -1;

                
        // Create a new command buffer for each render pass to the current drawable.
        m_metalCommandBuffer = [s_metalCommandQueue commandBuffer];
        m_metalCommandBuffer.label = @"MetalGfxDevice";

        if(m_renderPassDesc == nil)
            return false;

        // Create a render command encoder.
        m_renderEncoder = [m_metalCommandBuffer renderCommandEncoderWithDescriptor:m_renderPassDesc];
        m_renderEncoder.label = @"MyRenderEncoder";

        _setCanvas( static_cast<MetalSurface*>(m_pCanvas.rawPtr()), m_canvasSize.w, m_canvasSize.h );
        _setBlendMode(m_blendMode);
        _setMorphFactor(m_morphFactor);

        if (m_bTintGradient)
            _setTintGradient(m_tintGradientRect, m_tintGradient);
        else
            _setTintColor(m_tintColor);

        _setBlitSource( static_cast<MetalSurface*>(m_pBlitSource.rawPtr()) );

        return true;
    }

    bool MetalGfxDevice::endRender()
    {
        if( !m_bRendering )
            return false;

        // Finalize any ongoing operation

        _endCommand();
        _executeBuffer();
        
        //
        
        [m_renderEncoder endEncoding];

        // Schedule a present once the framebuffer is complete using the current drawable.

        if( m_drawableToAutoPresent != nil )
            [m_metalCommandBuffer presentDrawable:m_drawableToAutoPresent];
        
        // Finalize rendering here & push the command buffer to the GPU.
        [m_metalCommandBuffer commit];

        m_bRendering = false;

        // Restore previously active device and exit

        s_pActiveDevice = m_pPrevActiveDevice;
        return true;
    }

    bool MetalGfxDevice::isIdle()
    {
        return !m_bRendering;
    }

    void MetalGfxDevice::flush()
    {
    
    }


    //____ fill() ____ [standard] __________________________________________________

    void MetalGfxDevice::fill(const RectI& rect, const Color& col)
    {
        // Skip calls that won't affect destination

        if (col.a == 0 && (m_blendMode == BlendMode::Blend))
            return;

        if (!rect.intersectsWith(m_clipBounds))
            return;

        //

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 4)
        {
            _resizeBuffers();
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
                int    dx1 = patch.x;
                int    dy1 = patch.y;
                int dx2 = patch.x + patch.w;
                int dy2 = patch.y + patch.h;

                m_pVertexBuffer[m_vertexOfs].coord.x = dx1;
                m_pVertexBuffer[m_vertexOfs].coord.y = dy1;
                m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                m_vertexOfs++;

                m_pVertexBuffer[m_vertexOfs].coord.x = dx2;
                m_pVertexBuffer[m_vertexOfs].coord.y = dy1;
                m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                m_vertexOfs++;

                m_pVertexBuffer[m_vertexOfs].coord.x = dx2;
                m_pVertexBuffer[m_vertexOfs].coord.y = dy2;
                m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                m_vertexOfs++;

                m_pVertexBuffer[m_vertexOfs].coord.x = dx1;
                m_pVertexBuffer[m_vertexOfs].coord.y = dy1;
                m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                m_vertexOfs++;

                m_pVertexBuffer[m_vertexOfs].coord.x = dx2;
                m_pVertexBuffer[m_vertexOfs].coord.y = dy2;
                m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                m_vertexOfs++;

                m_pVertexBuffer[m_vertexOfs].coord.x = dx1;
                m_pVertexBuffer[m_vertexOfs].coord.y = dy2;
                m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                m_vertexOfs++;
            }
        }

        float * pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        m_pExtrasBuffer[m_extrasOfs++] = pConv[col.r];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[col.g];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[col.b];
        m_pExtrasBuffer[m_extrasOfs++] = col.a / 255.f;
    }
/*
	void MetalGfxDevice::fill(const RectI& rect, const Color& col)
	{
        [m_renderEncoder setRenderPipelineState:m_cmdFill];

        Vertex triangleVertices[6];
     
        vector_uint2 _viewportSize;
        
        _viewportSize.x = m_viewportSize.w;
        _viewportSize.y = m_viewportSize.h;
        
        triangleVertices[0].position = {(float)rect.x,(float)rect.y};
        triangleVertices[0].color = {col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f};

        triangleVertices[1].position = {(float)(rect.x + rect.w),(float)rect.y};
        triangleVertices[1].color = {col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f};

        triangleVertices[2].position = {(float)(rect.x + rect.w),(float)(rect.y + rect.h)};
        triangleVertices[2].color = {col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f};

        triangleVertices[3].position = {(float)rect.x,(float)rect.y};
        triangleVertices[3].color = {col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f};

        triangleVertices[4].position = {(float)(rect.x + rect.w),(float)(rect.y + rect.h)};
        triangleVertices[4].color = {col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f};

        triangleVertices[5].position = {(float)(rect.x),(float)(rect.y + rect.h)};
        triangleVertices[5].color = {col.r/255.f, col.g/255.f, col.b/255.f, col.a/255.f};

        [m_renderEncoder setVertexBytes:triangleVertices length:sizeof(triangleVertices) atIndex:0];
        
        [m_renderEncoder setVertexBytes:&_viewportSize length:sizeof(_viewportSize) atIndex:1];

        // Draw the triangle.
        [m_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:0
                          vertexCount:6];
        
        
	}
*/
	void MetalGfxDevice::fill(const RectF& rect, const Color& col)
	{
	}

	void MetalGfxDevice::plotPixels(int nCoords, const CoordI * pCoords, const Color * pColors)
	{
	}

	void MetalGfxDevice::drawLine(CoordI begin, CoordI end, Color color, float thickness)
	{
	}

	void MetalGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
	{
	}

	void MetalGfxDevice::_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2])
	{
	}

	void MetalGfxDevice::_transformDrawSegments(const RectI& dest, int nSegments, const Color * pSegmentColors, int nEdges, const int * pEdges, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
	}


    //____ _dummyFinalizer() __________________________________________________________

    void MetalGfxDevice::_dummyFinalizer()
    {
    }

    //____ _drawCmdFinalizer() __________________________________________________________

    void MetalGfxDevice::_drawCmdFinalizer()
    {
        m_pCommandBuffer[m_commandOfs++] = m_vertexOfs - m_cmdBeginVertexOfs;
    }

    //____ _resizeBuffers() ____________________________________________________
    
    void MetalGfxDevice::_resizeBuffers()
    {
        if( m_commandOfs > m_commandBufferSize/2 )
        {
            m_commandBufferSize *= 2;
            int * pNewBuffer = new int[m_commandBufferSize];
            memcpy( pNewBuffer, m_pCommandBuffer, m_commandOfs * sizeof(int));
            delete [] m_pCommandBuffer;
            m_pCommandBuffer = pNewBuffer;
        }

        if( m_vertexOfs > m_vertexBufferSize/2 )
        {
            m_vertexBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_vertexBufferSize*sizeof(Vertex) options:MTLResourceStorageModeShared];

            Vertex * pNewBuffer = (Vertex *)[newId contents];
            memcpy( pNewBuffer, m_pVertexBuffer, m_vertexOfs * sizeof(float));
            m_pVertexBuffer = pNewBuffer;
            
//            [m_vertexBufferId release];
            m_vertexBufferId = newId;
        }

        if( m_extrasOfs > m_extrasBufferSize/2 || m_neededExtrasBufferSize > m_extrasBufferSize )
        {
            m_extrasBufferSize *= 2;
            while( m_extrasBufferSize < m_neededExtrasBufferSize )
                m_extrasBufferSize *= 2;
            m_neededExtrasBufferSize = 0;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_extrasBufferSize*sizeof(float) options:MTLResourceStorageModeShared];

            float * pNewBuffer = (float*)[newId contents];
            memcpy( pNewBuffer, m_pExtrasBuffer, m_extrasOfs * sizeof(float));
            m_pExtrasBuffer = pNewBuffer;
            
//            [m_extraBufferId release];
            m_extrasBufferId = newId;
        }
    }


    //____ _executeBuffer() ___________________________________________________

    void MetalGfxDevice::_executeBuffer()
    {
    /*
        LOG_GLERROR(glGetError());

        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, c_vertexBufferSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);        // Orphan current buffer if still in use.
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexOfs * sizeof(Vertex), m_vertexBufferData);            // Fill new buffer with as much data as needed.

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
    */
        
       
        
        int * pCmd = m_pCommandBuffer;
        int * pCmdEnd = &m_pCommandBuffer[m_commandOfs];

        int vertexOfs = 0;
        int surfaceOfs = 0;

        // Clear pending flags of active BlitSource and Canvas.

    //    if (m_pActiveBlitSource)
    //        m_pActiveBlitSource->m_bPendingReads = false;

        while (pCmd < pCmdEnd)
        {
            Command cmd = (Command) * pCmd++;

            switch (cmd)
            {
    /*
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
                case Command::SetMorphFactor:
                {
                    _setMorphFactor((*pCmd++) / 1024.f);
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
    */
                case Command::Fill:
                {
                    int nVertices = *pCmd++;
                    if( nVertices > 0 )
                    {
                        if( m_bGradientActive )
                        {
    //                        glUseProgram(m_fillGradientProg[m_bActiveCanvasIsA8]);
                        }
                        else
                            [m_renderEncoder setRenderPipelineState:m_cmdFill];

                        
                        [m_renderEncoder setVertexBuffer:m_vertexBufferId offset:0 atIndex:0];
                        
                        [m_renderEncoder setVertexBuffer:m_extrasBufferId offset:0 atIndex:1];
                        [m_renderEncoder setVertexBuffer:m_uniformBufferId offset:0 atIndex:2];

                        
                        
                        [m_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        vertexOfs += nVertices;

    //                    if( m_bMipmappedActiveCanvas )
    //                        m_pActiveCanvas->m_bMipmapStale = true;
                    }
                    break;

    /*
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
    */
                }
    /*
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
     */
                default:
    //                assert(false);
                    break;
            }
        }

        m_vertexOfs = 0;
        m_extrasOfs = 0;
        m_commandOfs = 0;
        m_surfaceOfs = 0;
        m_segmentsTintTexOfs = 0;
        m_clipWriteOfs = 0;
        m_clipCurrOfs = -1;

    //    LOG_GLERROR(glGetError());
    }

    //____ _setCanvas() _______________________________________________________

    void MetalGfxDevice::_setCanvas( MetalSurface * pCanvas, int width, int height )
    {
        bool bWasAlphaOnly = m_bActiveCanvasIsA8;

        if (pCanvas)
        {
/*
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
*/
        }
        else
        {
            // Set the region of the drawable to draw into.
            [m_renderEncoder setViewport:(MTLViewport){0.0, 0.0, (double) width, (double) height, 0.0, 1.0 }];

/*
            if( Base::activeContext()->gammaCorrection() )
                glEnable(GL_FRAMEBUFFER_SRGB);
            else
                glDisable(GL_FRAMEBUFFER_SRGB);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
 
            glViewport(0, 0, width, height);
            glScissor(0, 0, width, height);
 */
        }

        int canvasYstart    = pCanvas ? 0 : height;
        int canvasYmul        = pCanvas ? 1 : -1;


        // Updating canvas info for our shaders
/*
        m_pUniformBuffer->canvasDimX = (GLfloat) width;
        m_pUniformBuffer->canvasDimY = (GLfloat) height;
        m_pUniformBuffer->canvasYOfs = canvasYstart;
        m_pUniformBuffer->canvasYMul = canvasYmul;

        glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, 4*4, &m_canvasUBOBuffer);
*/
 
        m_pActiveCanvas = pCanvas;
        m_bMipmappedActiveCanvas = m_pActiveCanvas ? m_pActiveCanvas->isMipmapped() : false;

        bool bIsAlphaOnly = m_pActiveCanvas ? m_pActiveCanvas->pixelFormat() == PixelFormat::A_8 : false;

        if (bIsAlphaOnly != bWasAlphaOnly)
            _setBlendMode(m_activeBlendMode);

        m_bActiveCanvasIsA8 = bIsAlphaOnly;

    }

    //____ _setBlendMode() _______________________________________________________

    void MetalGfxDevice::_setBlendMode( BlendMode mode )
    {
/*
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
 */
    }

    //____ _setMorphFactor() __________________________________________________

    void MetalGfxDevice::_setMorphFactor(float morphFactor)
    {
//        glBlendColor(1.f, 1.f, 1.f, morphFactor);
    }

    //____ _setBlitSource() _______________________________________________________

    void MetalGfxDevice::_setBlitSource( MetalSurface * pSurf )
    {
/*
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
            pSurf->m_bPendingReads = false;            // Clear this as we pass it by...

            m_pUniformBuffer->textureSize = pSurf->size();
            glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
            glBufferSubData(GL_UNIFORM_BUFFER, ((char*)&m_pUniformBuffer->textureSize) - ((char*)&m_canvasUBOBuffer), sizeof(SizeI), &m_pUniformBuffer->textureSize);

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
*/
    }

    //____ _setTintColor() ____________________________________________________

    void MetalGfxDevice::_setTintColor(Color color)
    {
        float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        float r, g, b, a;

        m_pUniformBuffer->flatTint[0] = r = pConv[color.r];
        m_pUniformBuffer->flatTint[1] = g = pConv[color.g];
        m_pUniformBuffer->flatTint[2] = b = pConv[color.b];
        m_pUniformBuffer->flatTint[3] = a = m_linearToLinearTable[color.a];

//        glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
//        glBufferSubData(GL_UNIFORM_BUFFER, 16, 4 * 4 /*sizeof(canvasUBO)*/, &m_pUniformBuffer->flatTint);
    }

    //____ _setTintGradient() _________________________________________________

    void MetalGfxDevice::_setTintGradient(const RectI& rect, const Color colors[4])
    {
        m_bGradientActive = true;

        m_pUniformBuffer->tintRect = rect;

        float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        m_pUniformBuffer->topLeftTint[0] = pConv[colors[0].r];
        m_pUniformBuffer->topLeftTint[1] = pConv[colors[0].g];
        m_pUniformBuffer->topLeftTint[2] = pConv[colors[0].b];
        m_pUniformBuffer->topLeftTint[3] = m_linearToLinearTable[colors[0].a];

        m_pUniformBuffer->topRightTint[0] = pConv[colors[1].r];
        m_pUniformBuffer->topRightTint[1] = pConv[colors[1].g];
        m_pUniformBuffer->topRightTint[2] = pConv[colors[1].b];
        m_pUniformBuffer->topRightTint[3] = m_linearToLinearTable[colors[1].a];

        m_pUniformBuffer->bottomRightTint[0] = pConv[colors[2].r];
        m_pUniformBuffer->bottomRightTint[1] = pConv[colors[2].g];
        m_pUniformBuffer->bottomRightTint[2] = pConv[colors[2].b];
        m_pUniformBuffer->bottomRightTint[3] = m_linearToLinearTable[colors[2].a];

        m_pUniformBuffer->bottomLeftTint[0] = pConv[colors[3].r];
        m_pUniformBuffer->bottomLeftTint[1] = pConv[colors[3].g];
        m_pUniformBuffer->bottomLeftTint[2] = pConv[colors[3].b];
        m_pUniformBuffer->bottomLeftTint[3] = m_linearToLinearTable[colors[3].a];

//        glBindBuffer(GL_UNIFORM_BUFFER, m_canvasUBOId);
//        glBufferSubData(GL_UNIFORM_BUFFER, 16+16, 20 * 4 /*sizeof(canvasUBO)*/, &m_pUniformBuffer->tintRect);
    }

    //____ _clearTintGradient() _________________________________________________

    void MetalGfxDevice::_clearTintGradient()
    {
        m_bGradientActive = false;
    }


    //____ _initTables() ___________________________________________________________

    void MetalGfxDevice::_initTables()
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

    float MetalGfxDevice::_scaleThickness(float thickness, float slope)
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
