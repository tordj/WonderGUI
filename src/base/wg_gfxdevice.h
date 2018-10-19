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
		
		// Low level draw methods.
	
		virtual void	fill(const Color& col);
		virtual void	fill( const Rect& rect, const Color& col );
		virtual void	fill(const RectF& rect, const Color& col) = 0;

        virtual void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors) = 0;

	 	virtual void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f ) = 0;
		virtual void	drawLine( Coord begin, Direction dir, int length, Color col, float thickness = 1.f);

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

		// Low level blit methods

		virtual void	transformBlit(const Rect& dest, Coord src, const int simpleTransform[2][2] ) = 0;
		virtual void	transformBlit(const Rect& dest, CoordF src, const float complexTransform[2][2] ) = 0;

		virtual void	transformBlitPatches(const Rect& dest, Coord src, const int simpleTransform[2][2], int nPatches, const Rect * pPatches ) = 0;
		virtual void	transformBlitPatches(const Rect& dest, CoordF src, const float complexTransform[2][2], int nPatches, const Rect * pPatches ) = 0;

		virtual void	drawSegments(const Rect& dest, int nSegments, Color * pSegmentColors, int * pEdges, int edgeStripPitch) = 0;

		// Special draw/blit methods

		virtual void	drawHorrWave(Coord begin, int length, const WaveLine * pTopBorder, const WaveLine * pBottomBorder, Color frontFill, Color backFill) = 0;
		virtual void	drawElipse(const RectF& canvas, float thickness, Color color, float outlineThickness = 0.f, Color outlineColor = Color::Black);


		virtual void	blitFromCanvas(Coord dest, const Rect& src );			// Blit from surface that has been used as canvas. Will flip Y on OpenGL.

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

