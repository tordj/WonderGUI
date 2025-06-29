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
#include <wg_metalsurfacefactory.h>
#include <wg_metaledgemapfactory.h>
#include <wg_metaledgemap.h>
#include <wg_gfxbase.h>
#include <wg_gfxutil.h>


namespace wg
{
    using namespace Util;

	const TypeInfo MetalGfxDevice::TYPEINFO = { "MetalGfxDevice", &GfxDevice::TYPEINFO };

    MetalGfxDevice *    MetalGfxDevice::s_pActiveDevice = nullptr;

    id<MTLDevice>       MetalGfxDevice::s_metalDevice = nil;
    id<MTLCommandQueue> MetalGfxDevice::s_metalCommandQueue = nil;

    //____ setMetalDevice() ______________________________________________________

    void MetalGfxDevice::setMetalDevice( id<MTLDevice> device )
    {
        [s_metalCommandQueue release];

		s_metalDevice = device;

        if( device == nil )
            s_metalCommandQueue = nil;
        else
            s_metalCommandQueue = [s_metalDevice newCommandQueue];
    }

	//____ create() _______________________________________________________________

	MetalGfxDevice_p MetalGfxDevice::create()
	{
		return MetalGfxDevice_p(new MetalGfxDevice());
    }

    //____ constructor() ______________________________________________________

MetalGfxDevice::MetalGfxDevice()
	{
		assert(sizeof(Gradient) % 4 == 0);		// Buffering will fail if not!
	
        m_defaultCanvas.ref = CanvasRef::Default;
    
        m_bFullyInitialized = true;

        m_flushesInProgress = 0;
        
        _initTables();

		// Modify shader source
			
		std::string shaderSource = shaders;
		auto maxSegPos = shaderSource.find("$MAXSEG");

		while(maxSegPos != std::string::npos )
		{
			shaderSource.replace(maxSegPos, 7, std::to_string(c_maxSegments));
			maxSegPos = shaderSource.find("$MAXSEG");
		}
	
		// Create shader library
	
		NSError *error = nil;
        NSString *shaderString = [[NSString alloc] initWithUTF8String:shaderSource.c_str()];
        m_library = [s_metalDevice newLibraryWithSource:shaderString options:nil error:&error];

		[error release];
        [shaderString release];
    
        // Create and init Plot & Line pipelines

        for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
        {
//            if( blendMode != int(BlendMode::Ignore) && blendMode != int(BlendMode::Undefined) )
            {
                m_plotPipelines[blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Plot BGRA_8_linear Pipeline", @"plotVertexShader", @"plotFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_plotPipelines[blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Plot BGRX_8_linear Pipeline", @"plotVertexShader", @"plotFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_plotPipelines[blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Plot BGRA_8_sRGB Pipeline", @"plotVertexShader", @"plotFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_plotPipelines[blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"Plot BGRX_8_sRGB Pipeline", @"plotVertexShader", @"plotFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_plotPipelines[blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"Plot A_8 Pipeline", @"plotVertexShader", @"plotFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );
                
                m_lineFromToPipelines[blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Line BGRA_8_linear Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_lineFromToPipelines[blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Line BGRX_8_linear Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_lineFromToPipelines[blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Line BGRA_8_sRGB Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_lineFromToPipelines[blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"Line BGRX_8_sRGB Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_lineFromToPipelines[blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"Line A_8 Pipeline", @"lineFromToVertexShader", @"lineFromToFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );
            }
        }
        
        // Create and init Fill pipelines

        for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
        {
//            if( blendMode != int(BlendMode::Ignore) && blendMode != int(BlendMode::Undefined) )
            {
                m_fillPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Fill BGRA_8_linear Pipeline", @"fillVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillPipelines[0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Fill BGRX_8_linear Pipeline", @"fillVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_fillPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Fill BGRA_8_sRGB Pipeline", @"fillVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillPipelines[0][blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"Fill BGRX_8_sRGB Pipeline", @"fillVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_fillPipelines[0][blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"Fill A_8 Pipeline", @"fillVertexShader", @"fillFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_fillPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"GradientFill BGRA_8_linear Pipeline", @"fillGradientVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillPipelines[1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"GradientFill BGRX_8_linear Pipeline", @"fillGradientVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_fillPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"GradientFill BGRA_8_sRGB Pipeline", @"fillGradientVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillPipelines[1][blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"GradientFill BGRX_8_sRGB Pipeline", @"fillGradientVertexShader", @"fillFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_fillPipelines[1][blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"GradientFill A_8 Pipeline", @"fillGradientVertexShader", @"fillFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"AAFill BGRA_8_linear Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"AAFill BGRX_8_linear Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"AAFill BGRA_8_sRGB Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"AAFill BGRX_8_sRGB Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_fillAAPipelines[0][blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"AAFill A_8 Pipeline", @"fillAAVertexShader", @"fillAAFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"AAGradientFill BGRA_8_linear Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"AAGradientFill BGRX_8_linear Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"AAGradientFilll BGRA_8_sRGB Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"AAGradientFilll BGRX_8_sRGB Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_fillAAPipelines[1][blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"AAGradientFill A_8 Pipeline", @"fillGradientAAVertexShader", @"fillAAFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );
            }
        }
        
        // Create and init Blit pipelines

        for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
        {
//            if( blendMode != int(BlendMode::Ignore) && blendMode != int(BlendMode::Undefined) )
            {
               
                    // [BlitFragShader][bGradient][BlendMode][DestFormat]
                
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Blit BGRA_8_linear Pipeline", @"blitVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Blit BGRX_8_linear Pipeline", @"blitVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"Blit BGRA_8_sRGB Pipeline", @"blitVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"Blit BGRX_8_sRGB Pipeline", @"blitVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"Blit A_8 Pipeline", @"blitVertexShader", @"blitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Blit BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Blit BGRX_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"Blit BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"Blit BGRX_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"Blit A_8 Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"PaletteBlitNearest BGRA_8_linear Pipeline", @"blitVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"PaletteBlitNearest BGRX_8_linear Pipeline", @"blitVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"PaletteBlitNearest BGRA_8_sRGB Pipeline", @"blitVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"PaletteBlitNearest BGRX_8_sRGB Pipeline", @"blitVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"PaletteBlitNearest A_8 Pipeline", @"blitVertexShader", @"paletteBlitNearestFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"PaletteBlitNearest BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"PaletteBlitNearest BGRX_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"PaletteBlitNearest BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"PaletteBlitNearest BGRX_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"paletteBlitNearestFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"PaletteBlitNearest A_8 Gradient Pipeline", @"blitGradientVertexShader", @"paletteBlitNearestFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"PaletteBlitInterpolated BGRA_8_linear Pipeline", @"paletteBlitInterpolateVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"PaletteBlitInterpolated BGRX_8_linear Pipeline", @"paletteBlitInterpolateVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"PaletteBlitInterpolated BGRA_8_sRGB Pipeline", @"paletteBlitInterpolateVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"PaletteBlitInterpolated BGRX_8_sRGB Pipeline", @"paletteBlitInterpolateVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"PaletteBlitInterpolated A_8 Pipeline", @"paletteBlitInterpolateVertexShader", @"paletteBlitInterpolateFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"PaletteBlitInterpolated BGRA_8_linear Gradient Pipeline", @"paletteBlitInterpolateGradientVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"PaletteBlitInterpolated BGRX_8_linear Gradient Pipeline", @"paletteBlitInterpolateGradientVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"PaletteBlitInterpolated BGRA_8_sRGB Gradient Pipeline", @"paletteBlitInterpolateGradientVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"PaletteBlitInterpolated BGRX_8_sRGB Gradient Pipeline", @"paletteBlitInterpolateGradientVertexShader", @"paletteBlitInterpolateFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"PaletteBlitInterpolated A_8 Gradient Pipeline", @"paletteBlitInterpolateGradientVertexShader", @"paletteBlitInterpolateFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"A8SourceBlit BGRA_8_linear Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"A8SourceBlit BGRX_8_linear Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"A8SourceBlit BGRA_8_sRGB Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"A8SourceBlit BGRX_8_sRGB Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"A8SourceBlit A_8 Pipeline", @"blitVertexShader", @"alphaBlitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"A8SourceBlit BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"A8SourceBlit BGRX_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"A8SourceBlit BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"A8SourceBlit BGRX_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"A8SourceBlit A_8 Gradient Pipeline", @"blitGradientVertexShader", @"alphaBlitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"RGBXSourceBlit BGRA_8_linear Pipeline", @"blitVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"RGBXSourceBlit BGRX_8_linear Pipeline", @"blitVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"RGBXSourceBlit BGRA_8_sRGB Pipeline", @"blitVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"RGBXSourceBlit BGRX_8_sRGB Pipeline", @"blitVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"RGBXSourceBlit A_8 Pipeline", @"blitVertexShader", @"rgbxBlitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"RGBXSourceBlit BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"RGBXSourceBlit BGRX_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"RGBXSourceBlit BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"RGBXSourceBlit BGRX_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"rgbxBlitFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
                m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::Alpha_8]          = _compileRenderPipeline( @"RGBXSourceBlit A_8 Gradient Pipeline", @"blitGradientVertexShader", @"rgbxBlitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );
            }
        }
        
	// Create and init Blur pipelines

	for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
	{
//            if( blendMode != int(BlendMode::Ignore) && blendMode != int(BlendMode::Undefined) )
		{
		   
				// [BlitFragShader][bGradient][BlendMode][DestFormat]
			
			m_blurPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Blur BGRA_8_linear Pipeline", @"blitVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
			m_blurPipelines[0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Blur BGRX_8_linear Pipeline", @"blitVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
			m_blurPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"Blur BGRA_8_sRGB Pipeline", @"blitVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
			m_blurPipelines[0][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"Blur BGRX_8_sRGB Pipeline", @"blitVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
//			m_blurPipelines[0][blendMode][(int)DestFormat::Alpha_8]      = _compileRenderPipeline( @"Blit A_8 Pipeline", @"blitVertexShader", @"blurFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

			m_blurPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Blit BGRA_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
			m_blurPipelines[1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Blit BGRX_8_linear Gradient Pipeline", @"blitGradientVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_linear );
			m_blurPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB]   = _compileRenderPipeline( @"Blit BGRA_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
			m_blurPipelines[1][blendMode][(int)DestFormat::BGRX8_sRGB]   = _compileRenderPipeline( @"Blit BGRX_8_sRGB Gradient Pipeline", @"blitGradientVertexShader", @"blurFragmentShader", (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );
//			m_blitPipelines[1][blendMode][(int)DestFormat::Alpha_8]      = _compileRenderPipeline( @"Blit A_8 Gradient Pipeline", @"blitGradientVertexShader", @"blitFragmentShader_A8", (BlendMode) blendMode, PixelFormat::Alpha_8 );

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

		int maxSegments = c_maxSegments;				// std::min can't operate on static const only present in header. Does some introspection that fails then.
		int nbShaders = std::min(maxSegments, 16);
	
        for( int shader = 1 ; shader < nbShaders ; shader++ )
        {
            for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
            {
                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Segments BGRA_8_linear pipeline", @"segmentsVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Segments BGRX_8_linear pipeline", @"segmentsVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRX_8_linear );

                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Segments BGRA_8_sRGB pipeline", @"segmentsVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"Segments BGRX_8_sRGB pipeline", @"segmentsVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );

                m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"Segments A_8 pipeline", @"segmentsVertexShader", segFragShaders_A8[shader], (BlendMode) blendMode, PixelFormat::Alpha_8 );

                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRA8_linear] = _compileRenderPipeline( @"Segments BGRA_8_linear gradient pipeline", @"segmentsGradientVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_linear );
                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRX8_linear] = _compileRenderPipeline( @"Segments BGRX_8_linear gradient pipeline", @"segmentsGradientVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRX_8_linear );

                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRA8_sRGB] = _compileRenderPipeline( @"Segments BGRA_8_sRGB gradient pipeline", @"segmentsGradientVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRA_8_sRGB );
                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRX8_sRGB] = _compileRenderPipeline( @"Segments BGRX_8_sRGB gradient pipeline", @"segmentsGradientVertexShader", segFragShaders[shader], (BlendMode) blendMode, PixelFormat::BGRX_8_sRGB );

                m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::Alpha_8] = _compileRenderPipeline( @"Segments A_8 gradient pipeline", @"segmentsGradientVertexShader", segFragShaders_A8[shader], (BlendMode) blendMode, PixelFormat::Alpha_8 );
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
        
        [desc release];
        
        // Initialize our buffers
        
        m_pCommandBuffer = new int[m_commandBufferSize];

        m_vertexBufferId = [s_metalDevice newBufferWithLength:m_vertexBufferSize*sizeof(Vertex) options:MTLResourceStorageModeShared];
        m_pVertexBuffer = (Vertex *)[m_vertexBufferId contents];

        m_extrasBufferId = [s_metalDevice newBufferWithLength:m_extrasBufferSize*sizeof(float) options:MTLResourceStorageModeShared];
        m_pExtrasBuffer = (float *)[m_extrasBufferId contents];

        m_clipListBufferId = [s_metalDevice newBufferWithLength:m_clipListBufferSize*sizeof(RectI) options:MTLResourceStorageModeShared];
        m_pClipListBuffer = (RectI *)[m_clipListBufferId contents];
        
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
        [textureDescriptor release];
    
        // Initialize our shader environment

        m_uniform.flatTint = { 1.f, 1.f, 1.f, 1.f };
    }

    //____ destructor() _______________________________________________________

	MetalGfxDevice::~MetalGfxDevice()
	{
		while( m_flushesInProgress > 0 )
		{
			usleep(100);        // Sleep for 0.1 millisec
		};

//		[m_metalCommandBuffer release];				// We don't release metal command buffer, Metal takes care of that.
		m_metalCommandBuffer = nil;

        [m_defaultCanvasRenderPassDesc release];
		m_defaultCanvasRenderPassDesc = nil;
		m_drawableToAutoPresent = nil;

		[m_vertexBufferId release];
		m_vertexBufferId = nil;
		
		[m_extrasBufferId release];
		m_extrasBufferId = nil;
		
		[m_clipListBufferId release];
		m_clipListBufferId = nil;
		
		[m_segEdgeBufferId release];
		m_segEdgeBufferId = nil;
		
		[m_segPalTextureId release];
		m_segPalTextureId = nil;

		[m_segPalBufferId release];
		m_segPalBufferId = nil;
		

		delete [] m_pCommandBuffer;
		
		for( int mipmapped = 0 ; mipmapped < 2 ; mipmapped++ )
		{
			for( int interpolated = 0 ; interpolated < 2 ; interpolated++ )
			{
				for( int tiled = 0 ; tiled < 2 ; tiled++ )
				{
					[m_samplers[mipmapped][interpolated][tiled] release];
				}
			}
		}
		
		
		for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
		{
				[m_plotPipelines[blendMode][(int)DestFormat::BGRA8_linear] release];
				[m_plotPipelines[blendMode][(int)DestFormat::BGRX8_linear] release];
				[m_plotPipelines[blendMode][(int)DestFormat::BGRA8_sRGB] release];
				[m_plotPipelines[blendMode][(int)DestFormat::BGRX8_sRGB] release];
				[m_plotPipelines[blendMode][(int)DestFormat::Alpha_8] release];
				
				[m_lineFromToPipelines[blendMode][(int)DestFormat::BGRA8_linear] release];
				[m_lineFromToPipelines[blendMode][(int)DestFormat::BGRX8_linear] release];
				[m_lineFromToPipelines[blendMode][(int)DestFormat::BGRA8_sRGB] release];
				[m_lineFromToPipelines[blendMode][(int)DestFormat::BGRX8_sRGB] release];
				[m_lineFromToPipelines[blendMode][(int)DestFormat::Alpha_8] release];
		}
		
		for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
		{
			[m_fillPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_fillPipelines[0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_fillPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_fillPipelines[0][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_fillPipelines[0][blendMode][(int)DestFormat::Alpha_8] release];

			[m_fillPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_fillPipelines[1][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_fillPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_fillPipelines[1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_fillPipelines[1][blendMode][(int)DestFormat::Alpha_8] release];

			[m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_fillAAPipelines[0][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_fillAAPipelines[0][blendMode][(int)DestFormat::Alpha_8] release];

			[m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_fillAAPipelines[1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_fillAAPipelines[1][blendMode][(int)DestFormat::Alpha_8] release];
		}
		
		// Create and init Blit pipelines

		for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
		{
			[m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][0][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::Normal][1][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][0][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteNearest][1][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][0][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::PaletteInterpolated][1][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][0][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::A8Source][1][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][0][blendMode][(int)DestFormat::Alpha_8] release];

			[m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
			[m_blitPipelines[(int)BlitFragShader::RGBXSource][1][blendMode][(int)DestFormat::Alpha_8] release];
		}

		
		for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
		{
			[m_blurPipelines[0][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blurPipelines[0][blendMode][(int)DestFormat::BGRX8_linear] release];
			[m_blurPipelines[0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blurPipelines[0][blendMode][(int)DestFormat::BGRX8_sRGB]  release];

			[m_blurPipelines[1][blendMode][(int)DestFormat::BGRA8_linear] release];
			[m_blurPipelines[1][blendMode][(int)DestFormat::BGRX8_linear]  release];
			[m_blurPipelines[1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
			[m_blurPipelines[1][blendMode][(int)DestFormat::BGRX8_sRGB] release];
		}

        int maxSegments = c_maxSegments;                // std::min can't operate on static const only present in header. Does some introspection that fails then.
        int nbShaders = std::min(maxSegments, 16);
        for( int shader = 1 ; shader < nbShaders ; shader++ )
		{
			for( int blendMode = 0 ; blendMode < BlendMode_size ; blendMode++ )
			{
				[m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRA8_linear] release];
				[m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRX8_linear] release];

				[m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRA8_sRGB] release];
				[m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::BGRX8_sRGB] release];

				[m_segmentsPipelines[shader][0][blendMode][(int)DestFormat::Alpha_8] release];

				[m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRA8_linear] release];
				[m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRX8_linear] release];

				[m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRA8_sRGB] release];
				[m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::BGRX8_sRGB] release];

				[m_segmentsPipelines[shader][1][blendMode][(int)DestFormat::Alpha_8] release];
			}
		}
		
		[m_library release];
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& MetalGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& MetalGfxDevice::surfaceType( void ) const
	{
		return MetalSurface::TYPEINFO;
	}

	//____ surfaceFactory() _______________________________________________________

	SurfaceFactory_p MetalGfxDevice::surfaceFactory()
	{
        if( !m_pSurfaceFactory )
            m_pSurfaceFactory = MetalSurfaceFactory::create();

        return m_pSurfaceFactory;
	}

    //____ edgemapFactory() ______________________________________________________

    EdgemapFactory_p MetalGfxDevice::edgemapFactory()
    {
        if (!m_pEdgemapFactory)
            m_pEdgemapFactory = MetalEdgemapFactory::create();

        return m_pEdgemapFactory;

    }

	//____ canvas() ____________________________________________________________

    const CanvasInfo MetalGfxDevice::canvas(CanvasRef ref) const
	{
		if( ref == CanvasRef::Default )
			return m_defaultCanvas;
		else
		{
			//TODO: Error handling!
			return m_dummyCanvas;
		}
	}


    //____ _canvasWasChanged() ________________________________________________________

    void MetalGfxDevice::_canvasWasChanged()
    {
        _renderLayerWasChanged();
    }

    //____ _renderLayerWasChanged() ________________________________________________________

    void MetalGfxDevice::_renderLayerWasChanged()
    {
        Surface* pRenderSurface;

        bool bClear = false;
        if (m_renderLayer > 0 && m_layerSurfaces[m_renderLayer] == nullptr)
        {
            Surface::Blueprint bp;
            bp.canvas = true;
            bp.format = m_pCanvasLayers->layerFormat(m_renderLayer);
            bp.size = m_canvas.size/64;
            m_layerSurfaces[m_renderLayer] = MetalSurface::create(bp);
            bClear = true;
        }

        if (!m_canvas.pSurface && m_renderLayer == 0)
        {
            pRenderSurface = nullptr;
        }
        else
        {
            pRenderSurface = m_layerSurfaces[m_renderLayer];
            pRenderSurface->retain();
        }
        

        _endCommand();
        _beginStateCommandWithAlignedData(Command::SetCanvas, 4 + sizeof(void*)/sizeof(int));
        m_pCommandBuffer[m_commandOfs++] = m_canvas.size.w/64;
        m_pCommandBuffer[m_commandOfs++] = m_canvas.size.h/64;
        m_pCommandBuffer[m_commandOfs++] = bClear ? (int) CanvasInit::Discard : (int) CanvasInit::Keep;
        m_pCommandBuffer[m_commandOfs++] = Color::Black.argb;
        * (void**)(m_pCommandBuffer+m_commandOfs) = pRenderSurface;
        m_commandOfs += sizeof(void*)/sizeof(int);

        //TODO: Ordering all these state changes is inefficient. We should be able to compare
        // to previous state and only update what is necessary.
        
        // Update blit source
        
        _endCommand();
        _beginStateCommandWithAlignedData(Command::SetBlitSource, sizeof(void*)/sizeof(int));
        * (void**)(m_pCommandBuffer+m_commandOfs) = m_pBlitSource;
        m_commandOfs += sizeof(void*)/sizeof(int);
        if( m_pBlitSource )
            m_pBlitSource->retain();
        
        // Update blend mode
        
        _endCommand();
        _beginStateCommand(Command::SetBlendMode, 1);
        m_pCommandBuffer[m_commandOfs++] = (int) m_blendMode;
        
        // Update morph factor
        
        _endCommand();
        _beginStateCommand(Command::SetMorphFactor, 1);
        m_pCommandBuffer[m_commandOfs++] = (int)(m_morphFactor*1024);
        
        // Update tint color
        
        _endCommand();
        _beginStateCommandWithAlignedData(Command::SetTintColor, 2);
        *(int64_t*)(&m_pCommandBuffer[m_commandOfs]) = m_tintColor.argb;
        m_commandOfs += 2;
        
        // Update tint gradient
        
        if( m_bTintGradient )
        {
            _endCommand();
            _beginStateCommandWithAlignedData(Command::SetTintGradient, 4 + sizeof(Gradient)/4);
            m_pCommandBuffer[m_commandOfs++] = m_tintGradientRect.x / 64;
            m_pCommandBuffer[m_commandOfs++] = m_tintGradientRect.y / 64;
            m_pCommandBuffer[m_commandOfs++] = m_tintGradientRect.w / 64;
            m_pCommandBuffer[m_commandOfs++] = m_tintGradientRect.h / 64;
            *(Gradient*)(&m_pCommandBuffer[m_commandOfs]) = m_tintGradient;
            m_commandOfs += sizeof(Gradient)/4;
        }
        else
        {
            _endCommand();
            _beginStateCommand(Command::ClearTintGradient, 0);
        }
        
        if(bClear)
            _clearRenderLayer();
    }

    //____ _clipListWasChanged() ________________________________________________________

    void MetalGfxDevice::_clipListWasChanged()
    {
        m_clipCurrOfs = -1;
    }

    //____ _setDefaultCanvas() ___________________________________________________

    bool MetalGfxDevice::setDefaultCanvas( MTLRenderPassDescriptor* renderPassDesc, SizeI pixelSize, PixelFormat pixelFormat, int scale )
    {
        if( pixelFormat != PixelFormat::BGRA_8 && pixelFormat != PixelFormat::BGRA_8_linear && pixelFormat != PixelFormat::BGRA_8_sRGB && 
		    pixelFormat != PixelFormat::BGRX_8 && pixelFormat != PixelFormat::BGRX_8_linear && pixelFormat != PixelFormat::BGRX_8_sRGB &&
		    pixelFormat != PixelFormat::Alpha_8 )
        {
            GfxBase::throwError(ErrorLevel::SilentError, ErrorCode::InvalidParam, "pixelFormat must be BGRA_8, BGRA_8_linear, BGRA_8_sRGB or A_8", this, &TYPEINFO, __func__, __FILE__, __LINE__);
            return false;
        }

		if( pixelFormat == PixelFormat::BGRA_8 )
			pixelFormat = GfxBase::defaultToSRGB() ? PixelFormat::BGRA_8_sRGB : PixelFormat::BGRA_8_linear;

		if( pixelFormat == PixelFormat::BGRX_8 )
			pixelFormat = GfxBase::defaultToSRGB() ? PixelFormat::BGRX_8_sRGB : PixelFormat::BGRX_8_linear;

        if( m_defaultCanvasRenderPassDesc )
            [m_defaultCanvasRenderPassDesc release];
        m_defaultCanvasRenderPassDesc = renderPassDesc;
        m_defaultCanvasPixelFormat = pixelFormat;
        [m_defaultCanvasRenderPassDesc retain];
		m_defaultCanvas.size = pixelSize*64;
        m_defaultCanvas.scale = scale;
        
        return true;
    }

    //____ setTintColor() _______________________________________________________

    void MetalGfxDevice::setTintColor(HiColor color)
    {
        if (color == m_tintColor)
            return;

        if (!m_bRendering)
        {
            //TODO: Error handling!
            return;
        }

        GfxDevice::setTintColor(color);

        _endCommand();
        _beginStateCommandWithAlignedData(Command::SetTintColor, 2);
        *(int64_t*)(&m_pCommandBuffer[m_commandOfs]) = color.argb;
        m_commandOfs += 2;
    }

    //____ setTintGradient() ______________________________________________________

    void MetalGfxDevice::setTintGradient(const RectSPX& rect, const Gradient& gradient)
    {
        if (m_bTintGradient && gradient == m_tintGradient && rect == m_tintGradientRect)
            return;

        if (!m_bRendering)
        {
            //TODO: Error handling!
            return;
        }

        GfxDevice::setTintGradient(rect, gradient);

        _endCommand();
        _beginStateCommandWithAlignedData(Command::SetTintGradient, 4 + sizeof(Gradient)/4);
        m_pCommandBuffer[m_commandOfs++] = rect.x / 64;
        m_pCommandBuffer[m_commandOfs++] = rect.y / 64;
        m_pCommandBuffer[m_commandOfs++] = rect.w / 64;
        m_pCommandBuffer[m_commandOfs++] = rect.h / 64;
        *(Gradient*)(&m_pCommandBuffer[m_commandOfs]) = gradient;
        m_commandOfs += sizeof(Gradient)/4;
    }

    //____ clearTintGradient() _____________________________________________________

    void MetalGfxDevice::clearTintGradient()
    {
        if (m_bTintGradient == false)
            return;

        if (!m_bRendering)
        {
            //TODO: Error handling!
            return;
        }

        GfxDevice::clearTintGradient();

        _endCommand();
        _beginStateCommand(Command::ClearTintGradient, 0);
    }

    //____ setBlendMode() ____________________________________________________________

    bool MetalGfxDevice::setBlendMode(BlendMode blendMode)
    {
        if (blendMode == m_blendMode)
            return true;

        if (!m_bRendering)
        {
            //TODO: Error handling!
            return false;
        }
        
        GfxDevice::setBlendMode(blendMode);

        _endCommand();
        _beginStateCommand(Command::SetBlendMode, 1);
        m_pCommandBuffer[m_commandOfs++] = (int) blendMode;

        return true;
    }

    //____ setBlitSource() ___________________________________________________________

    bool MetalGfxDevice::setBlitSource(Surface * pSource)
    {
        if (pSource == m_pBlitSource)
            return true;

        if (!m_bRendering)
        {
            //TODO: Error handling!
            return false;
        }

        if (!pSource || pSource->typeInfo() != MetalSurface::TYPEINFO)
        {
            GfxBase::throwError(ErrorLevel::SilentError, ErrorCode::InvalidParam, "Provided surface is NOT a MetalSurface", this, &TYPEINFO, __func__, __FILE__, __LINE__);
            return false;
        }

        m_pBlitSource = pSource;

        //TODO: Check so that we don't overrun m_pSurfaceBuffer;
        
        _endCommand();
        _beginStateCommandWithAlignedData(Command::SetBlitSource, sizeof(void*)/sizeof(int));
        * (void**)(m_pCommandBuffer+m_commandOfs) = pSource;
        m_commandOfs += sizeof(void*)/sizeof(int);
        if( pSource )
            pSource->retain();

        return true;
    }

    //____ setMorphFactor() __________________________________________________________

    void MetalGfxDevice::setMorphFactor(float factor)
    {
        if (factor == m_morphFactor)
            return;

        if (!m_bRendering)
        {
            //TODO: Error handling!
            return;
        }

        limit(factor, 0.f, 1.f);

        m_morphFactor = factor;

        _endCommand();
        _beginStateCommand(Command::SetMorphFactor, 1);
        m_pCommandBuffer[m_commandOfs++] = (int)(factor*1024);
    }

	//____ setFixedBlendColor() __________________________________________________

	void MetalGfxDevice::setFixedBlendColor( HiColor color )
	{
		if( color == m_fixedBlendColor )
			return;

		if (!m_bRendering)
		{
			//TODO: Error handling!
			return;
		}

		m_fixedBlendColor = color;

		_endCommand();
		_beginStateCommandWithAlignedData(Command::SetFixedBlendColor, 2);
		*(int64_t*)(&m_pCommandBuffer[m_commandOfs]) = color.argb;
		m_commandOfs += 2;
	}

	//____ setBlurMatrices() _____________________________________________________

	void MetalGfxDevice::setBlurMatrices( spx radius, const float red[9], const float green[9], const float blue[9] )
	{
		GfxDevice::setBlurMatrices(radius, red, green, blue);
		_endCommand();
		_beginStateCommand(Command::SetBlurMatrices, 28);

		m_pCommandBuffer[m_commandOfs++] = radius;

		// Copy floats as they are
		
		memcpy( m_pCommandBuffer + m_commandOfs, red, sizeof(float)*9 );
		m_commandOfs += 9;
		memcpy( m_pCommandBuffer + m_commandOfs, green, sizeof(float)*9 );
		m_commandOfs += 9;
		memcpy( m_pCommandBuffer + m_commandOfs, blue, sizeof(float)*9 );
		m_commandOfs += 9;
	}

    //____ isCanvasReady() ___________________________________________________________

    bool MetalGfxDevice::isCanvasReady() const
    {
        return m_flushesInProgress == 0;
    }

    //____ autopresent() ________________________________________________

    void MetalGfxDevice::autopresent( id<MTLDrawable> drawable )
    {
        m_drawableToAutoPresent = drawable;
    }

    //____ beginRender() _____________________________________________________________

    bool MetalGfxDevice::beginRender()
    {
        if( m_bRendering == true )
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
                   
        //

        _resetBuffers();

        
        // Create a new command buffer for each render pass to the current drawable.
        m_metalCommandBuffer = [s_metalCommandQueue commandBuffer];
        m_metalCommandBuffer.label = @"MetalGfxDevice";

        // Set intial canvas
        
        m_pActiveCanvas     = nullptr;
        m_activeCanvasSize  = {0,0};
        
        // Set intial states in super
        
        GfxDevice::setBlitSource(nullptr);
        GfxDevice::setBlendMode(BlendMode::Blend);
        GfxDevice::setMorphFactor(0.5f);
		GfxDevice::setFixedBlendColor(HiColor::Black);
        GfxDevice::setTintColor(HiColor::White);
        GfxDevice::clearTintGradient();
        
        // Set initial active states
        
        m_pActiveBlitSource = nullptr;
        m_activeBlendMode   = BlendMode::Blend;
        m_activeMorphFactor = 0.5f;
		m_activeFixedBlendColor = HiColor::White;
		
		m_morphFactorInUse = -1.f;
		m_fixedBlendColorInUse = HiColor::Undefined;
		
        m_uniform.flatTint[0] = 1.f;
        m_uniform.flatTint[1] = 1.f;
        m_uniform.flatTint[2] = 1.f;
        m_uniform.flatTint[3] = 1.f;

        m_bGradientActive = false;

        // Wait for previous render pass to complete by doing a flushAndWait on what we have in buffer so far.
        
        if( m_flushesInProgress > 0 )
            flushAndWait();
        
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
        {
            [m_metalCommandBuffer presentDrawable:m_drawableToAutoPresent];
            m_drawableToAutoPresent = nil;
        }
        
        // Add a completion handler.
        [m_metalCommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> cb) {
            // Shared buffer is populated.
            m_flushesInProgress--;
        }];
        
        // Finalize rendering here & push the command buffer to the GPU.
        [m_metalCommandBuffer commit];

        [m_metalCommandBuffer waitUntilScheduled];

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

    void MetalGfxDevice::fill(const RectSPX& rect, HiColor col)
    {
        // Skip calls that won't affect destination

        if (col.a == 0 && (m_blendMode == BlendMode::Blend))
            return;

        if (!rect.isOverlapping(m_clipBounds))
            return;

        //

		if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8)
			_resizeBuffers(2, 6*m_nClipRects, 0, 8, 0);

		//
		
        if (((rect.x | rect.y | rect.w | rect.h) & 0x3F) == 0)
        {
            // No subpixel precision, make it quick and easy

            
            if (m_cmd != Command::Fill)
            {
                _endCommand();
                _beginDrawCommand(Command::Fill);
            }

            for (int i = 0; i < m_nClipRects; i++)
            {
                RectI patch = roundToPixels(RectSPX::overlap(m_pClipRects[i], rect));
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

            m_pExtrasBuffer[m_extrasOfs++] = col.r / 4096.f;
            m_pExtrasBuffer[m_extrasOfs++] = col.g / 4096.f;
            m_pExtrasBuffer[m_extrasOfs++] = col.b / 4096.f;
            m_pExtrasBuffer[m_extrasOfs++] = col.a / 4096.f;
        }
        else
        {
            // We have subpixel precision

			// Create our outer rectangle

			RectI outerRect( rect.x / 64, rect.y / 64, ((rect.x+rect.w+63)/64) - (rect.x/64), ((rect.y + rect.h + 63)/64) - (rect.y/64) );

			//
			
			if (m_cmd != Command::FillSubPixel)
			{
				_endCommand();
				_beginDrawCommand(Command::FillSubPixel);
			}

			for (int i = 0; i < m_nClipRects; i++)
			{
				RectI patch = RectI::overlap(m_pClipRects[i]/64, outerRect);
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

			m_pExtrasBuffer[m_extrasOfs++] = col.r / 4096.f;
			m_pExtrasBuffer[m_extrasOfs++] = col.g / 4096.f;
			m_pExtrasBuffer[m_extrasOfs++] = col.b / 4096.f;
			m_pExtrasBuffer[m_extrasOfs++] = col.a / 4096.f;

			// Provide rectangle center and radius

			SizeF    radius(rect.w / (2.f*64), rect.h / (2.f*64));
			CoordF    center( (rect.x/64.f) + radius.w, (rect.y/64.f) + radius.h);

			m_pExtrasBuffer[m_extrasOfs++] = center.x;
			m_pExtrasBuffer[m_extrasOfs++] = center.y;
			m_pExtrasBuffer[m_extrasOfs++] = radius.w;
			m_pExtrasBuffer[m_extrasOfs++] = radius.h;
			
        }
    }

    //____ plotPixels() _________________________________________________________________

	void MetalGfxDevice::plotPixels(int nPixels, const CoordSPX * pCoords, const HiColor * pColors)
	{
        if (nPixels == 0)
            return;

        if (m_vertexOfs > m_vertexBufferSize - nPixels || m_extrasOfs > m_extrasBufferSize - 4 * nPixels)
            _resizeBuffers(2, nPixels, 0, 4*nPixels, 0);

        if (m_cmd != Command::Plot)
        {
            _endCommand();
            _beginDrawCommand(Command::Plot);
        }

        for (int i = 0; i < m_nClipRects; i++)
        {
            const RectI& clip = m_pClipRects[i];
            for (int pixel = 0; pixel < nPixels; pixel++)
            {
                if (clip.contains(pCoords[pixel]))
                {
                    m_pVertexBuffer[m_vertexOfs].coord = pCoords[pixel] / 64;
                    m_pVertexBuffer[m_vertexOfs].extrasOfs = m_extrasOfs / 4;
                    m_vertexOfs++;

                    m_pExtrasBuffer[m_extrasOfs++] = pColors[pixel].r / 4096.f;
                    m_pExtrasBuffer[m_extrasOfs++] = pColors[pixel].g / 4096.f;
                    m_pExtrasBuffer[m_extrasOfs++] = pColors[pixel].b / 4096.f;
                    m_pExtrasBuffer[m_extrasOfs++] = pColors[pixel].a / 4096.f;
                }
            }
        }
    }

    //____ drawLine() ____ [from/to] __________________________________________________

	void MetalGfxDevice::drawLine(CoordSPX begin, CoordSPX end, HiColor color, spx _thickness)
	{
        //TODO: Proper 26:6 support
        begin = roundToPixels(begin);
        end = roundToPixels(end);
     
		float thickness = _thickness/64.f;
		
        // Skip calls that won't affect destination

        if (color.a == 0 && (m_blendMode == BlendMode::Blend))
            return;

        //
        
        if (m_vertexOfs > m_vertexBufferSize - 6 || m_extrasOfs > m_extrasBufferSize - 8 )
              _resizeBuffers(2, 6, 0, 8, 0);

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

        m_pExtrasBuffer[m_extrasOfs++] = color.r / 4096.f;
        m_pExtrasBuffer[m_extrasOfs++] = color.g / 4096.f;
        m_pExtrasBuffer[m_extrasOfs++] = color.b / 4096.f;
        m_pExtrasBuffer[m_extrasOfs++] = color.a / 4096.f;

        m_pExtrasBuffer[m_extrasOfs++] = s;
        m_pExtrasBuffer[m_extrasOfs++] = w;
        m_pExtrasBuffer[m_extrasOfs++] = slope;
        m_pExtrasBuffer[m_extrasOfs++] = bSteep;

	}

    //____ drawLine() ____ [start/direction] __________________________________________________

    void MetalGfxDevice::drawLine(CoordSPX begin, Direction dir, spx length, HiColor color, spx _thickness)
    {
        //TODO: Proper 26:6 support
        begin = roundToPixels(begin);
        length = roundToPixels(length);
 
		float thickness = _thickness/64.f;

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

        if (!outerRect.isOverlapping(m_clipBounds/64))
            return;

        //

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8)
			_resizeBuffers(2, 6 * m_nClipRects, 0, 8, 0);

        if (m_cmd != Command::FillSubPixel)
        {
            _endCommand();
            _beginDrawCommand(Command::FillSubPixel);
        }


        // Provide the patches

        for (int i = 0; i < m_nClipRects; i++)
        {
            RectI patch = RectI::overlap(m_pClipRects[i]/64, outerRect);
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

        m_pExtrasBuffer[m_extrasOfs++] = color.r / 4096.f;
        m_pExtrasBuffer[m_extrasOfs++] = color.g / 4096.f;
        m_pExtrasBuffer[m_extrasOfs++] = color.b / 4096.f;
        m_pExtrasBuffer[m_extrasOfs++] = color.a / 4096.f;


        // Provide rectangle center and raidus.

        SizeF    radius(rect.w / 2, rect.h / 2);
        CoordF    center(rect.x + radius.w, rect.y + radius.h);

        m_pExtrasBuffer[m_extrasOfs++] = center.x;
        m_pExtrasBuffer[m_extrasOfs++] = center.y;
        m_pExtrasBuffer[m_extrasOfs++] = radius.w;
        m_pExtrasBuffer[m_extrasOfs++] = radius.h;
    }

	//____ drawEdgemap() __________________________________________________________

	void MetalGfxDevice::drawEdgemap(CoordSPX destPos, Edgemap * pEdgemap )
	{
		if( pEdgemap->typeInfo() != MetalEdgemap::TYPEINFO )
		{
			//TODO: Throw an error.
			return;
		}
		
		auto pWave = static_cast<MetalEdgemap*>(pEdgemap);


		_transformDrawSegments( {destPos,pWave->m_size*64}, pWave->m_nbRenderSegments, pWave->m_pRenderColors,
							   pWave->m_size.w+1, pWave->m_pSamples, pWave->m_nbSegments-1, pWave->m_tintMode,
							   s_blitFlipTransforms[(int)GfxFlip::None] );
	}

	//____ flipDrawEdgemap() ______________________________________________________

	void MetalGfxDevice::flipDrawEdgemap(CoordSPX destPos, Edgemap * pEdgemap, GfxFlip flip)
	{
		if( pEdgemap->typeInfo() != MetalEdgemap::TYPEINFO )
		{
			//TODO: Throw an error.
			return;
		}
		
		auto pWave = static_cast<MetalEdgemap*>(pEdgemap);

		const int (&transform)[2][2] = s_blitFlipTransforms[(int)flip];

		RectSPX dest;
		dest.x = destPos.x;
		dest.y = destPos.y;
		dest.w = pWave->m_size.w*64 * abs(transform[0][0]) + pWave->m_size.h*64 * abs(transform[1][0]);
		dest.h = pWave->m_size.w*64 * abs(transform[0][1]) + pWave->m_size.h*64 * abs(transform[1][1]);

		_transformDrawSegments( dest, pWave->m_nbRenderSegments, pWave->m_pRenderColors,
							   pWave->m_size.w+1, pWave->m_pSamples, pWave->m_nbSegments-1, pWave->m_tintMode,
							   transform );
	}

    //____ _transformBlitSimple() ______________________________________________________

	void MetalGfxDevice::_transformBlitSimple(const RectSPX& dest, CoordSPX src, const int simpleTransform[2][2],  OpType type)
	{
        if (m_pBlitSource == nullptr)
            return;

        if (!dest.isOverlapping(m_clipBounds))
            return;

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8 )
			_resizeBuffers(2, 6 * m_nClipRects, 0, 8, 0);

		
		if( type == OpType::Blur && m_cmd != Command::Blur )
		{
			_endCommand();
			_beginDrawCommandWithSource(Command::Blur);
		}
        else if (m_cmd != Command::Blit)
        {
            _endCommand();
            _beginDrawCommandWithSource(Command::Blit);
        }

        //TODO: Proper 26:6 support
//        RectI dest = roundToPixels(_dest);

        for (int i = 0; i < m_nClipRects; i++)
        {
            RectI patch = roundToPixels(RectI::overlap(m_pClipRects[i], dest));
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

        m_pExtrasBuffer[m_extrasOfs++] = (float) roundToPixels(src.x);
        m_pExtrasBuffer[m_extrasOfs++] = (float) roundToPixels(src.y);
        m_pExtrasBuffer[m_extrasOfs++] = (float) roundToPixels(dest.x);
        m_pExtrasBuffer[m_extrasOfs++] = (float) roundToPixels(dest.y);

        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[0][0];
        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[0][1];
        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[1][0];
        m_pExtrasBuffer[m_extrasOfs++] = (float) simpleTransform[1][1];
    }

    //____ _transformBlitComplex() ______________________________________________________

	void MetalGfxDevice::_transformBlitComplex(const RectSPX& _dest, BinalCoord src, const binalInt complexTransform[2][2], OpType type)
	{
        if (m_pBlitSource == nullptr)
            return;

        if (!_dest.isOverlapping(m_clipBounds))
            return;

        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8)
			_resizeBuffers(2, 6 * m_nClipRects, 0, 8, 0);


		if( type == OpType::Blur && m_cmd != Command::Blur )
		{
			_endCommand();
			_beginDrawCommandWithSource(Command::Blur);
		}
		else if (m_cmd != Command::Blit)
        {
            _endCommand();
            _beginDrawCommandWithSource(Command::Blit);
        }

        //
        
        //TODO: Proper 26:6 support
		//TODO: Proper 26:6 support
		RectI dest = roundToPixels(_dest);
		
		//

        for (int i = 0; i < m_nClipRects; i++)
        {
			RectI patch = RectI::overlap(m_pClipRects[i]/64, dest);
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

        if (m_pBlitSource->sampleMethod() == SampleMethod::Bilinear)
        {
			m_pExtrasBuffer[m_extrasOfs++] = (float(src.x) / BINAL_MUL) + 0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = (float(src.y) / BINAL_MUL) + 0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = dest.x + 0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = dest.y + 0.5f;
        }
        else
        {
            m_pExtrasBuffer[m_extrasOfs++] = (float(src.x) / BINAL_MUL);                //TODO: Ugly patch. Figure out what exactly goes wrong and fix it!
            m_pExtrasBuffer[m_extrasOfs++] = (float(src.y) / BINAL_MUL);                //TODO: Ugly patch. Figure out what exactly goes wrong and fix it!
            m_pExtrasBuffer[m_extrasOfs++] = dest.x +0.5f;
            m_pExtrasBuffer[m_extrasOfs++] = dest.y +0.5f;
        }

        m_pExtrasBuffer[m_extrasOfs++] = float(complexTransform[0][0]) / BINAL_MUL;
		m_pExtrasBuffer[m_extrasOfs++] = float(complexTransform[0][1]) / BINAL_MUL;
		m_pExtrasBuffer[m_extrasOfs++] = float(complexTransform[1][0]) / BINAL_MUL;
		m_pExtrasBuffer[m_extrasOfs++] = float(complexTransform[1][1]) / BINAL_MUL;
	}

    //____ _transformDrawSegments() ___________________________________________________

	void MetalGfxDevice::_transformDrawSegments(const RectSPX& _destIn, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
        if (!_destIn.isOverlapping(m_clipBounds))
            return;

        //

		//TODO: Proper 26:6 support
		RectI _dest = roundToPixels(_destIn);
		
        int segEdgeSpaceNeeded = 4 * (nEdgeStrips - 1)*(nSegments - 1);
                
        if( m_segPalOfs == m_segPalBufferSize )
        {
            flushAndWait();
            m_nSegments = nSegments;
            _beginDrawCommandWithInt(Command::Segments, m_nSegments);
        }
        
        if (m_vertexOfs > m_vertexBufferSize - 6 * m_nClipRects || m_extrasOfs > m_extrasBufferSize - 8 || m_segEdgeOfs > m_segEdgeBufferSize - segEdgeSpaceNeeded )            // various data, transform , colors, edgestrips
        {
			_resizeBuffers(2, 6 * m_nClipRects, 0, 8, segEdgeSpaceNeeded);
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
            RectI patch = RectI::overlap(m_pClipRects[i]/64, dest);
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

        const HiColor* pSegCol = pSegmentColors;

        uint16_t* pMapRow = &m_pSegPalBuffer[m_segPalOfs*c_segPalEntrySize/2];
        int            mapPitch = c_maxSegments * 4 * 2;

        switch (tintMode)
        {
            case TintMode::None:
            case TintMode::Flat:
            {
                for (int i = 0; i < nSegments; i++)
                {
                    uint16_t r = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    uint16_t g = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    uint16_t b = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    uint16_t a = uint16_t(int(pSegCol->a) * 65535 / 4096);

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
                    int r1 = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    int g1 = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    int b1 = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    int a1 = uint16_t(int(pSegCol->a) * 65535 / 4096);
                    pSegCol++;

                    int r2 = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    int g2 = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    int b2 = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    int a2 = uint16_t(int(pSegCol->a) * 65535 / 4096);
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
                    int r1 = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    int g1 = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    int b1 = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    int a1 = uint16_t(int(pSegCol->a) * 65535 / 4096);
                    pSegCol++;

                    int r2 = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    int g2 = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    int b2 = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    int a2 = uint16_t(int(pSegCol->a) * 65535 / 4096);
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
                    pMapRow[i * 8 + 0] = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    pMapRow[i * 8 + 1] = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    pMapRow[i * 8 + 2] = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    pMapRow[i * 8 + 3] = uint16_t(int(pSegCol->a) * 65535 / 4096);
                    pSegCol++;

                    pMapRow[i * 8 + 4] = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    pMapRow[i * 8 + 5] = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    pMapRow[i * 8 + 6] = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    pMapRow[i * 8 + 7] = uint16_t(int(pSegCol->a) * 65535 / 4096);
                    pSegCol++;

                    pMapRow[mapPitch + i * 8 + 4] = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    pMapRow[mapPitch + i * 8 + 5] = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    pMapRow[mapPitch + i * 8 + 6] = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    pMapRow[mapPitch + i * 8 + 7] = uint16_t(int(pSegCol->a) * 65535 / 4096);
                    pSegCol++;

                    pMapRow[mapPitch + i * 8 + 0] = uint16_t(int(pSegCol->r) * 65535 / 4096);
                    pMapRow[mapPitch + i * 8 + 1] = uint16_t(int(pSegCol->g) * 65535 / 4096);
                    pMapRow[mapPitch + i * 8 + 2] = uint16_t(int(pSegCol->b) * 65535 / 4096);
                    pMapRow[mapPitch + i * 8 + 3] = uint16_t(int(pSegCol->a) * 65535 / 4096);
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

                float increment = edgeOut == edgeIn ? 100.f : 64.f / (edgeOut - edgeIn);
                float beginAdder;
                float endAdder;

                if ((edgeOut & 0xFFFFFFC0) <= (unsigned int) edgeIn)
                {
                    float firstPixelCoverage = ((64 - (edgeOut & 0x3F)) + (edgeOut - edgeIn) / 2) / 64.f;

                    beginAdder = increment * (edgeIn & 0x3F)/64.f + firstPixelCoverage;
                    endAdder = beginAdder;
                }
                else
                {
                    int height = 64 - (edgeIn & 0x3F);
                    int width = (int)(increment * height);
                    float firstPixelCoverage = (height * width) / (2 * 4096.f);
                    float lastPixelCoverage = 1.f - (edgeOut & 0x3F)*increment*(edgeOut & 0x3F) / (2*4096.f);

                    beginAdder = increment * (edgeIn & 0x3F) / 64.f + firstPixelCoverage;
                    endAdder = lastPixelCoverage - (1.f - (edgeOut & 0x3F)*increment / 64.f);
//                     endAdder = lastPixelCoverage - ((edgeOut & 0xFFFFFF00)-edgeIn)*increment / 256.f;
                }

                *pEdgeData++ = edgeIn/64.f;                    // Segment begin pixel
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
    
    void MetalGfxDevice::_resizeBuffers(int commandNeeded, int vertexNeeded, int clipListNeeded, int extrasNeeded, int segEdgeNeeded)
    {
        // We are modifying buffers even though a flush might be in progress.
        // We are assuming that original buffer will be kept alive and utilized during the flush.
        
        if( (m_commandBufferSize - m_commandOfs) < commandNeeded  )
        {
            m_commandBufferSize *= 2;
            int * pNewBuffer = new int[m_commandBufferSize];
            memcpy( pNewBuffer, m_pCommandBuffer, m_commandOfs * sizeof(int));
            delete [] m_pCommandBuffer;
            m_pCommandBuffer = pNewBuffer;
        }

        if( (m_vertexBufferSize - m_vertexOfs) <vertexNeeded )
        {
			while( (m_vertexBufferSize - m_vertexOfs) < vertexNeeded )
				m_vertexBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_vertexBufferSize*sizeof(Vertex) options:MTLResourceStorageModeShared];

            Vertex * pNewBuffer = (Vertex *)[newId contents];
            memcpy( pNewBuffer, m_pVertexBuffer, m_vertexOfs * sizeof(Vertex));
            m_pVertexBuffer = pNewBuffer;

			[m_vertexBufferId release];
            m_vertexBufferId = newId;
        }

        if( (m_extrasBufferSize - m_extrasOfs) < extrasNeeded )
        {
            while( (m_extrasBufferSize - m_extrasOfs) < extrasNeeded )
                m_extrasBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_extrasBufferSize*sizeof(float) options:MTLResourceStorageModeShared];

            float * pNewBuffer = (float*)[newId contents];
            memcpy( pNewBuffer, m_pExtrasBuffer, m_extrasOfs * sizeof(float));
            m_pExtrasBuffer = pNewBuffer;
            
			[m_extrasBufferId release];
            m_extrasBufferId = newId;
        }

        if( (m_clipListBufferSize - m_clipWriteOfs) < clipListNeeded )
        {
			while( (m_clipListBufferSize - m_clipWriteOfs) < clipListNeeded )
				m_clipListBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_clipListBufferSize*sizeof(RectI) options:MTLResourceStorageModeShared];

            RectI * pNewBuffer = (RectI *)[newId contents];
            memcpy( pNewBuffer, m_pClipListBuffer, m_clipWriteOfs * sizeof(RectI));
            m_pClipListBuffer = pNewBuffer;
            
			[m_clipListBufferId release];
            m_clipListBufferId = newId;
        }
        
        if( (m_segEdgeBufferSize - m_segEdgeOfs) < segEdgeNeeded )
        {
            while( (m_segEdgeBufferSize - m_segEdgeOfs) < segEdgeNeeded )
                m_segEdgeBufferSize *= 2;

            id<MTLBuffer> newId = [s_metalDevice newBufferWithLength:m_segEdgeBufferSize*sizeof(float) options:MTLResourceStorageModeManaged];

            float * pNewBuffer = (float*)[newId contents];
            memcpy( pNewBuffer, m_pSegEdgeBuffer, m_segEdgeOfs * sizeof(float));
            m_pSegEdgeBuffer = pNewBuffer;
            
			[m_segEdgeBufferId release];
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
            blitCommandEncoder = nil;
            
            //TODO: We need to sync this, don't start drawing segments until this has been fully uploaded.
        }

        // Create our render encoder.
        
        id<MTLRenderCommandEncoder> renderEncoder = nil;
        
        int * pCmd = m_pCommandBuffer;
        int * pCmdEnd = &m_pCommandBuffer[m_commandOfs];

        int vertexOfs = m_vertexFlushPoint;
      
        // Set initial state of canvas
        
        if( m_pActiveCanvas )
        {
            renderEncoder = _setCanvas( m_pActiveCanvas, m_activeCanvasSize.w, m_activeCanvasSize.h, CanvasInit::Keep, Color::Black );
            _setBlitSource( renderEncoder, m_pActiveBlitSource );
        }
        
        
        // Clear pending flags of active BlitSource and Canvas.

    //    if (m_pActiveBlitSource)
    //        m_pActiveBlitSource->m_bPendingReads = false;

        while (pCmd < pCmdEnd)
        {
            Command cmd = (Command) * pCmd++;

            switch (cmd)
            {
				case Command::None:
					break;
					
                case Command::SetCanvas:
                {
                    if( renderEncoder != nil )
                    {
                        [renderEncoder endEncoding];
                        renderEncoder = nil;
                    }
                    
                    if( pCmd[0] == 0 && pCmd[1] == 0)
                    {
                        m_pActiveCanvas = nullptr;
                        m_activeCanvasSize.clear();
                    }
                    else
                    {
                        MetalSurface* pSurf = *((MetalSurface**)(pCmd+4));
                        renderEncoder = _setCanvas(pSurf, pCmd[0], pCmd[1], (CanvasInit) pCmd[2], pCmd[3]);

                        if( pSurf )
                            pSurf->release();

                    }
                    pCmd += 4 + sizeof(MetalSurface*)/sizeof(int);
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
                    _setTintColor(renderEncoder, *(HiColor*)(pCmd));
                    pCmd+=2;
                    break;
                }
				case Command::SetFixedBlendColor:
				{
					_setFixedBlendColor(renderEncoder, *(HiColor*)(pCmd));
					pCmd+=2;
					break;
				}
				
				case Command::SetBlurMatrices:
				{
					spx radius = * pCmd++;
					
					_setBlurMatrices(renderEncoder, radius, (float *)(pCmd), (float *)(pCmd+9), (float *)(pCmd+18) );
					pCmd+=27;
					break;
				}
					
                case Command::SetTintGradient:
                {
                    RectI& rect = *(RectI*)pCmd;
                    pCmd += 4;
                    Gradient* pGradient = (Gradient*)pCmd;
                    pCmd += sizeof(Gradient)/4;

                    _setTintGradient(renderEncoder, rect, * pGradient);
                    break;
                }
                case Command::ClearTintGradient:
                {
                     _clearTintGradient(renderEncoder);
                   break;
                }
                case Command::SetBlitSource:
                {
                    MetalSurface* pSurf = *((MetalSurface**)(pCmd));
                    
                    if( pSurf && pSurf->m_bMipmapStale )
                    {
                        if( renderEncoder != nil )
                            [renderEncoder endEncoding];
                            
                        id<MTLBlitCommandEncoder> blitCommandEncoder = [m_metalCommandBuffer blitCommandEncoder];
                        
                        [blitCommandEncoder generateMipmapsForTexture:pSurf->m_texture];
                        [blitCommandEncoder endEncoding];
						blitCommandEncoder = nil;
                            
                        if( renderEncoder != nil )
                            renderEncoder = _setCanvas( m_pActiveCanvas, m_activeCanvasSize.w, m_activeCanvasSize.h, CanvasInit::Keep, Color::White );

                        pSurf->m_bMipmapStale = false;
                    }
                    
                    _setBlitSource(renderEncoder, pSurf);
                    if( pSurf )
                        pSurf->release();
                    pCmd += sizeof(MetalSurface*)/sizeof(int);
                    break;
                }
                case Command::Blit:
                {
                    int nVertices = *pCmd++;
                    if (nVertices > 0 && m_pActiveBlitSource)
                    {
                        MetalSurface* pSurf = m_pActiveBlitSource;
                      
                        BlitFragShader shader = BlitFragShader::Normal;

                        if(pSurf->m_pPixelDescription->type == PixelType::Index)
                        {
                            if( pSurf->sampleMethod() == SampleMethod::Bilinear )
                                shader = BlitFragShader::PaletteInterpolated;
                            else
                                shader = BlitFragShader::PaletteNearest;
                        }
                        else if(pSurf->m_pixelFormat == PixelFormat::Alpha_8)
                            shader = BlitFragShader::A8Source;
                        else if(pSurf->m_pPixelDescription->A_mask == 0)
                            shader = BlitFragShader::RGBXSource;
                        
                        [renderEncoder setRenderPipelineState:m_blitPipelines[(int)shader][m_bGradientActive][(int)m_activeBlendMode][(int)m_activeCanvasFormat] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        vertexOfs += nVertices;
                        
                        if( m_pActiveCanvas )
                            m_pActiveCanvas->m_bMipmapStale = m_pActiveCanvas->m_bMipmapped;
                    }
                    break;
                }
    
				case Command::Blur:
				{
					int nVertices = *pCmd++;
					if (nVertices > 0 && m_pActiveBlitSource)
					{
						auto size = m_pActiveBlitSource->pixelSize();

						float radiusX = m_activeBlurRadius / float(size.w*64);
						float radiusY = m_activeBlurRadius / float(size.h*64);
						
						m_blurUniform.offset[0] = { -radiusX * 0.7f, -radiusY * 0.7f };
						m_blurUniform.offset[1] = { 0, -radiusY };
						m_blurUniform.offset[2] = { radiusX * 0.7f, -radiusY * 0.7f };

						m_blurUniform.offset[3] = { -radiusX, 0 };
						m_blurUniform.offset[4] = { 0, 0 };
						m_blurUniform.offset[5] = { radiusX, 0 };

						m_blurUniform.offset[6] = { -radiusX * 0.7f, radiusY * 0.7f };
						m_blurUniform.offset[7] = { 0, radiusY };
						m_blurUniform.offset[8] = { radiusX * 0.7f, radiusY * 0.7f };

						[renderEncoder setFragmentBytes:&m_blurUniform length:sizeof(BlurUniform) atIndex: (unsigned) FragmentInputIndex::BlurUniform];


						[renderEncoder setRenderPipelineState:m_blurPipelines[m_bGradientActive][(int)m_activeBlendMode][(int)m_activeCanvasFormat] ];
						[renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
						vertexOfs += nVertices;

						if( m_pActiveCanvas )
							m_pActiveCanvas->m_bMipmapStale = m_pActiveCanvas->m_bMipmapped;
					}
					break;
				}
					
                case Command::Fill:
                {
                    int nVertices = *pCmd++;
                    if( nVertices > 0 )
                    {
                        [renderEncoder setRenderPipelineState:m_fillPipelines[m_bGradientActive][(int)m_activeBlendMode][(int)m_activeCanvasFormat] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        vertexOfs += nVertices;
                        
                        if( m_pActiveCanvas )
                            m_pActiveCanvas->m_bMipmapStale = m_pActiveCanvas->m_bMipmapped;
                    }
                    break;
                }
    
                case Command::FillSubPixel:
                {
                    int nVertices = *pCmd++;
                    if( nVertices > 0 )
                    {
                        [renderEncoder setRenderPipelineState:m_fillAAPipelines[m_bGradientActive][(int)m_activeBlendMode][(int)m_activeCanvasFormat] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        vertexOfs += nVertices;
                        
                        if( m_pActiveCanvas )
                            m_pActiveCanvas->m_bMipmapStale = m_pActiveCanvas->m_bMipmapped;
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
                        [renderEncoder setRenderPipelineState:m_lineFromToPipelines[(int)m_activeBlendMode][(int)m_activeCanvasFormat] ];
                        
                        for (int i = 0; i < clipListLen; i++)
                        {
                            RectI& clip = m_pClipListBuffer[clipListOfs++];
                            MTLScissorRect metalClip = {(unsigned) clip.x/64, (unsigned) clip.y/64, (unsigned) clip.w/64, (unsigned) clip.h/64};
                            [renderEncoder setScissorRect:metalClip];
                            [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        }

                        MTLScissorRect orgClip = {0, 0, (unsigned) m_activeCanvasSize.w, (unsigned) m_activeCanvasSize.h};
                        [renderEncoder setScissorRect:orgClip];

                        vertexOfs += nVertices;
                        
                        if( m_pActiveCanvas )
                            m_pActiveCanvas->m_bMipmapStale = m_pActiveCanvas->m_bMipmapped;
                    }
                    break;
                }
                case Command::Plot:
                {
                     int nVertices = *pCmd++;
                     if( nVertices > 0 )
                     {
                         [renderEncoder setRenderPipelineState:m_plotPipelines[(int)m_activeBlendMode][(int)m_activeCanvasFormat] ];
                         [renderEncoder drawPrimitives:MTLPrimitiveTypePoint vertexStart:vertexOfs vertexCount:nVertices];
                         vertexOfs += nVertices;
                         
                         if( m_pActiveCanvas )
                             m_pActiveCanvas->m_bMipmapStale = m_pActiveCanvas->m_bMipmapped;
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
                        [renderEncoder setRenderPipelineState:m_segmentsPipelines[nEdges][m_bGradientActive][(int)m_activeBlendMode][(int)m_activeCanvasFormat] ];
                        [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:vertexOfs vertexCount:nVertices];
                        vertexOfs += nVertices;
                        
                        if( m_pActiveCanvas )
                            m_pActiveCanvas->m_bMipmapStale = m_pActiveCanvas->m_bMipmapped;
                    }
                    break;
                }
     
                default:
    //                assert(false);
                    break;
            }
        }

        if( renderEncoder != nil )
        {
            [renderEncoder endEncoding];
            renderEncoder = nil;
        }

        //
        
        m_vertexFlushPoint = m_vertexOfs;
        m_segEdgeFlushPoint = m_segEdgeOfs;
        m_commandOfs = 0;
        m_segPalOfs = 0;
    }

    //____ _resetBuffers() ____________________________________________________

    void MetalGfxDevice::_resetBuffers()
    {
        // Prepare our buffers

        m_vertexOfs = 0;
        m_extrasOfs = 0;
        m_commandOfs = 0;
        m_segEdgeOfs = 0;
        m_segPalOfs = 0;
        m_clipWriteOfs = 0;
        m_clipCurrOfs = -1;

        m_vertexFlushPoint = 0;
        m_segEdgeFlushPoint = 0;
        
    }

    //____ _setCanvas() _______________________________________________________

    id<MTLRenderCommandEncoder> MetalGfxDevice::_setCanvas( MetalSurface * pCanvas, int width, int height, CanvasInit initOperation, Color clearColor )
    {
        id<MTLRenderCommandEncoder> renderEncoder;
        
        PixelFormat pixelFormat;
        
        if (pCanvas)
        {
            MTLRenderPassDescriptor* pDescriptor = [MTLRenderPassDescriptor new];
            
            pDescriptor.colorAttachments[0].texture = pCanvas->getTexture();
            pDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;

            switch(initOperation)
            {
                case CanvasInit::Keep:
                    pDescriptor.colorAttachments[0].loadAction = MTLLoadActionLoad;
                    break;
                    
                case CanvasInit::Clear:
                {
                    pDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
                    float* pConv = GfxBase::defaultToSRGB() ? m_sRGBtoLinearTable : m_linearToLinearTable;
                    pDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(pConv[clearColor.r]/4096.f, pConv[clearColor.g]/4096.f, pConv[clearColor.b]/4096.f, clearColor.a/255.f);
                    break;
                }
                case CanvasInit::Discard:
                    pDescriptor.colorAttachments[0].loadAction = MTLLoadActionDontCare;
                    break;
            }
            
            
            renderEncoder = [m_metalCommandBuffer renderCommandEncoderWithDescriptor:pDescriptor];
            renderEncoder.label = @"GfxDeviceMetal Render to Surface Pass";
            [pDescriptor release];
            
            pixelFormat = pCanvas->pixelFormat();
        }
        else
        {
           
            renderEncoder = [m_metalCommandBuffer renderCommandEncoderWithDescriptor:m_defaultCanvasRenderPassDesc];
            renderEncoder.label = @"GfxDeviceMetal Render Pass";
           
            pixelFormat = m_defaultCanvasPixelFormat;
        }

        [renderEncoder setViewport:(MTLViewport){0.0, 0.0, (double) width, (double) height, 0.0, 1.0 }];
        [renderEncoder setBlendColorRed:1.f green:1.f blue:1.f alpha:m_activeMorphFactor];


        // Updating canvas info for our shaders

        m_uniform.canvasDimX = (GLfloat) width;
        m_uniform.canvasDimY = (GLfloat) height;
        m_uniform.canvasYOfs = height;
        m_uniform.canvasYMul = -1;

        [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex: (unsigned) VertexInputIndex::Uniform];

        // Set vertex and extras buffer
        
        [renderEncoder setVertexBuffer:m_vertexBufferId offset:0 atIndex:(unsigned) VertexInputIndex::VertexBuffer];
        [renderEncoder setVertexBuffer:m_extrasBufferId offset:0 atIndex:(unsigned) VertexInputIndex::ExtrasBuffer];

        // Set buffers/textures for segments fragment shader
        
        [renderEncoder setFragmentTexture:m_segPalTextureId atIndex: (unsigned) TextureIndex::SegPal];
        [renderEncoder setFragmentBuffer:m_segEdgeBufferId offset:0 atIndex:(unsigned) FragmentInputIndex::ExtrasBuffer];

		[renderEncoder setFragmentBytes:&m_blurUniform length:sizeof(BlurUniform) atIndex: (unsigned) FragmentInputIndex::BlurUniform];


        //
        
        m_pActiveCanvas = pCanvas;
        m_activeCanvasSize = {width,height};

        switch(pixelFormat)
        {
            case PixelFormat::BGRX_8_linear:
                m_activeCanvasFormat = DestFormat::BGRX8_linear;
                break;

            case PixelFormat::BGRA_8_linear:
                m_activeCanvasFormat = DestFormat::BGRA8_linear;
                break;
                
            case PixelFormat::BGRA_8_sRGB:
                m_activeCanvasFormat = DestFormat::BGRA8_sRGB;
                break;

            case PixelFormat::BGRX_8_sRGB:
                m_activeCanvasFormat = DestFormat::BGRX8_sRGB;
                break;

            case PixelFormat::Alpha_8:
                m_activeCanvasFormat = DestFormat::Alpha_8;
                break;
                
            default:
                GfxBase::throwError(ErrorLevel::Error, ErrorCode::Internal, "Canvas format is neither BGRA_8_linear, BGRA_8_sRGB or A_8", this, &TYPEINFO, __func__, __FILE__, __LINE__ );
                break;
        }

        
        return renderEncoder;
    }

    //____ _setBlendMode() _______________________________________________________

    void MetalGfxDevice::_setBlendMode( id<MTLRenderCommandEncoder> renderEncoder, BlendMode mode )
    {
        m_activeBlendMode = mode;
		
		// If BlendMode is Morph or BlendFixedColor we must make sure Metal's blendColor is set accordingly
		
		if( mode == BlendMode::Morph )
		{
			if( m_morphFactorInUse != m_activeMorphFactor )
			{
				[renderEncoder setBlendColorRed:1.f green:1.f blue:1.f alpha:m_activeMorphFactor];

				m_morphFactorInUse = m_activeMorphFactor;
				m_fixedBlendColorInUse = HiColor::Undefined;
			}
		}
		else if( mode == BlendMode::BlendFixedColor )
		{
			if( m_fixedBlendColorInUse != m_activeFixedBlendColor )
			{
				[renderEncoder setBlendColorRed:(m_activeFixedBlendColor.r/4096.f)
										  green:(m_activeFixedBlendColor.g/4096.f)
										   blue:(m_activeFixedBlendColor.b/4096.f)
										  alpha:(m_activeFixedBlendColor.a/4096.f)];

				m_morphFactorInUse = -1;
				m_fixedBlendColorInUse = m_activeFixedBlendColor;
			}
		}
    }

    //____ _setMorphFactor() __________________________________________________

    void MetalGfxDevice::_setMorphFactor( id<MTLRenderCommandEncoder> renderEncoder, float morphFactor)
    {
        m_activeMorphFactor = morphFactor;

        if( m_activeBlendMode == BlendMode::Morph )
        {
            if( m_morphFactorInUse != m_activeMorphFactor )
            {
                [renderEncoder setBlendColorRed:1.f green:1.f blue:1.f alpha:m_activeMorphFactor];

                m_morphFactorInUse = m_activeMorphFactor;
            }
        }
    }

	//____ _setFixedBlendColor() __________________________________________________

	void MetalGfxDevice::_setFixedBlendColor( id<MTLRenderCommandEncoder> renderEncoder, HiColor color)
	{
		m_activeFixedBlendColor = color;

        if( m_activeBlendMode == BlendMode::BlendFixedColor )
        {
            if( m_fixedBlendColorInUse != m_activeFixedBlendColor )
            {
                [renderEncoder setBlendColorRed:(m_activeFixedBlendColor.r/4096.f)
                                          green:(m_activeFixedBlendColor.g/4096.f)
                                           blue:(m_activeFixedBlendColor.b/4096.f)
                                          alpha:(m_activeFixedBlendColor.a/4096.f)];

                m_fixedBlendColorInUse = m_activeFixedBlendColor;
            }
        }
	}

	//____ _setBlurMatrices() __________________________________________________

	void MetalGfxDevice::_setBlurMatrices( id<MTLRenderCommandEncoder> renderEncoder, spx radius, const float red[9], const float green[9], const float blue[9] )
	{
		m_activeBlurRadius = radius;
				
		m_blurUniform.colorMtx[0] = { red[0], green[0], blue[0], 0.f };
		m_blurUniform.colorMtx[1] = { red[1], green[1], blue[1], 0.f };
		m_blurUniform.colorMtx[2] = { red[2], green[2], blue[2], 0.f };
		m_blurUniform.colorMtx[3] = { red[3], green[3], blue[3], 0.f };
		m_blurUniform.colorMtx[4] = { red[4], green[4], blue[4], 1.f };
		m_blurUniform.colorMtx[5] = { red[5], green[5], blue[5], 0.f };
		m_blurUniform.colorMtx[6] = { red[6], green[6], blue[6], 0.f };
		m_blurUniform.colorMtx[7] = { red[7], green[7], blue[7], 0.f };
		m_blurUniform.colorMtx[8] = { red[8], green[8], blue[8], 0.f };
		
	}

    //____ _setBlitSource() _______________________________________________________

    void MetalGfxDevice::_setBlitSource( id<MTLRenderCommandEncoder> renderEncoder, MetalSurface * pSurf )
    {
        if( pSurf )
        {
            [renderEncoder setFragmentTexture:pSurf->getTexture() atIndex: (unsigned) TextureIndex::Texture];

            if(pSurf->pixelDescription()->type == PixelType::Index)
                [renderEncoder setFragmentSamplerState: m_samplers[0][0][pSurf->isTiling()] atIndex:0];
            else
                [renderEncoder setFragmentSamplerState: m_samplers[pSurf->isMipmapped()][pSurf->sampleMethod() == SampleMethod::Bilinear][pSurf->isTiling()] atIndex:0];

            m_pActiveBlitSource = pSurf;
            pSurf->m_bPendingReads = false;            // Clear this as we pass it by. All pending reads will have encoded before _executeBuffer() ends.

            m_uniform.textureSize = pSurf->pixelSize();
            [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];

            if (pSurf->m_pPalette)
            {
                [renderEncoder setFragmentTexture:pSurf->getPaletteTexture() atIndex:(unsigned) TextureIndex::Palette];
            }
        }
        else
        {
            [renderEncoder setFragmentTexture:nil atIndex:(unsigned) TextureIndex::Texture];
            [renderEncoder setFragmentSamplerState: m_samplers[0][0][0] atIndex:0];
            
            m_pActiveBlitSource = nullptr;
        }

    }

    //____ _setTintColor() ____________________________________________________

    void MetalGfxDevice::_setTintColor( id<MTLRenderCommandEncoder> renderEncoder, HiColor color)
    {
        m_uniform.flatTint[0] = color.r / 4096.f;
        m_uniform.flatTint[1] = color.g / 4096.f;
        m_uniform.flatTint[2] = color.b / 4096.f;
        m_uniform.flatTint[3] = color.a / 4096.f;

        [renderEncoder setVertexBytes:&m_uniform length:sizeof(Uniform) atIndex:(unsigned) VertexInputIndex::Uniform];
    }

    //____ _setTintGradient() _________________________________________________

    void MetalGfxDevice::_setTintGradient( id<MTLRenderCommandEncoder> renderEncoder, const RectI& rect, const Gradient& gradient)
    {
        m_bGradientActive = true;

        m_uniform.tintRect = rect;

        m_uniform.topLeftTint[0] = gradient.topLeft.r / 4096.f;
        m_uniform.topLeftTint[1] = gradient.topLeft.g / 4096.f;
        m_uniform.topLeftTint[2] = gradient.topLeft.b / 4096.f;
        m_uniform.topLeftTint[3] = gradient.topLeft.a / 4096.f;

        m_uniform.topRightTint[0] = gradient.topRight.r / 4096.f;
        m_uniform.topRightTint[1] = gradient.topRight.g / 4096.f;
        m_uniform.topRightTint[2] = gradient.topRight.b / 4096.f;
        m_uniform.topRightTint[3] = gradient.topRight.a / 4096.f;

        m_uniform.bottomRightTint[0] = gradient.bottomRight.r / 4096.f;
        m_uniform.bottomRightTint[1] = gradient.bottomRight.g / 4096.f;
        m_uniform.bottomRightTint[2] = gradient.bottomRight.b / 4096.f;
        m_uniform.bottomRightTint[3] = gradient.bottomRight.a / 4096.f;

        m_uniform.bottomLeftTint[0] = gradient.bottomLeft.r / 4096.f;
        m_uniform.bottomLeftTint[1] = gradient.bottomLeft.g / 4096.f;
        m_uniform.bottomLeftTint[2] = gradient.bottomLeft.b / 4096.f;
        m_uniform.bottomLeftTint[3] = gradient.bottomLeft.a / 4096.f;

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

            case PixelFormat::Alpha_8:
                descriptor.colorAttachments[0].pixelFormat = MTLPixelFormatR8Unorm;
                break;
                
            default:
                assert(false);
        }
       
        bool bNoAlpha = ( destFormat == PixelFormat::BGRX_8_linear || destFormat == PixelFormat::BGRX_8_sRGB );
            
        
        bool bAlphaOnly = (destFormat == PixelFormat::Alpha_8);
        
        switch( blendMode )
        {
            case BlendMode::Replace:

                if( bNoAlpha )
                {
                    descriptor.colorAttachments[0].blendingEnabled = YES;
                    descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorZero;
                }
                else
                    descriptor.colorAttachments[0].blendingEnabled = NO;
                break;

            case BlendMode::Undefined:
            case BlendMode::Blend:
                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;

                if(bAlphaOnly)
                {
                    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceColor;
                }
                else
                {
                    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
                }
                
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
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
                }
                else
                {
                    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Subtract:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationReverseSubtract;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationReverseSubtract;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;

                if( bAlphaOnly )
                {
                    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
                }
                else
                {
                    descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
                    descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
                }
                break;

            case BlendMode::Multiply:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorDestinationColor;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;

                break;

            case BlendMode::Invert:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOneMinusDestinationColor;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceColor;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;

                break;

            case BlendMode::Min:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationMin;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
                break;

            case BlendMode::Max:

                descriptor.colorAttachments[0].blendingEnabled = YES;
                descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationMax;
                descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
                descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOne;
                descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
                descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
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

			case BlendMode::BlendFixedColor:

				descriptor.colorAttachments[0].blendingEnabled = YES;
				descriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
				descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
				descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
				descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusBlendAlpha;

				if(bAlphaOnly)
				{
					descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorOne;
					descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusBlendColor;
				}
				else
				{
					descriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorBlendAlpha;
					descriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusBlendAlpha;
				}
				
				break;
				
            default:
                assert(false);
                break;
        }
        
        if(bNoAlpha)
        {
            descriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
            descriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorZero;
            descriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOne;
        }

        id<MTLRenderPipelineState> pipelineState = [s_metalDevice newRenderPipelineStateWithDescriptor:descriptor error:&error];
        [error release];
        
        [descriptor.vertexFunction release];
        [descriptor.fragmentFunction release];
        [descriptor release];
        
        return pipelineState;
    }



} // namespace wg
