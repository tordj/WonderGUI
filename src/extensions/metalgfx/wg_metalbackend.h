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

#ifndef WG_METALBACKEND_DOT_H
#define WG_METALBACKEND_DOT_H
#pragma once

#import "simd/simd.h"
#import "MetalKit/MetalKit.h"

#include <wg_gfxtypes.h>
#include <wg_gfxbackend.h>
#include <wg_metalsurface.h>


namespace wg
{
	class Surface;

	class MetalBackend;
	typedef	StrongPtr<MetalBackend>	MetalBackend_p;
	typedef	WeakPtr<MetalBackend>	MetalBackend_wp;

	class MetalBackend : public GfxBackend
	{
		friend class MetalSurface;
	public:
		//.____ Creation ________________________________________________

		static void setMetalDevice( id<MTLDevice> device );
		static id<MTLDevice> metalDevice() { return s_metalDevice; }

		static MetalBackend_p	create();

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ________________________________________________

		void	beginRender() override;
		void	endRender() override;

		void	beginSession( CanvasRef canvasRef, Surface * pCanvas, int nUpdateRects, const RectSPX * pUpdateRects, const SessionInfo * pInfo = nullptr ) override;
		void	endSession() override;

		void	setCanvas(Surface* pSurface) override;
		void	setCanvas(CanvasRef ref) override;

		void	setObjects(Object* const * pBeg, Object* const * pEnd) override;

		void	setRects(const RectSPX* pBeg, const RectSPX* pEnd) override;
		void	setColors(const HiColor* pBeg, const HiColor* pEnd) override;
		void	setTransforms(const Transform* pBeg, const Transform* pEnd) override;

		void	processCommands(const uint16_t* pBeg, const uint16_t* pEnd) override;


		//.____ Misc _________________________________________________________

		bool    setDefaultCanvas( MTLRenderPassDescriptor* renderPassDesc, SizeI pixelSize, PixelFormat pixelFormat, int scale = 64 );

		const CanvasInfo* canvasInfo(CanvasRef ref) const override;

		SurfaceFactory_p	surfaceFactory() override;
		EdgemapFactory_p	edgemapFactory() override;

		int		maxEdges() const override;

		const TypeInfo& surfaceType(void) const override;

	protected:
		MetalBackend();
		~MetalBackend();

		const static int	c_maxSegments = 16;

		enum class VertexInputIndex
		{
			VertexBuffer = 0,
			ExtrasBuffer = 1,
			Uniform = 2
		};

		enum class FragmentInputIndex
		{
			ExtrasBuffer = 0,
			BlurUniform = 1
		};


		enum class TextureIndex
		{
			Texture = 0,
			Palette = 1,
			SegPal = 2
		};

		enum class BlitFragShader
		{
			Normal = 0,
			PaletteNearest = 1,
			PaletteInterpolated = 2,
			A8Source = 3,
			RGBXSource = 4
		};

		enum class DestFormat
		{
			BGRX8_linear,
			BGRA8_linear,
			BGRX8_sRGB,
			BGRA8_sRGB,
			Alpha_8
		};
/*
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
			SetFixedBlendColor,
			SetBlurMatrices,
			SetBlitSource,
			Fill,
			FillSubPixel,                // Includes start/direction lines.
			Plot,
			LineFromTo,
			Blit,
			Blur,
			Segments,

		};
*/
		id<MTLRenderCommandEncoder> _setCanvas( MetalSurface * pCanvas, int width, int height, CanvasInit initOperation, Color clearColor );
		void            _setBlendMode( id<MTLRenderCommandEncoder>, BlendMode mode);
		void            _setMorphFactor( id<MTLRenderCommandEncoder>, float morphFactor);
		void            _setFixedBlendColor( id<MTLRenderCommandEncoder>, HiColor color);
		void 			_setBlurMatrices( id<MTLRenderCommandEncoder> renderEncoder, spx radius, const float red[9], const float green[9], const float blue[9] );
		void            _setBlitSource( id<MTLRenderCommandEncoder>, MetalSurface * pSurf);
		void            _setTintColor( id<MTLRenderCommandEncoder>, HiColor color);
		void            _setTintGradient( id<MTLRenderCommandEncoder>, const RectI& rect, const Gradient& gradient);
		void            _clearTintGradient( id<MTLRenderCommandEncoder> renderEncoder );

		void    _initTables();
		float    _scaleThickness(float thickeness, float slope);
		id<MTLRenderPipelineState> _compileRenderPipeline( NSString* label, NSString* vertexShader,
									NSString* fragmentShader, BlendMode blendMode, PixelFormat destFormat );

		void    _resizeBuffers();
		void    _executeBuffer();
		void    _resetBuffers();

		SurfaceFactory_p    m_pSurfaceFactory = nullptr;
		EdgemapFactory_p   m_pEdgemapFactory = nullptr;

		Object* const * m_pObjectsBeg = nullptr;
		Object* const * m_pObjectsEnd = nullptr;
		Object* const * m_pObjectsPtr = nullptr;

		const RectSPX* m_pRectsBeg = nullptr;
		const RectSPX* m_pRectsEnd = nullptr;
		const RectSPX* m_pRectsPtr = nullptr;

		const HiColor* m_pColorsBeg = nullptr;
		const HiColor* m_pColorsEnd = nullptr;
		const HiColor* m_pColorsPtr = nullptr;

		const Transform* m_pTransformsBeg = nullptr;
		const Transform* m_pTransformsEnd = nullptr;

		float    m_lineThicknessTable[17];

		float    m_sRGBtoLinearTable[256];
		float    m_linearToLinearTable[256];


		//

		Command         m_cmd;

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

		struct BlurUniform
		{
			simd_float4   	colorMtx[9];
			simd_float2		offset[9];
		};

		struct Vertex
		{
			CoordI          coord;
			int             extrasOfs;                        // Offset into extras buffer.
			simd_float2   uv;
		};

		// Buffers

		Uniform         m_uniform;
		BlurUniform		m_blurUniform;

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

		MetalSurface_p  m_pActiveBlitSource = nullptr;                  // Currently active blit source during buffer execution, not to confuse with m_pBlitSource which might not be active yet.
		MetalSurface_p  m_pActiveCanvas     = nullptr;                  // Currently active canvas during buffer execution, not to confuse with m_pCanvas which might not be active yet.
		SizeI           m_activeCanvasSize;
		DestFormat      m_activeCanvasFormat;
		bool            m_bGradientActive   = false;
		BlendMode       m_activeBlendMode   = BlendMode::Blend;
		float           m_activeMorphFactor = 0.5f;
		HiColor         m_activeFixedBlendColor = HiColor::White;
		spx				m_activeBlurRadius = 64;

		float           m_morphFactorInUse = -1;
		HiColor         m_fixedBlendColorInUse = HiColor::Undefined;




		id<MTLLibrary>              m_library = nil;
		id<MTLDrawable>             m_drawableToAutoPresent = nil;
		MTLRenderPassDescriptor*    m_defaultCanvasRenderPassDesc = nil;
		PixelFormat                 m_defaultCanvasPixelFormat;
		CanvasInfo                  m_defaultCanvas;

		id<MTLCommandBuffer>        m_metalCommandBuffer = nil;

		bool                        m_bRendering = false;               // Set to true while between beginRender() and endRender() calls.

		std::atomic<int>            m_flushesInProgress;                // Number of buffer flushes to complete before metal is idle.

		id<MTLRenderPipelineState>  m_plotPipelines[BlendMode_size][5];          // [BlendMode][DestFormat]
		id<MTLRenderPipelineState>  m_lineFromToPipelines[BlendMode_size][5];    // [BlendMode][DestFormat]
		id<MTLRenderPipelineState>  m_fillPipelines[2][BlendMode_size][5];       // [bGradient][BlendMode][DestFormat]
		id<MTLRenderPipelineState>  m_fillAAPipelines[2][BlendMode_size][5];     // [bGradient][BlendMode][DestFormat]

		id<MTLRenderPipelineState>  m_blitPipelines[5][2][BlendMode_size][5];   // [BlitFragShader][bGradient][BlendMode][DestFormat]

		id<MTLRenderPipelineState>  m_blurPipelines[2][BlendMode_size][5];   	// [bGradient][BlendMode][DestFormat]

		id<MTLRenderPipelineState>  m_segmentsPipelines[c_maxSegments][2][BlendMode_size][5];   // [nbEdges][bGradient][BlendMode][DestFormat]

		id<MTLSamplerState>         m_samplers[2][2][2];                        // [bMipMap][bInterpolate][bTile]

		static const char shaders[];

		static id<MTLDevice>        s_metalDevice;
		static id<MTLCommandQueue>  s_metalCommandQueue;

	};



}

// namespace wg
#endif //WG_METALBACKEND_DOT_H


