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

#ifndef WG_NULLGFXDEVICE_DOT_H
#define WG_NULLGFXDEVICE_DOT_H
#pragma once

#include <wg_types.h>
#include <wg_gfxdevice.h>

namespace wg 
{
	
	class Surface;
	class Rect;
	class Color;
	
	class NullGfxDevice;
	typedef	StrongPtr<NullGfxDevice>	NullGfxDevice_p;
	typedef	WeakPtr<NullGfxDevice>	NullGfxDevice_wp;
	
	class NullGfxDevice : public GfxDevice
	{
	public:
		//.____ Creation ________________________________________________

		static NullGfxDevice_p	create( Size size );
	
		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static NullGfxDevice_p	cast( Object * pObject );
		const char *			surfaceClassName( void ) const;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory();

		//.____ Geometry _________________________________________________

		bool		setCanvas(Surface * pCanvas);

		//.____ Rendering ________________________________________________

		void	fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches) override;
		void	fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches) override;

		void    plotPixelPatches(int nCoords, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches) override;

		void	drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches) override;

		void	transformBlit(const Rect& dest, Coord src, const int simpleTransform[2][2]) override;
		void	transformBlit(const Rect& dest, CoordF src, const float complexTransform[2][2]) override;

		void	transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) override;
		void	transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches) override;

		void	transformDrawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches);

	protected:
		NullGfxDevice( Size size );
		~NullGfxDevice();

	};
	

} // namespace wg
#endif //WG_NULLGFXDEVICE_DOT_H

