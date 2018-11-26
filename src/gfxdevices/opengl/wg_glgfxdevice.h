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

		static GlGfxDevice_p	create( const Rect& viewport );
		static GlGfxDevice_p	create( GlSurface * pCanvas );

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static GlGfxDevice_p	cast( Object * pObject );
		const char *			surfaceClassName( void ) const;


		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory();

		//.____ Geometry _________________________________________________

		bool	setCanvas( const Rect& viewport );
		bool	setCanvas( Surface * pCanvas );

		//.____ State _________________________________________________

		void	setClip(const Rect& clip) override;
		void	setTintColor( Color color ) override;
		bool	setBlendMode( BlendMode blendMode ) override;
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

//		void	blitFromCanvas(Coord dest, const Rect& src) override;


	protected:
		GlGfxDevice(Size viewportSize);
		GlGfxDevice( const Rect& viewport );
		GlGfxDevice(GlSurface * pCanvas);
		~GlGfxDevice();

		SurfaceFactory_p	m_pSurfaceFactory = nullptr;
		bool				m_bRendering = false;
  
  
	};
} // namespace wg
#endif //WG_GLGFXDEVICE_DOT_H

