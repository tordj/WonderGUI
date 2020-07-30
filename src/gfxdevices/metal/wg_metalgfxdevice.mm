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

        m_flushesInProgress = 0;
        
        m_canvasYstart = size.h;
        m_canvasYmul = -1;

        _initTables();
        
        //
        
        m_viewportSize = size;
        m_renderPassDesc = passDesc;
        
        NSError *error = nil;
        NSString *shaderSource = [[NSString alloc] initWithUTF8String:shaders];
                
        m_library = [s_metalDevice newLibraryWithSource:shaderSource options:nil error:&error];

        // Create and init Plot & Line pipelines

        for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
        {
//            if( blendMode != int(BlendMode::Ignore) && blendMode != int(BlendMode::Undefined) )
            {
                m_plotPipelines[blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Plot BGRA_8_linear Pipeline", @"plotVertexShader", @"plotFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_plotPipelines[blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Plot BGRA_8_sRGB Pipeline", @"plotVertexShader", @"plotFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_plotPipelines[blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"Plot A_8 Pipeline", @"plotVertexShader", @"plotFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );
                
                m_lineFromToPipelines[blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Line BGRA_8_linear Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_lineFromToPipelines[blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Line BGRA_8_sRGB Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_lineFromToPipelines[blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"Line A_8 Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );
            }
        }
        
        // Create and init Fill pipelines

        for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
        {
//            if( blendMode != int(BlendMode::Ignore) && blendMode != int(BlendMode::Undefined) )
            {
                m_fillPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Fill BGRA_8_linear Pipeline", @"fillVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Filll BGRA_8_sRGB Pipeline", @"fillVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillPipelines[0][blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"Fill A_8 Pipeline", @"fillVertexShader", @"fillFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_fillPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"GradientFill BGRA_8_linear Pipeline", @"fillGradientVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"GradientFilll BGRA_8_sRGB Pipeline", @"fillGradientVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillPipelines[1][blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"GradientFill A_8 Pipeline", @"fillGradientVertexShader", @"fillFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Fill BGRA_8_linear Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Filll BGRA_8_sRGB Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillAAPipelines[0][blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"Fill A_8 Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"GradientFill BGRA_8_linear Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"GradientFilll BGRA_8_sRGB Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillAAPipelines[1][blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"GradientFill A_8 Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );
            }
        }
        
        // Create and init Blit pipelines

        for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
        {
//            if( blendMode != int(BlendMode::Ignore) && blendMode != int(BlendMode::Undefined) )
            {
               
                    // [BlitFragShader][bGradient][BlendMode][DestFormat]
                
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Blit BGRA_8_linear Pipeline", @"blitVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"Blit BGRA_8_sRGB Pipeline", @"blitVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"Blit A_8 Pipeline", @"blitVertexShader", @"blitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Blit BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"Blit BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"Blit A_8 Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_blitPipelines[(int)BlitFragShader::ClutNearest][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"ClutBlitNearest BGRA_8_linear Pipeline", @"blitVertexShader", @"clutBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::ClutNearest][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"ClutBlitNearest BGRA_8_sRGB Pipeline", @"blitVertexShader", @"clutBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::ClutNearest][0][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"ClutBlitNearest A_8 Pipeline", @"blitVertexShader", @"clutBlitNearestFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_blitPipelines[(int)BlitFragShader::ClutNearest][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"ClutBlitNearest BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"clutBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::ClutNearest][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"ClutBlitNearest BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"clutBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::ClutNearest][1][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"ClutBlitNearest A_8 Gradient Pipeline", @"blitGradientVertexShader", @"clutBlitNearestFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_blitPipelines[(int)BlitFragShader::ClutInterpolated][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"ClutBlitInterpolated BGRA_8_linear Pipeline", @"clutBlitInterpolateVertexShader", @"clutBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::ClutInterpolated][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"ClutBlitInterpolated BGRA_8_sRGB Pipeline", @"clutBlitInterpolateVertexShader", @"clutBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::ClutInterpolated][0][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"ClutBlitInterpolated A_8 Pipeline", @"clutBlitInterpolateVertexShader", @"clutBlitInterpolateFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_blitPipelines[(int)BlitFragShader::ClutInterpolated][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"ClutBlitInterpolated BGRA_8_linear Gradient Pipeline", @"clutBlitInterpolateGradientVertexShader", @"clutBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::ClutInterpolated][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"ClutBlitInterpolated BGRA_8_sRGB Gradient Pipeline", @"clutBlitInterpolateGradientVertexShader", @"clutBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::ClutInterpolated][1][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"ClutBlitInterpolated A_8 Gradient Pipeline", @"clutBlitInterpolateGradientVertexShader", @"clutBlitInterpolateFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"A8SourceBlit BGRA_8_linear Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"A8SourceBlit BGRA_8_sRGB Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"A8SourceBlit A_8 Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );

                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"A8SourceBlit BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"A8SourceBlit BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::A_8]          = _compileRenderPipeline( @"A8SourceBlit A_8 Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::A_8 );
            }
        }
        
        // Create and init Segments pipelines

        
        NSString * segFragShaders[16] = { nil, @"segmentsFragmentShader1", @"segmentsFragmentShader2", @"segmentsFragmentShader3",
                                            @"segmentsFragmentShader4", @"segmentsFragmentShader5", @"segmentsFragmentShader6", @"segmentsFragmentShader7",
                                            @"segmentsFragmentShader8", @"segmentsFragmentShader9", @"segmentsFragmentShader10", @"segmentsFragmentShader11",
                                            @"segmentsFragmentShader12", @"segmentsFragmentShader13", @"segmentsFragmentShader14", @"segmentsFragmentShader15" };

        NSString * segFragShaders_A8[16] = { nil, @"segmentsFragmentShader1_A8", @"segmentsFragmentShader2_A8", @"segmentsFragmentShader3_A8",
                                            @"segmentsFragmentShader4_A8", @"segmentsFragmentShader5_A8", @"segmentsFragmentShader6_A8", @"segmentsFragmentShader7_A8",
                                            @"segmentsFragmentShader8_A8", @"segmentsFragmentShader9_A8", @"segmentsFragmentShader10_A8", @"segmentsFragmentShader11_A8",
                                            @"segmentsFragmentShader12_A8", @"segmentsFragmentShader13_A8", @"segmentsFragmentShader14_A8", @"segmentsFragmentShader15_A8" };

        
        for( int shader = 1 ; shader < 16 ; shader++ )
        {
            for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
            {
                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Segments BGRA_8_linear pipeline", @"segmentsVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_linear );

                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Segments BGRA_8_sRGB pipeline", @"segmentsVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );

                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"Segments A_8 pipeline", @"segmentsVertexShader", segFragShaders_A8[shader], (BlendMode) blendMode, PixelFormat::A_8 );

                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Segments BGRA_8_linear gradient pipeline", @"segmentsGradientVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_linear );

                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Segments BGRA_8_sRGB gradient pipeline", @"segmentsGradientVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );

                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::A_8] = _compileRenderPipeline( @"Segments A_8 gradient pipeline", @"segmentsGradientVertexShader", segFragShaders_A8[shader], (BlendMode) blendMode, PixelFormat::A_8 );
            }
        }
        
        // Create samplers
        
        MTLSamplerDescriptor *desc = [MTLSamplerDescriptor new];

        for( int mipmapped = 0 ; mipmapped < 2 ; mipmapped++ )
        {
            desc.mipFilter = (mipmapped == 0) ? MTLSamplerMipFilterNotMipmapped : MTLSamplerMipFilterLinear;
            
            for( int interpolated = 0 ; interpolated < 2 ; interpolated++ )
            {
                MTLSamplerMinMagFilter filter = (interpolated == 0) ? MTLSamplerMinMagFilterNearest : MTLSamplerMinMagFilterLinear;
                desc.minFilter = filter;
                desc.magFilter = filter;
                
                for( int tiled = 0 ; tiled < 2 ; tiled++ )
                {
                    MTLSamplerAddressMode addressMode = (tiled == 0) ? MTLSamplerAddressModeClampToZero : MTLSamplerAddressModeRepeat;
                    desc.rAddressMode = addressMode;
                    desc.sAddressMode = addressMode;
                    desc.tAddressMode = addressMode;

                    m_samplers[mipmapped][interpolated][tiled] = [s_metalDevice newSamplerStateWithDescriptor:desc];
                }
            }
        }
        
        desc = nil;
        
        // Initialize our buffers
        
        m_pCommandBuffer = new int[m_commandBufferSize];

        m_vertexBufferId = [s_metalDevice newBufferWithLength:m_vertexBufferSize*sizeof(Vertex) options:MTLResourceStorageModeShared];
        m_pVertexBuffer = (Vertex *)[m_vertexBufferId contents];

        m_extrasBufferId = [s_metalDevice newBufferWithLength:m_extrasBufferSize*sizeof(float) options:MTLResourceStorageModeShared];
        m_pExtrasBuffer = (float *)[m_extrasBufferId contents];

        m_clipListBufferId = [s_metalDevice newBufferWithLength:m_clipListBufferSize*sizeof(RectI) options:MTLResourceStorageModeShared];
        m_pClipListBuffer = (RectI *)[m_clipListBufferId contents];
        
        m_surfaceBufferId = [s_metalDevice newBufferWithLength:m_surfaceBufferSize*sizeof(MetalSurface_p) options:MTLResourceStorageModeShared];
        m_pSurfaceBuffer = (MetalSurface_p *)[m_surfaceBufferId contents];

        m_segEdgeBufferId = [s_metalDevice newBufferWithLength:m_segEdgeBufferSize*sizeof(float) options:MTLResourceStorageModeManaged];
        m_pSegEdgeBuffer = (float *)[m_segEdgeBufferId contents];
                
        m_segPalBufferId = [s_metalDevice newBufferWithLength:m_segPalBufferSize*c_segPalEntrySize options:MTLResourceStorageModeShared];
        m_pSegPalBuffer = (uint16_t *)[m_segPalBufferId contents];

        // Create the private segPal texture
        
        MTLTextureDescriptor *textureDescriptor = [[MTLTextureDescriptor alloc] init];

        textureDescriptor.pixelFormat   = MTLPixelFormatRGBA16Unorm;
        textureDescriptor.width         = c_maxSegments*2;
        textureDescriptor.height        = m_segPalBufferSize*2;   // Two pixels height for each palette entry.
        textureDescriptor.storageMode   = MTLStorageModePrivate;

        m_segPalTextureId = [MetalGfxDevice::s_metalDevice newTextureWithDescriptor:textureDescriptor];

        
        
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
/*
	//____ setCanvas() ________________________________________________________

	bool MetalGfxDevice::setCanvas(Surface * pCanvas, bool bResetClipList )
	{
		m_pCanvas = pCanvas;
		return true;
	}
*/
    //____ setCanvas() __________________________________________________________________

    bool MetalGfxDevice::setCanvas( SizeI canvasSize, bool bResetClipRects )
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
            _beginStateCommand(Command::SetCanvas, 2);
            m_pCommandBuffer[m_commandOfs++] = m_canvasSize.w;
            m_pCommandBuffer[m_commandOfs++] = m_canvasSize.h;
            m_pSurfaceBuffer[m_surfaceOfs++] = nullptr;
        }

        m_emptyCanvasSize = canvasSize;

        return true;
    }

    bool MetalGfxDevice::setCanvas( Surface * pSurface, bool bResetClipRects )
    {
        // Do NOT add any gl-calls here, INCLUDING glGetError()!!!
        // This method can be called without us having our GL-context.

        if( pSurface == nullptr )
            return setCanvas( m_emptyCanvasSize, bResetClipRects );

        if (!pSurface || pSurface->typeInfo() != MetalSurface::TYPEINFO)
            return false;

        if (pSurface->pixelFormat() != PixelFormat::CLUT_8_sRGB || pSurface->pixelFormat() == PixelFormat::CLUT_8_linear)
            return false;

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
            _beginStateCommand(Command::SetCanvas, 2);
            m_pCommandBuffer[m_commandOfs++] = m_canvasSize.w;
            m_pCommandBuffer[m_commandOfs++] = m_canvasSize.h;
            m_pSurfaceBuffer[m_surfaceOfs++] = static_cast<MetalSurface*>(pSurface);
        }
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
        return m_flushesInProgress == 0;
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
        
        m_bRendering = true;

        m_cmd = Command::None;
        m_pCmdFinalizer = &MetalGfxDevice::_dummyFinalizer;
        
        // If there already is an active device, that needs to be flushed before we
        // take over the role as the active device.

        if (s_pActiveDevice)
            s_pActiveDevice->flush();

        m_pPrevActiveDevice = s_pActiveDevice;
        s_pActiveDevice = this;
                
        // Create a new command buffer for each render pass to the current drawable.
        m_metalCommandBuffer = [s_metalCommandQueue commandBuffer];
        m_metalCommandBuffer.label = @"MetalGfxDevice";

        // Create a render command encoder.
        id<MTLRenderCommandEncoder> renderEncoder = [m_metalCommandBuffer renderCommandEncoderWithDescriptor:m_renderPassDesc];
        renderEncoder.label = @"MetalGfxDeviceRenderEncoder";

        _setCanvas( renderEncoder, static_cast<MetalSurface*>(m_pCanvas.rawPtr()), m_canvasSize.w, m_canvasSize.h );
        _setBlendMode( renderEncoder, m_blendMode);
        _setMorphFactor( renderEncoder, m_morphFactor);

         if (m_bTintGradient)
            _setTintGradient( renderEncoder, m_tintGradientRect, m_tintGradient);
        else
            _setTintColor( renderEncoder, m_tintColor);

        _setBlitSource( renderEncoder, static_cast<MetalSurface*>(m_pBlitSource.rawPtr()) );

        [renderEncoder endEncoding];

        // Wait for previous render pass to complete.
        
        if( m_flushesInProgress > 0 )
            flushAndWait();
        
        //

        _resetBuffers();
        
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

        m_flushesInProgress++;

        //
        
        // Schedule a present once the framebuffer is complete using the current drawable.

        if( m_drawableToAutoPresent != nil )
            [m_metalCommandBuffer presentDrawable:m_drawableToAutoPresent];

        // Add a completion handler.
        [m_metalCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Shared buffer is populated.
            m_flushesInProgress--;
        }];
        
        // Finalize rendering here & push the command buffer to the GPU.
        [m_metalCommandBuffer commit];

        m_bRendering = false;

        // Restore previously active device and exit

        s_pActiveDevice = m_pPrevActiveDevice;
        return true;
    }

    //____ isIdle() ______________________________________________________________

    bool MetalGfxDevice::isIdle()
    {
        return !m_bRendering && m_flushesInProgress == 0;
    }

    //____ flush() _______________________________________________________________

    void MetalGfxDevice::flush()
    {
        if( !m_bRendering )
            return;

        // Finalize any commands and execute queue.
        
        _endCommand();
        _executeBuffer();

        m_flushesInProgress++;

        // Add a completion handler.
        [m_metalCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Shared buffer is populated.
            m_flushesInProgress--;
        }];
        
        // Push the command buffer to the GPU.
        
        [m_metalCommandBuffer commit];
        
        // Create a new command buffer.
        m_metalCommandBuffer = [s_metalCommandQueue commandBuffer];
        m_metalCommandBuffer.label = @"MetalGfxDevice";
    }

    //____ flushAndWait() _______________________________________________________________

    void MetalGfxDevice::flushAndWait()
    {
        if( !m_bRendering )
            return;

        // Finalize any commands and execute queue.
        
        _endCommand();
        _executeBuffer();
                
        // Push the command buffer to the GPU.
        
        [m_metalCommandBuffer commit];
        [m_metalCommandBuffer waitUntilCompleted];
    
        _resetBuffers();
        
        // Create a new command buffer.
        m_metalCommandBuffer = [s_metalCommandQueue commandBuffer];
        m_metalCommandBuffer.label = @"MetalGfxDevice";
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
            _resizeBuffers();
        
        if (m_cmd != Command::Fill)
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
             _resizeBuffers();
        
        if (m_cmd != Command::FillSubPixel)
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

    //____ plotPixels() _________________________________________________________________

	void MetalGfxDevice::plotPixels(int nPixels, const CoordI * pCoords, const Color * pColors)
	{
        if (nPixels == 0)
            return;

        if (m_vertexOfs > m_vertexBufferSize - 1 || m_extrasOfs > m_extrasBufferSize - 4)
            _resizeBuffers();

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
                    m_pVertexBuffer[m_vertexOfs].coord = pCoords[pixel];
                    m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                    m_vertexOfs++;

                    Color color = pColors[pixel];

                    m_pExtrasBuffer[m_extrasOfs++] = pConv[color.r];
                    m_pExtrasBuffer[m_extrasOfs++] = pConv[color.g];
                    m_pExtrasBuffer[m_extrasOfs++] = pConv[color.b];
                    m_pExtrasBuffer[m_extrasOfs++] = color.a / 255.f;

                    if (m_vertexOfs == m_vertexBufferSize || m_extrasOfs == m_extrasBufferSize)
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

	void MetalGfxDevice::drawLine(CoordI begin, CoordI end, Color color, float thickness)
	{

        if (m_vertexOfs > m_vertexBufferSize - 6 || m_extrasOfs > m_extrasBufferSize - 8 )
              _resizeBuffers();

        if (m_cmd != Command::LineFromTo || m_clipCurrOfs == -1)
        {
            _endCommand();
            _beginClippedDrawCommand(Command::LineFromTo);
        }

        int     length;
        float   width;

        float    slope;
        float    s, w;
        bool    bSteep;

        CoordI    c1, c2, c3, c4;

        if (std::abs(begin.x - end.x) > std::abs(begin.y - end.y))
        {
            // Prepare mainly horizontal line segment

            if (begin.x > end.x)
                std::swap(begin, end);

            length = end.x - begin.x;
            if (length == 0)
                return;                                            // TODO: Should stil draw the caps!

            slope = ((float)(end.y - begin.y)) / length;
            width = _scaleThickness(thickness, slope);
            bSteep = false;

            s = ((begin.y + 0.5f) - (begin.x + 0.5f)*slope);
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
                std::swap(begin, end);

            length = end.y - begin.y;
            if (length == 0)
                return;                                            // TODO: Should stil draw the caps!

            slope = ((float)(end.x - begin.x)) / length;
            width = _scaleThickness(thickness, slope);
            bSteep = true;

//            s = (begin.x + 0.5f) - (m_canvasYstart + m_canvasYmul * (begin.y + 0.5f))*slope*m_canvasYmul;
            s = (begin.x + 0.5f) - ((begin.y + 0.5f))*slope;
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

        m_pVertexBuffer[m_vertexOfs].coord = c1;
        m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs/4;
        m_vertexOfs++;

        m_pVertexBuffer[m_vertexOfs].coord = c2;
        m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs/4;
        m_vertexOfs++;

        m_pVertexBuffer[m_vertexOfs].coord = c3;
        m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs/4;
        m_vertexOfs++;

        m_pVertexBuffer[m_vertexOfs].coord = c1;
        m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs/4;
        m_vertexOfs++;

        m_pVertexBuffer[m_vertexOfs].coord = c3;
        m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs/4;
        m_vertexOfs++;

        m_pVertexBuffer[m_vertexOfs].coord = c4;
        m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs/4;
        m_vertexOfs++;

        float * pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        m_pExtrasBuffer[m_extrasOfs++] = pConv[color.r];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[color.g];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[color.b];
        m_pExtrasBuffer[m_extrasOfs++] = color.a / 255.f;

        m_pExtrasBuffer[m_extrasOfs++] = s;
        m_pExtrasBuffer[m_extrasOfs++] = w;
        m_pExtrasBuffer[m_extrasOfs++] = slope;
        m_pExtrasBuffer[m_extrasOfs++] = bSteep;

	}

    //____ drawLine() ____ [start/direction] __________________________________________________

    void MetalGfxDevice::drawLine(CoordI begin, Direction dir, int length, Color color, float thickness)
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

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8)
            _resizeBuffers();

        if (m_cmd != Command::FillSubPixel)
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

        float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        m_pExtrasBuffer[m_extrasOfs++] = pConv[color.r];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[color.g];
        m_pExtrasBuffer[m_extrasOfs++] = pConv[color.b];
        m_pExtrasBuffer[m_extrasOfs++] = color.a / 255.f;

        // Provide rectangle center and raidus.

        SizeF    radius(rect.w / 2, rect.h / 2);
        CoordF    center(rect.x + radius.w, rect.y + radius.h);

        m_pExtrasBuffer[m_extrasOfs++] = center.x;
        m_pExtrasBuffer[m_extrasOfs++] = center.y;
        m_pExtrasBuffer[m_extrasOfs++] = radius.w;
        m_pExtrasBuffer[m_extrasOfs++] = radius.h;
    }


    //____ _transformBlit() ____ [simple] __________________________________________________

	void MetalGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
	{
        if (m_pBlitSource == nullptr)
            return;

        if (!dest.intersectsWith(m_clipBounds))
            return;

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8 )
            _resizeBuffers();

        if (m_cmd != Command::Blit)
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
            _resizeBuffers();

        if (m_cmd != Command::Blit)
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

    //____ _transformDrawSegments() ___________________________________________________

	void MetalGfxDevice::_transformDrawSegments(const RectI& _dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        if (!_dest.intersectsWith(m_clipBounds))
            return;

        //

        int segEdgeSpaceNeeded = 4 * (nEdgeStrips - 1)*(nSegments - 1);
                
        if( m_segPalOfs == m_segPalBufferSize )
            flushAndWait();
        
        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8 || m_segEdgeOfs > m_segEdgeBufferSize - segEdgeSpaceNeeded )            // various data, transform , colors, edgestrips
        {
            m_neededSegEdge = segEdgeSpaceNeeded;
            _resizeBuffers();
        }
        
        if (m_cmd != Command::Segments || m_nSegments != nSegments )
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
        if (uIncX != 0)                                // Columns are aligned horizontally
        {
            if (dest.w > maxCol)
            {
                if (uIncX < 0)
                    dest.x += dest.w - maxCol;
                dest.w = maxCol;
            }
        }
        else                                        // Columns are aligned vertically
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
                Vertex * pVertex = m_pVertexBuffer + m_vertexOfs;

                int        dx1 = patch.x;
                int        dx2 = patch.x + patch.w;
                int        dy1 = patch.y;
                int        dy2 = patch.y + patch.h;

                // Calc UV-coordinates. U is edge offset, V is pixel offset from begin in column.

                float    u1 = (float) (uTopLeft + (patch.x - dest.x) * simpleTransform[0][0] + (patch.y - dest.y) * simpleTransform[1][0]);
                float    v1 = (float) (vTopLeft + (patch.x - dest.x) * simpleTransform[0][1] + (patch.y - dest.y) * simpleTransform[1][1]);

                float    u2 = (float) (uTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][0] + (patch.y - dest.y) * simpleTransform[1][0]);
                float    v2 = (float) (vTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][1] + (patch.y - dest.y) * simpleTransform[1][1]);

                float    u3 = (float) (uTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][0] + (patch.y + patch.h - dest.y) * simpleTransform[1][0]);
                float    v3 = (float) (vTopLeft + (patch.x + patch.w - dest.x) * simpleTransform[0][1] + (patch.y + patch.h - dest.y) * simpleTransform[1][1]);

                float    u4 = (float) (uTopLeft + (patch.x - dest.x) * simpleTransform[0][0] + (patch.y + patch.h - dest.y) * simpleTransform[1][0]);
                float    v4 = (float) (vTopLeft + (patch.x - dest.x) * simpleTransform[0][1] + (patch.y + patch.h - dest.y) * simpleTransform[1][1]);

                float uMod = 0; //-0.5f;
                float vMod = -0.5f;
                
                CoordF    uv1 = { u1 + uMod, v1 + vMod };
                CoordF    uv2 = { u2 + uMod, v2 + vMod };
                CoordF    uv3 = { u3 + uMod, v3 + vMod };
                CoordF    uv4 = { u4 + uMod, v4 + vMod };


                //

                pVertex->coord.x = dx1;
                pVertex->coord.y = dy1;
                pVertex->extrasOfs = m_extrasOfs / 4;
                pVertex->uv = { uv1.x, uv1.y };
                pVertex++;

                pVertex->coord.x = dx2;
                pVertex->coord.y = dy1;
                pVertex->extrasOfs = m_extrasOfs / 4;
                pVertex->uv = { uv2.x, uv2.y };
                pVertex++;

                pVertex->coord.x = dx2;
                pVertex->coord.y = dy2;
                pVertex->extrasOfs = m_extrasOfs / 4;
                pVertex->uv = { uv3.x, uv3.y };
                pVertex++;

                pVertex->coord.x = dx1;
                pVertex->coord.y = dy1;
                pVertex->extrasOfs = m_extrasOfs / 4;
                pVertex->uv = { uv1.x, uv1.y };
                pVertex++;

                pVertex->coord.x = dx2;
                pVertex->coord.y = dy2;
                pVertex->extrasOfs = m_extrasOfs / 4;
                pVertex->uv = { uv3.x, uv3.y };
                pVertex++;

                pVertex->coord.x = dx1;
                pVertex->coord.y = dy2;
                pVertex->extrasOfs = m_extrasOfs / 4;
                pVertex->uv = { uv4.x, uv4.y };
                pVertex++;

                m_vertexOfs += 6;
            }
        }

        // Setup extras data

        GLfloat * pExtras = m_pExtrasBuffer + m_extrasOfs;

        // Add various data to extras

        pExtras[0] = (float) nSegments;
        pExtras[1] = (float) m_segEdgeOfs/4;
        pExtras[2] = (float)((_dest.w) * abs(simpleTransform[0][0]) + (_dest.h) * abs(simpleTransform[1][0]));
        pExtras[3] = (float)((_dest.w) * abs(simpleTransform[0][1]) + (_dest.h) * abs(simpleTransform[1][1]));
        pExtras[4] = float( 0.25f/c_maxSegments );
        pExtras[5] = float(m_segPalOfs + 0.25f) / m_segPalBufferSize;
        pExtras[6] = float(c_maxSegments*2);
        pExtras[7] = float(m_segPalBufferSize*2);

        m_extrasOfs += 8;
        
        // Add colors to segmentsTintTexMap

        float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;
        const Color* pSegCol = pSegmentColors;

        uint16_t* pMapRow = &m_pSegPalBuffer[m_segPalOfs*c_segPalEntrySize/2];
        int            mapPitch = c_maxSegments * 4 * 2;

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

                    pMapRow[i * 8 + 0] = r;
                    pMapRow[i * 8 + 1] = g;
                    pMapRow[i * 8 + 2] = b;
                    pMapRow[i * 8 + 3] = a;
                    pMapRow[i * 8 + 4] = r;
                    pMapRow[i * 8 + 5] = g;
                    pMapRow[i * 8 + 6] = b;
                    pMapRow[i * 8 + 7] = a;

                    pMapRow[mapPitch + i * 8 + 0] = r;
                    pMapRow[mapPitch + i * 8 + 1] = g;
                    pMapRow[mapPitch + i * 8 + 2] = b;
                    pMapRow[mapPitch + i * 8 + 3] = a;
                    pMapRow[mapPitch + i * 8 + 4] = r;
                    pMapRow[mapPitch + i * 8 + 5] = g;
                    pMapRow[mapPitch + i * 8 + 6] = b;
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

                    pMapRow[i * 8 + 0] = r1;
                    pMapRow[i * 8 + 1] = g1;
                    pMapRow[i * 8 + 2] = b1;
                    pMapRow[i * 8 + 3] = a1;
                    pMapRow[i * 8 + 4] = r2;
                    pMapRow[i * 8 + 5] = g2;
                    pMapRow[i * 8 + 6] = b2;
                    pMapRow[i * 8 + 7] = a2;

                    pMapRow[mapPitch + i * 8 + 0] = r1;
                    pMapRow[mapPitch + i * 8 + 1] = g1;
                    pMapRow[mapPitch + i * 8 + 2] = b1;
                    pMapRow[mapPitch + i * 8 + 3] = a1;
                    pMapRow[mapPitch + i * 8 + 4] = r2;
                    pMapRow[mapPitch + i * 8 + 5] = g2;
                    pMapRow[mapPitch + i * 8 + 6] = b2;
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

                    pMapRow[i * 8 + 0] = r1;
                    pMapRow[i * 8 + 1] = g1;
                    pMapRow[i * 8 + 2] = b1;
                    pMapRow[i * 8 + 3] = a1;
                    pMapRow[i * 8 + 4] = r1;
                    pMapRow[i * 8 + 5] = g1;
                    pMapRow[i * 8 + 6] = b1;
                    pMapRow[i * 8 + 7] = a1;

                    pMapRow[mapPitch + i * 8 + 0] = r2;
                    pMapRow[mapPitch + i * 8 + 1] = g2;
                    pMapRow[mapPitch + i * 8 + 2] = b2;
                    pMapRow[mapPitch + i * 8 + 3] = a2;
                    pMapRow[mapPitch + i * 8 + 4] = r2;
                    pMapRow[mapPitch + i * 8 + 5] = g2;
                    pMapRow[mapPitch + i * 8 + 6] = b2;
                    pMapRow[mapPitch + i * 8 + 7] = a2;
                }
                break;
            }

            case TintMode::GradientXY:
            {
                for (int i = 0; i < nSegments; i++)
                {
                    pMapRow[i * 8 + 0] = uint16_t(pConv[pSegCol->r] * 65535);
                    pMapRow[i * 8 + 1] = uint16_t(pConv[pSegCol->g] * 65535);
                    pMapRow[i * 8 + 2] = uint16_t(pConv[pSegCol->b] * 65535);
                    pMapRow[i * 8 + 3] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
                    pSegCol++;

                    pMapRow[i * 8 + 4] = uint16_t(pConv[pSegCol->r] * 65535);
                    pMapRow[i * 8 + 5] = uint16_t(pConv[pSegCol->g] * 65535);
                    pMapRow[i * 8 + 6] = uint16_t(pConv[pSegCol->b] * 65535);
                    pMapRow[i * 8 + 7] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
                    pSegCol++;

                    pMapRow[mapPitch + i * 8 + 4] = uint16_t(pConv[pSegCol->r] * 65535);
                    pMapRow[mapPitch + i * 8 + 5] = uint16_t(pConv[pSegCol->g] * 65535);
                    pMapRow[mapPitch + i * 8 + 6] = uint16_t(pConv[pSegCol->b] * 65535);
                    pMapRow[mapPitch + i * 8 + 7] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
                    pSegCol++;

                    pMapRow[mapPitch + i * 8 + 0] = uint16_t(pConv[pSegCol->r] * 65535);
                    pMapRow[mapPitch + i * 8 + 1] = uint16_t(pConv[pSegCol->g] * 65535);
                    pMapRow[mapPitch + i * 8 + 2] = uint16_t(pConv[pSegCol->b] * 65535);
                    pMapRow[mapPitch + i * 8 + 3] = uint16_t(m_linearToLinearTable[pSegCol->a] * 65535);
                    pSegCol++;
                }
                break;
            }
        }

        m_segPalOfs++;

        // Add our edge strips to segEdgeBuffer;
        
        float * pEdgeData = m_pSegEdgeBuffer + m_segEdgeOfs;

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
//                     endAdder = lastPixelCoverage - ((edgeOut & 0xFFFFFF00)-edgeIn)*increment / 256.f;
                }

                *pEdgeData++ = edgeIn/256.f;                    // Segment begin pixel
                *pEdgeData++ = increment;                        // Segment increment
                *pEdgeData++ = beginAdder;                    // Segment begin adder
                *pEdgeData++ = endAdder;                        // Segment end adder
            }

            pEdges += edgeStripPitch;
        }

        m_segEdgeOfs+= segEdgeSpaceNeeded;
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
        // We are modifying buffers even though a flush might be in progress.
        // We are assuming that original buffer will be kept alive and utilized during the flush.
        
        if( (m_commandBufferSize - m_commandOfs) < m_commandBufferSize/4 )
        {
            m_commandBufferSize *= 2;
            int * pNewBuffer = new int[m_commandBufferSize];
            memcpy( pNewBuffer, m_pCommandBuffer, m_commandOfs * sizeof(int));
            delete [] m_pCommandBuffer;
            m_pCommandBuffer = pNewBuffer;
        }

        if( (m_vertexBufferSize - m_vertexOfs) < m_vertexBufferSize/4 )
        {
            m_vertexBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_vertexBufferSize*sizeof(Vertex) options:MTLResourceStorageModeShared];

            Vertex * pNewBuffer = (Vertex *)[newId contents];
            memcpy( pNewBuffer, m_pVertexBuffer, m_vertexOfs * sizeof(float));
            m_pVertexBuffer = pNewBuffer;
            
            m_vertexBufferId = newId;
        }

        if( (m_extrasBufferSize - m_extrasOfs) < m_extrasBufferSize/4 || (m_extrasBufferSize - m_extrasOfs) < m_neededExtras )
        {
            m_extrasBufferSize *= 2;
            while( (m_extrasBufferSize - m_extrasOfs) < m_neededExtras )
                m_extrasBufferSize *= 2;
            m_neededExtras = 0;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_extrasBufferSize*sizeof(float) options:MTLResourceStorageModeShared];

            float * pNewBuffer = (float*)[newId contents];
            memcpy( pNewBuffer, m_pExtrasBuffer, m_extrasOfs * sizeof(float));
            m_pExtrasBuffer = pNewBuffer;
            
            m_extrasBufferId = newId;
        }
        
        if( (m_surfaceBufferSize - m_surfaceOfs) < m_surfaceBufferSize/4 )
        {
            m_surfaceBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_surfaceBufferSize*sizeof(MetalSurface_p) options:MTLResourceStorageModeShared];

            MetalSurface_p * pNewBuffer = (MetalSurface_p *)[newId contents];
            memcpy( pNewBuffer, m_pSurfaceBuffer, m_surfaceOfs * sizeof(MetalSurface_p));
            m_pSurfaceBuffer = pNewBuffer;
            
            m_surfaceBufferId = newId;
        }

        if( (m_clipListBufferSize - m_clipWriteOfs) < m_clipListBufferSize/4 )
        {
            m_clipListBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_clipListBufferSize*sizeof(RectI) options:MTLResourceStorageModeShared];

            RectI * pNewBuffer = (RectI *)[newId contents];
            memcpy( pNewBuffer, m_pClipListBuffer, m_clipWriteOfs * sizeof(RectI));
            m_pClipListBuffer = pNewBuffer;
            
            m_clipListBufferId = newId;
        }
        
        if( (m_segEdgeBufferSize - m_segEdgeOfs ) < m_segEdgeBufferSize/4 || (m_segEdgeBufferSize - m_segEdgeOfs) < m_neededSegEdge )
        {
            m_segEdgeBufferSize *= 2;
            while( (m_segEdgeBufferSize - m_segEdgeOfs) < m_neededSegEdge )
                m_segEdgeBufferSize *= 2;
            m_neededSegEdge = 0;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_segEdgeBufferSize*sizeof(float) options:MTLResourceStorageModeManaged];

            float * pNewBuffer = (float*)[newId contents];
            memcpy( pNewBuffer, m_pSegEdgeBuffer, m_segEdgeOfs * sizeof(float));
            m_pSegEdgeBuffer = pNewBuffer;
            
            m_segEdgeBufferId = newId;
        }

    }


    //____ _executeBuffer() ___________________________________________________

    void MetalGfxDevice::_executeBuffer()
    {
        if( m_commandOfs == 0 )
            return;
        
        // Sync segments segEdge buffer to GPU
        
        if( m_segEdgeOfs > m_segEdgeFlushPoint )
        {
            [m_segEdgeBufferId didModifyRange:NSMakeRange(m_segEdgeFlushPoint*sizeof(float), (m_segEdgeOfs-m_segEdgeFlushPoint)*sizeof(float))];
        }
        
        
        // Update segments palette texture
        
        if( m_segPalOfs > 0 )
        {
            int bytesPerRow = sizeof(uint16_t)*4*2*c_maxSegments;

            MTLSize textureSize = { (unsigned) c_maxSegments*2, (unsigned) m_segPalOfs*2, 1};
            MTLOrigin textureOrigin = {0,0,0};
                        
            id<MTLBlitCommandEncoder> blitCommandEncoder = [m_metalCommandBuffer blitCommandEncoder];
            [blitCommandEncoder copyFromBuffer:     m_segPalBufferId
                                sourceOffset:       0
                                sourceBytesPerRow:  bytesPerRow
                                sourceBytesPerImage:m_segPalOfs*c_segPalEntrySize
                                sourceSize:         textureSize
                                toTexture:          m_segPalTextureId
                                destinationSlice:   0
                                destinationLevel:   0
                                destinationOrigin:  textureOrigin];

            [blitCommandEncoder endEncoding];
            
            //TODO: We need to sync this, don't start drawing segments until this has been fully uploaded.
        }

        // Create our render encoder.

        id<MTLRenderCommandEncoder> renderEncoder = [m_metalCommandBuffer renderCommandEncoderWithDescriptor:m_renderPassDesc];
        renderEncoder.label = @"GfxDeviceMetalRenderEncoder";

        // Set buffers for vertex shaders
        
        [renderEncoder setVertexBuffer:m_vertexBufferId offset:0 atIndex:(unsigned) VertexInputIndex::VertexBuffer];
        
        [renderEncoder setVertexBuffer:m_extrasBufferId offset:0 atIndex:(unsigned) VertexInputIndex::ExtrasBuffer];
        [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];

        // Set buffers/textures for segments fragment shader
        
        [renderEncoder setFragmentTexture:m_segPalTextureId atIndex: (unsigned) TextureIndex::SegPal];
        [renderEncoder setFragmentBuffer:m_segEdgeBufferId offset:0 atIndex:(unsigned) FragmentInputIndex::ExtrasBuffer];
        
        int * pCmd = m_pCommandBuffer;
        int * pCmdEnd = &m_pCommandBuffer[m_commandOfs];

        int vertexOfs = m_vertexFlushPoint;
        int surfaceOfs = m_surfaceFlushPoint;
      
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
                    _setCanvas(renderEncoder, m_pSurfaceBuffer[surfaceOfs], pCmd[0], pCmd[1]);
                    m_pSurfaceBuffer[surfaceOfs++] = nullptr;
                    pCmd += 2;
                    break;
                }
                case Command::SetBlendMode:
                {
                    _setBlendMode(renderEncoder, (BlendMode)* pCmd++);
                    break;
                }
                case Command::SetMorphFactor:
                {
                    _setMorphFactor(renderEncoder, (*pCmd++) / 1024.f);
                    break;
                }
                case Command::SetTintColor:
                {
                    _setTintColor(renderEncoder, *(Color*)(pCmd++));
                    break;
                }
                case Command::SetTintGradient:
                {
                    RectI& rect = *(RectI*)pCmd;
                    pCmd += 4;
                    Color* pColors = (Color*)pCmd;
                    pCmd += 4;

                    _setTintGradient(renderEncoder, rect, pColors);
                    break;
                }
                case Command::ClearTintGradient:
                {
                     _clearTintGradient(renderEncoder);
                   break;
                }
                case Command::SetBlitSource:
                {
                    _setBlitSource(renderEncoder, m_pSurfaceBuffer[surfaceOfs]);
                    m_pSurfaceBuffer[surfaceOfs++] = nullptr;
                    break;
                }
                case Command::Blit:
                {
                    int nVertices = *pCmd++;
                    if (nVertices > 0 && m_pActiveBlitSource)
                    {
                        MetalSurface* pSurf = m_pActiveBlitSource;
                      
                        BlitFragShader shader = BlitFragShader::Normal;

                        if(pSurf->m_pixelDescription.bIndexed)
                        {
                            if( pSurf->scaleMode() == ScaleMode::Interpolate )
                                shader = BlitFragShader::ClutInterpolated;
                            else
                                shader = BlitFragShader::ClutNearest;
                        }
                        else if(pSurf->m_pixelDescription.format == PixelFormat::A_8)
                            shader = BlitFragShader::A8Source;
                        
                        [renderEncoder setRenderPipelineState:m_blitPipelines[(int)shader][m_bGradientActive][(int)m_activeBlendMode][(int)DestFormat::BGRA8_linear] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
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
                        [renderEncoder setRenderPipelineState:m_fillPipelines[m_bGradientActive][(int)m_activeBlendMode][(int)DestFormat::BGRA8_linear] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
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
                        [renderEncoder setRenderPipelineState:m_fillAAPipelines[m_bGradientActive][(int)m_activeBlendMode][(int)DestFormat::BGRA8_linear] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
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
                        [renderEncoder setRenderPipelineState:m_lineFromToPipelines[(int)m_activeBlendMode][(int)DestFormat::BGRA8_linear] ];
                        
                        for (int i = 0; i < clipListLen; i++)
                        {
                            RectI& clip = m_pClipListBuffer[clipListOfs++];
                            MTLScissorRect metalClip = {(unsigned) clip.x, (unsigned) clip.y, (unsigned) clip.w, (unsigned) clip.h};
                            [renderEncoder setScissorRect:metalClip];
                            [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        }

                        MTLScissorRect orgClip = {0, 0, (unsigned) m_canvasSize.w, (unsigned) m_canvasSize.h};
                        [renderEncoder setScissorRect:orgClip];

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
                         [renderEncoder setRenderPipelineState:m_plotPipelines[(int)m_activeBlendMode][(int)DestFormat::BGRA8_linear] ];
                         [renderEncoder drawPrimitives:MTLPrimitiveTypePoint vertexStart:vertexOfs vertexCount:nVertices];
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
                        [renderEncoder setRenderPipelineState:m_segmentsPipelines[nEdges][m_bGradientActive][(int)m_activeBlendMode][(int)DestFormat::BGRA8_linear] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        vertexOfs += nVertices;
                        
                        if( m_bMipmappedActiveCanvas )
                            m_pActiveCanvas->m_bMipmapStale = true;
                    }
                    break;
                }
     
                default:
    //                assert(false);
                    break;
            }
        }

        [renderEncoder endEncoding];
        
        //
        
        m_vertexFlushPoint = m_vertexOfs;
        m_surfaceFlushPoint = m_surfaceOfs;
        m_segEdgeFlushPoint = m_segEdgeOfs;
        m_commandOfs = 0;
    }

    //____ _resetBuffers() ____________________________________________________

    void MetalGfxDevice::_resetBuffers()
    {
        // Prepare our buffers

        m_vertexOfs = 0;
        m_extrasOfs = 0;
        m_commandOfs = 0;
        m_surfaceOfs = 0;
        m_segEdgeOfs = 0;
        m_segPalOfs = 0;
        m_clipWriteOfs = 0;
        m_clipCurrOfs = -1;

        m_vertexFlushPoint = 0;
        m_surfaceFlushPoint = 0;
        m_segEdgeFlushPoint = 0;
        
    }

    //____ _setCanvas() _______________________________________________________

    void MetalGfxDevice::_setCanvas( id<MTLRenderCommandEncoder> renderEncoder, MetalSurface * pCanvas, int width, int height )
    {
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
            [renderEncoder setViewport:(MTLViewport){0.0, 0.0, (double) width, (double) height, 0.0, 1.0 }];

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

        [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex: (unsigned) VertexInputIndex::Uniform];

        //
        
        m_pActiveCanvas = pCanvas;
        m_bMipmappedActiveCanvas = m_pActiveCanvas ? m_pActiveCanvas->isMipmapped() : false;
    }

    //____ _setBlendMode() _______________________________________________________

    void MetalGfxDevice::_setBlendMode( id<MTLRenderCommandEncoder> renderEncoder, BlendMode mode )
    {
        m_activeBlendMode = mode;
    }

    //____ _setMorphFactor() __________________________________________________

    void MetalGfxDevice::_setMorphFactor( id<MTLRenderCommandEncoder> renderEncoder, float morphFactor)
    {
        [renderEncoder setBlendColorRed:1.f green:1.f blue:1.f alpha:morphFactor];
    }

    //____ _setBlitSource() _______________________________________________________

    void MetalGfxDevice::_setBlitSource( id<MTLRenderCommandEncoder> renderEncoder, MetalSurface * pSurf )
    {
        if( pSurf )
        {
            [renderEncoder setFragmentTexture:pSurf->getTexture() atIndex: (unsigned) TextureIndex::Texture];

            if(pSurf->pixelDescription()->bIndexed)
                [renderEncoder setFragmentSamplerState: m_samplers[0][0][pSurf->isTiling()] atIndex:0];
            else
                [renderEncoder setFragmentSamplerState: m_samplers[pSurf->isMipmapped()][pSurf->scaleMode() == ScaleMode::Interpolate][pSurf->isTiling()] atIndex:0];

            if( pSurf->m_bMipmapStale )
            {
//                glGenerateMipmap(GL_TEXTURE_2D);
                pSurf->m_bMipmapStale = false;
            }

            m_pActiveBlitSource = pSurf;
            pSurf->m_bPendingReads = false;            // Clear this as we pass it by...

            m_uniform.textureSize = pSurf->size();
            [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];

            if (pSurf->m_pClut)
            {
                [renderEncoder setFragmentTexture:pSurf->getClutTexture() atIndex:(unsigned) TextureIndex::Clut];
            }
        }
        else
        {
            [renderEncoder setFragmentTexture:nil atIndex:(unsigned) TextureIndex::Texture];
            [renderEncoder setFragmentSamplerState: m_samplers[0][0][0] atIndex:0];
        }

    }

    //____ _setTintColor() ____________________________________________________

    void MetalGfxDevice::_setTintColor( id<MTLRenderCommandEncoder> renderEncoder, Color color)
    {
        float* pConv = Base::activeContext()->gammaCorrection() ? m_sRGBtoLinearTable : m_linearToLinearTable;

        float r, g, b, a;

        m_uniform.flatTint[0] = r = pConv[color.r];
        m_uniform.flatTint[1] = g = pConv[color.g];
        m_uniform.flatTint[2] = b = pConv[color.b];
        m_uniform.flatTint[3] = a = m_linearToLinearTable[color.a];

        [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];
    }

    //____ _setTintGradient() _________________________________________________

    void MetalGfxDevice::_setTintGradient( id<MTLRenderCommandEncoder> renderEncoder, const RectI& rect, const Color colors[4])
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

        [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];
    }

    //____ _clearTintGradient() _________________________________________________

    void MetalGfxDevice::_clearTintGradient( id<MTLRenderCommandEncoder> renderEncoder )
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
                descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatR8Unorm;
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
            case BlendMode::Undefined:

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
