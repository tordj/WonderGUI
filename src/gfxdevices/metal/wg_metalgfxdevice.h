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

#ifndef WG_METALGFXDEVICE_DOT_H
#define WG_METALGFXDEVICE_DOT_H
#pragma once

#import "simd/simd.h"
#import "MetalKit/MetalKit.h"
//#import "Metal/Metal.h"


#include <wg_types.h>
#include <wg_gfxdevice.h>
#include <wg_metalsurface.h>

namespace wg
{
	class Surface;

	class MetalGfxDevice;
	typedef	StrongPtr<MetalGfxDevice>	MetalGfxDevice_p;
	typedef	WeakPtr<MetalGfxDevice>	MetalGfxDevice_wp;

	class MetalGfxDevice : public GfxDevice
	{
        friend class MetalSurface;
	public:
		//.____ Creation ________________________________________________

        static void setMetalDevice( id<MTLDevice> device );
        static id<MTLDevice> metalDevice() { return s_metalDevice; }
        
        static MetalGfxDevice_p	create();

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		const TypeInfo&			surfaceType( void ) const override;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory() override;

        //.____ State _________________________________________________

        bool    setBaseCanvasFormat( MTLRenderPassDescriptor* renderPassDesc, PixelFormat pixelFormat );
        
        void    setTintColor(HiColor color) override;
        void    setTintGradient(const RectI& rect, const Gradient& gradient) override;
        void    clearTintGradient() override;

        bool    setBlendMode(BlendMode blendMode) override;
        bool    setBlitSource(Surface * pSource) override;
        void    setMorphFactor(float factor) override;

        bool    isCanvasReady() const;
        
        
        //.____ Rendering ________________________________________________
        
        bool    beginRender() override;
        bool    endRender() override;
        bool    isIdle() override;
        void    flush() override;
        void    flushAndWait();

        void    autopresent( id<MTLDrawable> drawable );           // endRender() will clear this, so needs to be set for each begin/end render cycle.

        using   GfxDevice::fill;
		void	fill(const RectI& rect, HiColor col) override;
		void	fill(const RectF& rect, HiColor col) override;

		void    plotPixels(int nCoords, const CoordI * pCoords, const HiColor * pColors) override;

		void	drawLine(CoordI begin, CoordI end, HiColor color, float thickness) override;
        void    drawLine(CoordI begin, Direction dir, int length, HiColor color, float thickness) override;


	protected:
		MetalGfxDevice();
		~MetalGfxDevice();

        void    _canvasWasChanged() override;
        void    _renderLayerWasChanged() override;    // Checked for errors before we get here.
        void    _clipListWasChanged() override;            // Called when cliplist has been changed.
        
		void	_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2]) override;
		void	_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2]) override;

		void	_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdges, const int * pEdges, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2]) override;

        enum class VertexInputIndex
        {
            VertexBuffer = 0,
            ExtrasBuffer = 1,
            Uniform = 2
        };

        enum class FragmentInputIndex
        {
            ExtrasBuffer = 0
        };

        
        enum class TextureIndex
        {
            Texture = 0,
            Clut = 1,
            SegPal = 2
        };
        
        enum class BlitFragShader
        {
            Normal = 0,
            ClutNearest = 1,
            ClutInterpolated = 2,
            A8Source = 3,
            RGBXSource = 4
        };
        
        enum class DestFormat
        {
            BGRX8_linear,
            BGRA8_linear,
            BGRX8_sRGB,
            BGRA8_sRGB,
            A_8
        };
        
        
        enum Command
        {
            None,
            SetCanvas,
//            SetClip,
            SetBlendMode,
            SetMorphFactor,
            SetTintColor,
            SetTintGradient,
            ClearTintGradient,
            SetBlitSource,
            Fill,
            FillSubPixel,                // Includes start/direction lines.
            Plot,
            LineFromTo,
            Blit,
            Segments,

        };

        id<MTLRenderCommandEncoder> _setCanvas( MetalSurface * pCanvas, int width, int height, CanvasInit initOperation, Color clearColor );
        void            _setBlendMode( id<MTLRenderCommandEncoder>, BlendMode mode);
        void            _setMorphFactor( id<MTLRenderCommandEncoder>, float morphFactor);
        void            _setBlitSource( id<MTLRenderCommandEncoder>, MetalSurface * pSurf);
        void            _setTintColor( id<MTLRenderCommandEncoder>, HiColor color);
        void            _setTintGradient( id<MTLRenderCommandEncoder>, const RectI& rect, const Gradient& gradient);
        void            _clearTintGradient( id<MTLRenderCommandEncoder> renderEncoder );
        
        inline void    _beginDrawCommand(Command cmd);
        inline void    _beginDrawCommandWithSource(Command cmd);
        inline void    _beginDrawCommandWithInt(Command cmd, int data);
        inline void    _beginClippedDrawCommand(Command cmd );
        inline void    _beginStateCommand(Command cmd, int dataSize);
        inline void    _endCommand();


        typedef void(MetalGfxDevice::*CmdFinalizer_p)();

        void    _dummyFinalizer();
        void    _drawCmdFinalizer();
        
        void    _initTables();
        float    _scaleThickness(float thickeness, float slope);
        id<MTLRenderPipelineState> _compileRenderPipeline( NSString* label, NSString* vertexShader,
                                    NSString* fragmentShader, BlendMode blendMode, PixelFormat destFormat );
        
        void    _resizeBuffers();
        void    _executeBuffer();
        void    _resetBuffers();

        SurfaceFactory_p    m_pSurfaceFactory = nullptr;

        float    m_lineThicknessTable[17];

        float    m_sRGBtoLinearTable[256];
        float    m_linearToLinearTable[256];
        
        
        //
        
        Command         m_cmd;
        CmdFinalizer_p  m_pCmdFinalizer;
        int             m_cmdBeginVertexOfs;                        // Saved for CmdFinalizer

        int             m_nSegments;                                // Number of segments for current segment command.

        int             m_canvasYstart;

        bool            m_bFullyInitialized = false;
        CanvasInit      m_beginRenderOp = CanvasInit::Keep;
        
        struct Uniform
        {
            float      canvasDimX;
            float      canvasDimY;
            int        canvasYOfs;
            int        canvasYMul;

            simd_float4 flatTint;

            RectI    tintRect;

            simd_float4   topLeftTint;
            simd_float4   topRightTint;
            simd_float4   bottomRightTint;
            simd_float4   bottomLeftTint;

            SizeI    textureSize;
        };

        struct Vertex
        {
            CoordI          coord;
            int             extrasOfs;                        // Offset into extras buffer.
            simd_float2   uv;
        };
           
        // Buffers

        Uniform         m_uniform;

        int             m_commandBufferSize = 4096;
        int *           m_pCommandBuffer = nullptr;         // Queue of commands to execute when flushing buffer
        int             m_commandOfs = 0;                   // Write offset in m_commandBuffer
        
        id<MTLBuffer>   m_vertexBufferId = nil;
        int             m_vertexBufferSize = 4096;
        Vertex *        m_pVertexBuffer = nullptr;          // Pointer to content of vertex buffer
        int             m_vertexFlushPoint = 0;
        int             m_vertexOfs = 0;                    // Write offset in m_pVertexData

        id<MTLBuffer>   m_extrasBufferId = nil;
        int             m_extrasBufferSize = 4096;
        float *         m_pExtrasBuffer = nullptr;          // Pointer to content of extras buffer
        int             m_extrasOfs = 0;                    // Write offset in m_pExtrasData
        int             m_neededExtras = 0;                 // Set to non-zero to potentially force a bigger jump in extrasBufferSize.
        
        id<MTLBuffer>   m_clipListBufferId = nil;
        int             m_clipListBufferSize = 256;
        RectI *         m_pClipListBuffer = nullptr;
        int             m_clipWriteOfs = 0;                 // Write offset in m_clipListBuffer
        int             m_clipCurrOfs = -1;                 // Offset to where current clipList is written to in clipListBuffer, or -1 if not written.

        id<MTLBuffer>   m_segEdgeBufferId = nil;
        int             m_segEdgeBufferSize = 16384;
        float *         m_pSegEdgeBuffer = nullptr;
        int             m_segEdgeFlushPoint = 0;
        int             m_segEdgeOfs = 0;
        int             m_neededSegEdge = 0;
        
        const int       c_segPalEntrySize = 2*4*4*c_maxSegments;  // Bytes per palette (4 pixels of 4 uint16_t per segment).

        id<MTLTexture>  m_segPalTextureId = nil;
        id<MTLBuffer>   m_segPalBufferId = nil;
        int             m_segPalBufferSize = 64;            // Number of complete palettes, not number of uint16_t.
        uint16_t *      m_pSegPalBuffer = nullptr;
        int             m_segPalOfs = 0;
                
        
        // Active state data

        MetalSurface *  m_pActiveBlitSource = nullptr;                  // Currently active blit source during buffer execution, not to confuse with m_pBlitSource which might not be active yet.
        MetalSurface *  m_pActiveCanvas     = nullptr;                  // Currently active canvas during buffer execution, not to confuse with m_pCanvas which might not be active yet.
        SizeI           m_activeCanvasSize;
        DestFormat      m_activeCanvasFormat;
        bool            m_bGradientActive   = false;
        BlendMode       m_activeBlendMode   = BlendMode::Blend;
        float           m_activeMorphFactor = 0.5f;
                
        SizeI                       m_viewportSize;
        
        id<MTLLibrary>              m_library;
        id<MTLDrawable>             m_drawableToAutoPresent = nil;
        MTLRenderPassDescriptor*    m_baseCanvasRenderPassDesc;
        PixelFormat                 m_baseCanvasPixelFormat;
        
        id<MTLCommandBuffer>        m_metalCommandBuffer;

        bool                        m_bRendering = false;               // Set to true while between beginRender() and endRender() calls.
        
        std::atomic<int>            m_flushesInProgress;                // Number of buffer flushes to complete before metal is idle.
                
        id<MTLRenderPipelineState>  m_plotPipelines[BlendMode_size][5];          // [BlendMode][DestFormat]
        id<MTLRenderPipelineState>  m_lineFromToPipelines[BlendMode_size][5];    // [BlendMode][DestFormat]
        id<MTLRenderPipelineState>  m_fillPipelines[2][BlendMode_size][5];       // [bGradient][BlendMode][DestFormat]
        id<MTLRenderPipelineState>  m_fillAAPipelines[2][BlendMode_size][5];     // [bGradient][BlendMode][DestFormat]
        
        id<MTLRenderPipelineState>  m_blitPipelines[5][2][BlendMode_size][5];   // [BlitFragShader][bGradient][BlendMode][DestFormat]

        id<MTLRenderPipelineState>  m_segmentsPipelines[c_maxSegments][2][BlendMode_size][5];   // [nbEdges][bGradient][BlendMode][DestFormat]
        
        id<MTLSamplerState>         m_samplers[2][2][2];                        // [bMipMap][bInterpolate][bTile]
        
        static const char shaders[];

        static id<MTLDevice>        s_metalDevice;
        static id<MTLCommandQueue>  s_metalCommandQueue;



        MetalGfxDevice *            m_pPrevActiveDevice; // Storage for previous active device when we become active.
        static MetalGfxDevice *    s_pActiveDevice;    // Pointer at Metal device currently in rendering state.
    };

    //____ _beginDrawCommand() ________________________________________________

    inline void MetalGfxDevice::_beginDrawCommand(Command cmd )
    {
        if (m_commandOfs > m_commandBufferSize - 2 )
            _resizeBuffers();

        m_cmd = cmd;
        m_pCmdFinalizer = &MetalGfxDevice::_drawCmdFinalizer;
        m_cmdBeginVertexOfs = m_vertexOfs;
        m_pCommandBuffer[m_commandOfs++] = cmd;
        
        if (m_pCanvas)
            static_cast<MetalSurface*>(m_pCanvas.rawPtr())->m_bBufferNeedsSync = true;
    }

    //____ _beginDrawCommandWithSource() ________________________________________________

    inline void MetalGfxDevice::_beginDrawCommandWithSource(Command cmd)
    {
        if (m_commandOfs > m_commandBufferSize - 2)
            _resizeBuffers();

        m_cmd = cmd;
        m_pCmdFinalizer = &MetalGfxDevice::_drawCmdFinalizer;
        m_cmdBeginVertexOfs = m_vertexOfs;
        m_pCommandBuffer[m_commandOfs++] = cmd;

        if( m_pBlitSource )
            static_cast<MetalSurface*>(m_pBlitSource.rawPtr())->m_bPendingReads = true;

        if( m_pCanvas)
            static_cast<MetalSurface*>(m_pCanvas.rawPtr())->m_bBufferNeedsSync = true;
    }

    //____ _beginDrawCommandWithInt() ________________________________________________

    inline void MetalGfxDevice::_beginDrawCommandWithInt(Command cmd, int data)
    {
        if (m_commandOfs > m_commandBufferSize - 3)
            _resizeBuffers();

        m_cmd = cmd;
        m_pCmdFinalizer = &MetalGfxDevice::_drawCmdFinalizer;
        m_cmdBeginVertexOfs = m_vertexOfs;
        m_pCommandBuffer[m_commandOfs++] = cmd;
        m_pCommandBuffer[m_commandOfs++] = data;

        if (m_pCanvas)
            static_cast<MetalSurface*>(m_pCanvas.rawPtr())->m_bBufferNeedsSync = true;
    }

    //____ _beginClippedDrawCommand() ________________________________________________

    inline void MetalGfxDevice::_beginClippedDrawCommand(Command cmd)
    {
        if (m_commandOfs > m_commandBufferSize - 4 || (m_clipCurrOfs == -1 && m_clipWriteOfs > m_clipListBufferSize - m_nClipRects) )
            _resizeBuffers();

        if (m_clipCurrOfs == -1)
        {
            m_clipCurrOfs = m_clipWriteOfs;

            for (int i = 0; i < m_nClipRects; i++)
            {
                RectI clip = m_pClipRects[i];
                clip.y = m_canvasSize.h - (clip.y + clip.h);

                m_pClipListBuffer[m_clipWriteOfs++] = clip;
            }
        }

        m_cmd = cmd;
        m_pCmdFinalizer = &MetalGfxDevice::_drawCmdFinalizer;
        m_cmdBeginVertexOfs = m_vertexOfs;
        m_pCommandBuffer[m_commandOfs++] = cmd;
        m_pCommandBuffer[m_commandOfs++] = m_clipCurrOfs;
        m_pCommandBuffer[m_commandOfs++] = m_nClipRects;

        if (m_pCanvas)
            static_cast<MetalSurface*>(m_pCanvas.rawPtr())->m_bBufferNeedsSync = true;
    }


    //____ _beginStateCommand() ________________________________________________

    inline void MetalGfxDevice::_beginStateCommand(Command cmd, int dataSize)
    {
        if (m_commandOfs > m_commandBufferSize - dataSize - 1 )
            _resizeBuffers();

        m_cmd = cmd;
        m_pCmdFinalizer = &MetalGfxDevice::_dummyFinalizer;

        m_pCommandBuffer[m_commandOfs++] = cmd;
    }

    //____ _endCommand() ______________________________________________________

    inline void MetalGfxDevice::_endCommand()
    {
        (this->*m_pCmdFinalizer)();
        m_pCmdFinalizer = &MetalGfxDevice::_dummyFinalizer;
    }



} // namespace wg
#endif //WG_METALGFXDEVICE_DOT_H

