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

#include <wg_nullgfxdevice.h>

namespace wg
{

	const TypeInfo NullGfxDevice::TYPEINFO = { "NullGfxDevice", &GfxDevice::TYPEINFO };
	const TypeInfo NullGfxDevice::SURFACETYPEINFO = { "[no surface exists for NullGfxDevice]", nullptr };


	//____ create() _______________________________________________________________

	NullGfxDevice_p NullGfxDevice::create()
	{
		return NullGfxDevice_p(new NullGfxDevice());
	}

	NullGfxDevice::NullGfxDevice()
	{
	}

	NullGfxDevice::~NullGfxDevice()
	{
	}

	//____ typeInfo() _________________________________________________________

	const TypeInfo& NullGfxDevice::typeInfo(void) const
	{
		return TYPEINFO;
	}

	//____ surfaceType() _______________________________________________________

	const TypeInfo& NullGfxDevice::surfaceType( void ) const
	{
		return SURFACETYPEINFO;
	}

	//____ surfaceFactory() _______________________________________________________

	SurfaceFactory_p NullGfxDevice::surfaceFactory()
	{
		return SurfaceFactory_p();
	}

	SizeI NullGfxDevice::canvasSize(CanvasRef ref) const
	{
		return SizeI();
	}


	void NullGfxDevice::_canvasWasChanged()
	{
	}

	void NullGfxDevice::_renderLayerWasChanged()
	{
	}

	void NullGfxDevice::fill(const RectI& rect, HiColor col)
	{
	}

	void NullGfxDevice::fill(const RectF& rect, HiColor col)
	{
	}

	void NullGfxDevice::plotPixels(int nCoords, const CoordI * pCoords, const HiColor * pColors)
	{
	}

	void NullGfxDevice::drawLine(CoordI begin, CoordI end, HiColor color, float thickness)
	{
	}

	void NullGfxDevice::_transformBlit(const RectI& dest, CoordI src, const int simpleTransform[2][2])
	{
	}

	void NullGfxDevice::_transformBlit(const RectI& dest, CoordF src, const float complexTransform[2][2])
	{
	}

	void NullGfxDevice::_transformDrawSegments(const RectI& dest, int nSegments, const HiColor * pSegmentColors, int nEdges, const int * pEdges, int edgeStripPitch, TintMode tintMode, const int simpleTransform[2][2])
	{
	}


} // namespace wg
