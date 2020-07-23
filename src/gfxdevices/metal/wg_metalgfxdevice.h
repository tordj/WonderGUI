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
	class Color;

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
        
        static MetalGfxDevice_p	create( SizeI size, MTLRenderPassDescriptor* passDesc );

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		const TypeInfo&			surfaceType( void ) const override;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory() override;

		//.____ Geometry _________________________________________________

		bool	setCanvas(Surface * pCanvas, bool bResetClipList = true ) override;

        //.____ State _________________________________________________

        bool    setClipList(int nRectangles, const RectI * pRectangles) override;
        void    clearClipList() override;
        void    setTintColor(Color color) override;
        void    setTintGradient(const RectI& rect, Color topLeft, Color topRight, Color bottomRight, Color bottomLeft) override;
        void    clearTintGradient() override;

        bool    setBlendMode(BlendMode blendMode) override;
        bool    setBlitSource(Surface * pSource) override;
        void    setMorphFactor(float factor) override;

        bool    isCanvasReady() const;
        
        void    setRenderPassDescriptor( MTLRenderPassDescriptor* passDesc );
        void    setDrawableToAutopresent( id<MTLDrawable> drawable );
        
        //.____ Rendering ________________________________________________

        bool    beginRender() override;
        bool    endRender() override;
        bool    isIdle() override;
        void    flush() override;

        
		void	fill(const RectI& rect, const Color& col) override;
		void	fill(const RectF& rect, const Color& col) override;

		void    plotPixels(int nCoords, const CoordI * pCoords, const Color * pColors) override;

		void	drawLine(CoordI begin, CoordI end, Color color, float thickness) override;


	protected:
		MetalGfxDevice( SizeI size, MTLRenderPassDescriptor* passDesc );
		~MetalGfxDevice();

		void	_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2]) override;
		void	_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2]) override;

		void	_transformDrawSegments(const RectI& dest, int nSegments, const Color * pSegmentColors, int nEdges, const int * pEdges, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2]) override;

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

        void            _setCanvas( MetalSurface * pCanvas, int width, int height );
        void            _setBlendMode(BlendMode mode);
        void            _setMorphFactor(float morphFactor);
        void            _setBlitSource(MetalSurface * pSurf);
        void            _setTintColor(Color color);
        void            _setTintGradient(const RectI& rect, const Color colors[4]);
        void            _clearTintGradient();

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
        
        void    _resizeBuffers();
        void    _executeBuffer();

        SurfaceFactory_p    m_pSurfaceFactory = nullptr;

        float    m_lineThicknessTable[17];

        float    m_sRGBtoLinearTable[256];
        float    m_linearToLinearTable[256];
        
        
        //
/*
        static const int c_commandBufferSize = 4096;
        static const int c_vertexBufferSize = 16384;                // Size of vertex buffer, in number of vertices.
        static const int c_extrasBufferSize = 65536*4;                // Size of extras buffer, in GLfloats.
        static const int c_surfaceBufferSize = 1024;                // Size of Surface_p buffer, used by SetBlitSource and SetCanvas commands.
        static const int c_clipListBufferSize = 4096;                // Size of clip rect buffer, containing clipLists needed for execution of certain commands in command buffer.
*/
        
        
        Command         m_cmd;
        CmdFinalizer_p  m_pCmdFinalizer;
        int             m_cmdBeginVertexOfs;                        // Saved for CmdFinalizer

//        GLuint            m_framebufferId;
//        int             m_nSegments;                                // Number of segments for current segment command.

        int             m_canvasYstart;
        int             m_canvasYmul;

        SizeI           m_emptyCanvasSize;

//        GLsync          m_idleSync = 0;

        bool            m_bFullyInitialized = false;

        struct UniformBuffer            // Uniform buffer object for canvas information.
        {                               // DO NOT CHANGE ORDER OF MEMBERS!!!
            float      canvasDimX;
            float      canvasDimY;
            int        canvasYOfs;
            int        canvasYMul;

            vector_float4 flatTint;

            RectI    tintRect;

            vector_float4   topLeftTint;
            vector_float4   topRightTint;
            vector_float4   bottomRightTint;
            vector_float4   bottomLeftTint;

            SizeI    textureSize;
        };

        struct Vertex
        {
            CoordI          coord;
            int             extrasOfs;                        // Offset into extras buffer.
            vector_float2   uv;
        };
           
        // Buffers

        id<MTLBuffer>   m_uniformBufferId = nil;
        UniformBuffer * m_pUniformBuffer = nullptr;
        
        int             m_commandBufferSize = 4096;
        int *           m_pCommandBuffer = nullptr;           // Queue of commands to execute when flushing buffer
        int             m_commandOfs = 0;                   // Write offset in m_commandBuffer
        
        id<MTLBuffer>   m_vertexBufferId = nil;
        int             m_vertexBufferSize = 4096;
        Vertex *        m_pVertexBuffer = nullptr;            // Pointer to content of vertex buffer
        int             m_vertexOfs = 0;                    // Write offset in m_pVertexData

        id<MTLBuffer>   m_extrasBufferId = nil;
        int             m_extrasBufferSize = 16384;
        float *         m_pExtrasBuffer = nullptr;            // Pointer to content of extras buffer
        int             m_extrasOfs = 0;                    // Write offset in m_pExtrasData
        int             m_neededExtrasBufferSize = 0;       // Set to non-zero to potentially force a bigger jump in extrasBufferSize.

//        MetalSurface_p  m_surfaceBuffer[c_surfaceBufferSize];
        int             m_surfaceBufferSize = 1024;
        int             m_surfaceOfs;                       // Write offset in m_surfaceBuffer

        id<MTLBuffer>   m_clipListBufferId = nil;
        int             m_clipListBufferSize = 1024;
        RectI *         m_pClipListBuffer = nullptr;
        int             m_clipWriteOfs = 0;                 // Write offset in m_clipListBuffer
        int             m_clipCurrOfs = -1;                 // Offset to where current clipList is written to in clipListBuffer, or -1 if not written.

//        GLuint         m_segmentsTintTexId;                                                    // GL texture handle.
//        uint16_t    m_segmentsTintTexMap[c_segmentsTintTexMapSize][c_maxSegments * 4 * 4];    // Horizontally aligned blocks of 2x2 pixels each, one for each segment color.
        int             m_segmentsTintTexOfs;               // Write offset in m_segmentsTintTexMap

        
        // Active state data

        MetalSurface *  m_pActiveBlitSource = nullptr;                  // Currently active blit source in OpenGL, not to confuse with m_pBlitSource which might not be active yet.
        MetalSurface *  m_pActiveCanvas     = nullptr;                      // Currently active canvas in OpenGL, not to confuse with m_pCanvas which might not be active yet.
        bool            m_bMipmappedActiveCanvas = false;               // Set if currently active canvas is a surface that is mipmapped.
        bool            m_bGradientActive   = false;
        BlendMode       m_activeBlendMode   = BlendMode::Blend;
        bool            m_bActiveCanvasIsA8 = false;
        
        
        SizeI                       m_viewportSize;
        
        id<MTLLibrary>              m_library;
        id<MTLDrawable>             m_drawableToAutoPresent = nil;
        MTLRenderPassDescriptor*    m_renderPassDesc;
        
        id<MTLCommandBuffer>        m_metalCommandBuffer;
        id<MTLRenderCommandEncoder> m_renderEncoder;
        bool                        m_bRendering = false;
        
        id<MTLRenderPipelineState>  m_cmdFill;

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

//        if (m_pCanvas)
//            static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
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

//        static_cast<GlSurface*>(m_pBlitSource.rawPtr())->m_bPendingReads = true;

//        if( m_pCanvas)
//            static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
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

//        if (m_pCanvas)
//            static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
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
                if( m_canvasYstart != 0 )
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

//        if (m_pCanvas)
//            static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
    }


    //____ _beginStateCommand() ________________________________________________

    inline void MetalGfxDevice::_beginStateCommand(Command cmd, int dataSize)
    {
        if (m_commandOfs > m_commandBufferSize - dataSize - 1 || m_surfaceOfs == m_surfaceBufferSize )
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

