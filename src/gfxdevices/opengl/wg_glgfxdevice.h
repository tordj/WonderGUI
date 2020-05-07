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

		static GlGfxDevice_p	create(const RectI& viewport, int uboBindingPoint = 0);
		static GlGfxDevice_p	create(GlSurface * pCanvas, int uboBindingPoint = 0);

		//.____ Identification __________________________________________

		const TypeInfo&		typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		const TypeInfo&		surfaceType(void) const override;


		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory() override;

		//.____ Geometry _________________________________________________

		bool	setCanvas(SizeI canvasSize, bool bResetClipRects = true );
		bool	setCanvas(Surface * pCanvas, bool bResetClipRects = true ) override;

		//.____ State _________________________________________________

		bool	setClipList(int nRectangles, const RectI * pRectangles) override;
		void	clearClipList() override;
		void	setTintColor(Color color) override;
		bool	setBlendMode(BlendMode blendMode) override;
		bool	setBlitSource(Surface * pSource) override;

		bool    isCanvasReady() const;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;
		bool    isIdle() override;
		void	flush();

		void	fill(const RectI& rect, const Color& col) override;
		void	fill(const RectF& rect, const Color& col) override;

		void    plotPixels(int nCoords, const CoordI * pCoords, const Color * pColors) override;

		void	drawLine(CoordI begin, CoordI end, Color color, float thickness) override;
		void	drawLine(CoordI begin, Direction dir, int length, Color col, float thickness) override;



	protected:
		GlGfxDevice(SizeI viewportSize, int uboBindingPoint);
		GlGfxDevice(const RectI& viewport, int uboBindingPoint);
		GlGfxDevice(GlSurface * pCanvas, int uboBindingPoint);
		~GlGfxDevice();

		void	_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2]) override;
		void	_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2]) override;

		void	_transformDrawSegments(const RectI& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, const int simpleTransform[2][2]) override;


		enum Command
		{
			None,
			SetCanvas,
//			SetClip,
			SetBlendMode,
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

		void	_initTables();
		float	_scaleThickness(float thickeness, float slope);

		void	_executeBuffer();

		SurfaceFactory_p	m_pSurfaceFactory = nullptr;

		float	m_lineThicknessTable[17];

		//


		static const int c_commandBufferSize = 256;
		static const int c_vertexBufferSize = 16384;				// Size of vertex buffer, in number of vertices.
		static const int c_extrasBufferSize = 65536*4;				// Size of extras buffer, in GLfloats.
		static const int c_surfaceBufferSize = 1024;				// Size of Surface_p buffer, used by SetBlitSource and SetCanvas commands.
		static const int c_clipListBufferSize = 4096;				// Size of clip rect buffer, containing clipLists needed for execution of certain commands in command buffer.

		Command			m_cmd;
		CmdFinalizer_p	m_pCmdFinalizer;
		int				m_cmdBeginVertexOfs;						// Saved for CmdFinalizer
		int				m_cmdBlitProgram;							// Blit program to be used by CmdFinalizer. Determined by surface type.

		GLuint			m_framebufferId;
		int				m_nSegments;								// Number of segments for current segment command.

		int				m_canvasYstart;
		int				m_canvasYmul;

		SizeI           m_emptyCanvasSize;

		GLsync          m_idleSync = 0;

		bool			m_bFullyInitialized = false;

		// Device programs

		GLuint  m_fillProg;
		GLuint  m_aaFillProg;
		GLuint  m_blitProg;
		GLint	m_blitProgTexSizeLoc;

		GLuint  m_alphaBlitProg;
		GLint	m_alphaBlitProgTexSizeLoc;

		GLuint  m_clutBlitNearestProg;
		GLint	m_clutBlitNearestProgTexSizeLoc;

		GLuint  m_clutBlitInterpolateProg;
		GLint	m_clutBlitInterpolateProgTexSizeLoc;

		GLuint  m_plotProg;
		GLuint  m_lineFromToProg;

		GLuint	m_segmentsProg[c_maxSegments];


		//

		struct canvasUBO			// Uniform buffer object for canvas information.
		{
			GLfloat	dimX;
			GLfloat	dimY;
			int		yOfs;
			int		yMul;
		};

		GLuint	m_canvasUBOId;
		canvasUBO	m_canvasUBOBuffer;

		//

		struct tintUBO
		{
			CoordI	coord;
			GLfloat	startB1;
			GLfloat	startG1;
			GLfloat	startR1;
			GLfloat	startA1;

			GLfloat	startB2;
			GLfloat	startG2;
			GLfloat	startR2;
			GLfloat	startA2;

			GLfloat	incB1;
			GLfloat	incG1;
			GLfloat	incR1;
			GLfloat	incA1;

			GLfloat	incB2;
			GLfloat	incG2;
			GLfloat	incR2;
			GLfloat	incA2;


		};


		//

		struct Vertex
		{
			CoordI	coord;
			Color	color;
			int		extrasOfs;						// Offset into extras buffer.
			CoordF	uv;
		};


		// Buffers

		int		m_vertexOfs;						// Write offset in m_vertexBufferData
		int		m_extrasOfs;						// Write offset in m_extrasBufferData
		int		m_commandOfs;						// Write offset in m_commandBuffer
		int		m_surfaceOfs;						// Write offset in m_surfaceBuffer
		int		m_clipWriteOfs;						// Write offset in m_clipListBuffer
		int		m_clipCurrOfs;						// Offset to where current clipList is written to in clipListBuffer, or -1 if not written.

		GLuint  m_vertexArrayId;
		GLuint  m_vertexBufferId;
		Vertex	m_vertexBufferData[c_vertexBufferSize];								// Space to store coordinates

		GLuint	m_extrasBufferTex;
		GLuint	m_extrasBufferId;
		GLfloat m_extrasBufferData[c_extrasBufferSize];								// Space to store additional primitive data for shaders


		int		m_commandBuffer[c_commandBufferSize];								// Queue of commands to execute when flushing buffer

		GlSurface_p m_surfaceBuffer[c_surfaceBufferSize];

		RectI	m_clipListBuffer[c_clipListBufferSize];

		GlSurface * m_pActiveBlitSource = nullptr;									// Currently active blit source in OpenGL, not to confuse with m_pBlitSource which might not be active yet.
		GlSurface * m_pActiveCanvas = nullptr;                                      // Currently active canvas in OpenGL, not to consfuse with m_pCanvas which might not be active yet.
		bool        m_bMipmappedActiveCanvas = false;                               // Set if currently active canvas is a surface that is mipmapped.


		// GL states saved between BeginRender() and EndRender().

		GLboolean	m_glDepthTest;
		GLboolean   m_glScissorTest;
		GLboolean	m_glBlendEnabled;
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
		static const char blitVertexShader[];
		static const char blitFragmentShader[];
		static const char plotVertexShader[];
		static const char plotFragmentShader[];
		static const char lineFromToVertexShader[];
		static const char lineFromToFragmentShader[];
		static const char aaFillVertexShader[];
		static const char aaFillFragmentShader[];

		static const char * segmentVertexShaders[c_maxSegments];			// One entry for each number of edges
		static const char * segmentFragmentShaders[c_maxSegments];		// One entry for each number of edges

		static const char alphaBlitFragmentShader[];

		static const char clutBlitNearestVertexShader[];
		static const char clutBlitNearestFragmentShader[];
		static const char clutBlitInterpolateVertexShader[];
		static const char clutBlitInterpolateFragmentShader[];

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
		m_commandBuffer[m_commandOfs++] = cmd;

		if (m_pCanvas)
			static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
	}

	inline void GlGfxDevice::_beginDrawCommandWithSource(Command cmd)
	{
		if (m_commandOfs > c_commandBufferSize - 2)
			_executeBuffer();

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_drawCmdFinalizer;
		m_cmdBeginVertexOfs = m_vertexOfs;
		m_commandBuffer[m_commandOfs++] = cmd;

		static_cast<GlSurface*>(m_pBlitSource.rawPtr())->m_bPendingReads = true;

		if( m_pCanvas)
			static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
	}

	inline void GlGfxDevice::_beginDrawCommandWithInt(Command cmd, int data)
	{
		if (m_commandOfs > c_commandBufferSize - 3)
			_executeBuffer();

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_drawCmdFinalizer;
		m_cmdBeginVertexOfs = m_vertexOfs;
		m_commandBuffer[m_commandOfs++] = cmd;
		m_commandBuffer[m_commandOfs++] = data;

		if (m_pCanvas)
			static_cast<GlSurface*>(m_pCanvas.rawPtr())->m_bBackingBufferStale = true;
	}


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
		m_commandBuffer[m_commandOfs++] = cmd;
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

		m_commandBuffer[m_commandOfs++] = cmd;
	}


	//____ _endCommand() ______________________________________________________

	inline void GlGfxDevice::_endCommand()
	{
		(this->*m_pCmdFinalizer)();
		m_pCmdFinalizer = &GlGfxDevice::_dummyFinalizer;
	}


} // namespace wg
#endif //WG_GLGFXDEVICE_DOT_H

