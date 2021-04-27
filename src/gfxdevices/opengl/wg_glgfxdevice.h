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
#ifndef WG_GLGFXDEVICE_DOT_H
#define WG_GLGFXDEVICE_DOT_H
#pragma once

#include <wg_geo.h>

#ifdef WIN32
#	include <GL/glew.h>
#else
#	ifdef __APPLE__
#		include <OpenGL/gl3.h>
#		define PATCH_LOGIC_PRO_X_VIEWPORT_BUG
#	else
#		include <GL/glew.h>
#	endif
#endif

#include <wg_gfxdevice.h>
#include <wg_glsurface.h>

namespace wg
{
	class GlSurface;
	class GlGfxDevice;
	typedef	StrongPtr<GlGfxDevice> GlGfxDevice_p;
	typedef	WeakPtr<GlGfxDevice>	GlGfxDevice_wp;

	class GlGfxDevice : public GfxDevice
	{
		friend class GlSurface;

	public:

		//.____ Creation __________________________________________

		static GlGfxDevice_p	create(int uboBindingPoint = 0);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		const TypeInfo&		surfaceType(void) const override;


		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory() override;

		//.____ State _________________________________________________

		void	setTintColor(HiColor color) override;
		void	setTintGradient(const RectI& rect, const Gradient& gradient) override;
		void	clearTintGradient() override;

		bool	setBlendMode(BlendMode blendMode) override;
		bool	setBlitSource(Surface * pSource) override;
		void	setMorphFactor(float factor) override;

		bool    isCanvasReady() const;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;
		bool    isIdle() override;
		void	flush() override;

		using 	GfxDevice::fill;
		void	fill(const RectI& rect, HiColor col) override;
		void	fill(const RectF& rect, HiColor col) override;

		void    plotPixels(int nCoords, const CoordI * pCoords, const HiColor * pColors) override;

		void	drawLine(CoordI begin, CoordI end, HiColor color, float thickness) override;
		void	drawLine(CoordI begin, Direction dir, int length, HiColor col, float thickness) override;



	protected:
		GlGfxDevice(int uboBindingPoint);
		~GlGfxDevice();

		void	_canvasWasChanged() override;
		void	_renderLayerWasChanged() override;
		void	_clipListWasChanged() override;

		void	_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2]) override;
		void	_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2]) override;

		void	_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2]) override;


				enum class Command
				{
					None,
					SetCanvas,
		//			SetClip,
					SetBlendMode,
					SetMorphFactor,
					SetTintColor,
					SetTintGradient,
					ClearTintGradient,
					SetBlitSource,
					Fill,
					FillSubPixel,				// Includes start/direction lines.
					Plot,
					LineFromTo,
					Blit,
					Segments,

				};

				void	_setCanvas( GlSurface * pCanvas, int width, int height );
				void	_setBlendMode(BlendMode mode);
				void	_setMorphFactor(float morphFactor);
				void	_setBlitSource(GlSurface * pSurf);

				inline void	_beginDrawCommand(Command cmd);
				inline void	_beginDrawCommandWithSource(Command cmd);
				inline void	_beginDrawCommandWithInt(Command cmd, int data);
				inline void	_beginClippedDrawCommand(Command cmd );
				inline void	_beginStateCommand(Command cmd, int dataSize);
				inline void	_endCommand();



				typedef void(GlGfxDevice::*CmdFinalizer_p)();

				void	_dummyFinalizer();
				void	_drawCmdFinalizer();



		GLuint  _createGLProgram(const char * pVertexShader, const char * pFragmentShader);
		void	_setDrawUniforms(GLuint progId, int uboBindingPoint);
		void	_setBlitUniforms(GLuint progId, int uboBindingPoint);
		void	_setClutBlitUniforms(GLuint progId, int uboBindingPoint);

		void	_initTables();
		float	_scaleThickness(float thickeness, float slope);

		void	_executeBuffer();

		void	_writeCanvasInfo();
		void	_writeTintInfo();

		SurfaceFactory_p	m_pSurfaceFactory = nullptr;

		float	m_lineThicknessTable[17];

		float	m_sRGBtoLinearTable[256];
		float	m_linearToLinearTable[256];

		//


		static const int c_commandBufferSize = 512*10;
		static const int c_vertexBufferSize = 16384*10;				// Size of vertex buffer, in number of vertices.
		static const int c_extrasBufferSize = 65536*4*10;				// Size of extras buffer, in GLfloats.
		static const int c_surfaceBufferSize = 1024*10;				// Size of Surface_p buffer, used by SetBlitSource and SetCanvas commands.
		static const int c_clipListBufferSize = 4096*10;				// Size of clip rect buffer, containing clipLists needed for execution of certain commands in command buffer.
		static const int c_segmentsTintTexMapSize = 64*10;				// Number of segments tint palettes that fit into segmentsTintTexMap.

		Command			m_cmd;
		CmdFinalizer_p	m_pCmdFinalizer;
		int				m_cmdBeginVertexOfs;						// Saved for CmdFinalizer

		GLuint			m_framebufferId;
		int				m_nSegments;								// Number of segments for current segment command.

		int				m_canvasYstart;
		int				m_canvasYmul;

		GLsync          m_idleSync = 0;

		bool			m_bFullyInitialized = false;
		
		CanvasInit		m_beginRenderOp = CanvasInit::Keep;

		CoordF			m_blitSourceSize;


		// Device programs

		GLuint  m_fillProg[2];									// [RGB/A_8 dest]
		GLuint  m_fillGradientProg[2];							// [RGB/A_8 dest]

		GLuint  m_aaFillProg[2];								// [RGB/A_8 dest]
		GLuint  m_aaFillGradientProg[2];						// [RGB/A_8 dest]

		GLuint  m_blitProg[2];									// [RGB/A_8 dest]
		GLuint  m_blitGradientProg[2];							// [RGB/A_8 dest]

		GLuint  m_alphaBlitProg[2];								// [RGB/A_8 dest]
		GLuint  m_alphaBlitGradientProg[2];						// [RGB/A_8 dest]

		GLuint  m_clutBlitNearestProg[2];						// [RGB/A_8 dest]
		GLuint  m_clutBlitNearestGradientProg[2];				// [RGB/A_8 dest]

		GLuint  m_clutBlitInterpolateProg[2];					// [RGB/A_8 dest]
		GLuint  m_clutBlitInterpolateGradientProg[2];			// [RGB/A_8 dest]

		GLuint  m_plotProg[2];									// [RGB/A_8 dest]
		GLuint  m_lineFromToProg[2];							// [RGB/A_8 dest]

		GLuint	m_segmentsProg[c_maxSegments][2][2];			// [nb segments][base tint gradient][RGB/A_8 dest]

		GLuint	m_blitProgMatrix[PixelFormat_size][2][2][2];	// [source format][interpolation][tintgradient][RGB/A_8 dest]

		//

		struct CanvasInfo
		{
			GLfloat	canvasDimX;			// Becomes X in shader
			GLfloat	canvasDimY;			// Becomes Y in shader
			float	canvasYOfs;			// Becomes Z in shader
			float	canvasYMul;			// Becomes W in shader
		};


		struct GradientTintInfo
		{
			GLfloat flatTint[4];		// This needs to be placed first, so first part is identical to FlatTintInfo

			RectF	tintRect;

			GLfloat	topLeftTint[4];
			GLfloat	topRightTint[4];
			GLfloat	bottomRightTint[4];
			GLfloat	bottomLeftTint[4];
		};

		struct FlatTintInfo
		{
			GLfloat flatTint[4];
		};


		//

		struct Vertex
		{
			CoordI	coord;
			CoordF	uv;								// Actually contains blitSourceSize in most cases.
			int		extrasOfs;						// Offset into extras buffer for various data.
			int		canvasInfoOfs;					// Offset into extras buffer for canvas info.
			int		tintInfoOfs;					// Offset into extras buffer for tint info.
		};


		//

		CanvasInfo			m_canvasInfo;
		GradientTintInfo	m_tintInfo;
		

		// Buffers

		int		m_vertexOfs;						// Write offset in m_vertexBufferData
		int		m_extrasOfs;						// Write offset in m_extrasBufferData
		int		m_commandOfs;						// Write offset in m_commandBuffer
		int		m_surfaceOfs;						// Write offset in m_surfaceBuffer
		int		m_segmentsTintTexOfs;				// Write offset in m_segmentsTintTexMap
		int		m_clipWriteOfs;						// Write offset in m_clipListBuffer
		int		m_clipCurrOfs;						// Offset to where current clipList is written to in clipListBuffer, or -1 if not written.

		int		m_canvasInfoOfs;					// Offset in m_extrasBufferData where current CanvasInfo is written.
		int		m_tintInfoOfs;						// Offset in m_extrasBufferData where current TintInfo is written.


		GLuint  m_vertexArrayId;
		GLuint  m_vertexBufferId;
		Vertex	m_vertexBufferData[c_vertexBufferSize];								// Space to store coordinates

		GLuint	m_extrasBufferTex;
		GLuint	m_extrasBufferId;
		GLfloat m_extrasBufferData[c_extrasBufferSize];								// Space to store additional primitive data for shaders

		GLuint 		m_segmentsTintTexId;													// GL texture handle.
		uint16_t	m_segmentsTintTexMap[c_segmentsTintTexMapSize][c_maxSegments * 4 * 4];	// Horizontally aligned blocks of 2x2 pixels each, one for each segment color.

		int		m_commandBuffer[c_commandBufferSize];								// Queue of commands to execute when flushing buffer

		GlSurface_p m_surfaceBuffer[c_surfaceBufferSize];

		RectI	m_clipListBuffer[c_clipListBufferSize];

		// Active state data

		GlSurface_p m_pActiveBlitSource = nullptr;									// Currently active blit source in OpenGL, not to confuse with m_pBlitSource which might not be active yet.
		GlSurface_p m_pActiveCanvas = nullptr;                                      // Currently active canvas in OpenGL, not to confuse with m_pCanvas which might not be active yet.
        SizeI       m_activeCanvasSize;                                             // Currently active canvas size in OpenGL, " -
		bool        m_bMipmappedActiveCanvas = false;                               // Set if currently active canvas is a surface that is mipmapped.
		bool		m_bGradientActive = false;										
		BlendMode	m_activeBlendMode = BlendMode::Blend;
		bool		m_bActiveCanvasIsA8 = false;

		// GL states saved between BeginRender() and EndRender().

		GLboolean	m_glDepthTest;
		GLboolean   m_glScissorTest;
		GLboolean	m_glBlendEnabled;
		GLboolean	m_glSRGBEnabled;
		GLint		m_glBlendSrc;
		GLint		m_glBlendDst;
		GLint		m_glViewport[4];
		GLint		m_glScissorBox[4];
		GLint		m_glReadFrameBuffer;
		GLint		m_glDrawFrameBuffer;

		//

		static int s_bindingPointCanvasUBO;

		GlGfxDevice *			m_pPrevActiveDevice; // Storage for previous active device when we become active.

		static GlGfxDevice *	s_pActiveDevice;	// Pointer at GL device currently in rendering state.

		//

		static const char fillVertexShader[];
		static const char fillFragmentShader[];
		static const char fillFragmentShader_A8[];
		static const char blitVertexShader[];
		static const char blitFragmentShader[];
		static const char blitFragmentShader_A8[];
		static const char plotVertexShader[];
		static const char plotFragmentShader[];
		static const char plotFragmentShader_A8[];
		static const char lineFromToVertexShader[];
		static const char lineFromToFragmentShader[];
		static const char lineFromToFragmentShader_A8[];
		static const char aaFillVertexShader[];
		static const char aaFillFragmentShader[];
		static const char aaFillFragmentShader_A8[];

		static const char segmentsVertexShader[];
		static const char segmentsVertexShaderGradient[];
		static const char segmentsFragmentShader[];
		static const char segmentsFragmentShader_A8[];

		static const char alphaBlitFragmentShader[];
		static const char alphaBlitFragmentShader_A8[];

		static const char clutBlitNearestVertexShader[];
		static const char clutBlitNearestFragmentShader[];
		static const char clutBlitNearestFragmentShader_A8[];
		static const char clutBlitInterpolateVertexShader[];
		static const char clutBlitInterpolateFragmentShader[];
		static const char clutBlitInterpolateFragmentShader_A8[];

		static const char fillGradientVertexShader[];
		static const char aaFillGradientVertexShader[];
		static const char blitGradientVertexShader[];
		static const char clutBlitNearestGradientVertexShader[];
		static const char clutBlitInterpolateGradientVertexShader[];


		//

		static void onGlError(GLenum errorCode, const Object * pObject, const TypeInfo& pClassType, const char * func, const char * file, int line);

	};

	//____ _beginDrawCommand() ________________________________________________

	inline void GlGfxDevice::_beginDrawCommand(Command cmd )
	{
		if (m_commandOfs > c_commandBufferSize - 2 )
			_executeBuffer();

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_drawCmdFinalizer;
		m_cmdBeginVertexOfs = m_vertexOfs;
		m_commandBuffer[m_commandOfs++] = (int) cmd;

		if (m_pCanvas)
			static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
	}

	//____ _beginDrawCommandWithSource() ________________________________________________

inline void GlGfxDevice::_beginDrawCommandWithSource(Command cmd)
	{
		if (m_commandOfs > c_commandBufferSize - 2)
			_executeBuffer();

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_drawCmdFinalizer;
		m_cmdBeginVertexOfs = m_vertexOfs;
		m_commandBuffer[m_commandOfs++] = (int) cmd;

		if( m_pBlitSource )
			static_cast<GlSurface*>(m_pBlitSource.rawPtr())->m_bPendingReads = true;

		if( m_pCanvas)
			static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
	}

	//____ _beginDrawCommandWithInt() ________________________________________________

	inline void GlGfxDevice::_beginDrawCommandWithInt(Command cmd, int data)
	{
		if (m_commandOfs > c_commandBufferSize - 3)
			_executeBuffer();

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_drawCmdFinalizer;
		m_cmdBeginVertexOfs = m_vertexOfs;
		m_commandBuffer[m_commandOfs++] = (int) cmd;
		m_commandBuffer[m_commandOfs++] = data;

		if (m_pCanvas)
			static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
	}

	//____ _beginClippedDrawCommand() ________________________________________________

	inline void GlGfxDevice::_beginClippedDrawCommand(Command cmd)
	{
		if (m_commandOfs > c_commandBufferSize - 4 || (m_clipCurrOfs == -1 && m_clipWriteOfs > c_clipListBufferSize - m_nClipRects) )
			_executeBuffer();

		if (m_clipCurrOfs == -1)
		{
			m_clipCurrOfs = m_clipWriteOfs;

			for (int i = 0; i < m_nClipRects; i++)
			{
				RectI clip = m_pClipRects[i];
				if( m_canvasYstart != 0 )
					clip.y = m_canvasSize.h - (clip.y + clip.h);

				m_clipListBuffer[m_clipWriteOfs++] = clip;
			}
		}

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_drawCmdFinalizer;
		m_cmdBeginVertexOfs = m_vertexOfs;
		m_commandBuffer[m_commandOfs++] = (int) cmd;
		m_commandBuffer[m_commandOfs++] = m_clipCurrOfs;
		m_commandBuffer[m_commandOfs++] = m_nClipRects;

		if (m_pCanvas)
			static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
	}


	//____ _beginStateCommand() ________________________________________________

	inline void GlGfxDevice::_beginStateCommand(Command cmd, int dataSize)
	{
		if (m_commandOfs > c_commandBufferSize - dataSize - 1 || m_surfaceOfs == c_surfaceBufferSize )
			_executeBuffer();

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_dummyFinalizer;

		m_commandBuffer[m_commandOfs++] = (int) cmd;
	}


	//____ _endCommand() ______________________________________________________

	inline void GlGfxDevice::_endCommand()
	{
		(this->*m_pCmdFinalizer)();
		m_pCmdFinalizer = &GlGfxDevice::_dummyFinalizer;
		m_cmd = Command::None;
	}


} // namespace wg
#endif //WG_GLGFXDEVICE_DOT_H

