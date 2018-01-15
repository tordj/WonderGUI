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
#ifndef WG_STREAMGFXDEVICE_DOT_H
#define WG_STREAMGFXDEVICE_DOT_H
#pragma once

#include <wg_geo.h>

#include <wg_gfxdevice.h>
#include <wg_gfxoutstream.h>

namespace wg
{
	class StreamSurface;
	class StreamGfxDevice;
	typedef	StrongPtr<StreamGfxDevice> StreamGfxDevice_p;
	typedef	WeakPtr<StreamGfxDevice>	StreamGfxDevice_wp;

	class StreamGfxDevice : public GfxDevice
	{
	public:

		//.____ Creation __________________________________________

		static StreamGfxDevice_p	create( Size canvas, GfxOutStream * pStream );

		//.____ Identification __________________________________________

		bool					isInstanceOf( const char * pClassName ) const;
		const char *			className( void ) const;
		static const char		CLASSNAME[];
		static StreamGfxDevice_p	cast( Object * pObject );
		const char *			surfaceClassName( void ) const;

		//.____ Misc _______________________________________________________

		SurfaceFactory_p		surfaceFactory();

		//.____ Geometry _________________________________________________

		bool	setCanvas( Surface * pCanvas );

		//.____ State _________________________________________________

		void	setTintColor( Color color ) override;
		bool	setBlendMode( BlendMode blendMode ) override;

		//.____ Rendering ________________________________________________

		bool	beginRender() override;
		bool	endRender() override;

		void	fill( const Rect& rect, const Color& col ) override; 
        
        void    plotPixels( int nCoords, const Coord * pCoords, const Color * pColors) override;
        void    clipPlotPixels( const Rect& clip, int nCoords, const Coord * pCoords, const Color * pColors) override;

		void	drawLine( Coord begin, Coord end, Color color, float thickness = 1.f ) override;
		void	clipDrawLine( const Rect& clip, Coord begin, Coord end, Color color, float thickness = 1.f ) override;

		void	clipDrawHorrWave(const Rect&clip, Coord begin, int length, const WaveLine* topLine, const WaveLine* bottomLine, Color front, Color back);


		void	blit( Surface * src, const Rect& srcrect, Coord dest  ) override;

		void	stretchBlit( Surface * pSrc, const RectF& source, const Rect& dest ) override;

		void	fillSubPixel( const RectF& rect, const Color& col ) override;

	protected:
		StreamGfxDevice( Size canvas, GfxOutStream * pStream );
		~StreamGfxDevice();

		void	_drawStraightLine(Coord start, Orientation orientation, int _length, const Color& _col) override;

		void	_setBlendMode(BlendMode blendMode);
		void	_setFramebuffer();

       
        SurfaceFactory_p	m_pSurfaceFactory;
		GfxOutStream_p		m_pStream;
	    bool	m_bRendering;
	};
} // namespace wg
#endif //WG_STREAMGFXDEVICE_DOT_H

