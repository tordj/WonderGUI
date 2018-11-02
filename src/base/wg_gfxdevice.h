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

#ifndef	WG_GFXDEVICE_DOT_H
#define WG_GFXDEVICE_DOT_H
#pragma once

#include <climits>

#include <wg_pointers.h>
#include <wg_types.h>
#include <wg_color.h>


#include <wg_geo.h>
#include <wg_surface.h>
#include <wg_surfacefactory.h>

namespace wg
{
	class	Rect;
	class	Border;
	class	Surface;
	class	LegacyTextItem;
	class	CaretInstance;
	class 	Pen;

	class GfxDevice;
	typedef	StrongPtr<GfxDevice>	GfxDevice_p;
	typedef	WeakPtr<GfxDevice>	GfxDevice_wp;


	//____ WaveLine ___________________________________________________________

	struct WaveLine
	{
		int		length;
		float	thickness;
		Color	color;
		int *	pWave;      // Pixel offset in 24.8 format.
		int		hold;      // Value for extending the line if it is too short (or completely missing).
	};
	
	//____ GfxDevice __________________________________________________________

	class GfxDevice : public Object
	{
	public:
		friend class GfxStreamPlayer;

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static GfxDevice_p		cast( Object * pObject );
		virtual const char *	surfaceClassName( void ) const = 0;

		//.____ Misc _______________________________________________________

		virtual SurfaceFactory_p	surfaceFactory() = 0;
		
		//.____ Geometry _________________________________________________

		virtual bool		setCanvas(Surface * pCanvas) = 0;
		Surface_p	canvas() const { return m_pCanvas; }

		inline Size	canvasSize() const { return m_canvasSize; }

		//.____ State _________________________________________________

		virtual void		setClip(const Rect& clip);
		inline Rect			clip() const { return m_clip; }
		
		virtual void		setTintColor( Color color );
		inline const Color&	tintColor() const { return m_tintColor; }

		virtual bool		setBlendMode( BlendMode blendMode );
		inline BlendMode 	blendMode() const { return m_blendMode; }

		virtual bool		setBlitSource(Surface * pSource);
		inline Surface_p 	blitSource() const { return m_pBlitSource; }


		//.____ Rendering ________________________________________________
	
		virtual bool	beginRender();
		virtual bool	endRender();
		
		// Draw methods.
	
		virtual void	fill(const Color& col);
		virtual void	fill( const Rect& rect, const Color& col );
		virtual void	fill(const RectF& rect, const Color& col);

        virtual void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors);

	 	virtual void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f );
		virtual void	drawLine( Coord begin, Direction dir, int length, Color col, float thickness = 1.f);

		// Draw patch methods

		virtual void	fillPatches(const Color& col, int nPatches, const Rect * pPatches );
		virtual void	fillPatches(const Rect& rect, const Color& col, int nPatches, const Rect * pPatches) = 0;
		virtual void	fillPatches(const RectF& rect, const Color& col, int nPatches, const Rect * pPatches) = 0;

		virtual void    plotPixelPatches(int nCoords, const Coord * pCoords, const Color * pColors, int nPatches, const Rect * pPatches) = 0;

		virtual void	drawLinePatches(Coord begin, Coord end, Color color, float thickness, int nPatches, const Rect * pPatches) = 0;
		virtual void	drawLinePatches(Coord begin, Direction dir, int length, Color col, float thickness, int nPatches, const Rect * pPatches);


		// Blit methods

		virtual void	blit(Coord dest);
		virtual void	blit(Coord dest, const Rect& src);

		virtual void	flipBlit(Coord dest, GfxFlip flip );
		virtual void	flipBlit(Coord dest, const Rect& src, GfxFlip flip );

		virtual void	stretchBlit(const Rect& dest);
		virtual void	stretchBlit(const Rect& dest, const Rect& src);
		virtual void	stretchBlit(const Rect& dest, const RectF& src);

		virtual void	stretchFlipBlit(const Rect& dest, GfxFlip flip);
		virtual void	stretchFlipBlit(const Rect& dest, const Rect& src, GfxFlip flip);
		virtual void	stretchFlipBlit(const Rect& dest, const RectF& src, GfxFlip flip);

		virtual void	rotScaleBlit(const Rect& dest, CoordF srcCenter, float rotationDegrees, float scale);

		// Patch blit methods

		virtual void	blitPatches(Coord dest, int nPatches, const Rect * pPatches);
		virtual void	blitPatches(Coord dest, const Rect& src, int nPatches, const Rect * pPatches);

		virtual void	flipBlitPatches(Coord dest, GfxFlip flip, int nPatches, const Rect * pPatches);
		virtual void	flipBlitPatches(Coord dest, const Rect& src, GfxFlip flip, int nPatches, const Rect * pPatches);

		virtual void	stretchBlitPatches(const Rect& dest, int nPatches, const Rect * pPatches);
		virtual void	stretchBlitPatches(const Rect& dest, const Rect& src, int nPatches, const Rect * pPatches);
		virtual void	stretchBlitPatches(const Rect& dest, const RectF& src, int nPatches, const Rect * pPatches);

		virtual void	stretchFlipBlitPatches(const Rect& dest, GfxFlip flip, int nPatches, const Rect * pPatches);
		virtual void	stretchFlipBlitPatches(const Rect& dest, const Rect& src, GfxFlip flip, int nPatches, const Rect * pPatches);
		virtual void	stretchFlipBlitPatches(const Rect& dest, const RectF& src, GfxFlip flip, int nPatches, const Rect * pPatches);

		virtual void	rotScaleBlitPatches(const Rect& dest, CoordF srcCenter, float rotationDegrees, float scale, int nPatches, const Rect * pPatches);

		// Draw segments methods

		virtual void	drawWave(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill);
		virtual void	flipDrawWave(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip);

		virtual void	drawElipse(const RectF& canvas, float thickness, Color color, float outlineThickness = 0.f, Color outlineColor = Color::Black);

		virtual void	drawSegments(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch);
		virtual void	flipDrawSegments(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, GfxFlip flip);


		// Draw segment patches methods

		virtual void	drawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, int nPatches, const Rect * pPatches);
		virtual void	flipDrawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, GfxFlip flip, int nPatches, const Rect * pPatches);

		virtual void	drawElipsePatches(const RectF& canvas, float thickness, Color color, float outlineThickness, Color outlineColor, int nPatches, const Rect * pPatches);

		virtual void	drawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, int nPatches, const Rect * pPatches);
		virtual void	flipDrawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, GfxFlip flip, int nPatches, const Rect * pPatches);


		// Low level methods

		virtual void	transformBlit(const Rect& dest, Coord src, const int simpleTransform[2][2] ) = 0;
		virtual void	transformBlit(const Rect& dest, CoordF src, const float complexTransform[2][2] ) = 0;

		virtual void	transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches ) = 0;
		virtual void	transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches ) = 0;

		virtual void	transformDrawWavePatches(const Rect& dest, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill, const int simpleTransform[2][2], int nPatches, const Rect * pPatches);
		virtual void	transformDrawSegmentPatches(const Rect& dest, int nSegments, Color * pSegmentColors, int nEdges, int * pEdges, int edgeStripPitch, const int simpleTransform[2][2], int nPatches, const Rect * pPatches) = 0;

		// Special draw/blit methods

		virtual void	blitFromCanvas(Coord dest, const Rect& src );			// Blit fr om surface that has been used as canvas. Will flip Y on OpenGL.

		virtual void	blitNinePatch(const Rect& dstRect, const Border& dstFrame, const Rect& srcRect, const Border& srcFrame);




		//.____ Deprecated ________________________________________________

		virtual void	blitHorrBar(		const Rect& _src,
											const Border& _borders, bool _bTile,
											Coord dest, int _len );
	
		virtual void	blitVertBar(		const Rect& _src,
											const Border& _borders, bool _bTile,
											Coord dest, int _len );
		
		
	protected:
		GfxDevice( Size canvasSize );
		virtual ~GfxDevice();

		// Static, shared data

		static	int		s_gfxDeviceCount;				// Number of existing gfxDevices. Ref count for shared data.

		void	_genCurveTab();
		void	_traceLine(int * pDest, int nPoints, const WaveLine * pWave, int offset);

		const static int c_nCurveTabEntries = 1024;
		static int *	s_pCurveTab;

		//

		Surface_p	m_pCanvas;
		Surface_p	m_pBlitSource;

		Rect		m_clip;				// Clipping rectangle.
		Color		m_tintColor;		// Current Tint color.
		BlendMode	m_blendMode;		// Current BlendMode.
		uint32_t	m_renderFlags;		// Current flags.
	
		Size		m_canvasSize;	
	};	

} // namespace wg
#endif	// WG_GFXDEVICE_DOT_H

