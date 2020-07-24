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

        // Create and init Fill pipelines
        
        m_pipelineFill = _compileRenderPipeline( @"Fill Pipeline", @"fillVertexShader", @"fillFragmentShader", BlendMode::Blend, PixelFormat::BGRA_8_linear );
        
        
        // Create and init GradientFill pipelines

        m_pipelineFillGradient = _compileRenderPipeline( @"GradientFill Pipeline", @"fillGradientVertexShader", @"fillFragmentShader", BlendMode::Blend, PixelFormat::BGRA_8_linear );

        // Create and init AA Fill pipelines

        m_pipelineFillAA = _compileRenderPipeline( @"FillAA Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader", BlendMode::Blend, PixelFormat::BGRA_8_linear );

           
        // Create and init AA GradientFill pipelines
 
        m_pipelineFillGradientAA = _compileRenderPipeline( @"GradientFillAA Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader", BlendMode::Blend, PixelFormat::BGRA_8_linear );

        
        // Create and init Blit pipelines
        
        m_pipelineBlit = _compileRenderPipeline( @"Blit Pipeline", @"blitVertexShader", @"blitFragmentShader", BlendMode::Blend, PixelFormat::BGRA_8_linear );

        // Create and init GradientBlit pipelines
         
        m_pipelineBlitGradient = _compileRenderPipeline( @"GradientBlit Pipeline", @"blitGradientVertexShader", @"blitFragmentShader", BlendMode::Blend, PixelFormat::BGRA_8_linear );

        
        // Initialize our buffers
        
        m_pCommandBuffer = new int[m_commandBufferSize];

        m_vertexBufferId = [s_metalDevice newBufferWithLength:m_vertexBufferSize*sizeof(float) options:MTLResourceStorageModeShared];
        m_pVertexBuffer = (Vertex *)[m_vertexBufferId contents];

        m_extrasBufferId = [s_metalDevice newBufferWithLength:m_extrasBufferSize*sizeof(float) options:MTLResourceStorageModeShared];
        m_pExtrasBuffer = (float *)[m_extrasBufferId contents];
     
        m_surfaceBufferId = [s_metalDevice newBufferWithLength:m_surfaceBufferSize*sizeof(MetalSurface_p) options:MTLResourceStorageModeShared];
        m_pSurfaceBuffer = (MetalSurface_p *)[m_surfaceBufferId contents];
        
        // Initialize our shader environment
                
        m_uniform.canvasDimX = size.w;
        m_uniform.canvasDimY = size.h;
        m_uniform.canvasYOfs = m_canvasYstart;
        m_uniform.canvasYMul = m_canvasYmul;

        m_uniform.flatTint = { 1.f, 1.f, 1.f, 1.f };
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

    //____ setTintColor() _______________________________________________________

    void MetalGfxDevice::setTintColor(Color color)
    {
        GfxDevice::setTintColor(color);

        if( m_bRendering )
        {
            _endCommand();
            _beginStateCommand(Command::SetTintColor, 1);
            m_pCommandBuffer[m_commandOfs++] = color.argb;
        }
    }

    //____ setTintGradient() ______________________________________________________

    void MetalGfxDevice::setTintGradient(const RectI& rect, Color topLeft, Color topRight, Color bottomRight, Color bottomLeft)
    {
        GfxDevice::setTintGradient(rect, topLeft, topRight, bottomRight, bottomLeft);

        if (m_bRendering)
        {
            _endCommand();
            _beginStateCommand(Command::SetTintGradient, 8);
            m_pCommandBuffer[m_commandOfs++] = rect.x;
            m_pCommandBuffer[m_commandOfs++] = rect.y;
            m_pCommandBuffer[m_commandOfs++] = rect.w;
            m_pCommandBuffer[m_commandOfs++] = rect.h;
            m_pCommandBuffer[m_commandOfs++] = topLeft.argb;
            m_pCommandBuffer[m_commandOfs++] = topRight.argb;
            m_pCommandBuffer[m_commandOfs++] = bottomRight.argb;
            m_pCommandBuffer[m_commandOfs++] = bottomLeft.argb;
        }
    }

    //____ clearTintGradient() _____________________________________________________

    void MetalGfxDevice::clearTintGradient()
    {
        GfxDevice::clearTintGradient();

        if (m_bRendering)
        {
            _endCommand();
            _beginStateCommand(Command::ClearTintGradient, 0);
        }
    }

    //____ setBlendMode() ____________________________________________________________

    bool MetalGfxDevice::setBlendMode(BlendMode blendMode)
    {
        if (blendMode == m_blendMode)
            return true;

        GfxDevice::setBlendMode(blendMode);

        if( m_bRendering )
        {
            _endCommand();
            _beginStateCommand(Command::SetBlendMode, 1);
            m_pCommandBuffer[m_commandOfs++] = (int) blendMode;
        }

        return true;
    }

    //____ setBlitSource() ___________________________________________________________

    bool MetalGfxDevice::setBlitSource(Surface * pSource)
    {
        if (!pSource || pSource->typeInfo() != MetalSurface::TYPEINFO)
            return false;

        m_pBlitSource = pSource;

        if (m_bRendering)
        {
            //TODO: Check so that we don't overrun m_pSurfaceBuffer;
            
            _endCommand();
            _beginStateCommand(Command::SetBlitSource, 0);
            m_pSurfaceBuffer[m_surfaceOfs++] = static_cast<MetalSurface*>(pSource);
        }

        return true;
    }

    //____ setMorphFactor() __________________________________________________________

    void MetalGfxDevice::setMorphFactor(float factor)
    {
        limit(factor, 0.f, 1.f);

        m_morphFactor = factor;

        if (m_bRendering)
        {
            _endCommand();
            _beginStateCommand(Command::SetMorphFactor, 1);
            m_pCommandBuffer[m_commandOfs++] = (int)(factor*1024);
        }
    }

    //____ isCanvasReady() ___________________________________________________________

    bool MetalGfxDevice::isCanvasReady() const
    {
        return m_bIdle;
    }

    //____ setRenderPassDescriptor() _________________________________________________

    void MetalGfxDevice::setRenderPassDescriptor( MTLRenderPassDescriptor* passDesc )
    {
        m_renderPassDesc = passDesc;
    }

    //____ setDrawableToAutopresent() ________________________________________________

    void MetalGfxDevice::setDrawableToAutopresent( id<MTLDrawable> drawable )
    {
        m_drawableToAutoPresent = drawable;
    }

    //____ beginRender() _____________________________________________________________

    bool MetalGfxDevice::beginRender()
    {
        if( m_bRendering == true || m_renderPassDesc == nil )
            return false;

        while( !m_bIdle )
            usleep(50);
              
        // If there already is an active device, that needs to be flushed before we
        // take over the role as the active device.

        if (s_pActiveDevice)
            s_pActiveDevice->flush();

        m_pPrevActiveDevice = s_pActiveDevice;
        s_pActiveDevice = this;

        //

        m_bIdle = false;
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

    //____ endRender() ___________________________________________________________

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

        // Add a completion handler and commit the command buffer.
        [m_metalCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Shared buffer is populated.
            m_bIdle = true;
        }];
        
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


    //____ flush() _______________________________________________________________

    void MetalGfxDevice::flush()
    {
        if( !m_bRendering )
            return;

        // Finalize any commands and execute queue.
        
        _endCommand();
        _executeBuffer();
                
        // Finalize queued rendering and push the comand buffer to the GPU.
        
        [m_renderEncoder endEncoding];
        [m_metalCommandBuffer commit];
        

        // Create a new command buffer.
        m_metalCommandBuffer = [s_metalCommandQueue commandBuffer];
        m_metalCommandBuffer.label = @"MetalGfxDevice";

        // Create a new render command encoder.
        m_renderEncoder = [m_metalCommandBuffer renderCommandEncoderWithDescriptor:m_renderPassDesc];
        m_renderEncoder.label = @"MyRenderEncoder";
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

    //____ fill() ____ [subpixel] __________________________________________________

	void MetalGfxDevice::fill(const RectF& rect, const Color& col)
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

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8)
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

        // Provide color

        float * pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        m_pExtrasBuffer[m_extrasOfs++] = pConv[col.r];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[col.g];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[col.b];
        m_pExtrasBuffer[m_extrasOfs++] = col.a / 255.f;

        // Provide rectangle center and radius

        SizeF    radius(rect.w / 2, rect.h / 2);
        CoordF    center(rect.x + radius.w, rect.y + radius.h);

        m_pExtrasBuffer[m_extrasOfs++] = center.x;
        m_pExtrasBuffer[m_extrasOfs++] = center.y;
        m_pExtrasBuffer[m_extrasOfs++] = radius.w;
        m_pExtrasBuffer[m_extrasOfs++] = radius.h;
	}

	void MetalGfxDevice::plotPixels(int nCoords, const CoordI * pCoords, const Color * pColors)
	{
	}

	void MetalGfxDevice::drawLine(CoordI begin, CoordI end, Color color, float thickness)
	{
	}

    //____ _transformBlit() ____ [simple] __________________________________________________

	void MetalGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
	{
        if (m_pBlitSource == nullptr)
            return;

        if (!dest.intersectsWith(m_clipBounds))
            return;

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8 )
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
                int        dx1 = patch.x;
                int        dx2 = patch.x + patch.w;
                int        dy1 = patch.y;
                int        dy2 = patch.y + patch.h;

                Vertex * pVertex = m_pVertexBuffer + m_vertexOfs;

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

        m_pExtrasBuffer[m_extrasOfs++] = (float) src.x;
        m_pExtrasBuffer[m_extrasOfs++] = (float) src.y;
        m_pExtrasBuffer[m_extrasOfs++] = (float) dest.x;
        m_pExtrasBuffer[m_extrasOfs++] = (float) dest.y;

        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[0][0];
        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[0][1];
        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[1][0];
        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[1][1];
    }

    //____ _transformBlit() ____ [complex] __________________________________________________

	void MetalGfxDevice::_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2])
	{
        if (m_pBlitSource == nullptr)
            return;

        if (!dest.intersectsWith(m_clipBounds))
            return;

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8)
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
                Vertex * pVertex = m_pVertexBuffer + m_vertexOfs;

                int        dx1 = patch.x;
                int        dx2 = patch.x + patch.w;
                int        dy1 = patch.y;
                int        dy2 = patch.y + patch.h;

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
            m_pExtrasBuffer[m_extrasOfs++] = src.x + 0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = src.y + 0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = GLfloat(dest.x) + 0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = GLfloat(dest.y) + 0.5f;
        }
        else
        {
            m_pExtrasBuffer[m_extrasOfs++] = src.x;
            m_pExtrasBuffer[m_extrasOfs++] = src.y;
            m_pExtrasBuffer[m_extrasOfs++] = GLfloat(dest.x) +0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = GLfloat(dest.y) +0.5f;
        }

        m_pExtrasBuffer[m_extrasOfs++] = complexTransform[0][0];
        m_pExtrasBuffer[m_extrasOfs++] = complexTransform[0][1];
        m_pExtrasBuffer[m_extrasOfs++] = complexTransform[1][0];
        m_pExtrasBuffer[m_extrasOfs++] = complexTransform[1][1];
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
        
        [m_renderEncoder setVertexBuffer:m_vertexBufferId offset:0 atIndex:(unsigned) VertexInputIndex::VertexBuffer];
        
        [m_renderEncoder setVertexBuffer:m_extrasBufferId offset:0 atIndex:(unsigned) VertexInputIndex::ExtrasBuffer];
        [m_renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];

        
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
    
                case Command::SetCanvas:
                {
                    _setCanvas(m_pSurfaceBuffer[surfaceOfs], pCmd[0], pCmd[1]);
                    m_pSurfaceBuffer[surfaceOfs++] = nullptr;
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
                    _setBlitSource(m_pSurfaceBuffer[surfaceOfs]);
                    m_pSurfaceBuffer[surfaceOfs++] = nullptr;
                    break;
                }
                case Command::Blit:
                {
                    int nVertices = *pCmd++;
                    if (nVertices > 0 && m_pActiveBlitSource)
                    {
                        MetalSurface* pSurf = m_pActiveBlitSource;
                                                
                        if( m_bGradientActive )
                            [m_renderEncoder setRenderPipelineState:m_pipelineBlitGradient];
                        else
                            [m_renderEncoder setRenderPipelineState:m_pipelineBlit];

                        [m_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
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
                            [m_renderEncoder setRenderPipelineState:m_pipelineFillGradient];
                        else
                            [m_renderEncoder setRenderPipelineState:m_pipelineFill];

                        [m_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
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
                            [m_renderEncoder setRenderPipelineState:m_pipelineFillGradientAA];
                        else
                            [m_renderEncoder setRenderPipelineState:m_pipelineFillAA];

                        [m_renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        vertexOfs += nVertices;

                        if( m_bMipmappedActiveCanvas )
                            m_pActiveCanvas->m_bMipmapStale = true;
                    }
                    break;
                }
                    /*

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
 */
        }

        int canvasYstart    = pCanvas ? 0 : height;
        int canvasYmul        = pCanvas ? 1 : -1;


        // Updating canvas info for our shaders

        m_uniform.canvasDimX = (GLfloat) width;
        m_uniform.canvasDimY = (GLfloat) height;
        m_uniform.canvasYOfs = canvasYstart;
        m_uniform.canvasYMul = canvasYmul;

        [m_renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex: (unsigned) VertexInputIndex::Uniform];

        //
        
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
        [m_renderEncoder setBlendColorRed:1.f green:1.f blue:1.f alpha:morphFactor];
    }

    //____ _setBlitSource() _______________________________________________________

    void MetalGfxDevice::_setBlitSource( MetalSurface * pSurf )
    {
        if( pSurf )
        {
            [m_renderEncoder setFragmentTexture:pSurf->getTexture() atIndex: (unsigned) TextureIndex::Texture];
            
            if( pSurf->m_bMipmapStale )
            {
//                glGenerateMipmap(GL_TEXTURE_2D);
                pSurf->m_bMipmapStale = false;
            }

            m_pActiveBlitSource = pSurf;
            pSurf->m_bPendingReads = false;            // Clear this as we pass it by...

            m_uniform.textureSize = pSurf->size();
            [m_renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];

            if (pSurf->m_pClut)
            {
                [m_renderEncoder setFragmentTexture:pSurf->getClutTexture() atIndex:(unsigned) TextureIndex::Clut];
            }
        }
        else
            [m_renderEncoder setFragmentTexture:nil atIndex:(unsigned) TextureIndex::Texture];


    }

    //____ _setTintColor() ____________________________________________________

    void MetalGfxDevice::_setTintColor(Color color)
    {
        float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        float r, g, b, a;

        m_uniform.flatTint[0] = r = pConv[color.r];
        m_uniform.flatTint[1] = g = pConv[color.g];
        m_uniform.flatTint[2] = b = pConv[color.b];
        m_uniform.flatTint[3] = a = m_linearToLinearTable[color.a];

        [m_renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];
    }

    //____ _setTintGradient() _________________________________________________

    void MetalGfxDevice::_setTintGradient(const RectI& rect, const Color colors[4])
    {
        m_bGradientActive = true;

        m_uniform.tintRect = rect;

        float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        m_uniform.topLeftTint[0] = pConv[colors[0].r];
        m_uniform.topLeftTint[1] = pConv[colors[0].g];
        m_uniform.topLeftTint[2] = pConv[colors[0].b];
        m_uniform.topLeftTint[3] = m_linearToLinearTable[colors[0].a];

        m_uniform.topRightTint[0] = pConv[colors[1].r];
        m_uniform.topRightTint[1] = pConv[colors[1].g];
        m_uniform.topRightTint[2] = pConv[colors[1].b];
        m_uniform.topRightTint[3] = m_linearToLinearTable[colors[1].a];

        m_uniform.bottomRightTint[0] = pConv[colors[2].r];
        m_uniform.bottomRightTint[1] = pConv[colors[2].g];
        m_uniform.bottomRightTint[2] = pConv[colors[2].b];
        m_uniform.bottomRightTint[3] = m_linearToLinearTable[colors[2].a];

        m_uniform.bottomLeftTint[0] = pConv[colors[3].r];
        m_uniform.bottomLeftTint[1] = pConv[colors[3].g];
        m_uniform.bottomLeftTint[2] = pConv[colors[3].b];
        m_uniform.bottomLeftTint[3] = m_linearToLinearTable[colors[3].a];

        [m_renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];
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

    //____ _compileRenderPipeline() _______________________________________________

    id<MTLRenderPipelineState> MetalGfxDevice::_compileRenderPipeline( NSString* label, NSString* vertexShader,
                                 NSString* fragmentShader, BlendMode blendMode, PixelFormat destFormat )
    {
        NSError *error = nil;
        MTLRenderPipelineDescriptor *descriptor = [[MTLRenderPipelineDescriptor alloc] init];

        descriptor.label = label;
        descriptor.vertexFunction = [m_library newFunctionWithName:vertexShader];
        descriptor.fragmentFunction = [m_library newFunctionWithName:fragmentShader];
        
        // Set pixelFormat

        switch(destFormat)
        {
            case PixelFormat::BGRA_8_linear:
            case PixelFormat::BGRX_8_linear:
                descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
                break;

            case PixelFormat::BGRA_8_sRGB:
            case PixelFormat::BGRX_8_sRGB:
                descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
                break;

            case PixelFormat::A_8:
                descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatA8Unorm;
                break;
                
            default:
                assert(false);
        }
        
        bool bAlphaOnly = (destFormat == PixelFormat::A_8);
        
        switch( blendMode )
        {
            case BlendMode::Replace:
                descriptor.colorAttachments[0].blendingEnabled = NO;
                break;

            case BlendMode::Blend:
                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                break;

            case BlendMode::Morph:
                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorBlendAlpha;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusBlendAlpha;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorBlendAlpha;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusBlendAlpha;
                break;

            case BlendMode::Add:
 
                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                else
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Subtract:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationReverseSubtract;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationReverseSubtract;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                else
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Multiply:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorDestinationColor;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorZero;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorDestinationAlpha;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorZero;
                }
                else
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Invert:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOneMinusDestinationColor;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceColor;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOneMinusDestinationAlpha;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                }
                else
                {
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Min:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationMin;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationMin;
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                else
                {
                    descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Max:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationMax;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationMax;
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                else
                {
                    descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                    descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                    descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Ignore:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                break;

            default:
                assert(false);
                break;
        }
        
        return [s_metalDevice newRenderPipelineStateWithDescriptor:descriptor error:&error];
    }



} // namespace wg
