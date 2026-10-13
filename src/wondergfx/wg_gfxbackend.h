/*=========================================================================

                             >>> WonderGUI <<<

  This file is part of Tord Bärnfors' WonderGUI UI Toolkit and copyright
  Tord Bärnfors, Sweden [mail: first name AT barnfors DOT c_o_m].

                                -----------

  The WonderGUI UI Toolkit is free software; you can redistribute
  this file and/or modify it under the terms of the GNU General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

                                -----------

  The WonderGUI UI Toolkit is also available for use in commercial
  closed source projects under a separate license. Interested parties
  should contact Bärnfors Technology AB [www.barnfors.com] for details.

=========================================================================*/
#ifndef	WG_GFXBACKEND_DOT_H
#define WG_GFXBACKEND_DOT_H
#pragma once

#include <climits>
#include <bitset>

#include <wg_gfxdevice.h>

#include <wg_pointers.h>
#include <wg_gfxtypes.h>
#include <wg_color.h>
#include <wg_edgemap.h>


#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>
#include <wg_edgemapfactory.h>
#include <wg_canvaslayers.h>

#include <vector>

namespace wg
{


	class GfxBackend;
	typedef	StrongPtr<GfxBackend>	GfxBackend_p;
	typedef	WeakPtr<GfxBackend>		GfxBackend_wp;


	//____ GfxBackend __________________________________________________________

	class GfxBackend : public Object
	{
	public:

		struct SessionInfo
		{
			// Commands

			int		nSetCanvas;			// Number of setCanvas calls in session.
			int		nStateChanges;		// Number of times state will change through session.
			int		nLines;				// Number of line commands.
			int		nFill;
			int		nBlit;				// Includes Blit, ClipBlit och Tile.
			int		nBlur;
			int		nEdgemapDraws;
			int		nLineCoords;		// Total number of coordinates for line commands (=total lines in line commands *2)

			// Total content of coord buffers

			int		nLineClipRects;		// Total number of clip rectangles for line commands.
			int		nRects;				// Total number of rects for fill, blit, blur and edgemapDraw commands.

			// Total size of other buffers

			int		nColors;			// Total number of colors to be used through all commands.
			int		nTransforms;		// Total number of transforms for session.
			int		nObjects;			// Total number of object references for session.

			// Shapes

			int		nShapeDraws;		// Number of DrawShape commands.
			int		nShapeSegments;		// Total number of segments (fill, outline, slices...) of all shapes.
			int		nShapeTints;		// Total number of Tints encoded in DrawShape commands.
		};

		enum class Command : uint16_t
		{
			None,
			StateChange,
			Fill,
			Line,
			Blit,
			ClipBlit,					// Might be blitting from outside source surface.
			Tile,
			Blur,
			DrawEdgemap,
			DrawShape					// See below.
		};

		/*
			DrawShape draws an analytic shape, made of segments (fill, outline...) that are
			drawn together. Layout, in 16-bit words:

			  uint16	Command::DrawShape
			  uint16	ShapeKind
			  uint16	nRects			Clip rectangles in rect buffer.
			  uint16	nColors			Colors in color buffer.
			  uint16	nWords			Words of payload following this header.
			  uint16	padding
			  ...		payload

			The command starts at a 32-bit boundary (padded with Command::None if needed), so
			32-bit values in the payload are aligned. Backends that don't support a kind of
			shape can skip it using nWords, nRects and nColors.

			ShapeKind::RoundedBox payload:

			  spx		rect x, y, w, h
			  spx		radius top-left, top-right, bottom-right, bottom-left
			  spx		outline top, right, bottom, left
			  spx		softness		Reserved for soft edges (shadows, glows), always 0 for now.
			  uint16	flags			See RoundedBoxFlags.
			  uint16	padding
			  ...		fill tint		Encoded as by TintTools::encodeTint(), placed in rect. Only if HasFillTint.
			  ...		outline tint	Encoded as by TintTools::encodeTint(), placed in rect. Only if HasOutlineTint.

			  Colors: fill color if HasFill, outline color if HasOutline, then the colors of
			  the encoded tints.

			  Radii are clamped as by Util::clampCornerRadii().

			ShapeKind::Ring payload:

			  spx		rect x, y, w, h
			  spx		thickness		Width of ring, outline included. 0 for no hole.
			  spx		outline
			  spx		softness		Reserved, always 0 for now.
			  uint16	flags			See RingFlags.
			  uint16	nSlices
			  uint32	start			Clockwise from 12 o'clock, 0x80000000 is a full turn.
			  per slice:
			    uint32	size			0x80000000 is a full turn.
			    uint16	flags			See RingSliceFlags.
			    uint16	padding
			  ...		tints			Encoded as by TintTools::encodeTint(), placed in rect: outline, hub, back
										and slices, each only if flagged as tinted.

			  Colors: outline, hub and back color if they are drawn, color of each slice that
			  is drawn, then the colors of the encoded tints.

			  Segments are numbered back (0), outline (1), hub (2) and slices (3...).
		*/

		enum class ShapeKind : uint16_t
		{
			RoundedBox = 1,
			Ring = 2
		};

		enum class RingFlags : uint16_t
		{
			Square			= 1,
			HasOutline		= 1 << 1,
			HasHub			= 1 << 2,
			HasBack			= 1 << 3,
			HasOutlineTint	= 1 << 4,
			HasHubTint		= 1 << 5,
			HasBackTint		= 1 << 6
		};

		enum class RingSliceFlags : uint16_t
		{
			Drawn			= 1,
			Tinted			= 1 << 1
		};

		enum class RoundedBoxFlags : uint16_t
		{
			HasFill			= 1,
			HasOutline		= 1 << 1,
			HasFillTint		= 1 << 2,
			HasOutlineTint	= 1 << 3
		};

		static const int	c_drawShapeHeaderWords = 6;

		enum class StateChange : uint8_t
		{
			BlitSource		= 1,
			TintColor		= 1 << 1,		// Flat tint color. Multiplied with Tint if both are set.
			Tint			= 1 << 2,		// Tint (gradient), encoded as described in TintTools.
			BlendMode		= 1 << 3,
			MorphFactor		= 1 << 4,
			FixedBlendColor	= 1 << 5,
			Blur			= 1 << 6,
		};
		
		
		//.____ Identification __________________________________________

		const TypeInfo& typeInfo(void) const override;
		const static TypeInfo	TYPEINFO;

		//.____ Rendering ________________________________________________

		virtual void	beginRender() = 0;
		virtual void	endRender() = 0;
		
		virtual void	beginSession( CanvasRef canvasRef, Surface * pCanvas, int nUpdateRects, const RectSPX * pUpdateRects, const SessionInfo * pInfo = nullptr ) = 0;
		virtual void	endSession() = 0;

		virtual void	setCanvas( Surface * pSurface ) = 0;
		virtual void	setCanvas( CanvasRef ref ) = 0;

		virtual void	setObjects(Object* const * pBeg, Object* const * pEnd) = 0;

		virtual void	setRects(const RectSPX* pBeg, const RectSPX* pEnd) = 0;
		virtual void	setColors(const HiColor* pBeg, const HiColor* pEnd) = 0;
		virtual void	setTransforms(const Transform * pBeg, const Transform * pEnd) = 0;

		virtual void	processCommands( const uint16_t* pBeg, const uint16_t * pEnd, int version = 2) = 0;


		//.____ Misc _________________________________________________________

		virtual const CanvasInfo *	canvasInfo(CanvasRef ref) const = 0;

		virtual SurfaceFactory_p	surfaceFactory() = 0;
		virtual EdgemapFactory_p	edgemapFactory() = 0;

		virtual int		maxEdges() const = 0;

		virtual bool	canBeBlitSource(const TypeInfo& type) const = 0;
		virtual bool	canBeCanvas(const TypeInfo& type) const = 0;

		virtual void	waitForCompletion() = 0;

		static const int	NbStandardTransforms = GfxFlip_size + 3;

	protected:
		GfxBackend() {};
		virtual ~GfxBackend() {};

		static const Transform			s_standardTransforms[NbStandardTransforms];
		static const int				s_defaultBlur[9];
		static const spx				s_defaultBlurRadius;

	};

} // namespace wg
#endif	// WG_GFXBACKEND_DOT_H

