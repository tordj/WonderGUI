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
#		include <GLES3/gl3.h>
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
	public:

		//.____ Creation __________________________________________

		static GlGfxDevice_p	create(const Rect& viewport);
		static GlGfxDevice_p	create(GlSurface * pCanvas);

		//.____ Identification __________________________________________

		bool					isInstanceOf(const char * pClassName) const;
		const char *			className(void) const;
		static const char		CLASSNAME[];
		static GlGfxDevice_p	cast(Object * pObject);
		const char *			surfaceClassName(void) const;


		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory();

		//.____ Geometry _________________________________________________

		bool	setCanvas(Size canvasSize);
		bool	setCanvas(Surface * pCanvas);

		//.____ State _________________________________________________

		void	setClip(const Rect& clip) override;
		void	setTintColor(Color color) override;
		bool	setBlendMode(BlendMode blendMode) override;
		bool	setBlitSource(Surface * pSource) override;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;

		void	fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches) override;
		void	fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches) override;

		void    plotPixelPatches(int nCoords, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches) override;

		void	drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches) override;
		void	drawLinePatches(Coord begin, Direction dir, int length, Color col, float thickness, int nPatches, const Rect * pPatches) override;

		void	transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;
		void	transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches) override;

		void	transformDrawSegmentPatches(const Rect& dest, int nSegments, const Color * pSegmentColors, int nEdgeStrips, const int * pEdgeStrips, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;


	protected:
		GlGfxDevice(Size viewportSize);
		GlGfxDevice(const Rect& viewport);
		GlGfxDevice(GlSurface * pCanvas);
		~GlGfxDevice();

		enum Command
		{
			None,
			SetCanvas,
			SetClip,
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
		void	_setClip(Rect clip);
		void	_setBlendMode(BlendMode mode);
		void	_setBlitSource(GlSurface * pSurf);

		inline void	_beginDrawCommand(Command cmd);
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
		bool				m_bRendering = false;

		float	m_lineThicknessTable[17];

		//


		static const int c_commandBufferSize = 256;
		static const int c_vertexBufferSize = 16384;				// Size of vertex buffer, in number of vertices.
		static const int c_extrasBufferSize = 32768;				// Size of extras buffer, in GLfloats. 
		static const int c_surfaceBufferSize = 1024;				// SIze of Surface_p buffer, used by SetBlitSource and SetCanvas commands.

		Command			m_cmd;
		CmdFinalizer_p	m_pCmdFinalizer;
		int				m_cmdBeginVertexOfs;						// Saved for CmdFinalizer

		GLuint			m_framebufferId;

		int				m_canvasYstart;
		int				m_canvasYmul;

		// Device programs

		GLuint  m_fillProg;
		GLint	m_fillProgDimLoc;
		GLint	m_fillProgYofsLoc;
		GLint	m_fillProgYmulLoc;

		GLuint  m_aaFillProg;
		GLint	m_aaFillProgDimLoc;
		GLint	m_aaFillProgYofsLoc;
		GLint	m_aaFillProgYmulLoc;

		GLuint  m_blitProg;
		GLint	m_blitProgDimLoc;
		GLint	m_blitProgYofsLoc;
		GLint	m_blitProgYmulLoc;
		GLint	m_blitProgTexSizeLoc;

		GLuint  m_plotProg;
		GLint   m_plotProgDimLoc;
		GLint	m_plotProgYofsLoc;
		GLint	m_plotProgYmulLoc;

		GLuint  m_lineFromToProg;
		GLint   m_lineFromToProgDimLoc;
		GLint	m_lineFromToProgYofsLoc;
		GLint	m_lineFromToProgYmulLoc;

		//

		struct Vertex
		{
			Coord	coord;
			Color	color;
			int		extrasOfs;						// Offset into extras buffer.
		};


		// Buffers

		int		m_vertexOfs;						// Write offset in m_vertexBufferData
		int		m_extrasOfs;						// Write offset in m_extrasBufferData
		int		m_commandOfs;						// Write offset in m_commandBuffer
		int		m_surfaceOfs;						// Write offset in m_surfaceBuffer

		GLuint  m_vertexArrayId;
		GLuint  m_vertexBufferId;
		Vertex	m_vertexBufferData[c_vertexBufferSize];								// Space to store coordinates

		GLuint	m_extrasBufferTex;
		GLuint	m_extrasBufferId;
		GLfloat m_extrasBufferData[c_extrasBufferSize];								// Space to store additional primitive data for shaders

		int		m_commandBuffer[c_commandBufferSize];								// Queue of commands to execute when flushing buffer

		GlSurface_p m_surfaceBuffer[c_surfaceBufferSize];

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

  
	};

	//____ _beginDrawCommand() ________________________________________________

	inline void GlGfxDevice::_beginDrawCommand(Command cmd)
	{
		if (m_commandOfs > c_commandBufferSize - 2)
			_executeBuffer();

		m_cmd = cmd;
		m_pCmdFinalizer = &GlGfxDevice::_drawCmdFinalizer;
		m_cmdBeginVertexOfs = m_vertexOfs;
		m_commandBuffer[m_commandOfs++] = cmd;
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

